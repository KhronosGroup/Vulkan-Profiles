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
 * Author: Christophe Riccio <christophe@lunarg.com>
 * Author: Mark Lobodzinski <mark@lunarg.com>
 */

#include <vulkan/vulkan_core.h>

#include <gtest/gtest.h>
#include "profiles_test_helper.h"

static VkPhysicalDevice gpu_profile;
static VkPhysicalDevice gpu_native;
static profiles_test::VulkanInstanceBuilder inst_builder;

class TestsCapabilitiesGenerated : public VkTestFramework {
  public:
    TestsCapabilitiesGenerated(){};
    ~TestsCapabilitiesGenerated(){};

    static void SetUpTestSuite() {
        VkResult err = VK_SUCCESS;

        VkProfileLayerSettingsEXT settings;
        settings.profile_file = JSON_TEST_FILES_PATH "VP_LUNARG_test_api_generated.json";
        settings.emulate_portability = true;
        settings.profile_name = "VP_LUNARG_test_api";
        settings.simulate_capabilities = SIMULATE_MAX_ENUM;
        settings.debug_reports = DEBUG_REPORT_ERROR_BIT;

        err = inst_builder.init(&settings);
        ASSERT_EQ(err, VK_SUCCESS);

        err = inst_builder.getPhysicalDevice(profiles_test::MODE_PROFILE, &gpu_profile);
        ASSERT_EQ(err, VK_SUCCESS);

        err = inst_builder.getPhysicalDevice(profiles_test::MODE_NATIVE, &gpu_native);
        ASSERT_EQ(err, VK_SUCCESS);
    };

    static void TearDownTestSuite() {
        inst_builder.reset();
    };

};

bool IsSupported(VkPhysicalDevice device, const char* extension_name){
    uint32_t count = 0;
    vkEnumerateDeviceExtensionProperties(device, nullptr, &count, nullptr);
    std::vector<VkExtensionProperties> extensions(count);
    vkEnumerateDeviceExtensionProperties(device, nullptr, &count, extensions.data());

    for (const auto& ext : extensions) {
        if (strcmp(ext.extensionName, extension_name) == 0) {
            return true;
        }
    }

    return false;
}

TEST_F(TestsCapabilitiesGenerated, TestDeviceGeneratedCommandsPropertiesNV) {
#ifdef VK_NV_device_generated_commands
    bool supported = false;
    supported = supported && IsSupported(gpu_profile, "VK_NV_device_generated_commands");

    VkPhysicalDeviceDeviceGeneratedCommandsPropertiesNV device_generated_commands_properties_native{};
    device_generated_commands_properties_native.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DEVICE_GENERATED_COMMANDS_PROPERTIES_NV;

    VkPhysicalDeviceProperties2 gpu_props_native{};
    gpu_props_native.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props_native.pNext = &device_generated_commands_properties_native;
    vkGetPhysicalDeviceProperties2(gpu_native, &gpu_props_native);

    VkPhysicalDeviceDeviceGeneratedCommandsPropertiesNV device_generated_commands_properties_profile{};
    device_generated_commands_properties_profile.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DEVICE_GENERATED_COMMANDS_PROPERTIES_NV;

    VkPhysicalDeviceProperties2 gpu_props_profile{};
    gpu_props_profile.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props_profile.pNext = &device_generated_commands_properties_profile;
    vkGetPhysicalDeviceProperties2(gpu_profile, &gpu_props_profile);

    EXPECT_EQ(device_generated_commands_properties_profile.maxGraphicsShaderGroupCount, 1);
    EXPECT_EQ(device_generated_commands_properties_profile.maxIndirectSequenceCount, 2);
    EXPECT_EQ(device_generated_commands_properties_profile.maxIndirectCommandsTokenCount, 3);
    EXPECT_EQ(device_generated_commands_properties_profile.maxIndirectCommandsStreamCount, 4);
    EXPECT_EQ(device_generated_commands_properties_profile.maxIndirectCommandsTokenOffset, 5);
    EXPECT_EQ(device_generated_commands_properties_profile.maxIndirectCommandsStreamStride, 6);
    EXPECT_EQ(device_generated_commands_properties_profile.minSequencesCountBufferOffsetAlignment, 7);
    EXPECT_EQ(device_generated_commands_properties_profile.minSequencesIndexBufferOffsetAlignment, 8);
    EXPECT_EQ(device_generated_commands_properties_profile.minIndirectCommandsBufferOffsetAlignment, 9);
#endif
}

TEST_F(TestsCapabilitiesGenerated, TestMultiDrawPropertiesEXT) {
#ifdef VK_EXT_multi_draw
    bool supported = false;
    supported = supported && IsSupported(gpu_profile, "VK_EXT_multi_draw");

    VkPhysicalDeviceMultiDrawPropertiesEXT multi_draw_properties_native{};
    multi_draw_properties_native.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTI_DRAW_PROPERTIES_EXT;

    VkPhysicalDeviceProperties2 gpu_props_native{};
    gpu_props_native.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props_native.pNext = &multi_draw_properties_native;
    vkGetPhysicalDeviceProperties2(gpu_native, &gpu_props_native);

    VkPhysicalDeviceMultiDrawPropertiesEXT multi_draw_properties_profile{};
    multi_draw_properties_profile.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTI_DRAW_PROPERTIES_EXT;

    VkPhysicalDeviceProperties2 gpu_props_profile{};
    gpu_props_profile.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props_profile.pNext = &multi_draw_properties_profile;
    vkGetPhysicalDeviceProperties2(gpu_profile, &gpu_props_profile);

    EXPECT_EQ(multi_draw_properties_profile.maxMultiDrawCount, 10);
#endif
}

TEST_F(TestsCapabilitiesGenerated, TestPushDescriptorPropertiesKHR) {
#ifdef VK_KHR_push_descriptor
    bool supported = false;
    supported = supported && IsSupported(gpu_profile, "VK_KHR_push_descriptor");

    VkPhysicalDevicePushDescriptorPropertiesKHR push_descriptor_properties_native{};
    push_descriptor_properties_native.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PUSH_DESCRIPTOR_PROPERTIES_KHR;

    VkPhysicalDeviceProperties2 gpu_props_native{};
    gpu_props_native.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props_native.pNext = &push_descriptor_properties_native;
    vkGetPhysicalDeviceProperties2(gpu_native, &gpu_props_native);

    VkPhysicalDevicePushDescriptorPropertiesKHR push_descriptor_properties_profile{};
    push_descriptor_properties_profile.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PUSH_DESCRIPTOR_PROPERTIES_KHR;

    VkPhysicalDeviceProperties2 gpu_props_profile{};
    gpu_props_profile.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props_profile.pNext = &push_descriptor_properties_profile;
    vkGetPhysicalDeviceProperties2(gpu_profile, &gpu_props_profile);

    EXPECT_EQ(push_descriptor_properties_profile.maxPushDescriptors, 11);
#endif
}

TEST_F(TestsCapabilitiesGenerated, TestDriverPropertiesKHR) {
#ifdef VK_KHR_driver_properties
    bool supported = false;
    supported = supported && IsSupported(gpu_profile, "VK_KHR_driver_properties");

    VkPhysicalDeviceDriverPropertiesKHR driver_properties_native{};
    driver_properties_native.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DRIVER_PROPERTIES_KHR;

    VkPhysicalDeviceProperties2 gpu_props_native{};
    gpu_props_native.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props_native.pNext = &driver_properties_native;
    vkGetPhysicalDeviceProperties2(gpu_native, &gpu_props_native);

    VkPhysicalDeviceDriverPropertiesKHR driver_properties_profile{};
    driver_properties_profile.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DRIVER_PROPERTIES_KHR;

    VkPhysicalDeviceProperties2 gpu_props_profile{};
    gpu_props_profile.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props_profile.pNext = &driver_properties_profile;
    vkGetPhysicalDeviceProperties2(gpu_profile, &gpu_props_profile);

    if (supported) {
    EXPECT_EQ(0, strncmp(driver_properties_profile.driverName, driver_properties_native.driverName, 12));
    }

    if (supported) {
    EXPECT_EQ(0, strncmp(driver_properties_profile.driverInfo, driver_properties_native.driverInfo, 12));
    }

#endif
}

TEST_F(TestsCapabilitiesGenerated, TestIDPropertiesKHR) {
#ifdef VK_KHR_external_memory_capabilities
#ifdef VK_KHR_external_semaphore_capabilities
#ifdef VK_KHR_external_fence_capabilities
    bool supported = false;
    supported = supported && IsSupported(gpu_profile, "VK_KHR_external_memory_capabilities");

    supported = supported && IsSupported(gpu_profile, "VK_KHR_external_semaphore_capabilities");

    supported = supported && IsSupported(gpu_profile, "VK_KHR_external_fence_capabilities");

    VkPhysicalDeviceIDPropertiesKHR idproperties_native{};
    idproperties_native.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ID_PROPERTIES_KHR;

    VkPhysicalDeviceProperties2 gpu_props_native{};
    gpu_props_native.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props_native.pNext = &idproperties_native;
    vkGetPhysicalDeviceProperties2(gpu_native, &gpu_props_native);

    VkPhysicalDeviceIDPropertiesKHR idproperties_profile{};
    idproperties_profile.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ID_PROPERTIES_KHR;

    VkPhysicalDeviceProperties2 gpu_props_profile{};
    gpu_props_profile.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props_profile.pNext = &idproperties_profile;
    vkGetPhysicalDeviceProperties2(gpu_profile, &gpu_props_profile);

    if (supported) {
    EXPECT_EQ(idproperties_profile.deviceUUID[0], idproperties_native.deviceUUID[0]);
    EXPECT_EQ(idproperties_profile.deviceUUID[1], idproperties_native.deviceUUID[1]);
    EXPECT_EQ(idproperties_profile.deviceUUID[2], idproperties_native.deviceUUID[2]);
    EXPECT_EQ(idproperties_profile.deviceUUID[3], idproperties_native.deviceUUID[3]);
    EXPECT_EQ(idproperties_profile.deviceUUID[4], idproperties_native.deviceUUID[4]);
    EXPECT_EQ(idproperties_profile.deviceUUID[5], idproperties_native.deviceUUID[5]);
    EXPECT_EQ(idproperties_profile.deviceUUID[6], idproperties_native.deviceUUID[6]);
    EXPECT_EQ(idproperties_profile.deviceUUID[7], idproperties_native.deviceUUID[7]);
    EXPECT_EQ(idproperties_profile.deviceUUID[8], idproperties_native.deviceUUID[8]);
    EXPECT_EQ(idproperties_profile.deviceUUID[9], idproperties_native.deviceUUID[9]);
    EXPECT_EQ(idproperties_profile.deviceUUID[10], idproperties_native.deviceUUID[10]);
    EXPECT_EQ(idproperties_profile.deviceUUID[11], idproperties_native.deviceUUID[11]);
    EXPECT_EQ(idproperties_profile.deviceUUID[12], idproperties_native.deviceUUID[12]);
    EXPECT_EQ(idproperties_profile.deviceUUID[13], idproperties_native.deviceUUID[13]);
    EXPECT_EQ(idproperties_profile.deviceUUID[14], idproperties_native.deviceUUID[14]);
    EXPECT_EQ(idproperties_profile.deviceUUID[15], idproperties_native.deviceUUID[15]);
    }

    if (supported) {
    EXPECT_EQ(idproperties_profile.driverUUID[0], idproperties_native.driverUUID[0]);
    EXPECT_EQ(idproperties_profile.driverUUID[1], idproperties_native.driverUUID[1]);
    EXPECT_EQ(idproperties_profile.driverUUID[2], idproperties_native.driverUUID[2]);
    EXPECT_EQ(idproperties_profile.driverUUID[3], idproperties_native.driverUUID[3]);
    EXPECT_EQ(idproperties_profile.driverUUID[4], idproperties_native.driverUUID[4]);
    EXPECT_EQ(idproperties_profile.driverUUID[5], idproperties_native.driverUUID[5]);
    EXPECT_EQ(idproperties_profile.driverUUID[6], idproperties_native.driverUUID[6]);
    EXPECT_EQ(idproperties_profile.driverUUID[7], idproperties_native.driverUUID[7]);
    EXPECT_EQ(idproperties_profile.driverUUID[8], idproperties_native.driverUUID[8]);
    EXPECT_EQ(idproperties_profile.driverUUID[9], idproperties_native.driverUUID[9]);
    EXPECT_EQ(idproperties_profile.driverUUID[10], idproperties_native.driverUUID[10]);
    EXPECT_EQ(idproperties_profile.driverUUID[11], idproperties_native.driverUUID[11]);
    EXPECT_EQ(idproperties_profile.driverUUID[12], idproperties_native.driverUUID[12]);
    EXPECT_EQ(idproperties_profile.driverUUID[13], idproperties_native.driverUUID[13]);
    EXPECT_EQ(idproperties_profile.driverUUID[14], idproperties_native.driverUUID[14]);
    EXPECT_EQ(idproperties_profile.driverUUID[15], idproperties_native.driverUUID[15]);
    }

    if (supported) {
    EXPECT_EQ(idproperties_profile.deviceLUID[0], idproperties_native.deviceLUID[0]);
    EXPECT_EQ(idproperties_profile.deviceLUID[1], idproperties_native.deviceLUID[1]);
    EXPECT_EQ(idproperties_profile.deviceLUID[2], idproperties_native.deviceLUID[2]);
    EXPECT_EQ(idproperties_profile.deviceLUID[3], idproperties_native.deviceLUID[3]);
    EXPECT_EQ(idproperties_profile.deviceLUID[4], idproperties_native.deviceLUID[4]);
    EXPECT_EQ(idproperties_profile.deviceLUID[5], idproperties_native.deviceLUID[5]);
    EXPECT_EQ(idproperties_profile.deviceLUID[6], idproperties_native.deviceLUID[6]);
    EXPECT_EQ(idproperties_profile.deviceLUID[7], idproperties_native.deviceLUID[7]);
    }

    if (supported) {
    EXPECT_EQ(idproperties_profile.deviceNodeMask, idproperties_native.deviceNodeMask);
    }

    if (supported) {
    EXPECT_EQ(idproperties_profile.deviceLUIDValid, idproperties_native.deviceLUIDValid);
    }

#endif
#endif
#endif
}

TEST_F(TestsCapabilitiesGenerated, TestMultiviewPropertiesKHR) {
#ifdef VK_KHR_multiview
    bool supported = false;
    supported = supported && IsSupported(gpu_profile, "VK_KHR_multiview");

    VkPhysicalDeviceMultiviewPropertiesKHR multiview_properties_native{};
    multiview_properties_native.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTIVIEW_PROPERTIES_KHR;

    VkPhysicalDeviceProperties2 gpu_props_native{};
    gpu_props_native.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props_native.pNext = &multiview_properties_native;
    vkGetPhysicalDeviceProperties2(gpu_native, &gpu_props_native);

    VkPhysicalDeviceMultiviewPropertiesKHR multiview_properties_profile{};
    multiview_properties_profile.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTIVIEW_PROPERTIES_KHR;

    VkPhysicalDeviceProperties2 gpu_props_profile{};
    gpu_props_profile.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props_profile.pNext = &multiview_properties_profile;
    vkGetPhysicalDeviceProperties2(gpu_profile, &gpu_props_profile);

    EXPECT_EQ(multiview_properties_profile.maxMultiviewViewCount, 22);
    EXPECT_EQ(multiview_properties_profile.maxMultiviewInstanceIndex, 23);
#endif
}

TEST_F(TestsCapabilitiesGenerated, TestDiscardRectanglePropertiesEXT) {
#ifdef VK_EXT_discard_rectangles
    bool supported = false;
    supported = supported && IsSupported(gpu_profile, "VK_EXT_discard_rectangles");

    VkPhysicalDeviceDiscardRectanglePropertiesEXT discard_rectangle_properties_native{};
    discard_rectangle_properties_native.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DISCARD_RECTANGLE_PROPERTIES_EXT;

    VkPhysicalDeviceProperties2 gpu_props_native{};
    gpu_props_native.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props_native.pNext = &discard_rectangle_properties_native;
    vkGetPhysicalDeviceProperties2(gpu_native, &gpu_props_native);

    VkPhysicalDeviceDiscardRectanglePropertiesEXT discard_rectangle_properties_profile{};
    discard_rectangle_properties_profile.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DISCARD_RECTANGLE_PROPERTIES_EXT;

    VkPhysicalDeviceProperties2 gpu_props_profile{};
    gpu_props_profile.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props_profile.pNext = &discard_rectangle_properties_profile;
    vkGetPhysicalDeviceProperties2(gpu_profile, &gpu_props_profile);

    EXPECT_EQ(discard_rectangle_properties_profile.maxDiscardRectangles, 24);
#endif
}

TEST_F(TestsCapabilitiesGenerated, TestMultiviewPerViewAttributesPropertiesNVX) {
#ifdef VK_NVX_multiview_per_view_attributes
    bool supported = false;
    supported = supported && IsSupported(gpu_profile, "VK_NVX_multiview_per_view_attributes");

    VkPhysicalDeviceMultiviewPerViewAttributesPropertiesNVX multiview_per_view_attributes_properties_native{};
    multiview_per_view_attributes_properties_native.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTIVIEW_PER_VIEW_ATTRIBUTES_PROPERTIES_NVX;

    VkPhysicalDeviceProperties2 gpu_props_native{};
    gpu_props_native.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props_native.pNext = &multiview_per_view_attributes_properties_native;
    vkGetPhysicalDeviceProperties2(gpu_native, &gpu_props_native);

    VkPhysicalDeviceMultiviewPerViewAttributesPropertiesNVX multiview_per_view_attributes_properties_profile{};
    multiview_per_view_attributes_properties_profile.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTIVIEW_PER_VIEW_ATTRIBUTES_PROPERTIES_NVX;

    VkPhysicalDeviceProperties2 gpu_props_profile{};
    gpu_props_profile.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props_profile.pNext = &multiview_per_view_attributes_properties_profile;
    vkGetPhysicalDeviceProperties2(gpu_profile, &gpu_props_profile);

    EXPECT_EQ(multiview_per_view_attributes_properties_profile.perViewPositionAllComponents, VK_TRUE);
#endif
}

TEST_F(TestsCapabilitiesGenerated, TestPointClippingPropertiesKHR) {
#ifdef VK_KHR_maintenance2
    bool supported = false;
    supported = supported && IsSupported(gpu_profile, "VK_KHR_maintenance2");

    VkPhysicalDevicePointClippingPropertiesKHR point_clipping_properties_native{};
    point_clipping_properties_native.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_POINT_CLIPPING_PROPERTIES_KHR;

    VkPhysicalDeviceProperties2 gpu_props_native{};
    gpu_props_native.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props_native.pNext = &point_clipping_properties_native;
    vkGetPhysicalDeviceProperties2(gpu_native, &gpu_props_native);

    VkPhysicalDevicePointClippingPropertiesKHR point_clipping_properties_profile{};
    point_clipping_properties_profile.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_POINT_CLIPPING_PROPERTIES_KHR;

    VkPhysicalDeviceProperties2 gpu_props_profile{};
    gpu_props_profile.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props_profile.pNext = &point_clipping_properties_profile;
    vkGetPhysicalDeviceProperties2(gpu_profile, &gpu_props_profile);

    if (supported) {
    EXPECT_EQ(point_clipping_properties_profile.pointClippingBehavior, point_clipping_properties_native.pointClippingBehavior);
    }

#endif
}

TEST_F(TestsCapabilitiesGenerated, TestSamplerFilterMinmaxPropertiesEXT) {
#ifdef VK_EXT_sampler_filter_minmax
    bool supported = false;
    supported = supported && IsSupported(gpu_profile, "VK_EXT_sampler_filter_minmax");

    VkPhysicalDeviceSamplerFilterMinmaxPropertiesEXT sampler_filter_minmax_properties_native{};
    sampler_filter_minmax_properties_native.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SAMPLER_FILTER_MINMAX_PROPERTIES_EXT;

    VkPhysicalDeviceProperties2 gpu_props_native{};
    gpu_props_native.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props_native.pNext = &sampler_filter_minmax_properties_native;
    vkGetPhysicalDeviceProperties2(gpu_native, &gpu_props_native);

    VkPhysicalDeviceSamplerFilterMinmaxPropertiesEXT sampler_filter_minmax_properties_profile{};
    sampler_filter_minmax_properties_profile.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SAMPLER_FILTER_MINMAX_PROPERTIES_EXT;

    VkPhysicalDeviceProperties2 gpu_props_profile{};
    gpu_props_profile.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props_profile.pNext = &sampler_filter_minmax_properties_profile;
    vkGetPhysicalDeviceProperties2(gpu_profile, &gpu_props_profile);

    EXPECT_EQ(sampler_filter_minmax_properties_profile.filterMinmaxSingleComponentFormats, VK_TRUE);
    EXPECT_EQ(sampler_filter_minmax_properties_profile.filterMinmaxImageComponentMapping, VK_TRUE);
#endif
}

TEST_F(TestsCapabilitiesGenerated, TestSampleLocationsPropertiesEXT) {
#ifdef VK_EXT_sample_locations
    bool supported = false;
    supported = supported && IsSupported(gpu_profile, "VK_EXT_sample_locations");

    VkPhysicalDeviceSampleLocationsPropertiesEXT sample_locations_properties_native{};
    sample_locations_properties_native.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SAMPLE_LOCATIONS_PROPERTIES_EXT;

    VkPhysicalDeviceProperties2 gpu_props_native{};
    gpu_props_native.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props_native.pNext = &sample_locations_properties_native;
    vkGetPhysicalDeviceProperties2(gpu_native, &gpu_props_native);

    VkPhysicalDeviceSampleLocationsPropertiesEXT sample_locations_properties_profile{};
    sample_locations_properties_profile.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SAMPLE_LOCATIONS_PROPERTIES_EXT;

    VkPhysicalDeviceProperties2 gpu_props_profile{};
    gpu_props_profile.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props_profile.pNext = &sample_locations_properties_profile;
    vkGetPhysicalDeviceProperties2(gpu_profile, &gpu_props_profile);

    EXPECT_EQ(sample_locations_properties_profile.sampleLocationSampleCounts, VK_SAMPLE_COUNT_32_BIT);
    EXPECT_EQ(sample_locations_properties_profile.maxSampleLocationGridSize.width, 27);
    EXPECT_EQ(sample_locations_properties_profile.maxSampleLocationGridSize.height, 28);
    EXPECT_EQ(sample_locations_properties_profile.sampleLocationCoordinateRange[0], 29.0);
    EXPECT_EQ(sample_locations_properties_profile.sampleLocationCoordinateRange[1], 29.0);
    EXPECT_EQ(sample_locations_properties_profile.sampleLocationSubPixelBits, 30);
    EXPECT_EQ(sample_locations_properties_profile.variableSampleLocations, VK_TRUE);
#endif
}

TEST_F(TestsCapabilitiesGenerated, TestBlendOperationAdvancedPropertiesEXT) {
#ifdef VK_EXT_blend_operation_advanced
    bool supported = false;
    supported = supported && IsSupported(gpu_profile, "VK_EXT_blend_operation_advanced");

    VkPhysicalDeviceBlendOperationAdvancedPropertiesEXT blend_operation_advanced_properties_native{};
    blend_operation_advanced_properties_native.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BLEND_OPERATION_ADVANCED_PROPERTIES_EXT;

    VkPhysicalDeviceProperties2 gpu_props_native{};
    gpu_props_native.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props_native.pNext = &blend_operation_advanced_properties_native;
    vkGetPhysicalDeviceProperties2(gpu_native, &gpu_props_native);

    VkPhysicalDeviceBlendOperationAdvancedPropertiesEXT blend_operation_advanced_properties_profile{};
    blend_operation_advanced_properties_profile.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BLEND_OPERATION_ADVANCED_PROPERTIES_EXT;

    VkPhysicalDeviceProperties2 gpu_props_profile{};
    gpu_props_profile.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props_profile.pNext = &blend_operation_advanced_properties_profile;
    vkGetPhysicalDeviceProperties2(gpu_profile, &gpu_props_profile);

    EXPECT_EQ(blend_operation_advanced_properties_profile.advancedBlendMaxColorAttachments, 31);
    EXPECT_EQ(blend_operation_advanced_properties_profile.advancedBlendIndependentBlend, VK_TRUE);
    EXPECT_EQ(blend_operation_advanced_properties_profile.advancedBlendNonPremultipliedSrcColor, VK_TRUE);
    EXPECT_EQ(blend_operation_advanced_properties_profile.advancedBlendNonPremultipliedDstColor, VK_TRUE);
    EXPECT_EQ(blend_operation_advanced_properties_profile.advancedBlendCorrelatedOverlap, VK_TRUE);
    EXPECT_EQ(blend_operation_advanced_properties_profile.advancedBlendAllOperations, VK_TRUE);
#endif
}

TEST_F(TestsCapabilitiesGenerated, TestInlineUniformBlockPropertiesEXT) {
#ifdef VK_EXT_inline_uniform_block
    bool supported = false;
    supported = supported && IsSupported(gpu_profile, "VK_EXT_inline_uniform_block");

    VkPhysicalDeviceInlineUniformBlockPropertiesEXT inline_uniform_block_properties_native{};
    inline_uniform_block_properties_native.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_INLINE_UNIFORM_BLOCK_PROPERTIES_EXT;

    VkPhysicalDeviceProperties2 gpu_props_native{};
    gpu_props_native.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props_native.pNext = &inline_uniform_block_properties_native;
    vkGetPhysicalDeviceProperties2(gpu_native, &gpu_props_native);

    VkPhysicalDeviceInlineUniformBlockPropertiesEXT inline_uniform_block_properties_profile{};
    inline_uniform_block_properties_profile.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_INLINE_UNIFORM_BLOCK_PROPERTIES_EXT;

    VkPhysicalDeviceProperties2 gpu_props_profile{};
    gpu_props_profile.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props_profile.pNext = &inline_uniform_block_properties_profile;
    vkGetPhysicalDeviceProperties2(gpu_profile, &gpu_props_profile);

    EXPECT_EQ(inline_uniform_block_properties_profile.maxInlineUniformBlockSize, 32);
    EXPECT_EQ(inline_uniform_block_properties_profile.maxPerStageDescriptorInlineUniformBlocks, 33);
    EXPECT_EQ(inline_uniform_block_properties_profile.maxPerStageDescriptorUpdateAfterBindInlineUniformBlocks, 34);
    EXPECT_EQ(inline_uniform_block_properties_profile.maxDescriptorSetInlineUniformBlocks, 35);
    EXPECT_EQ(inline_uniform_block_properties_profile.maxDescriptorSetUpdateAfterBindInlineUniformBlocks, 36);
#endif
}

TEST_F(TestsCapabilitiesGenerated, TestMaintenance3PropertiesKHR) {
#ifdef VK_KHR_maintenance3
    bool supported = false;
    supported = supported && IsSupported(gpu_profile, "VK_KHR_maintenance3");

    VkPhysicalDeviceMaintenance3PropertiesKHR maintenance_3_properties_native{};
    maintenance_3_properties_native.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MAINTENANCE_3_PROPERTIES_KHR;

    VkPhysicalDeviceProperties2 gpu_props_native{};
    gpu_props_native.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props_native.pNext = &maintenance_3_properties_native;
    vkGetPhysicalDeviceProperties2(gpu_native, &gpu_props_native);

    VkPhysicalDeviceMaintenance3PropertiesKHR maintenance_3_properties_profile{};
    maintenance_3_properties_profile.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MAINTENANCE_3_PROPERTIES_KHR;

    VkPhysicalDeviceProperties2 gpu_props_profile{};
    gpu_props_profile.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props_profile.pNext = &maintenance_3_properties_profile;
    vkGetPhysicalDeviceProperties2(gpu_profile, &gpu_props_profile);

    EXPECT_EQ(maintenance_3_properties_profile.maxPerSetDescriptors, 37);
    EXPECT_EQ(maintenance_3_properties_profile.maxMemoryAllocationSize, 38);
#endif
}

TEST_F(TestsCapabilitiesGenerated, TestMaintenance4PropertiesKHR) {
#ifdef VK_KHR_maintenance4
    bool supported = false;
    supported = supported && IsSupported(gpu_profile, "VK_KHR_maintenance4");

    VkPhysicalDeviceMaintenance4PropertiesKHR maintenance_4_properties_native{};
    maintenance_4_properties_native.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MAINTENANCE_4_PROPERTIES_KHR;

    VkPhysicalDeviceProperties2 gpu_props_native{};
    gpu_props_native.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props_native.pNext = &maintenance_4_properties_native;
    vkGetPhysicalDeviceProperties2(gpu_native, &gpu_props_native);

    VkPhysicalDeviceMaintenance4PropertiesKHR maintenance_4_properties_profile{};
    maintenance_4_properties_profile.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MAINTENANCE_4_PROPERTIES_KHR;

    VkPhysicalDeviceProperties2 gpu_props_profile{};
    gpu_props_profile.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props_profile.pNext = &maintenance_4_properties_profile;
    vkGetPhysicalDeviceProperties2(gpu_profile, &gpu_props_profile);

    EXPECT_EQ(maintenance_4_properties_profile.maxBufferSize, 39);
#endif
}

TEST_F(TestsCapabilitiesGenerated, TestFloatControlsPropertiesKHR) {
#ifdef VK_KHR_shader_float_controls
    bool supported = false;
    supported = supported && IsSupported(gpu_profile, "VK_KHR_shader_float_controls");

    VkPhysicalDeviceFloatControlsPropertiesKHR float_controls_properties_native{};
    float_controls_properties_native.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FLOAT_CONTROLS_PROPERTIES_KHR;

    VkPhysicalDeviceProperties2 gpu_props_native{};
    gpu_props_native.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props_native.pNext = &float_controls_properties_native;
    vkGetPhysicalDeviceProperties2(gpu_native, &gpu_props_native);

    VkPhysicalDeviceFloatControlsPropertiesKHR float_controls_properties_profile{};
    float_controls_properties_profile.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FLOAT_CONTROLS_PROPERTIES_KHR;

    VkPhysicalDeviceProperties2 gpu_props_profile{};
    gpu_props_profile.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props_profile.pNext = &float_controls_properties_profile;
    vkGetPhysicalDeviceProperties2(gpu_profile, &gpu_props_profile);

    if (supported) {
    EXPECT_EQ(float_controls_properties_profile.denormBehaviorIndependence, float_controls_properties_native.denormBehaviorIndependence);
    }

    if (supported) {
    EXPECT_EQ(float_controls_properties_profile.roundingModeIndependence, float_controls_properties_native.roundingModeIndependence);
    }

    EXPECT_EQ(float_controls_properties_profile.shaderSignedZeroInfNanPreserveFloat16, VK_TRUE);
    EXPECT_EQ(float_controls_properties_profile.shaderSignedZeroInfNanPreserveFloat32, VK_TRUE);
    EXPECT_EQ(float_controls_properties_profile.shaderSignedZeroInfNanPreserveFloat64, VK_TRUE);
    EXPECT_EQ(float_controls_properties_profile.shaderDenormPreserveFloat16, VK_TRUE);
    EXPECT_EQ(float_controls_properties_profile.shaderDenormPreserveFloat32, VK_TRUE);
    EXPECT_EQ(float_controls_properties_profile.shaderDenormPreserveFloat64, VK_TRUE);
    EXPECT_EQ(float_controls_properties_profile.shaderDenormFlushToZeroFloat16, VK_TRUE);
    EXPECT_EQ(float_controls_properties_profile.shaderDenormFlushToZeroFloat32, VK_TRUE);
    EXPECT_EQ(float_controls_properties_profile.shaderDenormFlushToZeroFloat64, VK_TRUE);
    EXPECT_EQ(float_controls_properties_profile.shaderRoundingModeRTEFloat16, VK_TRUE);
    EXPECT_EQ(float_controls_properties_profile.shaderRoundingModeRTEFloat32, VK_TRUE);
    EXPECT_EQ(float_controls_properties_profile.shaderRoundingModeRTEFloat64, VK_TRUE);
    EXPECT_EQ(float_controls_properties_profile.shaderRoundingModeRTZFloat16, VK_TRUE);
    EXPECT_EQ(float_controls_properties_profile.shaderRoundingModeRTZFloat32, VK_TRUE);
    EXPECT_EQ(float_controls_properties_profile.shaderRoundingModeRTZFloat64, VK_TRUE);
#endif
}

TEST_F(TestsCapabilitiesGenerated, TestExternalMemoryHostPropertiesEXT) {
#ifdef VK_EXT_external_memory_host
    bool supported = false;
    supported = supported && IsSupported(gpu_profile, "VK_EXT_external_memory_host");

    VkPhysicalDeviceExternalMemoryHostPropertiesEXT external_memory_host_properties_native{};
    external_memory_host_properties_native.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTERNAL_MEMORY_HOST_PROPERTIES_EXT;

    VkPhysicalDeviceProperties2 gpu_props_native{};
    gpu_props_native.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props_native.pNext = &external_memory_host_properties_native;
    vkGetPhysicalDeviceProperties2(gpu_native, &gpu_props_native);

    VkPhysicalDeviceExternalMemoryHostPropertiesEXT external_memory_host_properties_profile{};
    external_memory_host_properties_profile.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTERNAL_MEMORY_HOST_PROPERTIES_EXT;

    VkPhysicalDeviceProperties2 gpu_props_profile{};
    gpu_props_profile.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props_profile.pNext = &external_memory_host_properties_profile;
    vkGetPhysicalDeviceProperties2(gpu_profile, &gpu_props_profile);

    EXPECT_EQ(external_memory_host_properties_profile.minImportedHostPointerAlignment, 42);
#endif
}

TEST_F(TestsCapabilitiesGenerated, TestConservativeRasterizationPropertiesEXT) {
#ifdef VK_EXT_conservative_rasterization
    bool supported = false;
    supported = supported && IsSupported(gpu_profile, "VK_EXT_conservative_rasterization");

    VkPhysicalDeviceConservativeRasterizationPropertiesEXT conservative_rasterization_properties_native{};
    conservative_rasterization_properties_native.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_CONSERVATIVE_RASTERIZATION_PROPERTIES_EXT;

    VkPhysicalDeviceProperties2 gpu_props_native{};
    gpu_props_native.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props_native.pNext = &conservative_rasterization_properties_native;
    vkGetPhysicalDeviceProperties2(gpu_native, &gpu_props_native);

    VkPhysicalDeviceConservativeRasterizationPropertiesEXT conservative_rasterization_properties_profile{};
    conservative_rasterization_properties_profile.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_CONSERVATIVE_RASTERIZATION_PROPERTIES_EXT;

    VkPhysicalDeviceProperties2 gpu_props_profile{};
    gpu_props_profile.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props_profile.pNext = &conservative_rasterization_properties_profile;
    vkGetPhysicalDeviceProperties2(gpu_profile, &gpu_props_profile);

    if (supported) {
    EXPECT_EQ(conservative_rasterization_properties_profile.primitiveOverestimationSize, conservative_rasterization_properties_native.primitiveOverestimationSize);
    }

    EXPECT_EQ(conservative_rasterization_properties_profile.maxExtraPrimitiveOverestimationSize, 44.0);
    EXPECT_EQ(conservative_rasterization_properties_profile.extraPrimitiveOverestimationSizeGranularity, 45.0);
    EXPECT_EQ(conservative_rasterization_properties_profile.primitiveUnderestimation, VK_TRUE);
    if (supported) {
    EXPECT_EQ(conservative_rasterization_properties_profile.conservativePointAndLineRasterization, conservative_rasterization_properties_native.conservativePointAndLineRasterization);
    }

    if (supported) {
    EXPECT_EQ(conservative_rasterization_properties_profile.degenerateTrianglesRasterized, conservative_rasterization_properties_native.degenerateTrianglesRasterized);
    }

    if (supported) {
    EXPECT_EQ(conservative_rasterization_properties_profile.degenerateLinesRasterized, conservative_rasterization_properties_native.degenerateLinesRasterized);
    }

    EXPECT_EQ(conservative_rasterization_properties_profile.fullyCoveredFragmentShaderInputVariable, VK_TRUE);
    EXPECT_EQ(conservative_rasterization_properties_profile.conservativeRasterizationPostDepthCoverage, VK_TRUE);
#endif
}

TEST_F(TestsCapabilitiesGenerated, TestShaderCorePropertiesAMD) {
#ifdef VK_AMD_shader_core_properties
    bool supported = false;
    supported = supported && IsSupported(gpu_profile, "VK_AMD_shader_core_properties");

    VkPhysicalDeviceShaderCorePropertiesAMD shader_core_properties_native{};
    shader_core_properties_native.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_CORE_PROPERTIES_AMD;

    VkPhysicalDeviceProperties2 gpu_props_native{};
    gpu_props_native.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props_native.pNext = &shader_core_properties_native;
    vkGetPhysicalDeviceProperties2(gpu_native, &gpu_props_native);

    VkPhysicalDeviceShaderCorePropertiesAMD shader_core_properties_profile{};
    shader_core_properties_profile.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_CORE_PROPERTIES_AMD;

    VkPhysicalDeviceProperties2 gpu_props_profile{};
    gpu_props_profile.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props_profile.pNext = &shader_core_properties_profile;
    vkGetPhysicalDeviceProperties2(gpu_profile, &gpu_props_profile);

    if (supported) {
    EXPECT_EQ(shader_core_properties_profile.shaderEngineCount, shader_core_properties_native.shaderEngineCount);
    }

    if (supported) {
    EXPECT_EQ(shader_core_properties_profile.shaderArraysPerEngineCount, shader_core_properties_native.shaderArraysPerEngineCount);
    }

    if (supported) {
    EXPECT_EQ(shader_core_properties_profile.computeUnitsPerShaderArray, shader_core_properties_native.computeUnitsPerShaderArray);
    }

    if (supported) {
    EXPECT_EQ(shader_core_properties_profile.simdPerComputeUnit, shader_core_properties_native.simdPerComputeUnit);
    }

    if (supported) {
    EXPECT_EQ(shader_core_properties_profile.wavefrontsPerSimd, shader_core_properties_native.wavefrontsPerSimd);
    }

    EXPECT_EQ(shader_core_properties_profile.wavefrontSize, 51);
    if (supported) {
    EXPECT_EQ(shader_core_properties_profile.sgprsPerSimd, shader_core_properties_native.sgprsPerSimd);
    }

    EXPECT_EQ(shader_core_properties_profile.minSgprAllocation, 53);
    EXPECT_EQ(shader_core_properties_profile.maxSgprAllocation, 54);
    EXPECT_EQ(shader_core_properties_profile.sgprAllocationGranularity, 55);
    if (supported) {
    EXPECT_EQ(shader_core_properties_profile.vgprsPerSimd, shader_core_properties_native.vgprsPerSimd);
    }

    EXPECT_EQ(shader_core_properties_profile.minVgprAllocation, 57);
    EXPECT_EQ(shader_core_properties_profile.maxVgprAllocation, 58);
    EXPECT_EQ(shader_core_properties_profile.vgprAllocationGranularity, 59);
#endif
}

TEST_F(TestsCapabilitiesGenerated, TestShaderCoreProperties2AMD) {
#ifdef VK_AMD_shader_core_properties2
    bool supported = false;
    supported = supported && IsSupported(gpu_profile, "VK_AMD_shader_core_properties2");

    VkPhysicalDeviceShaderCoreProperties2AMD shader_core_properties_2_native{};
    shader_core_properties_2_native.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_CORE_PROPERTIES_2_AMD;

    VkPhysicalDeviceProperties2 gpu_props_native{};
    gpu_props_native.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props_native.pNext = &shader_core_properties_2_native;
    vkGetPhysicalDeviceProperties2(gpu_native, &gpu_props_native);

    VkPhysicalDeviceShaderCoreProperties2AMD shader_core_properties_2_profile{};
    shader_core_properties_2_profile.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_CORE_PROPERTIES_2_AMD;

    VkPhysicalDeviceProperties2 gpu_props_profile{};
    gpu_props_profile.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props_profile.pNext = &shader_core_properties_2_profile;
    vkGetPhysicalDeviceProperties2(gpu_profile, &gpu_props_profile);

    EXPECT_EQ(shader_core_properties_2_profile.activeComputeUnitCount, 60);
#endif
}

TEST_F(TestsCapabilitiesGenerated, TestDescriptorIndexingPropertiesEXT) {
#ifdef VK_EXT_descriptor_indexing
    bool supported = false;
    supported = supported && IsSupported(gpu_profile, "VK_EXT_descriptor_indexing");

    VkPhysicalDeviceDescriptorIndexingPropertiesEXT descriptor_indexing_properties_native{};
    descriptor_indexing_properties_native.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_PROPERTIES_EXT;

    VkPhysicalDeviceProperties2 gpu_props_native{};
    gpu_props_native.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props_native.pNext = &descriptor_indexing_properties_native;
    vkGetPhysicalDeviceProperties2(gpu_native, &gpu_props_native);

    VkPhysicalDeviceDescriptorIndexingPropertiesEXT descriptor_indexing_properties_profile{};
    descriptor_indexing_properties_profile.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_PROPERTIES_EXT;

    VkPhysicalDeviceProperties2 gpu_props_profile{};
    gpu_props_profile.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props_profile.pNext = &descriptor_indexing_properties_profile;
    vkGetPhysicalDeviceProperties2(gpu_profile, &gpu_props_profile);

    EXPECT_EQ(descriptor_indexing_properties_profile.maxUpdateAfterBindDescriptorsInAllPools, 61);
    EXPECT_EQ(descriptor_indexing_properties_profile.shaderUniformBufferArrayNonUniformIndexingNative, VK_TRUE);
    EXPECT_EQ(descriptor_indexing_properties_profile.shaderSampledImageArrayNonUniformIndexingNative, VK_TRUE);
    EXPECT_EQ(descriptor_indexing_properties_profile.shaderStorageBufferArrayNonUniformIndexingNative, VK_TRUE);
    EXPECT_EQ(descriptor_indexing_properties_profile.shaderStorageImageArrayNonUniformIndexingNative, VK_TRUE);
    EXPECT_EQ(descriptor_indexing_properties_profile.shaderInputAttachmentArrayNonUniformIndexingNative, VK_TRUE);
    EXPECT_EQ(descriptor_indexing_properties_profile.robustBufferAccessUpdateAfterBind, VK_TRUE);
    EXPECT_EQ(descriptor_indexing_properties_profile.quadDivergentImplicitLod, VK_TRUE);
    EXPECT_EQ(descriptor_indexing_properties_profile.maxPerStageDescriptorUpdateAfterBindSamplers, 62);
    EXPECT_EQ(descriptor_indexing_properties_profile.maxPerStageDescriptorUpdateAfterBindUniformBuffers, 63);
    EXPECT_EQ(descriptor_indexing_properties_profile.maxPerStageDescriptorUpdateAfterBindStorageBuffers, 64);
    EXPECT_EQ(descriptor_indexing_properties_profile.maxPerStageDescriptorUpdateAfterBindSampledImages, 65);
    EXPECT_EQ(descriptor_indexing_properties_profile.maxPerStageDescriptorUpdateAfterBindStorageImages, 66);
    EXPECT_EQ(descriptor_indexing_properties_profile.maxPerStageDescriptorUpdateAfterBindInputAttachments, 67);
    EXPECT_EQ(descriptor_indexing_properties_profile.maxPerStageUpdateAfterBindResources, 68);
    EXPECT_EQ(descriptor_indexing_properties_profile.maxDescriptorSetUpdateAfterBindSamplers, 69);
    EXPECT_EQ(descriptor_indexing_properties_profile.maxDescriptorSetUpdateAfterBindUniformBuffers, 70);
    EXPECT_EQ(descriptor_indexing_properties_profile.maxDescriptorSetUpdateAfterBindUniformBuffersDynamic, 71);
    EXPECT_EQ(descriptor_indexing_properties_profile.maxDescriptorSetUpdateAfterBindStorageBuffers, 72);
    EXPECT_EQ(descriptor_indexing_properties_profile.maxDescriptorSetUpdateAfterBindStorageBuffersDynamic, 73);
    EXPECT_EQ(descriptor_indexing_properties_profile.maxDescriptorSetUpdateAfterBindSampledImages, 74);
    EXPECT_EQ(descriptor_indexing_properties_profile.maxDescriptorSetUpdateAfterBindStorageImages, 75);
    EXPECT_EQ(descriptor_indexing_properties_profile.maxDescriptorSetUpdateAfterBindInputAttachments, 76);
#endif
}

TEST_F(TestsCapabilitiesGenerated, TestTimelineSemaphorePropertiesKHR) {
#ifdef VK_KHR_timeline_semaphore
    bool supported = false;
    supported = supported && IsSupported(gpu_profile, "VK_KHR_timeline_semaphore");

    VkPhysicalDeviceTimelineSemaphorePropertiesKHR timeline_semaphore_properties_native{};
    timeline_semaphore_properties_native.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TIMELINE_SEMAPHORE_PROPERTIES_KHR;

    VkPhysicalDeviceProperties2 gpu_props_native{};
    gpu_props_native.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props_native.pNext = &timeline_semaphore_properties_native;
    vkGetPhysicalDeviceProperties2(gpu_native, &gpu_props_native);

    VkPhysicalDeviceTimelineSemaphorePropertiesKHR timeline_semaphore_properties_profile{};
    timeline_semaphore_properties_profile.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TIMELINE_SEMAPHORE_PROPERTIES_KHR;

    VkPhysicalDeviceProperties2 gpu_props_profile{};
    gpu_props_profile.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props_profile.pNext = &timeline_semaphore_properties_profile;
    vkGetPhysicalDeviceProperties2(gpu_profile, &gpu_props_profile);

    EXPECT_EQ(timeline_semaphore_properties_profile.maxTimelineSemaphoreValueDifference, 77);
#endif
}

TEST_F(TestsCapabilitiesGenerated, TestVertexAttributeDivisorPropertiesEXT) {
#ifdef VK_EXT_vertex_attribute_divisor
    bool supported = false;
    supported = supported && IsSupported(gpu_profile, "VK_EXT_vertex_attribute_divisor");

    VkPhysicalDeviceVertexAttributeDivisorPropertiesEXT vertex_attribute_divisor_properties_native{};
    vertex_attribute_divisor_properties_native.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VERTEX_ATTRIBUTE_DIVISOR_PROPERTIES_EXT;

    VkPhysicalDeviceProperties2 gpu_props_native{};
    gpu_props_native.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props_native.pNext = &vertex_attribute_divisor_properties_native;
    vkGetPhysicalDeviceProperties2(gpu_native, &gpu_props_native);

    VkPhysicalDeviceVertexAttributeDivisorPropertiesEXT vertex_attribute_divisor_properties_profile{};
    vertex_attribute_divisor_properties_profile.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VERTEX_ATTRIBUTE_DIVISOR_PROPERTIES_EXT;

    VkPhysicalDeviceProperties2 gpu_props_profile{};
    gpu_props_profile.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props_profile.pNext = &vertex_attribute_divisor_properties_profile;
    vkGetPhysicalDeviceProperties2(gpu_profile, &gpu_props_profile);

    EXPECT_EQ(vertex_attribute_divisor_properties_profile.maxVertexAttribDivisor, 78);
#endif
}

TEST_F(TestsCapabilitiesGenerated, TestPCIBusInfoPropertiesEXT) {
#ifdef VK_EXT_pci_bus_info
    bool supported = false;
    supported = supported && IsSupported(gpu_profile, "VK_EXT_pci_bus_info");

    VkPhysicalDevicePCIBusInfoPropertiesEXT pcibus_info_properties_native{};
    pcibus_info_properties_native.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PCI_BUS_INFO_PROPERTIES_EXT;

    VkPhysicalDeviceProperties2 gpu_props_native{};
    gpu_props_native.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props_native.pNext = &pcibus_info_properties_native;
    vkGetPhysicalDeviceProperties2(gpu_native, &gpu_props_native);

    VkPhysicalDevicePCIBusInfoPropertiesEXT pcibus_info_properties_profile{};
    pcibus_info_properties_profile.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PCI_BUS_INFO_PROPERTIES_EXT;

    VkPhysicalDeviceProperties2 gpu_props_profile{};
    gpu_props_profile.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props_profile.pNext = &pcibus_info_properties_profile;
    vkGetPhysicalDeviceProperties2(gpu_profile, &gpu_props_profile);

    if (supported) {
    EXPECT_EQ(pcibus_info_properties_profile.pciDomain, pcibus_info_properties_native.pciDomain);
    }

    if (supported) {
    EXPECT_EQ(pcibus_info_properties_profile.pciBus, pcibus_info_properties_native.pciBus);
    }

    if (supported) {
    EXPECT_EQ(pcibus_info_properties_profile.pciDevice, pcibus_info_properties_native.pciDevice);
    }

    if (supported) {
    EXPECT_EQ(pcibus_info_properties_profile.pciFunction, pcibus_info_properties_native.pciFunction);
    }

#endif
}

TEST_F(TestsCapabilitiesGenerated, TestDepthStencilResolvePropertiesKHR) {
#ifdef VK_KHR_depth_stencil_resolve
    bool supported = false;
    supported = supported && IsSupported(gpu_profile, "VK_KHR_depth_stencil_resolve");

    VkPhysicalDeviceDepthStencilResolvePropertiesKHR depth_stencil_resolve_properties_native{};
    depth_stencil_resolve_properties_native.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DEPTH_STENCIL_RESOLVE_PROPERTIES_KHR;

    VkPhysicalDeviceProperties2 gpu_props_native{};
    gpu_props_native.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props_native.pNext = &depth_stencil_resolve_properties_native;
    vkGetPhysicalDeviceProperties2(gpu_native, &gpu_props_native);

    VkPhysicalDeviceDepthStencilResolvePropertiesKHR depth_stencil_resolve_properties_profile{};
    depth_stencil_resolve_properties_profile.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DEPTH_STENCIL_RESOLVE_PROPERTIES_KHR;

    VkPhysicalDeviceProperties2 gpu_props_profile{};
    gpu_props_profile.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props_profile.pNext = &depth_stencil_resolve_properties_profile;
    vkGetPhysicalDeviceProperties2(gpu_profile, &gpu_props_profile);

    EXPECT_EQ(depth_stencil_resolve_properties_profile.supportedDepthResolveModes, VK_RESOLVE_MODE_MIN_BIT);
    EXPECT_EQ(depth_stencil_resolve_properties_profile.supportedStencilResolveModes, VK_RESOLVE_MODE_MAX_BIT);
    EXPECT_EQ(depth_stencil_resolve_properties_profile.independentResolveNone, VK_TRUE);
    EXPECT_EQ(depth_stencil_resolve_properties_profile.independentResolve, VK_TRUE);
#endif
}

TEST_F(TestsCapabilitiesGenerated, TestTransformFeedbackPropertiesEXT) {
#ifdef VK_EXT_transform_feedback
    bool supported = false;
    supported = supported && IsSupported(gpu_profile, "VK_EXT_transform_feedback");

    VkPhysicalDeviceTransformFeedbackPropertiesEXT transform_feedback_properties_native{};
    transform_feedback_properties_native.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TRANSFORM_FEEDBACK_PROPERTIES_EXT;

    VkPhysicalDeviceProperties2 gpu_props_native{};
    gpu_props_native.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props_native.pNext = &transform_feedback_properties_native;
    vkGetPhysicalDeviceProperties2(gpu_native, &gpu_props_native);

    VkPhysicalDeviceTransformFeedbackPropertiesEXT transform_feedback_properties_profile{};
    transform_feedback_properties_profile.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TRANSFORM_FEEDBACK_PROPERTIES_EXT;

    VkPhysicalDeviceProperties2 gpu_props_profile{};
    gpu_props_profile.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props_profile.pNext = &transform_feedback_properties_profile;
    vkGetPhysicalDeviceProperties2(gpu_profile, &gpu_props_profile);

    EXPECT_EQ(transform_feedback_properties_profile.maxTransformFeedbackStreams, 85);
    EXPECT_EQ(transform_feedback_properties_profile.maxTransformFeedbackBuffers, 86);
    EXPECT_EQ(transform_feedback_properties_profile.maxTransformFeedbackBufferSize, 87);
    EXPECT_EQ(transform_feedback_properties_profile.maxTransformFeedbackStreamDataSize, 88);
    EXPECT_EQ(transform_feedback_properties_profile.maxTransformFeedbackBufferDataSize, 89);
    EXPECT_EQ(transform_feedback_properties_profile.maxTransformFeedbackBufferDataStride, 90);
    EXPECT_EQ(transform_feedback_properties_profile.transformFeedbackQueries, VK_TRUE);
    EXPECT_EQ(transform_feedback_properties_profile.transformFeedbackStreamsLinesTriangles, VK_TRUE);
    EXPECT_EQ(transform_feedback_properties_profile.transformFeedbackRasterizationStreamSelect, VK_TRUE);
    EXPECT_EQ(transform_feedback_properties_profile.transformFeedbackDraw, VK_TRUE);
#endif
}

TEST_F(TestsCapabilitiesGenerated, TestShadingRateImagePropertiesNV) {
#ifdef VK_NV_shading_rate_image
    bool supported = false;
    supported = supported && IsSupported(gpu_profile, "VK_NV_shading_rate_image");

    VkPhysicalDeviceShadingRateImagePropertiesNV shading_rate_image_properties_native{};
    shading_rate_image_properties_native.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADING_RATE_IMAGE_PROPERTIES_NV;

    VkPhysicalDeviceProperties2 gpu_props_native{};
    gpu_props_native.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props_native.pNext = &shading_rate_image_properties_native;
    vkGetPhysicalDeviceProperties2(gpu_native, &gpu_props_native);

    VkPhysicalDeviceShadingRateImagePropertiesNV shading_rate_image_properties_profile{};
    shading_rate_image_properties_profile.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADING_RATE_IMAGE_PROPERTIES_NV;

    VkPhysicalDeviceProperties2 gpu_props_profile{};
    gpu_props_profile.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props_profile.pNext = &shading_rate_image_properties_profile;
    vkGetPhysicalDeviceProperties2(gpu_profile, &gpu_props_profile);

    if (supported) {
    EXPECT_EQ(shading_rate_image_properties_profile.shadingRateTexelSize.width, shading_rate_image_properties_native.shadingRateTexelSize.width);
    EXPECT_EQ(shading_rate_image_properties_profile.shadingRateTexelSize.height, shading_rate_image_properties_native.shadingRateTexelSize.height);
    }

    EXPECT_EQ(shading_rate_image_properties_profile.shadingRatePaletteSize, 93);
    EXPECT_EQ(shading_rate_image_properties_profile.shadingRateMaxCoarseSamples, 94);
#endif
}

TEST_F(TestsCapabilitiesGenerated, TestMeshShaderPropertiesNV) {
#ifdef VK_NV_mesh_shader
    bool supported = false;
    supported = supported && IsSupported(gpu_profile, "VK_NV_mesh_shader");

    VkPhysicalDeviceMeshShaderPropertiesNV mesh_shader_properties_native{};
    mesh_shader_properties_native.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MESH_SHADER_PROPERTIES_NV;

    VkPhysicalDeviceProperties2 gpu_props_native{};
    gpu_props_native.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props_native.pNext = &mesh_shader_properties_native;
    vkGetPhysicalDeviceProperties2(gpu_native, &gpu_props_native);

    VkPhysicalDeviceMeshShaderPropertiesNV mesh_shader_properties_profile{};
    mesh_shader_properties_profile.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MESH_SHADER_PROPERTIES_NV;

    VkPhysicalDeviceProperties2 gpu_props_profile{};
    gpu_props_profile.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props_profile.pNext = &mesh_shader_properties_profile;
    vkGetPhysicalDeviceProperties2(gpu_profile, &gpu_props_profile);

    EXPECT_EQ(mesh_shader_properties_profile.maxDrawMeshTasksCount, 95);
    EXPECT_EQ(mesh_shader_properties_profile.maxTaskWorkGroupInvocations, 96);
    EXPECT_EQ(mesh_shader_properties_profile.maxTaskWorkGroupSize[0], 97);
    EXPECT_EQ(mesh_shader_properties_profile.maxTaskWorkGroupSize[1], 97);
    EXPECT_EQ(mesh_shader_properties_profile.maxTaskWorkGroupSize[2], 97);
    EXPECT_EQ(mesh_shader_properties_profile.maxTaskTotalMemorySize, 98);
    EXPECT_EQ(mesh_shader_properties_profile.maxTaskOutputCount, 99);
    EXPECT_EQ(mesh_shader_properties_profile.maxMeshWorkGroupInvocations, 100);
    EXPECT_EQ(mesh_shader_properties_profile.maxMeshWorkGroupSize[0], 101);
    EXPECT_EQ(mesh_shader_properties_profile.maxMeshWorkGroupSize[1], 101);
    EXPECT_EQ(mesh_shader_properties_profile.maxMeshWorkGroupSize[2], 101);
    EXPECT_EQ(mesh_shader_properties_profile.maxMeshTotalMemorySize, 102);
    EXPECT_EQ(mesh_shader_properties_profile.maxMeshOutputVertices, 103);
    EXPECT_EQ(mesh_shader_properties_profile.maxMeshOutputPrimitives, 104);
    EXPECT_EQ(mesh_shader_properties_profile.maxMeshMultiviewViewCount, 105);
    EXPECT_EQ(mesh_shader_properties_profile.meshOutputPerVertexGranularity, 106);
    EXPECT_EQ(mesh_shader_properties_profile.meshOutputPerPrimitiveGranularity, 107);
#endif
}

TEST_F(TestsCapabilitiesGenerated, TestMeshShaderPropertiesEXT) {
#ifdef VK_EXT_mesh_shader
    bool supported = false;
    supported = supported && IsSupported(gpu_profile, "VK_EXT_mesh_shader");

    VkPhysicalDeviceMeshShaderPropertiesEXT mesh_shader_properties_native{};
    mesh_shader_properties_native.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MESH_SHADER_PROPERTIES_EXT;

    VkPhysicalDeviceProperties2 gpu_props_native{};
    gpu_props_native.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props_native.pNext = &mesh_shader_properties_native;
    vkGetPhysicalDeviceProperties2(gpu_native, &gpu_props_native);

    VkPhysicalDeviceMeshShaderPropertiesEXT mesh_shader_properties_profile{};
    mesh_shader_properties_profile.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MESH_SHADER_PROPERTIES_EXT;

    VkPhysicalDeviceProperties2 gpu_props_profile{};
    gpu_props_profile.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props_profile.pNext = &mesh_shader_properties_profile;
    vkGetPhysicalDeviceProperties2(gpu_profile, &gpu_props_profile);

    EXPECT_EQ(mesh_shader_properties_profile.maxTaskWorkGroupTotalCount, 108);
    EXPECT_EQ(mesh_shader_properties_profile.maxTaskWorkGroupCount[0], 109);
    EXPECT_EQ(mesh_shader_properties_profile.maxTaskWorkGroupCount[1], 109);
    EXPECT_EQ(mesh_shader_properties_profile.maxTaskWorkGroupCount[2], 109);
    EXPECT_EQ(mesh_shader_properties_profile.maxTaskWorkGroupInvocations, 110);
    EXPECT_EQ(mesh_shader_properties_profile.maxTaskWorkGroupSize[0], 111);
    EXPECT_EQ(mesh_shader_properties_profile.maxTaskWorkGroupSize[1], 111);
    EXPECT_EQ(mesh_shader_properties_profile.maxTaskWorkGroupSize[2], 111);
    EXPECT_EQ(mesh_shader_properties_profile.maxTaskPayloadSize, 112);
    EXPECT_EQ(mesh_shader_properties_profile.maxTaskSharedMemorySize, 113);
    EXPECT_EQ(mesh_shader_properties_profile.maxTaskPayloadAndSharedMemorySize, 114);
    EXPECT_EQ(mesh_shader_properties_profile.maxMeshWorkGroupTotalCount, 115);
    EXPECT_EQ(mesh_shader_properties_profile.maxMeshWorkGroupCount[0], 116);
    EXPECT_EQ(mesh_shader_properties_profile.maxMeshWorkGroupCount[1], 116);
    EXPECT_EQ(mesh_shader_properties_profile.maxMeshWorkGroupCount[2], 116);
    EXPECT_EQ(mesh_shader_properties_profile.maxMeshWorkGroupInvocations, 117);
    EXPECT_EQ(mesh_shader_properties_profile.maxMeshWorkGroupSize[0], 118);
    EXPECT_EQ(mesh_shader_properties_profile.maxMeshWorkGroupSize[1], 118);
    EXPECT_EQ(mesh_shader_properties_profile.maxMeshWorkGroupSize[2], 118);
    EXPECT_EQ(mesh_shader_properties_profile.maxMeshSharedMemorySize, 119);
    EXPECT_EQ(mesh_shader_properties_profile.maxMeshPayloadAndSharedMemorySize, 120);
    EXPECT_EQ(mesh_shader_properties_profile.maxMeshOutputMemorySize, 121);
    EXPECT_EQ(mesh_shader_properties_profile.maxMeshPayloadAndOutputMemorySize, 122);
    EXPECT_EQ(mesh_shader_properties_profile.maxMeshOutputComponents, 123);
    EXPECT_EQ(mesh_shader_properties_profile.maxMeshOutputVertices, 124);
    EXPECT_EQ(mesh_shader_properties_profile.maxMeshOutputPrimitives, 125);
    EXPECT_EQ(mesh_shader_properties_profile.maxMeshOutputLayers, 126);
    EXPECT_EQ(mesh_shader_properties_profile.maxMeshMultiviewViewCount, 127);
    if (supported) {
    EXPECT_EQ(mesh_shader_properties_profile.meshOutputPerVertexGranularity, mesh_shader_properties_native.meshOutputPerVertexGranularity);
    }

    if (supported) {
    EXPECT_EQ(mesh_shader_properties_profile.meshOutputPerPrimitiveGranularity, mesh_shader_properties_native.meshOutputPerPrimitiveGranularity);
    }

    EXPECT_EQ(mesh_shader_properties_profile.maxPreferredTaskWorkGroupInvocations, 130);
    EXPECT_EQ(mesh_shader_properties_profile.maxPreferredMeshWorkGroupInvocations, 131);
    if (supported) {
    EXPECT_EQ(mesh_shader_properties_profile.prefersLocalInvocationVertexOutput, mesh_shader_properties_native.prefersLocalInvocationVertexOutput);
    }

    if (supported) {
    EXPECT_EQ(mesh_shader_properties_profile.prefersLocalInvocationPrimitiveOutput, mesh_shader_properties_native.prefersLocalInvocationPrimitiveOutput);
    }

    if (supported) {
    EXPECT_EQ(mesh_shader_properties_profile.prefersCompactVertexOutput, mesh_shader_properties_native.prefersCompactVertexOutput);
    }

    if (supported) {
    EXPECT_EQ(mesh_shader_properties_profile.prefersCompactPrimitiveOutput, mesh_shader_properties_native.prefersCompactPrimitiveOutput);
    }

#endif
}

TEST_F(TestsCapabilitiesGenerated, TestAccelerationStructurePropertiesKHR) {
#ifdef VK_KHR_acceleration_structure
    bool supported = false;
    supported = supported && IsSupported(gpu_profile, "VK_KHR_acceleration_structure");

    VkPhysicalDeviceAccelerationStructurePropertiesKHR acceleration_structure_properties_native{};
    acceleration_structure_properties_native.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ACCELERATION_STRUCTURE_PROPERTIES_KHR;

    VkPhysicalDeviceProperties2 gpu_props_native{};
    gpu_props_native.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props_native.pNext = &acceleration_structure_properties_native;
    vkGetPhysicalDeviceProperties2(gpu_native, &gpu_props_native);

    VkPhysicalDeviceAccelerationStructurePropertiesKHR acceleration_structure_properties_profile{};
    acceleration_structure_properties_profile.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ACCELERATION_STRUCTURE_PROPERTIES_KHR;

    VkPhysicalDeviceProperties2 gpu_props_profile{};
    gpu_props_profile.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props_profile.pNext = &acceleration_structure_properties_profile;
    vkGetPhysicalDeviceProperties2(gpu_profile, &gpu_props_profile);

    EXPECT_EQ(acceleration_structure_properties_profile.maxGeometryCount, 132);
    EXPECT_EQ(acceleration_structure_properties_profile.maxInstanceCount, 133);
    EXPECT_EQ(acceleration_structure_properties_profile.maxPrimitiveCount, 134);
    EXPECT_EQ(acceleration_structure_properties_profile.maxPerStageDescriptorAccelerationStructures, 135);
    EXPECT_EQ(acceleration_structure_properties_profile.maxPerStageDescriptorUpdateAfterBindAccelerationStructures, 136);
    EXPECT_EQ(acceleration_structure_properties_profile.maxDescriptorSetAccelerationStructures, 137);
    EXPECT_EQ(acceleration_structure_properties_profile.maxDescriptorSetUpdateAfterBindAccelerationStructures, 138);
    EXPECT_EQ(acceleration_structure_properties_profile.minAccelerationStructureScratchOffsetAlignment, 139);
#endif
}

TEST_F(TestsCapabilitiesGenerated, TestRayTracingPipelinePropertiesKHR) {
#ifdef VK_KHR_ray_tracing_pipeline
    bool supported = false;
    supported = supported && IsSupported(gpu_profile, "VK_KHR_ray_tracing_pipeline");

    VkPhysicalDeviceRayTracingPipelinePropertiesKHR ray_tracing_pipeline_properties_native{};
    ray_tracing_pipeline_properties_native.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_PROPERTIES_KHR;

    VkPhysicalDeviceProperties2 gpu_props_native{};
    gpu_props_native.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props_native.pNext = &ray_tracing_pipeline_properties_native;
    vkGetPhysicalDeviceProperties2(gpu_native, &gpu_props_native);

    VkPhysicalDeviceRayTracingPipelinePropertiesKHR ray_tracing_pipeline_properties_profile{};
    ray_tracing_pipeline_properties_profile.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_PROPERTIES_KHR;

    VkPhysicalDeviceProperties2 gpu_props_profile{};
    gpu_props_profile.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props_profile.pNext = &ray_tracing_pipeline_properties_profile;
    vkGetPhysicalDeviceProperties2(gpu_profile, &gpu_props_profile);

    if (supported) {
    EXPECT_EQ(ray_tracing_pipeline_properties_profile.shaderGroupHandleSize, ray_tracing_pipeline_properties_native.shaderGroupHandleSize);
    }

    EXPECT_EQ(ray_tracing_pipeline_properties_profile.maxRayRecursionDepth, 141);
    EXPECT_EQ(ray_tracing_pipeline_properties_profile.maxShaderGroupStride, 142);
    if (supported) {
    EXPECT_EQ(ray_tracing_pipeline_properties_profile.shaderGroupBaseAlignment, ray_tracing_pipeline_properties_native.shaderGroupBaseAlignment);
    }

    if (supported) {
    EXPECT_EQ(ray_tracing_pipeline_properties_profile.shaderGroupHandleCaptureReplaySize, ray_tracing_pipeline_properties_native.shaderGroupHandleCaptureReplaySize);
    }

    EXPECT_EQ(ray_tracing_pipeline_properties_profile.maxRayDispatchInvocationCount, 145);
    EXPECT_EQ(ray_tracing_pipeline_properties_profile.shaderGroupHandleAlignment, 146);
    EXPECT_EQ(ray_tracing_pipeline_properties_profile.maxRayHitAttributeSize, 147);
#endif
}

TEST_F(TestsCapabilitiesGenerated, TestRayTracingPropertiesNV) {
#ifdef VK_NV_ray_tracing
    bool supported = false;
    supported = supported && IsSupported(gpu_profile, "VK_NV_ray_tracing");

    VkPhysicalDeviceRayTracingPropertiesNV ray_tracing_properties_native{};
    ray_tracing_properties_native.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PROPERTIES_NV;

    VkPhysicalDeviceProperties2 gpu_props_native{};
    gpu_props_native.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props_native.pNext = &ray_tracing_properties_native;
    vkGetPhysicalDeviceProperties2(gpu_native, &gpu_props_native);

    VkPhysicalDeviceRayTracingPropertiesNV ray_tracing_properties_profile{};
    ray_tracing_properties_profile.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PROPERTIES_NV;

    VkPhysicalDeviceProperties2 gpu_props_profile{};
    gpu_props_profile.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props_profile.pNext = &ray_tracing_properties_profile;
    vkGetPhysicalDeviceProperties2(gpu_profile, &gpu_props_profile);

    if (supported) {
    EXPECT_EQ(ray_tracing_properties_profile.shaderGroupHandleSize, ray_tracing_properties_native.shaderGroupHandleSize);
    }

    EXPECT_EQ(ray_tracing_properties_profile.maxRecursionDepth, 149);
    EXPECT_EQ(ray_tracing_properties_profile.maxShaderGroupStride, 150);
    if (supported) {
    EXPECT_EQ(ray_tracing_properties_profile.shaderGroupBaseAlignment, ray_tracing_properties_native.shaderGroupBaseAlignment);
    }

    EXPECT_EQ(ray_tracing_properties_profile.maxGeometryCount, 152);
    EXPECT_EQ(ray_tracing_properties_profile.maxInstanceCount, 153);
    EXPECT_EQ(ray_tracing_properties_profile.maxTriangleCount, 154);
    EXPECT_EQ(ray_tracing_properties_profile.maxDescriptorSetAccelerationStructures, 155);
#endif
}

TEST_F(TestsCapabilitiesGenerated, TestFragmentDensityMapPropertiesEXT) {
#ifdef VK_EXT_fragment_density_map
    bool supported = false;
    supported = supported && IsSupported(gpu_profile, "VK_EXT_fragment_density_map");

    VkPhysicalDeviceFragmentDensityMapPropertiesEXT fragment_density_map_properties_native{};
    fragment_density_map_properties_native.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_DENSITY_MAP_PROPERTIES_EXT;

    VkPhysicalDeviceProperties2 gpu_props_native{};
    gpu_props_native.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props_native.pNext = &fragment_density_map_properties_native;
    vkGetPhysicalDeviceProperties2(gpu_native, &gpu_props_native);

    VkPhysicalDeviceFragmentDensityMapPropertiesEXT fragment_density_map_properties_profile{};
    fragment_density_map_properties_profile.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_DENSITY_MAP_PROPERTIES_EXT;

    VkPhysicalDeviceProperties2 gpu_props_profile{};
    gpu_props_profile.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props_profile.pNext = &fragment_density_map_properties_profile;
    vkGetPhysicalDeviceProperties2(gpu_profile, &gpu_props_profile);

    EXPECT_EQ(fragment_density_map_properties_profile.minFragmentDensityTexelSize.width, 156);
    EXPECT_EQ(fragment_density_map_properties_profile.minFragmentDensityTexelSize.height, 157);
    EXPECT_EQ(fragment_density_map_properties_profile.maxFragmentDensityTexelSize.width, 158);
    EXPECT_EQ(fragment_density_map_properties_profile.maxFragmentDensityTexelSize.height, 159);
    EXPECT_EQ(fragment_density_map_properties_profile.fragmentDensityInvocations, VK_TRUE);
#endif
}

TEST_F(TestsCapabilitiesGenerated, TestFragmentDensityMap2PropertiesEXT) {
#ifdef VK_EXT_fragment_density_map2
    bool supported = false;
    supported = supported && IsSupported(gpu_profile, "VK_EXT_fragment_density_map2");

    VkPhysicalDeviceFragmentDensityMap2PropertiesEXT fragment_density_map_2_properties_native{};
    fragment_density_map_2_properties_native.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_DENSITY_MAP_2_PROPERTIES_EXT;

    VkPhysicalDeviceProperties2 gpu_props_native{};
    gpu_props_native.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props_native.pNext = &fragment_density_map_2_properties_native;
    vkGetPhysicalDeviceProperties2(gpu_native, &gpu_props_native);

    VkPhysicalDeviceFragmentDensityMap2PropertiesEXT fragment_density_map_2_properties_profile{};
    fragment_density_map_2_properties_profile.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_DENSITY_MAP_2_PROPERTIES_EXT;

    VkPhysicalDeviceProperties2 gpu_props_profile{};
    gpu_props_profile.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props_profile.pNext = &fragment_density_map_2_properties_profile;
    vkGetPhysicalDeviceProperties2(gpu_profile, &gpu_props_profile);

    if (supported) {
    EXPECT_EQ(fragment_density_map_2_properties_profile.subsampledLoads, fragment_density_map_2_properties_native.subsampledLoads);
    }

    if (supported) {
    EXPECT_EQ(fragment_density_map_2_properties_profile.subsampledCoarseReconstructionEarlyAccess, fragment_density_map_2_properties_native.subsampledCoarseReconstructionEarlyAccess);
    }

    EXPECT_EQ(fragment_density_map_2_properties_profile.maxSubsampledArrayLayers, 160);
    EXPECT_EQ(fragment_density_map_2_properties_profile.maxDescriptorSetSubsampledSamplers, 161);
#endif
}

TEST_F(TestsCapabilitiesGenerated, TestFragmentDensityMapOffsetPropertiesQCOM) {
#ifdef VK_QCOM_fragment_density_map_offset
    bool supported = false;
    supported = supported && IsSupported(gpu_profile, "VK_QCOM_fragment_density_map_offset");

    VkPhysicalDeviceFragmentDensityMapOffsetPropertiesQCOM fragment_density_map_offset_properties_native{};
    fragment_density_map_offset_properties_native.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_DENSITY_MAP_OFFSET_PROPERTIES_QCOM;

    VkPhysicalDeviceProperties2 gpu_props_native{};
    gpu_props_native.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props_native.pNext = &fragment_density_map_offset_properties_native;
    vkGetPhysicalDeviceProperties2(gpu_native, &gpu_props_native);

    VkPhysicalDeviceFragmentDensityMapOffsetPropertiesQCOM fragment_density_map_offset_properties_profile{};
    fragment_density_map_offset_properties_profile.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_DENSITY_MAP_OFFSET_PROPERTIES_QCOM;

    VkPhysicalDeviceProperties2 gpu_props_profile{};
    gpu_props_profile.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props_profile.pNext = &fragment_density_map_offset_properties_profile;
    vkGetPhysicalDeviceProperties2(gpu_profile, &gpu_props_profile);

    EXPECT_EQ(fragment_density_map_offset_properties_profile.fragmentDensityOffsetGranularity.width, 162);
    EXPECT_EQ(fragment_density_map_offset_properties_profile.fragmentDensityOffsetGranularity.height, 163);
#endif
}

TEST_F(TestsCapabilitiesGenerated, TestCooperativeMatrixPropertiesNV) {
#ifdef VK_NV_cooperative_matrix
    bool supported = false;
    supported = supported && IsSupported(gpu_profile, "VK_NV_cooperative_matrix");

    VkPhysicalDeviceCooperativeMatrixPropertiesNV cooperative_matrix_properties_native{};
    cooperative_matrix_properties_native.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_COOPERATIVE_MATRIX_PROPERTIES_NV;

    VkPhysicalDeviceProperties2 gpu_props_native{};
    gpu_props_native.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props_native.pNext = &cooperative_matrix_properties_native;
    vkGetPhysicalDeviceProperties2(gpu_native, &gpu_props_native);

    VkPhysicalDeviceCooperativeMatrixPropertiesNV cooperative_matrix_properties_profile{};
    cooperative_matrix_properties_profile.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_COOPERATIVE_MATRIX_PROPERTIES_NV;

    VkPhysicalDeviceProperties2 gpu_props_profile{};
    gpu_props_profile.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props_profile.pNext = &cooperative_matrix_properties_profile;
    vkGetPhysicalDeviceProperties2(gpu_profile, &gpu_props_profile);

    EXPECT_EQ(cooperative_matrix_properties_profile.cooperativeMatrixSupportedStages, VK_SHADER_STAGE_TASK_BIT_EXT);
#endif
}

TEST_F(TestsCapabilitiesGenerated, TestPerformanceQueryPropertiesKHR) {
#ifdef VK_KHR_performance_query
    bool supported = false;
    supported = supported && IsSupported(gpu_profile, "VK_KHR_performance_query");

    VkPhysicalDevicePerformanceQueryPropertiesKHR performance_query_properties_native{};
    performance_query_properties_native.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PERFORMANCE_QUERY_PROPERTIES_KHR;

    VkPhysicalDeviceProperties2 gpu_props_native{};
    gpu_props_native.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props_native.pNext = &performance_query_properties_native;
    vkGetPhysicalDeviceProperties2(gpu_native, &gpu_props_native);

    VkPhysicalDevicePerformanceQueryPropertiesKHR performance_query_properties_profile{};
    performance_query_properties_profile.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PERFORMANCE_QUERY_PROPERTIES_KHR;

    VkPhysicalDeviceProperties2 gpu_props_profile{};
    gpu_props_profile.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props_profile.pNext = &performance_query_properties_profile;
    vkGetPhysicalDeviceProperties2(gpu_profile, &gpu_props_profile);

    EXPECT_EQ(performance_query_properties_profile.allowCommandBufferQueryCopies, VK_TRUE);
#endif
}

TEST_F(TestsCapabilitiesGenerated, TestShaderSMBuiltinsPropertiesNV) {
#ifdef VK_NV_shader_sm_builtins
    bool supported = false;
    supported = supported && IsSupported(gpu_profile, "VK_NV_shader_sm_builtins");

    VkPhysicalDeviceShaderSMBuiltinsPropertiesNV shader_smbuiltins_properties_native{};
    shader_smbuiltins_properties_native.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_SM_BUILTINS_PROPERTIES_NV;

    VkPhysicalDeviceProperties2 gpu_props_native{};
    gpu_props_native.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props_native.pNext = &shader_smbuiltins_properties_native;
    vkGetPhysicalDeviceProperties2(gpu_native, &gpu_props_native);

    VkPhysicalDeviceShaderSMBuiltinsPropertiesNV shader_smbuiltins_properties_profile{};
    shader_smbuiltins_properties_profile.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_SM_BUILTINS_PROPERTIES_NV;

    VkPhysicalDeviceProperties2 gpu_props_profile{};
    gpu_props_profile.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props_profile.pNext = &shader_smbuiltins_properties_profile;
    vkGetPhysicalDeviceProperties2(gpu_profile, &gpu_props_profile);

    EXPECT_EQ(shader_smbuiltins_properties_profile.shaderSMCount, 165);
    EXPECT_EQ(shader_smbuiltins_properties_profile.shaderWarpsPerSM, 166);
#endif
}

TEST_F(TestsCapabilitiesGenerated, TestTexelBufferAlignmentPropertiesEXT) {
#ifdef VK_EXT_texel_buffer_alignment
    bool supported = false;
    supported = supported && IsSupported(gpu_profile, "VK_EXT_texel_buffer_alignment");

    VkPhysicalDeviceTexelBufferAlignmentPropertiesEXT texel_buffer_alignment_properties_native{};
    texel_buffer_alignment_properties_native.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TEXEL_BUFFER_ALIGNMENT_PROPERTIES_EXT;

    VkPhysicalDeviceProperties2 gpu_props_native{};
    gpu_props_native.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props_native.pNext = &texel_buffer_alignment_properties_native;
    vkGetPhysicalDeviceProperties2(gpu_native, &gpu_props_native);

    VkPhysicalDeviceTexelBufferAlignmentPropertiesEXT texel_buffer_alignment_properties_profile{};
    texel_buffer_alignment_properties_profile.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TEXEL_BUFFER_ALIGNMENT_PROPERTIES_EXT;

    VkPhysicalDeviceProperties2 gpu_props_profile{};
    gpu_props_profile.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props_profile.pNext = &texel_buffer_alignment_properties_profile;
    vkGetPhysicalDeviceProperties2(gpu_profile, &gpu_props_profile);

    EXPECT_EQ(texel_buffer_alignment_properties_profile.storageTexelBufferOffsetAlignmentBytes, 167);
    if (supported) {
    EXPECT_EQ(texel_buffer_alignment_properties_profile.storageTexelBufferOffsetSingleTexelAlignment, texel_buffer_alignment_properties_native.storageTexelBufferOffsetSingleTexelAlignment);
    }

    EXPECT_EQ(texel_buffer_alignment_properties_profile.uniformTexelBufferOffsetAlignmentBytes, 168);
    if (supported) {
    EXPECT_EQ(texel_buffer_alignment_properties_profile.uniformTexelBufferOffsetSingleTexelAlignment, texel_buffer_alignment_properties_native.uniformTexelBufferOffsetSingleTexelAlignment);
    }

#endif
}

TEST_F(TestsCapabilitiesGenerated, TestSubgroupSizeControlPropertiesEXT) {
#ifdef VK_EXT_subgroup_size_control
    bool supported = false;
    supported = supported && IsSupported(gpu_profile, "VK_EXT_subgroup_size_control");

    VkPhysicalDeviceSubgroupSizeControlPropertiesEXT subgroup_size_control_properties_native{};
    subgroup_size_control_properties_native.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SUBGROUP_SIZE_CONTROL_PROPERTIES_EXT;

    VkPhysicalDeviceProperties2 gpu_props_native{};
    gpu_props_native.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props_native.pNext = &subgroup_size_control_properties_native;
    vkGetPhysicalDeviceProperties2(gpu_native, &gpu_props_native);

    VkPhysicalDeviceSubgroupSizeControlPropertiesEXT subgroup_size_control_properties_profile{};
    subgroup_size_control_properties_profile.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SUBGROUP_SIZE_CONTROL_PROPERTIES_EXT;

    VkPhysicalDeviceProperties2 gpu_props_profile{};
    gpu_props_profile.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props_profile.pNext = &subgroup_size_control_properties_profile;
    vkGetPhysicalDeviceProperties2(gpu_profile, &gpu_props_profile);

    EXPECT_EQ(subgroup_size_control_properties_profile.minSubgroupSize, 169);
    EXPECT_EQ(subgroup_size_control_properties_profile.maxSubgroupSize, 170);
    EXPECT_EQ(subgroup_size_control_properties_profile.maxComputeWorkgroupSubgroups, 171);
    EXPECT_EQ(subgroup_size_control_properties_profile.requiredSubgroupSizeStages, VK_SHADER_STAGE_CALLABLE_BIT_NV);
#endif
}

TEST_F(TestsCapabilitiesGenerated, TestSubpassShadingPropertiesHUAWEI) {
#ifdef VK_HUAWEI_subpass_shading
    bool supported = false;
    supported = supported && IsSupported(gpu_profile, "VK_HUAWEI_subpass_shading");

    VkPhysicalDeviceSubpassShadingPropertiesHUAWEI subpass_shading_properties_native{};
    subpass_shading_properties_native.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SUBPASS_SHADING_PROPERTIES_HUAWEI;

    VkPhysicalDeviceProperties2 gpu_props_native{};
    gpu_props_native.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props_native.pNext = &subpass_shading_properties_native;
    vkGetPhysicalDeviceProperties2(gpu_native, &gpu_props_native);

    VkPhysicalDeviceSubpassShadingPropertiesHUAWEI subpass_shading_properties_profile{};
    subpass_shading_properties_profile.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SUBPASS_SHADING_PROPERTIES_HUAWEI;

    VkPhysicalDeviceProperties2 gpu_props_profile{};
    gpu_props_profile.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props_profile.pNext = &subpass_shading_properties_profile;
    vkGetPhysicalDeviceProperties2(gpu_profile, &gpu_props_profile);

    EXPECT_EQ(subpass_shading_properties_profile.maxSubpassShadingWorkgroupSizeAspectRatio, 173);
#endif
}

TEST_F(TestsCapabilitiesGenerated, TestLineRasterizationPropertiesEXT) {
#ifdef VK_EXT_line_rasterization
    bool supported = false;
    supported = supported && IsSupported(gpu_profile, "VK_EXT_line_rasterization");

    VkPhysicalDeviceLineRasterizationPropertiesEXT line_rasterization_properties_native{};
    line_rasterization_properties_native.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_LINE_RASTERIZATION_PROPERTIES_EXT;

    VkPhysicalDeviceProperties2 gpu_props_native{};
    gpu_props_native.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props_native.pNext = &line_rasterization_properties_native;
    vkGetPhysicalDeviceProperties2(gpu_native, &gpu_props_native);

    VkPhysicalDeviceLineRasterizationPropertiesEXT line_rasterization_properties_profile{};
    line_rasterization_properties_profile.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_LINE_RASTERIZATION_PROPERTIES_EXT;

    VkPhysicalDeviceProperties2 gpu_props_profile{};
    gpu_props_profile.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props_profile.pNext = &line_rasterization_properties_profile;
    vkGetPhysicalDeviceProperties2(gpu_profile, &gpu_props_profile);

    EXPECT_EQ(line_rasterization_properties_profile.lineSubPixelPrecisionBits, 174);
#endif
}

TEST_F(TestsCapabilitiesGenerated, TestCustomBorderColorPropertiesEXT) {
#ifdef VK_EXT_custom_border_color
    bool supported = false;
    supported = supported && IsSupported(gpu_profile, "VK_EXT_custom_border_color");

    VkPhysicalDeviceCustomBorderColorPropertiesEXT custom_border_color_properties_native{};
    custom_border_color_properties_native.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_CUSTOM_BORDER_COLOR_PROPERTIES_EXT;

    VkPhysicalDeviceProperties2 gpu_props_native{};
    gpu_props_native.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props_native.pNext = &custom_border_color_properties_native;
    vkGetPhysicalDeviceProperties2(gpu_native, &gpu_props_native);

    VkPhysicalDeviceCustomBorderColorPropertiesEXT custom_border_color_properties_profile{};
    custom_border_color_properties_profile.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_CUSTOM_BORDER_COLOR_PROPERTIES_EXT;

    VkPhysicalDeviceProperties2 gpu_props_profile{};
    gpu_props_profile.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props_profile.pNext = &custom_border_color_properties_profile;
    vkGetPhysicalDeviceProperties2(gpu_profile, &gpu_props_profile);

    EXPECT_EQ(custom_border_color_properties_profile.maxCustomBorderColorSamplers, 175);
#endif
}

TEST_F(TestsCapabilitiesGenerated, TestExtendedDynamicState3PropertiesEXT) {
#ifdef VK_EXT_extended_dynamic_state3
    bool supported = false;
    supported = supported && IsSupported(gpu_profile, "VK_EXT_extended_dynamic_state3");

    VkPhysicalDeviceExtendedDynamicState3PropertiesEXT extended_dynamic_state_3_properties_native{};
    extended_dynamic_state_3_properties_native.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTENDED_DYNAMIC_STATE_3_PROPERTIES_EXT;

    VkPhysicalDeviceProperties2 gpu_props_native{};
    gpu_props_native.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props_native.pNext = &extended_dynamic_state_3_properties_native;
    vkGetPhysicalDeviceProperties2(gpu_native, &gpu_props_native);

    VkPhysicalDeviceExtendedDynamicState3PropertiesEXT extended_dynamic_state_3_properties_profile{};
    extended_dynamic_state_3_properties_profile.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTENDED_DYNAMIC_STATE_3_PROPERTIES_EXT;

    VkPhysicalDeviceProperties2 gpu_props_profile{};
    gpu_props_profile.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props_profile.pNext = &extended_dynamic_state_3_properties_profile;
    vkGetPhysicalDeviceProperties2(gpu_profile, &gpu_props_profile);

    EXPECT_EQ(extended_dynamic_state_3_properties_profile.dynamicPrimitiveTopologyUnrestricted, VK_TRUE);
#endif
}

TEST_F(TestsCapabilitiesGenerated, TestRobustness2PropertiesEXT) {
#ifdef VK_EXT_robustness2
    bool supported = false;
    supported = supported && IsSupported(gpu_profile, "VK_EXT_robustness2");

    VkPhysicalDeviceRobustness2PropertiesEXT robustness_2_properties_native{};
    robustness_2_properties_native.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ROBUSTNESS_2_PROPERTIES_EXT;

    VkPhysicalDeviceProperties2 gpu_props_native{};
    gpu_props_native.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props_native.pNext = &robustness_2_properties_native;
    vkGetPhysicalDeviceProperties2(gpu_native, &gpu_props_native);

    VkPhysicalDeviceRobustness2PropertiesEXT robustness_2_properties_profile{};
    robustness_2_properties_profile.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ROBUSTNESS_2_PROPERTIES_EXT;

    VkPhysicalDeviceProperties2 gpu_props_profile{};
    gpu_props_profile.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props_profile.pNext = &robustness_2_properties_profile;
    vkGetPhysicalDeviceProperties2(gpu_profile, &gpu_props_profile);

    EXPECT_EQ(robustness_2_properties_profile.robustStorageBufferAccessSizeAlignment, 176);
    EXPECT_EQ(robustness_2_properties_profile.robustUniformBufferAccessSizeAlignment, 177);
#endif
}

TEST_F(TestsCapabilitiesGenerated, TestPortabilitySubsetPropertiesKHR) {
#ifdef VK_KHR_portability_subset
    bool supported = false;
    supported = supported && IsSupported(gpu_profile, "VK_KHR_portability_subset");

    VkPhysicalDevicePortabilitySubsetPropertiesKHR portability_subset_properties_native{};
    portability_subset_properties_native.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PORTABILITY_SUBSET_PROPERTIES_KHR;

    VkPhysicalDeviceProperties2 gpu_props_native{};
    gpu_props_native.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props_native.pNext = &portability_subset_properties_native;
    vkGetPhysicalDeviceProperties2(gpu_native, &gpu_props_native);

    VkPhysicalDevicePortabilitySubsetPropertiesKHR portability_subset_properties_profile{};
    portability_subset_properties_profile.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PORTABILITY_SUBSET_PROPERTIES_KHR;

    VkPhysicalDeviceProperties2 gpu_props_profile{};
    gpu_props_profile.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props_profile.pNext = &portability_subset_properties_profile;
    vkGetPhysicalDeviceProperties2(gpu_profile, &gpu_props_profile);

    EXPECT_EQ(portability_subset_properties_profile.minVertexInputBindingStrideAlignment, 178);
#endif
}

TEST_F(TestsCapabilitiesGenerated, TestFragmentShadingRatePropertiesKHR) {
#ifdef VK_KHR_fragment_shading_rate
    bool supported = false;
    supported = supported && IsSupported(gpu_profile, "VK_KHR_fragment_shading_rate");

    VkPhysicalDeviceFragmentShadingRatePropertiesKHR fragment_shading_rate_properties_native{};
    fragment_shading_rate_properties_native.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_SHADING_RATE_PROPERTIES_KHR;

    VkPhysicalDeviceProperties2 gpu_props_native{};
    gpu_props_native.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props_native.pNext = &fragment_shading_rate_properties_native;
    vkGetPhysicalDeviceProperties2(gpu_native, &gpu_props_native);

    VkPhysicalDeviceFragmentShadingRatePropertiesKHR fragment_shading_rate_properties_profile{};
    fragment_shading_rate_properties_profile.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_SHADING_RATE_PROPERTIES_KHR;

    VkPhysicalDeviceProperties2 gpu_props_profile{};
    gpu_props_profile.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props_profile.pNext = &fragment_shading_rate_properties_profile;
    vkGetPhysicalDeviceProperties2(gpu_profile, &gpu_props_profile);

    EXPECT_EQ(fragment_shading_rate_properties_profile.minFragmentShadingRateAttachmentTexelSize.width, 179);
    EXPECT_EQ(fragment_shading_rate_properties_profile.minFragmentShadingRateAttachmentTexelSize.height, 180);
    EXPECT_EQ(fragment_shading_rate_properties_profile.maxFragmentShadingRateAttachmentTexelSize.width, 181);
    EXPECT_EQ(fragment_shading_rate_properties_profile.maxFragmentShadingRateAttachmentTexelSize.height, 182);
    EXPECT_EQ(fragment_shading_rate_properties_profile.maxFragmentShadingRateAttachmentTexelSizeAspectRatio, 183);
    EXPECT_EQ(fragment_shading_rate_properties_profile.primitiveFragmentShadingRateWithMultipleViewports, VK_TRUE);
    EXPECT_EQ(fragment_shading_rate_properties_profile.layeredShadingRateAttachments, VK_TRUE);
    EXPECT_EQ(fragment_shading_rate_properties_profile.fragmentShadingRateNonTrivialCombinerOps, VK_TRUE);
    EXPECT_EQ(fragment_shading_rate_properties_profile.maxFragmentSize.width, 184);
    EXPECT_EQ(fragment_shading_rate_properties_profile.maxFragmentSize.height, 185);
    EXPECT_EQ(fragment_shading_rate_properties_profile.maxFragmentSizeAspectRatio, 186);
    EXPECT_EQ(fragment_shading_rate_properties_profile.maxFragmentShadingRateCoverageSamples, 187);
    EXPECT_EQ(fragment_shading_rate_properties_profile.maxFragmentShadingRateRasterizationSamples, VK_SAMPLE_COUNT_64_BIT);
    EXPECT_EQ(fragment_shading_rate_properties_profile.fragmentShadingRateWithShaderDepthStencilWrites, VK_TRUE);
    EXPECT_EQ(fragment_shading_rate_properties_profile.fragmentShadingRateWithSampleMask, VK_TRUE);
    EXPECT_EQ(fragment_shading_rate_properties_profile.fragmentShadingRateWithShaderSampleMask, VK_TRUE);
    EXPECT_EQ(fragment_shading_rate_properties_profile.fragmentShadingRateWithConservativeRasterization, VK_TRUE);
    EXPECT_EQ(fragment_shading_rate_properties_profile.fragmentShadingRateWithFragmentShaderInterlock, VK_TRUE);
    EXPECT_EQ(fragment_shading_rate_properties_profile.fragmentShadingRateWithCustomSampleLocations, VK_TRUE);
    EXPECT_EQ(fragment_shading_rate_properties_profile.fragmentShadingRateStrictMultiplyCombiner, VK_TRUE);
#endif
}

TEST_F(TestsCapabilitiesGenerated, TestFragmentShadingRateEnumsPropertiesNV) {
#ifdef VK_NV_fragment_shading_rate_enums
    bool supported = false;
    supported = supported && IsSupported(gpu_profile, "VK_NV_fragment_shading_rate_enums");

    VkPhysicalDeviceFragmentShadingRateEnumsPropertiesNV fragment_shading_rate_enums_properties_native{};
    fragment_shading_rate_enums_properties_native.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_SHADING_RATE_ENUMS_PROPERTIES_NV;

    VkPhysicalDeviceProperties2 gpu_props_native{};
    gpu_props_native.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props_native.pNext = &fragment_shading_rate_enums_properties_native;
    vkGetPhysicalDeviceProperties2(gpu_native, &gpu_props_native);

    VkPhysicalDeviceFragmentShadingRateEnumsPropertiesNV fragment_shading_rate_enums_properties_profile{};
    fragment_shading_rate_enums_properties_profile.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_SHADING_RATE_ENUMS_PROPERTIES_NV;

    VkPhysicalDeviceProperties2 gpu_props_profile{};
    gpu_props_profile.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props_profile.pNext = &fragment_shading_rate_enums_properties_profile;
    vkGetPhysicalDeviceProperties2(gpu_profile, &gpu_props_profile);

    EXPECT_EQ(fragment_shading_rate_enums_properties_profile.maxFragmentShadingRateInvocationCount, VK_SAMPLE_COUNT_1_BIT);
#endif
}

TEST_F(TestsCapabilitiesGenerated, TestProvokingVertexPropertiesEXT) {
#ifdef VK_EXT_provoking_vertex
    bool supported = false;
    supported = supported && IsSupported(gpu_profile, "VK_EXT_provoking_vertex");

    VkPhysicalDeviceProvokingVertexPropertiesEXT provoking_vertex_properties_native{};
    provoking_vertex_properties_native.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROVOKING_VERTEX_PROPERTIES_EXT;

    VkPhysicalDeviceProperties2 gpu_props_native{};
    gpu_props_native.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props_native.pNext = &provoking_vertex_properties_native;
    vkGetPhysicalDeviceProperties2(gpu_native, &gpu_props_native);

    VkPhysicalDeviceProvokingVertexPropertiesEXT provoking_vertex_properties_profile{};
    provoking_vertex_properties_profile.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROVOKING_VERTEX_PROPERTIES_EXT;

    VkPhysicalDeviceProperties2 gpu_props_profile{};
    gpu_props_profile.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props_profile.pNext = &provoking_vertex_properties_profile;
    vkGetPhysicalDeviceProperties2(gpu_profile, &gpu_props_profile);

    EXPECT_EQ(provoking_vertex_properties_profile.provokingVertexModePerPipeline, VK_TRUE);
    EXPECT_EQ(provoking_vertex_properties_profile.transformFeedbackPreservesTriangleFanProvokingVertex, VK_TRUE);
#endif
}

TEST_F(TestsCapabilitiesGenerated, TestShaderIntegerDotProductPropertiesKHR) {
#ifdef VK_KHR_shader_integer_dot_product
    bool supported = false;
    supported = supported && IsSupported(gpu_profile, "VK_KHR_shader_integer_dot_product");

    VkPhysicalDeviceShaderIntegerDotProductPropertiesKHR shader_integer_dot_product_properties_native{};
    shader_integer_dot_product_properties_native.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_INTEGER_DOT_PRODUCT_PROPERTIES_KHR;

    VkPhysicalDeviceProperties2 gpu_props_native{};
    gpu_props_native.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props_native.pNext = &shader_integer_dot_product_properties_native;
    vkGetPhysicalDeviceProperties2(gpu_native, &gpu_props_native);

    VkPhysicalDeviceShaderIntegerDotProductPropertiesKHR shader_integer_dot_product_properties_profile{};
    shader_integer_dot_product_properties_profile.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_INTEGER_DOT_PRODUCT_PROPERTIES_KHR;

    VkPhysicalDeviceProperties2 gpu_props_profile{};
    gpu_props_profile.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props_profile.pNext = &shader_integer_dot_product_properties_profile;
    vkGetPhysicalDeviceProperties2(gpu_profile, &gpu_props_profile);

    EXPECT_EQ(shader_integer_dot_product_properties_profile.integerDotProduct8BitUnsignedAccelerated, VK_TRUE);
    EXPECT_EQ(shader_integer_dot_product_properties_profile.integerDotProduct8BitSignedAccelerated, VK_TRUE);
    EXPECT_EQ(shader_integer_dot_product_properties_profile.integerDotProduct8BitMixedSignednessAccelerated, VK_TRUE);
    EXPECT_EQ(shader_integer_dot_product_properties_profile.integerDotProduct4x8BitPackedUnsignedAccelerated, VK_TRUE);
    EXPECT_EQ(shader_integer_dot_product_properties_profile.integerDotProduct4x8BitPackedSignedAccelerated, VK_TRUE);
    EXPECT_EQ(shader_integer_dot_product_properties_profile.integerDotProduct4x8BitPackedMixedSignednessAccelerated, VK_TRUE);
    EXPECT_EQ(shader_integer_dot_product_properties_profile.integerDotProduct16BitUnsignedAccelerated, VK_TRUE);
    EXPECT_EQ(shader_integer_dot_product_properties_profile.integerDotProduct16BitSignedAccelerated, VK_TRUE);
    EXPECT_EQ(shader_integer_dot_product_properties_profile.integerDotProduct16BitMixedSignednessAccelerated, VK_TRUE);
    EXPECT_EQ(shader_integer_dot_product_properties_profile.integerDotProduct32BitUnsignedAccelerated, VK_TRUE);
    EXPECT_EQ(shader_integer_dot_product_properties_profile.integerDotProduct32BitSignedAccelerated, VK_TRUE);
    EXPECT_EQ(shader_integer_dot_product_properties_profile.integerDotProduct32BitMixedSignednessAccelerated, VK_TRUE);
    EXPECT_EQ(shader_integer_dot_product_properties_profile.integerDotProduct64BitUnsignedAccelerated, VK_TRUE);
    EXPECT_EQ(shader_integer_dot_product_properties_profile.integerDotProduct64BitSignedAccelerated, VK_TRUE);
    EXPECT_EQ(shader_integer_dot_product_properties_profile.integerDotProduct64BitMixedSignednessAccelerated, VK_TRUE);
    EXPECT_EQ(shader_integer_dot_product_properties_profile.integerDotProductAccumulatingSaturating8BitUnsignedAccelerated, VK_TRUE);
    EXPECT_EQ(shader_integer_dot_product_properties_profile.integerDotProductAccumulatingSaturating8BitSignedAccelerated, VK_TRUE);
    EXPECT_EQ(shader_integer_dot_product_properties_profile.integerDotProductAccumulatingSaturating8BitMixedSignednessAccelerated, VK_TRUE);
    EXPECT_EQ(shader_integer_dot_product_properties_profile.integerDotProductAccumulatingSaturating4x8BitPackedUnsignedAccelerated, VK_TRUE);
    EXPECT_EQ(shader_integer_dot_product_properties_profile.integerDotProductAccumulatingSaturating4x8BitPackedSignedAccelerated, VK_TRUE);
    EXPECT_EQ(shader_integer_dot_product_properties_profile.integerDotProductAccumulatingSaturating4x8BitPackedMixedSignednessAccelerated, VK_TRUE);
    EXPECT_EQ(shader_integer_dot_product_properties_profile.integerDotProductAccumulatingSaturating16BitUnsignedAccelerated, VK_TRUE);
    EXPECT_EQ(shader_integer_dot_product_properties_profile.integerDotProductAccumulatingSaturating16BitSignedAccelerated, VK_TRUE);
    EXPECT_EQ(shader_integer_dot_product_properties_profile.integerDotProductAccumulatingSaturating16BitMixedSignednessAccelerated, VK_TRUE);
    EXPECT_EQ(shader_integer_dot_product_properties_profile.integerDotProductAccumulatingSaturating32BitUnsignedAccelerated, VK_TRUE);
    EXPECT_EQ(shader_integer_dot_product_properties_profile.integerDotProductAccumulatingSaturating32BitSignedAccelerated, VK_TRUE);
    EXPECT_EQ(shader_integer_dot_product_properties_profile.integerDotProductAccumulatingSaturating32BitMixedSignednessAccelerated, VK_TRUE);
    EXPECT_EQ(shader_integer_dot_product_properties_profile.integerDotProductAccumulatingSaturating64BitUnsignedAccelerated, VK_TRUE);
    EXPECT_EQ(shader_integer_dot_product_properties_profile.integerDotProductAccumulatingSaturating64BitSignedAccelerated, VK_TRUE);
    EXPECT_EQ(shader_integer_dot_product_properties_profile.integerDotProductAccumulatingSaturating64BitMixedSignednessAccelerated, VK_TRUE);
#endif
}

TEST_F(TestsCapabilitiesGenerated, TestDrmPropertiesEXT) {
#ifdef VK_EXT_physical_device_drm
    bool supported = false;
    supported = supported && IsSupported(gpu_profile, "VK_EXT_physical_device_drm");

    VkPhysicalDeviceDrmPropertiesEXT drm_properties_native{};
    drm_properties_native.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DRM_PROPERTIES_EXT;

    VkPhysicalDeviceProperties2 gpu_props_native{};
    gpu_props_native.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props_native.pNext = &drm_properties_native;
    vkGetPhysicalDeviceProperties2(gpu_native, &gpu_props_native);

    VkPhysicalDeviceDrmPropertiesEXT drm_properties_profile{};
    drm_properties_profile.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DRM_PROPERTIES_EXT;

    VkPhysicalDeviceProperties2 gpu_props_profile{};
    gpu_props_profile.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props_profile.pNext = &drm_properties_profile;
    vkGetPhysicalDeviceProperties2(gpu_profile, &gpu_props_profile);

    if (supported) {
    EXPECT_EQ(drm_properties_profile.hasPrimary, drm_properties_native.hasPrimary);
    }

    if (supported) {
    EXPECT_EQ(drm_properties_profile.hasRender, drm_properties_native.hasRender);
    }

    if (supported) {
    EXPECT_EQ(drm_properties_profile.primaryMajor, drm_properties_native.primaryMajor);
    }

    if (supported) {
    EXPECT_EQ(drm_properties_profile.primaryMinor, drm_properties_native.primaryMinor);
    }

    if (supported) {
    EXPECT_EQ(drm_properties_profile.renderMajor, drm_properties_native.renderMajor);
    }

    if (supported) {
    EXPECT_EQ(drm_properties_profile.renderMinor, drm_properties_native.renderMinor);
    }

#endif
}

TEST_F(TestsCapabilitiesGenerated, TestFragmentShaderBarycentricPropertiesKHR) {
#ifdef VK_KHR_fragment_shader_barycentric
    bool supported = false;
    supported = supported && IsSupported(gpu_profile, "VK_KHR_fragment_shader_barycentric");

    VkPhysicalDeviceFragmentShaderBarycentricPropertiesKHR fragment_shader_barycentric_properties_native{};
    fragment_shader_barycentric_properties_native.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_SHADER_BARYCENTRIC_PROPERTIES_KHR;

    VkPhysicalDeviceProperties2 gpu_props_native{};
    gpu_props_native.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props_native.pNext = &fragment_shader_barycentric_properties_native;
    vkGetPhysicalDeviceProperties2(gpu_native, &gpu_props_native);

    VkPhysicalDeviceFragmentShaderBarycentricPropertiesKHR fragment_shader_barycentric_properties_profile{};
    fragment_shader_barycentric_properties_profile.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_SHADER_BARYCENTRIC_PROPERTIES_KHR;

    VkPhysicalDeviceProperties2 gpu_props_profile{};
    gpu_props_profile.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props_profile.pNext = &fragment_shader_barycentric_properties_profile;
    vkGetPhysicalDeviceProperties2(gpu_profile, &gpu_props_profile);

    EXPECT_EQ(fragment_shader_barycentric_properties_profile.triStripVertexOrderIndependentOfProvokingVertex, VK_TRUE);
#endif
}

TEST_F(TestsCapabilitiesGenerated, TestGraphicsPipelineLibraryPropertiesEXT) {
#ifdef VK_EXT_graphics_pipeline_library
    bool supported = false;
    supported = supported && IsSupported(gpu_profile, "VK_EXT_graphics_pipeline_library");

    VkPhysicalDeviceGraphicsPipelineLibraryPropertiesEXT graphics_pipeline_library_properties_native{};
    graphics_pipeline_library_properties_native.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_GRAPHICS_PIPELINE_LIBRARY_PROPERTIES_EXT;

    VkPhysicalDeviceProperties2 gpu_props_native{};
    gpu_props_native.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props_native.pNext = &graphics_pipeline_library_properties_native;
    vkGetPhysicalDeviceProperties2(gpu_native, &gpu_props_native);

    VkPhysicalDeviceGraphicsPipelineLibraryPropertiesEXT graphics_pipeline_library_properties_profile{};
    graphics_pipeline_library_properties_profile.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_GRAPHICS_PIPELINE_LIBRARY_PROPERTIES_EXT;

    VkPhysicalDeviceProperties2 gpu_props_profile{};
    gpu_props_profile.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props_profile.pNext = &graphics_pipeline_library_properties_profile;
    vkGetPhysicalDeviceProperties2(gpu_profile, &gpu_props_profile);

    EXPECT_EQ(graphics_pipeline_library_properties_profile.graphicsPipelineLibraryFastLinking, VK_TRUE);
    EXPECT_EQ(graphics_pipeline_library_properties_profile.graphicsPipelineLibraryIndependentInterpolationDecoration, VK_TRUE);
#endif
}

TEST_F(TestsCapabilitiesGenerated, TestShaderModuleIdentifierPropertiesEXT) {
#ifdef VK_EXT_shader_module_identifier
    bool supported = false;
    supported = supported && IsSupported(gpu_profile, "VK_EXT_shader_module_identifier");

    VkPhysicalDeviceShaderModuleIdentifierPropertiesEXT shader_module_identifier_properties_native{};
    shader_module_identifier_properties_native.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_MODULE_IDENTIFIER_PROPERTIES_EXT;

    VkPhysicalDeviceProperties2 gpu_props_native{};
    gpu_props_native.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props_native.pNext = &shader_module_identifier_properties_native;
    vkGetPhysicalDeviceProperties2(gpu_native, &gpu_props_native);

    VkPhysicalDeviceShaderModuleIdentifierPropertiesEXT shader_module_identifier_properties_profile{};
    shader_module_identifier_properties_profile.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_MODULE_IDENTIFIER_PROPERTIES_EXT;

    VkPhysicalDeviceProperties2 gpu_props_profile{};
    gpu_props_profile.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props_profile.pNext = &shader_module_identifier_properties_profile;
    vkGetPhysicalDeviceProperties2(gpu_profile, &gpu_props_profile);

    if (supported) {
    EXPECT_EQ(shader_module_identifier_properties_profile.shaderModuleIdentifierAlgorithmUUID[0], shader_module_identifier_properties_native.shaderModuleIdentifierAlgorithmUUID[0]);
    EXPECT_EQ(shader_module_identifier_properties_profile.shaderModuleIdentifierAlgorithmUUID[1], shader_module_identifier_properties_native.shaderModuleIdentifierAlgorithmUUID[1]);
    EXPECT_EQ(shader_module_identifier_properties_profile.shaderModuleIdentifierAlgorithmUUID[2], shader_module_identifier_properties_native.shaderModuleIdentifierAlgorithmUUID[2]);
    EXPECT_EQ(shader_module_identifier_properties_profile.shaderModuleIdentifierAlgorithmUUID[3], shader_module_identifier_properties_native.shaderModuleIdentifierAlgorithmUUID[3]);
    EXPECT_EQ(shader_module_identifier_properties_profile.shaderModuleIdentifierAlgorithmUUID[4], shader_module_identifier_properties_native.shaderModuleIdentifierAlgorithmUUID[4]);
    EXPECT_EQ(shader_module_identifier_properties_profile.shaderModuleIdentifierAlgorithmUUID[5], shader_module_identifier_properties_native.shaderModuleIdentifierAlgorithmUUID[5]);
    EXPECT_EQ(shader_module_identifier_properties_profile.shaderModuleIdentifierAlgorithmUUID[6], shader_module_identifier_properties_native.shaderModuleIdentifierAlgorithmUUID[6]);
    EXPECT_EQ(shader_module_identifier_properties_profile.shaderModuleIdentifierAlgorithmUUID[7], shader_module_identifier_properties_native.shaderModuleIdentifierAlgorithmUUID[7]);
    EXPECT_EQ(shader_module_identifier_properties_profile.shaderModuleIdentifierAlgorithmUUID[8], shader_module_identifier_properties_native.shaderModuleIdentifierAlgorithmUUID[8]);
    EXPECT_EQ(shader_module_identifier_properties_profile.shaderModuleIdentifierAlgorithmUUID[9], shader_module_identifier_properties_native.shaderModuleIdentifierAlgorithmUUID[9]);
    EXPECT_EQ(shader_module_identifier_properties_profile.shaderModuleIdentifierAlgorithmUUID[10], shader_module_identifier_properties_native.shaderModuleIdentifierAlgorithmUUID[10]);
    EXPECT_EQ(shader_module_identifier_properties_profile.shaderModuleIdentifierAlgorithmUUID[11], shader_module_identifier_properties_native.shaderModuleIdentifierAlgorithmUUID[11]);
    EXPECT_EQ(shader_module_identifier_properties_profile.shaderModuleIdentifierAlgorithmUUID[12], shader_module_identifier_properties_native.shaderModuleIdentifierAlgorithmUUID[12]);
    EXPECT_EQ(shader_module_identifier_properties_profile.shaderModuleIdentifierAlgorithmUUID[13], shader_module_identifier_properties_native.shaderModuleIdentifierAlgorithmUUID[13]);
    EXPECT_EQ(shader_module_identifier_properties_profile.shaderModuleIdentifierAlgorithmUUID[14], shader_module_identifier_properties_native.shaderModuleIdentifierAlgorithmUUID[14]);
    EXPECT_EQ(shader_module_identifier_properties_profile.shaderModuleIdentifierAlgorithmUUID[15], shader_module_identifier_properties_native.shaderModuleIdentifierAlgorithmUUID[15]);
    }

#endif
}

TEST_F(TestsCapabilitiesGenerated, TestOpacityMicromapPropertiesEXT) {
#ifdef VK_EXT_opacity_micromap
    bool supported = false;
    supported = supported && IsSupported(gpu_profile, "VK_EXT_opacity_micromap");

    VkPhysicalDeviceOpacityMicromapPropertiesEXT opacity_micromap_properties_native{};
    opacity_micromap_properties_native.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_OPACITY_MICROMAP_PROPERTIES_EXT;

    VkPhysicalDeviceProperties2 gpu_props_native{};
    gpu_props_native.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props_native.pNext = &opacity_micromap_properties_native;
    vkGetPhysicalDeviceProperties2(gpu_native, &gpu_props_native);

    VkPhysicalDeviceOpacityMicromapPropertiesEXT opacity_micromap_properties_profile{};
    opacity_micromap_properties_profile.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_OPACITY_MICROMAP_PROPERTIES_EXT;

    VkPhysicalDeviceProperties2 gpu_props_profile{};
    gpu_props_profile.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props_profile.pNext = &opacity_micromap_properties_profile;
    vkGetPhysicalDeviceProperties2(gpu_profile, &gpu_props_profile);

    EXPECT_EQ(opacity_micromap_properties_profile.maxOpacity2StateSubdivisionLevel, 195);
    EXPECT_EQ(opacity_micromap_properties_profile.maxOpacity4StateSubdivisionLevel, 196);
#endif
}

TEST_F(TestsCapabilitiesGenerated, TestPipelineRobustnessPropertiesEXT) {
#ifdef VK_EXT_pipeline_robustness
    bool supported = false;
    supported = supported && IsSupported(gpu_profile, "VK_EXT_pipeline_robustness");

    VkPhysicalDevicePipelineRobustnessPropertiesEXT pipeline_robustness_properties_native{};
    pipeline_robustness_properties_native.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PIPELINE_ROBUSTNESS_PROPERTIES_EXT;

    VkPhysicalDeviceProperties2 gpu_props_native{};
    gpu_props_native.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props_native.pNext = &pipeline_robustness_properties_native;
    vkGetPhysicalDeviceProperties2(gpu_native, &gpu_props_native);

    VkPhysicalDevicePipelineRobustnessPropertiesEXT pipeline_robustness_properties_profile{};
    pipeline_robustness_properties_profile.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PIPELINE_ROBUSTNESS_PROPERTIES_EXT;

    VkPhysicalDeviceProperties2 gpu_props_profile{};
    gpu_props_profile.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props_profile.pNext = &pipeline_robustness_properties_profile;
    vkGetPhysicalDeviceProperties2(gpu_profile, &gpu_props_profile);

    if (supported) {
    EXPECT_EQ(pipeline_robustness_properties_profile.defaultRobustnessStorageBuffers, pipeline_robustness_properties_native.defaultRobustnessStorageBuffers);
    }

    if (supported) {
    EXPECT_EQ(pipeline_robustness_properties_profile.defaultRobustnessUniformBuffers, pipeline_robustness_properties_native.defaultRobustnessUniformBuffers);
    }

    if (supported) {
    EXPECT_EQ(pipeline_robustness_properties_profile.defaultRobustnessVertexInputs, pipeline_robustness_properties_native.defaultRobustnessVertexInputs);
    }

    if (supported) {
    EXPECT_EQ(pipeline_robustness_properties_profile.defaultRobustnessImages, pipeline_robustness_properties_native.defaultRobustnessImages);
    }

#endif
}

TEST_F(TestsCapabilitiesGenerated, TestImageProcessingPropertiesQCOM) {
#ifdef VK_QCOM_image_processing
    bool supported = false;
    supported = supported && IsSupported(gpu_profile, "VK_QCOM_image_processing");

    VkPhysicalDeviceImageProcessingPropertiesQCOM image_processing_properties_native{};
    image_processing_properties_native.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGE_PROCESSING_PROPERTIES_QCOM;

    VkPhysicalDeviceProperties2 gpu_props_native{};
    gpu_props_native.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props_native.pNext = &image_processing_properties_native;
    vkGetPhysicalDeviceProperties2(gpu_native, &gpu_props_native);

    VkPhysicalDeviceImageProcessingPropertiesQCOM image_processing_properties_profile{};
    image_processing_properties_profile.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGE_PROCESSING_PROPERTIES_QCOM;

    VkPhysicalDeviceProperties2 gpu_props_profile{};
    gpu_props_profile.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props_profile.pNext = &image_processing_properties_profile;
    vkGetPhysicalDeviceProperties2(gpu_profile, &gpu_props_profile);

    EXPECT_EQ(image_processing_properties_profile.maxWeightFilterPhases, 201);
    EXPECT_EQ(image_processing_properties_profile.maxWeightFilterDimension.width, 202);
    EXPECT_EQ(image_processing_properties_profile.maxWeightFilterDimension.height, 203);
    EXPECT_EQ(image_processing_properties_profile.maxBlockMatchRegion.width, 204);
    EXPECT_EQ(image_processing_properties_profile.maxBlockMatchRegion.height, 205);
    EXPECT_EQ(image_processing_properties_profile.maxBoxFilterBlockSize.width, 206);
    EXPECT_EQ(image_processing_properties_profile.maxBoxFilterBlockSize.height, 207);
#endif
}

TEST_F(TestsCapabilitiesGenerated, TestOpticalFlowPropertiesNV) {
#ifdef VK_NV_optical_flow
    bool supported = false;
    supported = supported && IsSupported(gpu_profile, "VK_NV_optical_flow");

    VkPhysicalDeviceOpticalFlowPropertiesNV optical_flow_properties_native{};
    optical_flow_properties_native.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_OPTICAL_FLOW_PROPERTIES_NV;

    VkPhysicalDeviceProperties2 gpu_props_native{};
    gpu_props_native.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props_native.pNext = &optical_flow_properties_native;
    vkGetPhysicalDeviceProperties2(gpu_native, &gpu_props_native);

    VkPhysicalDeviceOpticalFlowPropertiesNV optical_flow_properties_profile{};
    optical_flow_properties_profile.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_OPTICAL_FLOW_PROPERTIES_NV;

    VkPhysicalDeviceProperties2 gpu_props_profile{};
    gpu_props_profile.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props_profile.pNext = &optical_flow_properties_profile;
    vkGetPhysicalDeviceProperties2(gpu_profile, &gpu_props_profile);

    if (supported) {
    EXPECT_EQ(optical_flow_properties_profile.hintSupported, optical_flow_properties_native.hintSupported);
    }

    if (supported) {
    EXPECT_EQ(optical_flow_properties_profile.costSupported, optical_flow_properties_native.costSupported);
    }

    if (supported) {
    EXPECT_EQ(optical_flow_properties_profile.bidirectionalFlowSupported, optical_flow_properties_native.bidirectionalFlowSupported);
    }

    if (supported) {
    EXPECT_EQ(optical_flow_properties_profile.globalFlowSupported, optical_flow_properties_native.globalFlowSupported);
    }

    if (supported) {
    EXPECT_EQ(optical_flow_properties_profile.minWidth, optical_flow_properties_native.minWidth);
    }

    if (supported) {
    EXPECT_EQ(optical_flow_properties_profile.minHeight, optical_flow_properties_native.minHeight);
    }

    if (supported) {
    EXPECT_EQ(optical_flow_properties_profile.maxWidth, optical_flow_properties_native.maxWidth);
    }

    if (supported) {
    EXPECT_EQ(optical_flow_properties_profile.maxHeight, optical_flow_properties_native.maxHeight);
    }

    if (supported) {
    EXPECT_EQ(optical_flow_properties_profile.maxNumRegionsOfInterest, optical_flow_properties_native.maxNumRegionsOfInterest);
    }

#endif
}

TEST_F(TestsCapabilitiesGenerated, TestShaderCoreBuiltinsPropertiesARM) {
#ifdef VK_ARM_shader_core_builtins
    bool supported = false;
    supported = supported && IsSupported(gpu_profile, "VK_ARM_shader_core_builtins");

    VkPhysicalDeviceShaderCoreBuiltinsPropertiesARM shader_core_builtins_properties_native{};
    shader_core_builtins_properties_native.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_CORE_BUILTINS_PROPERTIES_ARM;

    VkPhysicalDeviceProperties2 gpu_props_native{};
    gpu_props_native.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props_native.pNext = &shader_core_builtins_properties_native;
    vkGetPhysicalDeviceProperties2(gpu_native, &gpu_props_native);

    VkPhysicalDeviceShaderCoreBuiltinsPropertiesARM shader_core_builtins_properties_profile{};
    shader_core_builtins_properties_profile.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_CORE_BUILTINS_PROPERTIES_ARM;

    VkPhysicalDeviceProperties2 gpu_props_profile{};
    gpu_props_profile.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    gpu_props_profile.pNext = &shader_core_builtins_properties_profile;
    vkGetPhysicalDeviceProperties2(gpu_profile, &gpu_props_profile);

    EXPECT_EQ(shader_core_builtins_properties_profile.shaderCoreCount, 213);
    EXPECT_EQ(shader_core_builtins_properties_profile.shaderWarpsPerCore, 214);
#endif
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_UNDEFINED) {
    VkFormat format = VK_FORMAT_UNDEFINED;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_MINMAX_BIT | VK_FORMAT_FEATURE_FRAGMENT_DENSITY_MAP_BIT_EXT;
    VkFormatFeatureFlags optimal_tiling_features = 0;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_TRANSFER_DST_BIT_KHR | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT_KHR | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_R4G4_UNORM_PACK8) {
    VkFormat format = VK_FORMAT_R4G4_UNORM_PACK8;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT | VK_FORMAT_FEATURE_DISJOINT_BIT_KHR;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT_KHR | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_MINMAX_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT_KHR | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_R4G4B4A4_UNORM_PACK16) {
    VkFormat format = VK_FORMAT_R4G4B4A4_UNORM_PACK16;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_TRANSFER_SRC_BIT_KHR | VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_MINMAX_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_B4G4R4A4_UNORM_PACK16) {
    VkFormat format = VK_FORMAT_B4G4R4A4_UNORM_PACK16;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = 0;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_STORAGE_IMAGE_ATOMIC_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT_KHR;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_R5G6B5_UNORM_PACK16) {
    VkFormat format = VK_FORMAT_R5G6B5_UNORM_PACK16;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT;
    VkFormatFeatureFlags optimal_tiling_features = 0;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_B5G6R5_UNORM_PACK16) {
    VkFormat format = VK_FORMAT_B5G6R5_UNORM_PACK16;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = 0;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT | VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT_KHR;
    VkFormatFeatureFlags buffer_features = 0;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_R5G5B5A1_UNORM_PACK16) {
    VkFormat format = VK_FORMAT_R5G5B5A1_UNORM_PACK16;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT_KHR | VK_FORMAT_FEATURE_DISJOINT_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_B5G5R5A1_UNORM_PACK16) {
    VkFormat format = VK_FORMAT_B5G5R5A1_UNORM_PACK16;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = 0;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT_KHR | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT_KHR;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_A1R5G5B5_UNORM_PACK16) {
    VkFormat format = VK_FORMAT_A1R5G5B5_UNORM_PACK16;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = 0;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT_KHR;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT_KHR | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_ACCELERATION_STRUCTURE_VERTEX_BUFFER_BIT_KHR | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_IMG;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_R8_UNORM) {
    VkFormat format = VK_FORMAT_R8_UNORM;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_EXT;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT;
    VkFormatFeatureFlags buffer_features = 0;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_R8_SNORM) {
    VkFormat format = VK_FORMAT_R8_SNORM;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = 0;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_ACCELERATION_STRUCTURE_VERTEX_BUFFER_BIT_KHR | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT_KHR;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_STORAGE_IMAGE_ATOMIC_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_R8_USCALED) {
    VkFormat format = VK_FORMAT_R8_USCALED;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = 0;
    VkFormatFeatureFlags optimal_tiling_features = 0;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT_KHR | VK_FORMAT_FEATURE_BLIT_SRC_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_R8_SSCALED) {
    VkFormat format = VK_FORMAT_R8_SSCALED;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = 0;
    VkFormatFeatureFlags optimal_tiling_features = 0;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_R8_UINT) {
    VkFormat format = VK_FORMAT_R8_UINT;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT_KHR | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_IMG | VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT_KHR | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_IMG | VK_FORMAT_FEATURE_STORAGE_IMAGE_ATOMIC_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_R8_SINT) {
    VkFormat format = VK_FORMAT_R8_SINT;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_DISJOINT_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT_KHR;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT_KHR | VK_FORMAT_FEATURE_TRANSFER_DST_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_R8_SRGB) {
    VkFormat format = VK_FORMAT_R8_SRGB;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT_KHR;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_DISJOINT_BIT_KHR | VK_FORMAT_FEATURE_ACCELERATION_STRUCTURE_VERTEX_BUFFER_BIT_KHR;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT_KHR | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_IMG;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_R8G8_UNORM) {
    VkFormat format = VK_FORMAT_R8G8_UNORM;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_R8G8_SNORM) {
    VkFormat format = VK_FORMAT_R8G8_SNORM;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_BLIT_SRC_BIT;
    VkFormatFeatureFlags buffer_features = 0;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_R8G8_USCALED) {
    VkFormat format = VK_FORMAT_R8G8_USCALED;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT | VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_MINMAX_BIT;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT;
    VkFormatFeatureFlags buffer_features = 0;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_R8G8_SSCALED) {
    VkFormat format = VK_FORMAT_R8G8_SSCALED;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = 0;
    VkFormatFeatureFlags optimal_tiling_features = 0;
    VkFormatFeatureFlags buffer_features = 0;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_R8G8_UINT) {
    VkFormat format = VK_FORMAT_R8G8_UINT;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_STORAGE_IMAGE_ATOMIC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT_KHR | VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT;
    VkFormatFeatureFlags optimal_tiling_features = 0;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_FORMAT_FEATURE_DISJOINT_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_R8G8_SINT) {
    VkFormat format = VK_FORMAT_R8G8_SINT;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT_KHR;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_DISJOINT_BIT_KHR | VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_MINMAX_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT_KHR;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT_KHR | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_R8G8_SRGB) {
    VkFormat format = VK_FORMAT_R8G8_SRGB;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = 0;
    VkFormatFeatureFlags optimal_tiling_features = 0;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT_KHR | VK_FORMAT_FEATURE_BLIT_DST_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_R8G8B8_UNORM) {
    VkFormat format = VK_FORMAT_R8G8B8_UNORM;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_TRANSFER_DST_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT_KHR;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT_KHR;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_R8G8B8_SNORM) {
    VkFormat format = VK_FORMAT_R8G8B8_SNORM;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = 0;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT_KHR;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT_KHR | VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT_KHR;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_R8G8B8_USCALED) {
    VkFormat format = VK_FORMAT_R8G8B8_USCALED;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_DISJOINT_BIT_KHR | VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT;
    VkFormatFeatureFlags optimal_tiling_features = 0;
    VkFormatFeatureFlags buffer_features = 0;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_R8G8B8_SSCALED) {
    VkFormat format = VK_FORMAT_R8G8B8_SSCALED;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_ACCELERATION_STRUCTURE_VERTEX_BUFFER_BIT_KHR | VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_TRANSFER_DST_BIT_KHR;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_R8G8B8_UINT) {
    VkFormat format = VK_FORMAT_R8G8B8_UINT;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = 0;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_FRAGMENT_DENSITY_MAP_BIT_EXT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_IMG;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT_KHR | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_MINMAX_BIT_EXT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_R8G8B8_SINT) {
    VkFormat format = VK_FORMAT_R8G8B8_SINT;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR | VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_TRANSFER_SRC_BIT_KHR | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT;
    VkFormatFeatureFlags buffer_features = 0;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_R8G8B8_SRGB) {
    VkFormat format = VK_FORMAT_R8G8B8_SRGB;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = 0;
    VkFormatFeatureFlags optimal_tiling_features = 0;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_TRANSFER_SRC_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_B8G8R8_UNORM) {
    VkFormat format = VK_FORMAT_B8G8R8_UNORM;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT_KHR | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT_KHR | VK_FORMAT_FEATURE_DISJOINT_BIT | VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT_KHR;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_B8G8R8_SNORM) {
    VkFormat format = VK_FORMAT_B8G8R8_SNORM;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = 0;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_B8G8R8_USCALED) {
    VkFormat format = VK_FORMAT_B8G8R8_USCALED;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_TRANSFER_SRC_BIT_KHR;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT_KHR | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT_KHR | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_MINMAX_BIT_EXT | VK_FORMAT_FEATURE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_FRAGMENT_DENSITY_MAP_BIT_EXT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT_KHR | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_MINMAX_BIT_EXT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_B8G8R8_SSCALED) {
    VkFormat format = VK_FORMAT_B8G8R8_SSCALED;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_TRANSFER_DST_BIT_KHR;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT_KHR | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_B8G8R8_UINT) {
    VkFormat format = VK_FORMAT_B8G8R8_UINT;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_MINMAX_BIT_EXT;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT_KHR | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_EXT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT_KHR | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT | VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT_KHR;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_B8G8R8_SINT) {
    VkFormat format = VK_FORMAT_B8G8R8_SINT;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_TRANSFER_DST_BIT | VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_FORMAT_FEATURE_ACCELERATION_STRUCTURE_VERTEX_BUFFER_BIT_KHR | VK_FORMAT_FEATURE_STORAGE_IMAGE_ATOMIC_BIT;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_TRANSFER_SRC_BIT_KHR;
    VkFormatFeatureFlags buffer_features = 0;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_B8G8R8_SRGB) {
    VkFormat format = VK_FORMAT_B8G8R8_SRGB;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT_KHR | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_EXT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_R8G8B8A8_UNORM) {
    VkFormat format = VK_FORMAT_R8G8B8A8_UNORM;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT_KHR | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_MINMAX_BIT | VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT;
    VkFormatFeatureFlags optimal_tiling_features = 0;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_FRAGMENT_DENSITY_MAP_BIT_EXT | VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT_KHR | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_R8G8B8A8_SNORM) {
    VkFormat format = VK_FORMAT_R8G8B8A8_SNORM;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_MINMAX_BIT_EXT;
    VkFormatFeatureFlags optimal_tiling_features = 0;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_R8G8B8A8_USCALED) {
    VkFormat format = VK_FORMAT_R8G8B8A8_USCALED;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_MINMAX_BIT_EXT;
    VkFormatFeatureFlags optimal_tiling_features = 0;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_BLIT_DST_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_R8G8B8A8_SSCALED) {
    VkFormat format = VK_FORMAT_R8G8B8A8_SSCALED;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_TRANSFER_DST_BIT_KHR | VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT | VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT_KHR;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_R8G8B8A8_UINT) {
    VkFormat format = VK_FORMAT_R8G8B8A8_UINT;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT_KHR | VK_FORMAT_FEATURE_DISJOINT_BIT | VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT_KHR;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_FRAGMENT_DENSITY_MAP_BIT_EXT | VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_R8G8B8A8_SINT) {
    VkFormat format = VK_FORMAT_R8G8B8A8_SINT;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = 0;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT_KHR;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT_KHR | VK_FORMAT_FEATURE_DISJOINT_BIT_KHR | VK_FORMAT_FEATURE_BLIT_DST_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_R8G8B8A8_SRGB) {
    VkFormat format = VK_FORMAT_R8G8B8A8_SRGB;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = 0;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_MINMAX_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_MINMAX_BIT_EXT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_B8G8R8A8_UNORM) {
    VkFormat format = VK_FORMAT_B8G8R8A8_UNORM;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = 0;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT_KHR | VK_FORMAT_FEATURE_DISJOINT_BIT_KHR | VK_FORMAT_FEATURE_ACCELERATION_STRUCTURE_VERTEX_BUFFER_BIT_KHR | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT;
    VkFormatFeatureFlags buffer_features = 0;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_B8G8R8A8_SNORM) {
    VkFormat format = VK_FORMAT_B8G8R8A8_SNORM;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT_KHR | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT_KHR;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT_KHR | VK_FORMAT_FEATURE_ACCELERATION_STRUCTURE_VERTEX_BUFFER_BIT_KHR;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_B8G8R8A8_USCALED) {
    VkFormat format = VK_FORMAT_B8G8R8A8_USCALED;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT;
    VkFormatFeatureFlags optimal_tiling_features = 0;
    VkFormatFeatureFlags buffer_features = 0;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_B8G8R8A8_SSCALED) {
    VkFormat format = VK_FORMAT_B8G8R8A8_SSCALED;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_STORAGE_IMAGE_ATOMIC_BIT;
    VkFormatFeatureFlags optimal_tiling_features = 0;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_TRANSFER_DST_BIT_KHR | VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_B8G8R8A8_UINT) {
    VkFormat format = VK_FORMAT_B8G8R8A8_UINT;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = 0;
    VkFormatFeatureFlags optimal_tiling_features = 0;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_TRANSFER_DST_BIT_KHR;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_B8G8R8A8_SINT) {
    VkFormat format = VK_FORMAT_B8G8R8A8_SINT;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT_KHR | VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT;
    VkFormatFeatureFlags optimal_tiling_features = 0;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_TRANSFER_SRC_BIT_KHR | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_B8G8R8A8_SRGB) {
    VkFormat format = VK_FORMAT_B8G8R8A8_SRGB;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = 0;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_TRANSFER_SRC_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_MINMAX_BIT_EXT | VK_FORMAT_FEATURE_FRAGMENT_DENSITY_MAP_BIT_EXT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT_KHR;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_A8B8G8R8_UNORM_PACK32) {
    VkFormat format = VK_FORMAT_A8B8G8R8_UNORM_PACK32;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT_KHR | VK_FORMAT_FEATURE_TRANSFER_DST_BIT_KHR;
    VkFormatFeatureFlags optimal_tiling_features = 0;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_A8B8G8R8_SNORM_PACK32) {
    VkFormat format = VK_FORMAT_A8B8G8R8_SNORM_PACK32;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = 0;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT;
    VkFormatFeatureFlags buffer_features = 0;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_A8B8G8R8_USCALED_PACK32) {
    VkFormat format = VK_FORMAT_A8B8G8R8_USCALED_PACK32;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_MINMAX_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_IMG | VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT_KHR;
    VkFormatFeatureFlags buffer_features = 0;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_A8B8G8R8_SSCALED_PACK32) {
    VkFormat format = VK_FORMAT_A8B8G8R8_SSCALED_PACK32;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = 0;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_MINMAX_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_IMG | VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_DISJOINT_BIT_KHR;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_A8B8G8R8_UINT_PACK32) {
    VkFormat format = VK_FORMAT_A8B8G8R8_UINT_PACK32;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT | VK_FORMAT_FEATURE_FRAGMENT_DENSITY_MAP_BIT_EXT;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_EXT | VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_FORMAT_FEATURE_FRAGMENT_DENSITY_MAP_BIT_EXT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_STORAGE_IMAGE_ATOMIC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_A8B8G8R8_SINT_PACK32) {
    VkFormat format = VK_FORMAT_A8B8G8R8_SINT_PACK32;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = 0;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_ACCELERATION_STRUCTURE_VERTEX_BUFFER_BIT_KHR | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_A8B8G8R8_SRGB_PACK32) {
    VkFormat format = VK_FORMAT_A8B8G8R8_SRGB_PACK32;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_IMG | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT_KHR | VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT_KHR;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT_KHR | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_A2R10G10B10_UNORM_PACK32) {
    VkFormat format = VK_FORMAT_A2R10G10B10_UNORM_PACK32;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_DISJOINT_BIT | VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT_KHR;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_DISJOINT_BIT_KHR | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT_KHR | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT_KHR;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_FORMAT_FEATURE_DISJOINT_BIT_KHR;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_A2R10G10B10_SNORM_PACK32) {
    VkFormat format = VK_FORMAT_A2R10G10B10_SNORM_PACK32;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_TRANSFER_SRC_BIT_KHR;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_STORAGE_IMAGE_ATOMIC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_IMG | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT_KHR;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_A2R10G10B10_USCALED_PACK32) {
    VkFormat format = VK_FORMAT_A2R10G10B10_USCALED_PACK32;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT_KHR;
    VkFormatFeatureFlags buffer_features = 0;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_A2R10G10B10_SSCALED_PACK32) {
    VkFormat format = VK_FORMAT_A2R10G10B10_SSCALED_PACK32;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_STORAGE_IMAGE_ATOMIC_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT_KHR | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT | VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT;
    VkFormatFeatureFlags optimal_tiling_features = 0;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_TRANSFER_DST_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_A2R10G10B10_UINT_PACK32) {
    VkFormat format = VK_FORMAT_A2R10G10B10_UINT_PACK32;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT_KHR;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_FORMAT_FEATURE_ACCELERATION_STRUCTURE_VERTEX_BUFFER_BIT_KHR;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_A2R10G10B10_SINT_PACK32) {
    VkFormat format = VK_FORMAT_A2R10G10B10_SINT_PACK32;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = 0;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR | VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_DISJOINT_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_A2B10G10R10_UNORM_PACK32) {
    VkFormat format = VK_FORMAT_A2B10G10R10_UNORM_PACK32;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT_KHR;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_ACCELERATION_STRUCTURE_VERTEX_BUFFER_BIT_KHR;
    VkFormatFeatureFlags buffer_features = 0;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_A2B10G10R10_SNORM_PACK32) {
    VkFormat format = VK_FORMAT_A2B10G10R10_SNORM_PACK32;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_MINMAX_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT_KHR | VK_FORMAT_FEATURE_TRANSFER_DST_BIT_KHR | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_A2B10G10R10_USCALED_PACK32) {
    VkFormat format = VK_FORMAT_A2B10G10R10_USCALED_PACK32;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT | VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT_KHR | VK_FORMAT_FEATURE_ACCELERATION_STRUCTURE_VERTEX_BUFFER_BIT_KHR | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT_KHR;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT | VK_FORMAT_FEATURE_DISJOINT_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_A2B10G10R10_SSCALED_PACK32) {
    VkFormat format = VK_FORMAT_A2B10G10R10_SSCALED_PACK32;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = 0;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_IMG | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT_KHR;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_A2B10G10R10_UINT_PACK32) {
    VkFormat format = VK_FORMAT_A2B10G10R10_UINT_PACK32;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = 0;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT | VK_FORMAT_FEATURE_DISJOINT_BIT_KHR;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_DISJOINT_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT_KHR;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_A2B10G10R10_SINT_PACK32) {
    VkFormat format = VK_FORMAT_A2B10G10R10_SINT_PACK32;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_ACCELERATION_STRUCTURE_VERTEX_BUFFER_BIT_KHR | VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT_KHR | VK_FORMAT_FEATURE_BLIT_SRC_BIT;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT_KHR | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_EXT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_R16_UNORM) {
    VkFormat format = VK_FORMAT_R16_UNORM;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT_KHR | VK_FORMAT_FEATURE_DISJOINT_BIT;
    VkFormatFeatureFlags optimal_tiling_features = 0;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT_KHR;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_R16_SNORM) {
    VkFormat format = VK_FORMAT_R16_SNORM;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT_KHR | VK_FORMAT_FEATURE_FRAGMENT_DENSITY_MAP_BIT_EXT;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_TRANSFER_DST_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_MINMAX_BIT_EXT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_R16_USCALED) {
    VkFormat format = VK_FORMAT_R16_USCALED;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT_KHR;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_EXT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_R16_SSCALED) {
    VkFormat format = VK_FORMAT_R16_SSCALED;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_ACCELERATION_STRUCTURE_VERTEX_BUFFER_BIT_KHR | VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT;
    VkFormatFeatureFlags optimal_tiling_features = 0;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_IMG | VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT_KHR;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_R16_UINT) {
    VkFormat format = VK_FORMAT_R16_UINT;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_MINMAX_BIT_EXT;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT_KHR;
    VkFormatFeatureFlags buffer_features = 0;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_R16_SINT) {
    VkFormat format = VK_FORMAT_R16_SINT;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = 0;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT_KHR;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_R16_SFLOAT) {
    VkFormat format = VK_FORMAT_R16_SFLOAT;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_TRANSFER_DST_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT_KHR | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT_KHR;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT_KHR;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_DISJOINT_BIT_KHR | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_EXT | VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_R16G16_UNORM) {
    VkFormat format = VK_FORMAT_R16G16_UNORM;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_MINMAX_BIT | VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT_KHR | VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT;
    VkFormatFeatureFlags buffer_features = 0;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_R16G16_SNORM) {
    VkFormat format = VK_FORMAT_R16G16_SNORM;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = 0;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT_KHR | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_MINMAX_BIT_EXT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT_KHR;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_TRANSFER_DST_BIT_KHR;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_R16G16_USCALED) {
    VkFormat format = VK_FORMAT_R16G16_USCALED;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = 0;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_TRANSFER_DST_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_R16G16_SSCALED) {
    VkFormat format = VK_FORMAT_R16G16_SSCALED;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT_KHR;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_DISJOINT_BIT_KHR | VK_FORMAT_FEATURE_BLIT_DST_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_R16G16_UINT) {
    VkFormat format = VK_FORMAT_R16G16_UINT;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = 0;
    VkFormatFeatureFlags optimal_tiling_features = 0;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT | VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT_KHR | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_R16G16_SINT) {
    VkFormat format = VK_FORMAT_R16G16_SINT;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT;
    VkFormatFeatureFlags optimal_tiling_features = 0;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_TRANSFER_SRC_BIT_KHR | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT | VK_FORMAT_FEATURE_ACCELERATION_STRUCTURE_VERTEX_BUFFER_BIT_KHR;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_R16G16_SFLOAT) {
    VkFormat format = VK_FORMAT_R16G16_SFLOAT;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_TRANSFER_DST_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT_KHR;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT | VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_IMG;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_R16G16B16_UNORM) {
    VkFormat format = VK_FORMAT_R16G16B16_UNORM;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_TRANSFER_DST_BIT_KHR | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT_KHR | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT_KHR | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_R16G16B16_SNORM) {
    VkFormat format = VK_FORMAT_R16G16B16_SNORM;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = 0;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_TRANSFER_DST_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_EXT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_R16G16B16_USCALED) {
    VkFormat format = VK_FORMAT_R16G16B16_USCALED;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_STORAGE_IMAGE_ATOMIC_BIT;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT | VK_FORMAT_FEATURE_DISJOINT_BIT_KHR;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_R16G16B16_SSCALED) {
    VkFormat format = VK_FORMAT_R16G16B16_SSCALED;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_DISJOINT_BIT_KHR | VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT_KHR | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_IMG | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT;
    VkFormatFeatureFlags optimal_tiling_features = 0;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT_KHR | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_MINMAX_BIT_EXT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_R16G16B16_UINT) {
    VkFormat format = VK_FORMAT_R16G16B16_UINT;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_TRANSFER_DST_BIT_KHR;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT | VK_FORMAT_FEATURE_DISJOINT_BIT_KHR;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_R16G16B16_SINT) {
    VkFormat format = VK_FORMAT_R16G16B16_SINT;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT;
    VkFormatFeatureFlags optimal_tiling_features = 0;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT_KHR;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_R16G16B16_SFLOAT) {
    VkFormat format = VK_FORMAT_R16G16B16_SFLOAT;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT | VK_FORMAT_FEATURE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_DISJOINT_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT_KHR | VK_FORMAT_FEATURE_TRANSFER_DST_BIT_KHR | VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_R16G16B16A16_UNORM) {
    VkFormat format = VK_FORMAT_R16G16B16A16_UNORM;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_ACCELERATION_STRUCTURE_VERTEX_BUFFER_BIT_KHR | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT;
    VkFormatFeatureFlags buffer_features = 0;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_R16G16B16A16_SNORM) {
    VkFormat format = VK_FORMAT_R16G16B16A16_SNORM;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = 0;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT_KHR | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_FRAGMENT_DENSITY_MAP_BIT_EXT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_R16G16B16A16_USCALED) {
    VkFormat format = VK_FORMAT_R16G16B16A16_USCALED;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_ATOMIC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_IMG | VK_FORMAT_FEATURE_BLIT_SRC_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_IMG | VK_FORMAT_FEATURE_DISJOINT_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_R16G16B16A16_SSCALED) {
    VkFormat format = VK_FORMAT_R16G16B16A16_SSCALED;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT;
    VkFormatFeatureFlags optimal_tiling_features = 0;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT_KHR;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_R16G16B16A16_UINT) {
    VkFormat format = VK_FORMAT_R16G16B16A16_UINT;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_MINMAX_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_ATOMIC_BIT | VK_FORMAT_FEATURE_DISJOINT_BIT_KHR;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_R16G16B16A16_SINT) {
    VkFormat format = VK_FORMAT_R16G16B16A16_SINT;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = 0;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT_KHR | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_MINMAX_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT_KHR | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_MINMAX_BIT_EXT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_R16G16B16A16_SFLOAT) {
    VkFormat format = VK_FORMAT_R16G16B16A16_SFLOAT;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT_KHR;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT;
    VkFormatFeatureFlags buffer_features = 0;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_R32_UINT) {
    VkFormat format = VK_FORMAT_R32_UINT;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT_KHR;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT_KHR | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_IMG;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_R32_SINT) {
    VkFormat format = VK_FORMAT_R32_SINT;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT_KHR;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_MINMAX_BIT_EXT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT | VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT | VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT | VK_FORMAT_FEATURE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_R32_SFLOAT) {
    VkFormat format = VK_FORMAT_R32_SFLOAT;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT_KHR;
    VkFormatFeatureFlags optimal_tiling_features = 0;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT_KHR | VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT | VK_FORMAT_FEATURE_DISJOINT_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_R32G32_UINT) {
    VkFormat format = VK_FORMAT_R32G32_UINT;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_R32G32_SINT) {
    VkFormat format = VK_FORMAT_R32G32_SINT;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT_KHR | VK_FORMAT_FEATURE_DISJOINT_BIT_KHR | VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_MINMAX_BIT_EXT | VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_FORMAT_FEATURE_FRAGMENT_DENSITY_MAP_BIT_EXT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_EXT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_DISJOINT_BIT_KHR;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_R32G32_SFLOAT) {
    VkFormat format = VK_FORMAT_R32G32_SFLOAT;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT_KHR;
    VkFormatFeatureFlags optimal_tiling_features = 0;
    VkFormatFeatureFlags buffer_features = 0;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_R32G32B32_UINT) {
    VkFormat format = VK_FORMAT_R32G32B32_UINT;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_IMG | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT;
    VkFormatFeatureFlags optimal_tiling_features = 0;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT_KHR | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_R32G32B32_SINT) {
    VkFormat format = VK_FORMAT_R32G32B32_SINT;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_FRAGMENT_DENSITY_MAP_BIT_EXT;
    VkFormatFeatureFlags optimal_tiling_features = 0;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_R32G32B32_SFLOAT) {
    VkFormat format = VK_FORMAT_R32G32B32_SFLOAT;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT | VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT;
    VkFormatFeatureFlags buffer_features = 0;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_R32G32B32A32_UINT) {
    VkFormat format = VK_FORMAT_R32G32B32A32_UINT;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT_KHR | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT_KHR;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT_KHR | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_IMG | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_MINMAX_BIT | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_R32G32B32A32_SINT) {
    VkFormat format = VK_FORMAT_R32G32B32A32_SINT;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT;
    VkFormatFeatureFlags optimal_tiling_features = 0;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_R32G32B32A32_SFLOAT) {
    VkFormat format = VK_FORMAT_R32G32B32A32_SFLOAT;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = 0;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT_KHR;
    VkFormatFeatureFlags buffer_features = 0;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_R64_UINT) {
    VkFormat format = VK_FORMAT_R64_UINT;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_EXT;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_DISJOINT_BIT_KHR | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT;
    VkFormatFeatureFlags buffer_features = 0;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_R64_SINT) {
    VkFormat format = VK_FORMAT_R64_SINT;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = 0;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_STORAGE_IMAGE_ATOMIC_BIT | VK_FORMAT_FEATURE_DISJOINT_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT_KHR | VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_R64_SFLOAT) {
    VkFormat format = VK_FORMAT_R64_SFLOAT;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT_KHR | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT_KHR;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT_KHR;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT_KHR;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_R64G64_UINT) {
    VkFormat format = VK_FORMAT_R64G64_UINT;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = 0;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT_KHR | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT_KHR | VK_FORMAT_FEATURE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR | VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_DISJOINT_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_R64G64_SINT) {
    VkFormat format = VK_FORMAT_R64G64_SINT;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_MINMAX_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_EXT | VK_FORMAT_FEATURE_ACCELERATION_STRUCTURE_VERTEX_BUFFER_BIT_KHR | VK_FORMAT_FEATURE_BLIT_DST_BIT;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_DISJOINT_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_R64G64_SFLOAT) {
    VkFormat format = VK_FORMAT_R64G64_SFLOAT;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = 0;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT_KHR | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT | VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_R64G64B64_UINT) {
    VkFormat format = VK_FORMAT_R64G64B64_UINT;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT_KHR;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT_KHR | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_R64G64B64_SINT) {
    VkFormat format = VK_FORMAT_R64G64B64_SINT;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_BLIT_SRC_BIT;
    VkFormatFeatureFlags optimal_tiling_features = 0;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_DISJOINT_BIT_KHR | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_R64G64B64_SFLOAT) {
    VkFormat format = VK_FORMAT_R64G64B64_SFLOAT;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_MINMAX_BIT_EXT;
    VkFormatFeatureFlags optimal_tiling_features = 0;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_R64G64B64A64_UINT) {
    VkFormat format = VK_FORMAT_R64G64B64A64_UINT;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT;
    VkFormatFeatureFlags buffer_features = 0;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_R64G64B64A64_SINT) {
    VkFormat format = VK_FORMAT_R64G64B64A64_SINT;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_TRANSFER_DST_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT_KHR | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_TRANSFER_DST_BIT_KHR | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT;
    VkFormatFeatureFlags buffer_features = 0;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_R64G64B64A64_SFLOAT) {
    VkFormat format = VK_FORMAT_R64G64B64A64_SFLOAT;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT_KHR | VK_FORMAT_FEATURE_ACCELERATION_STRUCTURE_VERTEX_BUFFER_BIT_KHR;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_DISJOINT_BIT_KHR | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT_KHR | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_MINMAX_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT | VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT | VK_FORMAT_FEATURE_DISJOINT_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_B10G11R11_UFLOAT_PACK32) {
    VkFormat format = VK_FORMAT_B10G11R11_UFLOAT_PACK32;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_EXT;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT_KHR | VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_ATOMIC_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_DISJOINT_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_E5B9G9R9_UFLOAT_PACK32) {
    VkFormat format = VK_FORMAT_E5B9G9R9_UFLOAT_PACK32;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT_KHR | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_IMG;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_MINMAX_BIT_EXT | VK_FORMAT_FEATURE_STORAGE_IMAGE_ATOMIC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT_KHR;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_D16_UNORM) {
    VkFormat format = VK_FORMAT_D16_UNORM;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = 0;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_IMG;
    VkFormatFeatureFlags buffer_features = 0;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_X8_D24_UNORM_PACK32) {
    VkFormat format = VK_FORMAT_X8_D24_UNORM_PACK32;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_DISJOINT_BIT_KHR | VK_FORMAT_FEATURE_ACCELERATION_STRUCTURE_VERTEX_BUFFER_BIT_KHR | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_MINMAX_BIT;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_TRANSFER_SRC_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_MINMAX_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_D32_SFLOAT) {
    VkFormat format = VK_FORMAT_D32_SFLOAT;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT | VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_MINMAX_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_TRANSFER_DST_BIT_KHR | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_S8_UINT) {
    VkFormat format = VK_FORMAT_S8_UINT;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = 0;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT_KHR;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT | VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_D16_UNORM_S8_UINT) {
    VkFormat format = VK_FORMAT_D16_UNORM_S8_UINT;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_MINMAX_BIT_EXT | VK_FORMAT_FEATURE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_IMG;
    VkFormatFeatureFlags buffer_features = 0;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_D24_UNORM_S8_UINT) {
    VkFormat format = VK_FORMAT_D24_UNORM_S8_UINT;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_ACCELERATION_STRUCTURE_VERTEX_BUFFER_BIT_KHR | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT_KHR;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_TRANSFER_DST_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_MINMAX_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_D32_SFLOAT_S8_UINT) {
    VkFormat format = VK_FORMAT_D32_SFLOAT_S8_UINT;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_BLIT_DST_BIT;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_ATOMIC_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_DISJOINT_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_BC1_RGB_UNORM_BLOCK) {
    VkFormat format = VK_FORMAT_BC1_RGB_UNORM_BLOCK;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_FRAGMENT_DENSITY_MAP_BIT_EXT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT;
    VkFormatFeatureFlags buffer_features = 0;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_BC1_RGB_SRGB_BLOCK) {
    VkFormat format = VK_FORMAT_BC1_RGB_SRGB_BLOCK;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = 0;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_TRANSFER_SRC_BIT_KHR | VK_FORMAT_FEATURE_TRANSFER_DST_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_BC1_RGBA_UNORM_BLOCK) {
    VkFormat format = VK_FORMAT_BC1_RGBA_UNORM_BLOCK;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_FRAGMENT_DENSITY_MAP_BIT_EXT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT_KHR | VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT | VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT | VK_FORMAT_FEATURE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_BC1_RGBA_SRGB_BLOCK) {
    VkFormat format = VK_FORMAT_BC1_RGBA_SRGB_BLOCK;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_STORAGE_IMAGE_ATOMIC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_ACCELERATION_STRUCTURE_VERTEX_BUFFER_BIT_KHR | VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_BC2_UNORM_BLOCK) {
    VkFormat format = VK_FORMAT_BC2_UNORM_BLOCK;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_TRANSFER_DST_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_ATOMIC_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT_KHR | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_MINMAX_BIT_EXT;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT_KHR;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT_KHR | VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_BC2_SRGB_BLOCK) {
    VkFormat format = VK_FORMAT_BC2_SRGB_BLOCK;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_STORAGE_IMAGE_ATOMIC_BIT | VK_FORMAT_FEATURE_DISJOINT_BIT_KHR;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT_KHR | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_IMG;
    VkFormatFeatureFlags buffer_features = 0;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_BC3_UNORM_BLOCK) {
    VkFormat format = VK_FORMAT_BC3_UNORM_BLOCK;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT_KHR;
    VkFormatFeatureFlags optimal_tiling_features = 0;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT_KHR | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT_KHR;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_BC3_SRGB_BLOCK) {
    VkFormat format = VK_FORMAT_BC3_SRGB_BLOCK;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_TRANSFER_SRC_BIT_KHR | VK_FORMAT_FEATURE_FRAGMENT_DENSITY_MAP_BIT_EXT | VK_FORMAT_FEATURE_DISJOINT_BIT_KHR;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT_KHR | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_BC4_UNORM_BLOCK) {
    VkFormat format = VK_FORMAT_BC4_UNORM_BLOCK;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_TRANSFER_DST_BIT_KHR | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_EXT;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT_KHR | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT_KHR;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_BC4_SNORM_BLOCK) {
    VkFormat format = VK_FORMAT_BC4_SNORM_BLOCK;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_TRANSFER_SRC_BIT_KHR | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT_KHR;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_BC5_UNORM_BLOCK) {
    VkFormat format = VK_FORMAT_BC5_UNORM_BLOCK;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_EXT;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT_KHR;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_ACCELERATION_STRUCTURE_VERTEX_BUFFER_BIT_KHR | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT | VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_BC5_SNORM_BLOCK) {
    VkFormat format = VK_FORMAT_BC5_SNORM_BLOCK;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT;
    VkFormatFeatureFlags optimal_tiling_features = 0;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT_KHR | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_BC6H_UFLOAT_BLOCK) {
    VkFormat format = VK_FORMAT_BC6H_UFLOAT_BLOCK;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT | VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT_KHR | VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT_KHR | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT_KHR;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_BC6H_SFLOAT_BLOCK) {
    VkFormat format = VK_FORMAT_BC6H_SFLOAT_BLOCK;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_IMG | VK_FORMAT_FEATURE_ACCELERATION_STRUCTURE_VERTEX_BUFFER_BIT_KHR | VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_IMG;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_BC7_UNORM_BLOCK) {
    VkFormat format = VK_FORMAT_BC7_UNORM_BLOCK;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT_KHR | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT_KHR;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_IMG | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT_KHR | VK_FORMAT_FEATURE_ACCELERATION_STRUCTURE_VERTEX_BUFFER_BIT_KHR;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_BC7_SRGB_BLOCK) {
    VkFormat format = VK_FORMAT_BC7_SRGB_BLOCK;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_MINMAX_BIT_EXT;
    VkFormatFeatureFlags optimal_tiling_features = 0;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_ETC2_R8G8B8_UNORM_BLOCK) {
    VkFormat format = VK_FORMAT_ETC2_R8G8B8_UNORM_BLOCK;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = 0;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_TRANSFER_DST_BIT | VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT_KHR | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_ETC2_R8G8B8_SRGB_BLOCK) {
    VkFormat format = VK_FORMAT_ETC2_R8G8B8_SRGB_BLOCK;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_DISJOINT_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_MINMAX_BIT_EXT | VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT_KHR | VK_FORMAT_FEATURE_BLIT_DST_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_ETC2_R8G8B8A1_UNORM_BLOCK) {
    VkFormat format = VK_FORMAT_ETC2_R8G8B8A1_UNORM_BLOCK;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_IMG | VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT_KHR | VK_FORMAT_FEATURE_ACCELERATION_STRUCTURE_VERTEX_BUFFER_BIT_KHR;
    VkFormatFeatureFlags optimal_tiling_features = 0;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_TRANSFER_DST_BIT_KHR | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT_KHR | VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_ETC2_R8G8B8A1_SRGB_BLOCK) {
    VkFormat format = VK_FORMAT_ETC2_R8G8B8A1_SRGB_BLOCK;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_DISJOINT_BIT_KHR | VK_FORMAT_FEATURE_FRAGMENT_DENSITY_MAP_BIT_EXT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_FRAGMENT_DENSITY_MAP_BIT_EXT | VK_FORMAT_FEATURE_DISJOINT_BIT_KHR;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_ETC2_R8G8B8A8_UNORM_BLOCK) {
    VkFormat format = VK_FORMAT_ETC2_R8G8B8A8_UNORM_BLOCK;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = 0;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT_KHR | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_ETC2_R8G8B8A8_SRGB_BLOCK) {
    VkFormat format = VK_FORMAT_ETC2_R8G8B8A8_SRGB_BLOCK;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_TRANSFER_DST_BIT_KHR;
    VkFormatFeatureFlags buffer_features = 0;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_EAC_R11_UNORM_BLOCK) {
    VkFormat format = VK_FORMAT_EAC_R11_UNORM_BLOCK;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = 0;
    VkFormatFeatureFlags optimal_tiling_features = 0;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_STORAGE_IMAGE_ATOMIC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT_KHR | VK_FORMAT_FEATURE_TRANSFER_DST_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_EAC_R11_SNORM_BLOCK) {
    VkFormat format = VK_FORMAT_EAC_R11_SNORM_BLOCK;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = 0;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_DISJOINT_BIT_KHR;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT | VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT_KHR | VK_FORMAT_FEATURE_FRAGMENT_DENSITY_MAP_BIT_EXT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_EAC_R11G11_UNORM_BLOCK) {
    VkFormat format = VK_FORMAT_EAC_R11G11_UNORM_BLOCK;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = 0;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_TRANSFER_DST_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT | VK_FORMAT_FEATURE_DISJOINT_BIT_KHR;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_EAC_R11G11_SNORM_BLOCK) {
    VkFormat format = VK_FORMAT_EAC_R11G11_SNORM_BLOCK;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT;
    VkFormatFeatureFlags optimal_tiling_features = 0;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT | VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT_KHR;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_ASTC_4x4_UNORM_BLOCK) {
    VkFormat format = VK_FORMAT_ASTC_4x4_UNORM_BLOCK;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = 0;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_ATOMIC_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_ASTC_4x4_SRGB_BLOCK) {
    VkFormat format = VK_FORMAT_ASTC_4x4_SRGB_BLOCK;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = 0;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_IMG;
    VkFormatFeatureFlags buffer_features = 0;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_ASTC_5x4_UNORM_BLOCK) {
    VkFormat format = VK_FORMAT_ASTC_5x4_UNORM_BLOCK;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT_KHR | VK_FORMAT_FEATURE_TRANSFER_DST_BIT;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_ATOMIC_BIT;
    VkFormatFeatureFlags buffer_features = 0;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_ASTC_5x4_SRGB_BLOCK) {
    VkFormat format = VK_FORMAT_ASTC_5x4_SRGB_BLOCK;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_FRAGMENT_DENSITY_MAP_BIT_EXT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_ASTC_5x5_UNORM_BLOCK) {
    VkFormat format = VK_FORMAT_ASTC_5x5_UNORM_BLOCK;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT_KHR;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT_KHR | VK_FORMAT_FEATURE_STORAGE_IMAGE_ATOMIC_BIT;
    VkFormatFeatureFlags buffer_features = 0;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_ASTC_5x5_SRGB_BLOCK) {
    VkFormat format = VK_FORMAT_ASTC_5x5_SRGB_BLOCK;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT | VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT | VK_FORMAT_FEATURE_DISJOINT_BIT_KHR;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_ASTC_6x5_UNORM_BLOCK) {
    VkFormat format = VK_FORMAT_ASTC_6x5_UNORM_BLOCK;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT;
    VkFormatFeatureFlags optimal_tiling_features = 0;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_DISJOINT_BIT_KHR;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_ASTC_6x5_SRGB_BLOCK) {
    VkFormat format = VK_FORMAT_ASTC_6x5_SRGB_BLOCK;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = 0;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_DISJOINT_BIT_KHR;
    VkFormatFeatureFlags buffer_features = 0;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_ASTC_6x6_UNORM_BLOCK) {
    VkFormat format = VK_FORMAT_ASTC_6x6_UNORM_BLOCK;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT_KHR | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT_KHR;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_ASTC_6x6_SRGB_BLOCK) {
    VkFormat format = VK_FORMAT_ASTC_6x6_SRGB_BLOCK;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT | VK_FORMAT_FEATURE_DISJOINT_BIT_KHR;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_STORAGE_IMAGE_ATOMIC_BIT | VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_ASTC_8x5_UNORM_BLOCK) {
    VkFormat format = VK_FORMAT_ASTC_8x5_UNORM_BLOCK;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT_KHR | VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT_KHR | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT;
    VkFormatFeatureFlags optimal_tiling_features = 0;
    VkFormatFeatureFlags buffer_features = 0;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_ASTC_8x5_SRGB_BLOCK) {
    VkFormat format = VK_FORMAT_ASTC_8x5_SRGB_BLOCK;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = 0;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT_KHR | VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT_KHR;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_ASTC_8x6_UNORM_BLOCK) {
    VkFormat format = VK_FORMAT_ASTC_8x6_UNORM_BLOCK;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = 0;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_IMG | VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_ASTC_8x6_SRGB_BLOCK) {
    VkFormat format = VK_FORMAT_ASTC_8x6_SRGB_BLOCK;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT_KHR | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT_KHR;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_TRANSFER_DST_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT_KHR;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_ASTC_8x8_UNORM_BLOCK) {
    VkFormat format = VK_FORMAT_ASTC_8x8_UNORM_BLOCK;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT_KHR | VK_FORMAT_FEATURE_TRANSFER_DST_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT;
    VkFormatFeatureFlags optimal_tiling_features = 0;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_ASTC_8x8_SRGB_BLOCK) {
    VkFormat format = VK_FORMAT_ASTC_8x8_SRGB_BLOCK;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_TRANSFER_DST_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT_KHR;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT_KHR;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_ASTC_10x5_UNORM_BLOCK) {
    VkFormat format = VK_FORMAT_ASTC_10x5_UNORM_BLOCK;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT_KHR | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT_KHR | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT_KHR;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_ASTC_10x5_SRGB_BLOCK) {
    VkFormat format = VK_FORMAT_ASTC_10x5_SRGB_BLOCK;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT_KHR | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT_KHR | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_EXT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_ASTC_10x6_UNORM_BLOCK) {
    VkFormat format = VK_FORMAT_ASTC_10x6_UNORM_BLOCK;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = 0;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_MINMAX_BIT | VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_ASTC_10x6_SRGB_BLOCK) {
    VkFormat format = VK_FORMAT_ASTC_10x6_SRGB_BLOCK;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT_KHR;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_ACCELERATION_STRUCTURE_VERTEX_BUFFER_BIT_KHR | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_ACCELERATION_STRUCTURE_VERTEX_BUFFER_BIT_KHR | VK_FORMAT_FEATURE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_ASTC_10x8_UNORM_BLOCK) {
    VkFormat format = VK_FORMAT_ASTC_10x8_UNORM_BLOCK;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_TRANSFER_SRC_BIT_KHR;
    VkFormatFeatureFlags optimal_tiling_features = 0;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT | VK_FORMAT_FEATURE_DISJOINT_BIT_KHR;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_ASTC_10x8_SRGB_BLOCK) {
    VkFormat format = VK_FORMAT_ASTC_10x8_SRGB_BLOCK;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = 0;
    VkFormatFeatureFlags optimal_tiling_features = 0;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_ACCELERATION_STRUCTURE_VERTEX_BUFFER_BIT_KHR;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_ASTC_10x10_UNORM_BLOCK) {
    VkFormat format = VK_FORMAT_ASTC_10x10_UNORM_BLOCK;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT_KHR | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_EXT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_TRANSFER_DST_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR | VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT_KHR;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_ASTC_10x10_SRGB_BLOCK) {
    VkFormat format = VK_FORMAT_ASTC_10x10_SRGB_BLOCK;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = 0;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_STORAGE_IMAGE_ATOMIC_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_FRAGMENT_DENSITY_MAP_BIT_EXT | VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT_KHR;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_ASTC_12x10_UNORM_BLOCK) {
    VkFormat format = VK_FORMAT_ASTC_12x10_UNORM_BLOCK;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_IMG | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_TRANSFER_DST_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT_KHR | VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_ASTC_12x10_SRGB_BLOCK) {
    VkFormat format = VK_FORMAT_ASTC_12x10_SRGB_BLOCK;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_ATOMIC_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT_KHR;
    VkFormatFeatureFlags optimal_tiling_features = 0;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT_KHR | VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT_KHR;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_ASTC_12x12_UNORM_BLOCK) {
    VkFormat format = VK_FORMAT_ASTC_12x12_UNORM_BLOCK;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_FRAGMENT_DENSITY_MAP_BIT_EXT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT;
    VkFormatFeatureFlags optimal_tiling_features = 0;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT_KHR | VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_ASTC_12x12_SRGB_BLOCK) {
    VkFormat format = VK_FORMAT_ASTC_12x12_SRGB_BLOCK;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT_KHR | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT_KHR;
    VkFormatFeatureFlags buffer_features = 0;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_G8B8G8R8_422_UNORM) {
    VkFormat format = VK_FORMAT_G8B8G8R8_422_UNORM;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_TRANSFER_DST_BIT_KHR | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_EXT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT_KHR;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_DISJOINT_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_B8G8R8G8_422_UNORM) {
    VkFormat format = VK_FORMAT_B8G8R8G8_422_UNORM;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = 0;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT_KHR | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT;
    VkFormatFeatureFlags buffer_features = 0;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_G8_B8_R8_3PLANE_420_UNORM) {
    VkFormat format = VK_FORMAT_G8_B8_R8_3PLANE_420_UNORM;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_STORAGE_IMAGE_ATOMIC_BIT;
    VkFormatFeatureFlags optimal_tiling_features = 0;
    VkFormatFeatureFlags buffer_features = 0;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_G8_B8R8_2PLANE_420_UNORM) {
    VkFormat format = VK_FORMAT_G8_B8R8_2PLANE_420_UNORM;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = 0;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT;
    VkFormatFeatureFlags buffer_features = 0;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_G8_B8_R8_3PLANE_422_UNORM) {
    VkFormat format = VK_FORMAT_G8_B8_R8_3PLANE_422_UNORM;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = 0;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_DISJOINT_BIT_KHR | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_MINMAX_BIT_EXT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_ATOMIC_BIT | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_G8_B8R8_2PLANE_422_UNORM) {
    VkFormat format = VK_FORMAT_G8_B8R8_2PLANE_422_UNORM;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_EXT | VK_FORMAT_FEATURE_DISJOINT_BIT_KHR | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT_KHR;
    VkFormatFeatureFlags buffer_features = 0;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_G8_B8_R8_3PLANE_444_UNORM) {
    VkFormat format = VK_FORMAT_G8_B8_R8_3PLANE_444_UNORM;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT;
    VkFormatFeatureFlags optimal_tiling_features = 0;
    VkFormatFeatureFlags buffer_features = 0;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_R10X6_UNORM_PACK16) {
    VkFormat format = VK_FORMAT_R10X6_UNORM_PACK16;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_FRAGMENT_DENSITY_MAP_BIT_EXT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT_KHR;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_R10X6G10X6_UNORM_2PACK16) {
    VkFormat format = VK_FORMAT_R10X6G10X6_UNORM_2PACK16;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT_KHR | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT | VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT | VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_IMG;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT_KHR;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_R10X6G10X6B10X6A10X6_UNORM_4PACK16) {
    VkFormat format = VK_FORMAT_R10X6G10X6B10X6A10X6_UNORM_4PACK16;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = 0;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT_KHR;
    VkFormatFeatureFlags buffer_features = 0;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_G10X6B10X6G10X6R10X6_422_UNORM_4PACK16) {
    VkFormat format = VK_FORMAT_G10X6B10X6G10X6R10X6_422_UNORM_4PACK16;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT_KHR | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT_KHR | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_MINMAX_BIT_EXT;
    VkFormatFeatureFlags optimal_tiling_features = 0;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_B10X6G10X6R10X6G10X6_422_UNORM_4PACK16) {
    VkFormat format = VK_FORMAT_B10X6G10X6R10X6G10X6_422_UNORM_4PACK16;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = 0;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT;
    VkFormatFeatureFlags buffer_features = 0;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_G10X6_B10X6_R10X6_3PLANE_420_UNORM_3PACK16) {
    VkFormat format = VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_420_UNORM_3PACK16;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = 0;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT_KHR | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT;
    VkFormatFeatureFlags buffer_features = 0;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_G10X6_B10X6R10X6_2PLANE_420_UNORM_3PACK16) {
    VkFormat format = VK_FORMAT_G10X6_B10X6R10X6_2PLANE_420_UNORM_3PACK16;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_BLIT_DST_BIT;
    VkFormatFeatureFlags optimal_tiling_features = 0;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT_KHR | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_EXT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_G10X6_B10X6_R10X6_3PLANE_422_UNORM_3PACK16) {
    VkFormat format = VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_422_UNORM_3PACK16;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = 0;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT_KHR | VK_FORMAT_FEATURE_ACCELERATION_STRUCTURE_VERTEX_BUFFER_BIT_KHR | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_IMG;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_G10X6_B10X6R10X6_2PLANE_422_UNORM_3PACK16) {
    VkFormat format = VK_FORMAT_G10X6_B10X6R10X6_2PLANE_422_UNORM_3PACK16;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_EXT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_G10X6_B10X6_R10X6_3PLANE_444_UNORM_3PACK16) {
    VkFormat format = VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_444_UNORM_3PACK16;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT | VK_FORMAT_FEATURE_DISJOINT_BIT_KHR | VK_FORMAT_FEATURE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT_KHR | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT_KHR;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_R12X4_UNORM_PACK16) {
    VkFormat format = VK_FORMAT_R12X4_UNORM_PACK16;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_EXT;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT_KHR | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_R12X4G12X4_UNORM_2PACK16) {
    VkFormat format = VK_FORMAT_R12X4G12X4_UNORM_2PACK16;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_DISJOINT_BIT;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT;
    VkFormatFeatureFlags buffer_features = 0;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_R12X4G12X4B12X4A12X4_UNORM_4PACK16) {
    VkFormat format = VK_FORMAT_R12X4G12X4B12X4A12X4_UNORM_4PACK16;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT_KHR | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_ACCELERATION_STRUCTURE_VERTEX_BUFFER_BIT_KHR | VK_FORMAT_FEATURE_FRAGMENT_DENSITY_MAP_BIT_EXT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT_KHR | VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT_KHR | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_G12X4B12X4G12X4R12X4_422_UNORM_4PACK16) {
    VkFormat format = VK_FORMAT_G12X4B12X4G12X4R12X4_422_UNORM_4PACK16;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_TRANSFER_SRC_BIT_KHR | VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT_KHR;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT_KHR | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_B12X4G12X4R12X4G12X4_422_UNORM_4PACK16) {
    VkFormat format = VK_FORMAT_B12X4G12X4R12X4G12X4_422_UNORM_4PACK16;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_FORMAT_FEATURE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR;
    VkFormatFeatureFlags optimal_tiling_features = 0;
    VkFormatFeatureFlags buffer_features = 0;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_G12X4_B12X4_R12X4_3PLANE_420_UNORM_3PACK16) {
    VkFormat format = VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_420_UNORM_3PACK16;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT;
    VkFormatFeatureFlags optimal_tiling_features = 0;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_MINMAX_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_G12X4_B12X4R12X4_2PLANE_420_UNORM_3PACK16) {
    VkFormat format = VK_FORMAT_G12X4_B12X4R12X4_2PLANE_420_UNORM_3PACK16;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = 0;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT_KHR | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT_KHR | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_IMG | VK_FORMAT_FEATURE_BLIT_SRC_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_G12X4_B12X4_R12X4_3PLANE_422_UNORM_3PACK16) {
    VkFormat format = VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_422_UNORM_3PACK16;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_BLIT_SRC_BIT;
    VkFormatFeatureFlags optimal_tiling_features = 0;
    VkFormatFeatureFlags buffer_features = 0;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_G12X4_B12X4R12X4_2PLANE_422_UNORM_3PACK16) {
    VkFormat format = VK_FORMAT_G12X4_B12X4R12X4_2PLANE_422_UNORM_3PACK16;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_DISJOINT_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_IMG | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_EXT | VK_FORMAT_FEATURE_DISJOINT_BIT_KHR;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_G12X4_B12X4_R12X4_3PLANE_444_UNORM_3PACK16) {
    VkFormat format = VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_444_UNORM_3PACK16;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_DISJOINT_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT_KHR | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT;
    VkFormatFeatureFlags optimal_tiling_features = 0;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_IMG | VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT_KHR | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT_KHR;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_G16B16G16R16_422_UNORM) {
    VkFormat format = VK_FORMAT_G16B16G16R16_422_UNORM;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_MINMAX_BIT_EXT | VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT;
    VkFormatFeatureFlags optimal_tiling_features = 0;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_MINMAX_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_B16G16R16G16_422_UNORM) {
    VkFormat format = VK_FORMAT_B16G16R16G16_422_UNORM;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_MINMAX_BIT | VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_ATOMIC_BIT;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT_KHR | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_EXT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT_KHR;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_G16_B16_R16_3PLANE_420_UNORM) {
    VkFormat format = VK_FORMAT_G16_B16_R16_3PLANE_420_UNORM;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT_KHR;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT;
    VkFormatFeatureFlags buffer_features = 0;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_G16_B16R16_2PLANE_420_UNORM) {
    VkFormat format = VK_FORMAT_G16_B16R16_2PLANE_420_UNORM;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT_KHR;
    VkFormatFeatureFlags optimal_tiling_features = 0;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT_KHR;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_G16_B16_R16_3PLANE_422_UNORM) {
    VkFormat format = VK_FORMAT_G16_B16_R16_3PLANE_422_UNORM;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT_KHR;
    VkFormatFeatureFlags optimal_tiling_features = 0;
    VkFormatFeatureFlags buffer_features = 0;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_G16_B16R16_2PLANE_422_UNORM) {
    VkFormat format = VK_FORMAT_G16_B16R16_2PLANE_422_UNORM;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT | VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT;
    VkFormatFeatureFlags optimal_tiling_features = 0;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT_KHR;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_G16_B16_R16_3PLANE_444_UNORM) {
    VkFormat format = VK_FORMAT_G16_B16_R16_3PLANE_444_UNORM;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = 0;
    VkFormatFeatureFlags optimal_tiling_features = 0;
    VkFormatFeatureFlags buffer_features = 0;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_G8_B8R8_2PLANE_444_UNORM) {
    VkFormat format = VK_FORMAT_G8_B8R8_2PLANE_444_UNORM;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = 0;
    VkFormatFeatureFlags optimal_tiling_features = 0;
    VkFormatFeatureFlags buffer_features = 0;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_G10X6_B10X6R10X6_2PLANE_444_UNORM_3PACK16) {
    VkFormat format = VK_FORMAT_G10X6_B10X6R10X6_2PLANE_444_UNORM_3PACK16;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT_KHR | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_EXT | VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT;
    VkFormatFeatureFlags optimal_tiling_features = 0;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT_KHR | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_MINMAX_BIT_EXT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_G12X4_B12X4R12X4_2PLANE_444_UNORM_3PACK16) {
    VkFormat format = VK_FORMAT_G12X4_B12X4R12X4_2PLANE_444_UNORM_3PACK16;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = 0;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_DISJOINT_BIT_KHR | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT_KHR | VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_G16_B16R16_2PLANE_444_UNORM) {
    VkFormat format = VK_FORMAT_G16_B16R16_2PLANE_444_UNORM;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT_KHR | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT_KHR;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_A4R4G4B4_UNORM_PACK16) {
    VkFormat format = VK_FORMAT_A4R4G4B4_UNORM_PACK16;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_TRANSFER_DST_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT;
    VkFormatFeatureFlags optimal_tiling_features = 0;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_DISJOINT_BIT_KHR | VK_FORMAT_FEATURE_FRAGMENT_DENSITY_MAP_BIT_EXT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT_KHR;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_A4B4G4R4_UNORM_PACK16) {
    VkFormat format = VK_FORMAT_A4B4G4R4_UNORM_PACK16;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_FRAGMENT_DENSITY_MAP_BIT_EXT | VK_FORMAT_FEATURE_DISJOINT_BIT;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_DISJOINT_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_ASTC_4x4_SFLOAT_BLOCK) {
    VkFormat format = VK_FORMAT_ASTC_4x4_SFLOAT_BLOCK;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT_KHR | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT;
    VkFormatFeatureFlags optimal_tiling_features = 0;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT | VK_FORMAT_FEATURE_DISJOINT_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_MINMAX_BIT_EXT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_ASTC_5x4_SFLOAT_BLOCK) {
    VkFormat format = VK_FORMAT_ASTC_5x4_SFLOAT_BLOCK;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_TRANSFER_SRC_BIT_KHR;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_TRANSFER_DST_BIT_KHR | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_STORAGE_IMAGE_ATOMIC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_MINMAX_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT_KHR | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_ASTC_5x5_SFLOAT_BLOCK) {
    VkFormat format = VK_FORMAT_ASTC_5x5_SFLOAT_BLOCK;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_IMG | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_ASTC_6x5_SFLOAT_BLOCK) {
    VkFormat format = VK_FORMAT_ASTC_6x5_SFLOAT_BLOCK;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = 0;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_MINMAX_BIT;
    VkFormatFeatureFlags buffer_features = 0;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_ASTC_6x6_SFLOAT_BLOCK) {
    VkFormat format = VK_FORMAT_ASTC_6x6_SFLOAT_BLOCK;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = 0;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT_KHR | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT | VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_ATOMIC_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_TRANSFER_SRC_BIT_KHR;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_ASTC_8x5_SFLOAT_BLOCK) {
    VkFormat format = VK_FORMAT_ASTC_8x5_SFLOAT_BLOCK;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT_KHR | VK_FORMAT_FEATURE_STORAGE_IMAGE_ATOMIC_BIT | VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT_KHR;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_ASTC_8x6_SFLOAT_BLOCK) {
    VkFormat format = VK_FORMAT_ASTC_8x6_SFLOAT_BLOCK;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = 0;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_ACCELERATION_STRUCTURE_VERTEX_BUFFER_BIT_KHR | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_IMG | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_MINMAX_BIT_EXT;
    VkFormatFeatureFlags buffer_features = 0;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_ASTC_8x8_SFLOAT_BLOCK) {
    VkFormat format = VK_FORMAT_ASTC_8x8_SFLOAT_BLOCK;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT_KHR;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_TRANSFER_SRC_BIT_KHR | VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_ASTC_10x5_SFLOAT_BLOCK) {
    VkFormat format = VK_FORMAT_ASTC_10x5_SFLOAT_BLOCK;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = 0;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_ASTC_10x6_SFLOAT_BLOCK) {
    VkFormat format = VK_FORMAT_ASTC_10x6_SFLOAT_BLOCK;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_MINMAX_BIT_EXT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_ASTC_10x8_SFLOAT_BLOCK) {
    VkFormat format = VK_FORMAT_ASTC_10x8_SFLOAT_BLOCK;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT_KHR | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_DISJOINT_BIT_KHR | VK_FORMAT_FEATURE_TRANSFER_DST_BIT | VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT_KHR;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_ASTC_10x10_SFLOAT_BLOCK) {
    VkFormat format = VK_FORMAT_ASTC_10x10_SFLOAT_BLOCK;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = 0;
    VkFormatFeatureFlags optimal_tiling_features = 0;
    VkFormatFeatureFlags buffer_features = 0;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_ASTC_12x10_SFLOAT_BLOCK) {
    VkFormat format = VK_FORMAT_ASTC_12x10_SFLOAT_BLOCK;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = 0;
    VkFormatFeatureFlags optimal_tiling_features = 0;
    VkFormatFeatureFlags buffer_features = 0;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_ASTC_12x12_SFLOAT_BLOCK) {
    VkFormat format = VK_FORMAT_ASTC_12x12_SFLOAT_BLOCK;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_MINMAX_BIT_EXT;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT | VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_PVRTC1_2BPP_UNORM_BLOCK_IMG) {
    VkFormat format = VK_FORMAT_PVRTC1_2BPP_UNORM_BLOCK_IMG;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT_KHR | VK_FORMAT_FEATURE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_STORAGE_IMAGE_ATOMIC_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_PVRTC1_4BPP_UNORM_BLOCK_IMG) {
    VkFormat format = VK_FORMAT_PVRTC1_4BPP_UNORM_BLOCK_IMG;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = 0;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_MINMAX_BIT_EXT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_IMG | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR | VK_FORMAT_FEATURE_DISJOINT_BIT_KHR | VK_FORMAT_FEATURE_TRANSFER_DST_BIT_KHR | VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT_KHR;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_PVRTC2_2BPP_UNORM_BLOCK_IMG) {
    VkFormat format = VK_FORMAT_PVRTC2_2BPP_UNORM_BLOCK_IMG;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT_KHR | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT | VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT;
    VkFormatFeatureFlags buffer_features = 0;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_PVRTC2_4BPP_UNORM_BLOCK_IMG) {
    VkFormat format = VK_FORMAT_PVRTC2_4BPP_UNORM_BLOCK_IMG;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = 0;
    VkFormatFeatureFlags optimal_tiling_features = 0;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_TRANSFER_DST_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_PVRTC1_2BPP_SRGB_BLOCK_IMG) {
    VkFormat format = VK_FORMAT_PVRTC1_2BPP_SRGB_BLOCK_IMG;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = 0;
    VkFormatFeatureFlags optimal_tiling_features = 0;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_PVRTC1_4BPP_SRGB_BLOCK_IMG) {
    VkFormat format = VK_FORMAT_PVRTC1_4BPP_SRGB_BLOCK_IMG;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_MINMAX_BIT_EXT;
    VkFormatFeatureFlags optimal_tiling_features = VK_FORMAT_FEATURE_FRAGMENT_DENSITY_MAP_BIT_EXT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_PVRTC2_2BPP_SRGB_BLOCK_IMG) {
    VkFormat format = VK_FORMAT_PVRTC2_2BPP_SRGB_BLOCK_IMG;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT | VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT_KHR;
    VkFormatFeatureFlags optimal_tiling_features = 0;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT | VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT_KHR | VK_FORMAT_FEATURE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_PVRTC2_4BPP_SRGB_BLOCK_IMG) {
    VkFormat format = VK_FORMAT_PVRTC2_4BPP_SRGB_BLOCK_IMG;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT;
    VkFormatFeatureFlags optimal_tiling_features = 0;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_TRANSFER_SRC_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

TEST_F(TestsCapabilitiesGenerated, Test_FORMAT_R16G16_S10_5_NV) {
    VkFormat format = VK_FORMAT_R16G16_S10_5_NV;
    VkFormatProperties format_properties;
    vkGetPhysicalDeviceFormatProperties(gpu_profile, format, &format_properties);

    VkFormatFeatureFlags linear_tiling_features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT;
    VkFormatFeatureFlags optimal_tiling_features = 0;
    VkFormatFeatureFlags buffer_features = VK_FORMAT_FEATURE_TRANSFER_DST_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT;
    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);
    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);
    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);
}

