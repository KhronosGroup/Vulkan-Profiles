#!/usr/bin/python3
#
# Copyright (c) 2021-2026 LunarG, Inc.
# Copyright (c) 2024 RasterGrid Kft.
#
# Licensed under the Apache License, Version 2.0 (the "License")
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
# Authors:
# - Ziga Markus <ziga@lunarg.com>
# - Christophe Riccio <christophe@lunarg.com>

from vulkan_object import VulkanObject, Format
from source.generate_layer import LayerRegistry
from source.vulkan_object_version import VK_VERSION

PROFILES = '''
    "profiles": {
        "VP_LUNARG_test_api": {
            "label": "label",
            "description": "description",
            "version": 1,
            "api-version": "1.3.203",
            "contributors": {
                "Ziga Markus": {
                    "company": "LunarG",
                    "email": "ziga@lunarg.com",
                    "github": "ziga-lunarg",
                    "contact": true
                }
            },
            "history": [
                {
                    "revision": 2,
                    "date": "2022-01-03",
                    "author": "Christophe Riccio",
                    "comment": "Rebase to Vulkan 1.3.203 capabilities"
                },
                {
                    "revision": 1,
                    "date": "2021-12-08",
                    "author": "Ziga Markus",
                    "comment": "Initial revision against 1.2.198"
                }
            ],
            "capabilities": [ "baseline" ]
        }
    }
 '''

TESTS_HEADER = '''
/*
 * Copyright (C) 2021-2026 Valve Corporation
 * Copyright (C) 2021-2026 LunarG, Inc.
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
 * Authors:
 * - Christophe Riccio <christophe@lunarg.com>
 * - Daniel Rakos <daniel.rakos@rastergrid.com>
 * - Mark Lobodzinski <mark@lunarg.com>
 *
 * This file is ***GENERATED***.  Do Not Edit.
 * See scripts/source/generate_tests.py for modifications.
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

        const char* profile_file_data = JSON_TEST_FILES_PATH "VP_LUNARG_test_api_generated.json";
        const char* profile_name_data = "VP_LUNARG_test_api";
        const std::vector<const char*> simulate_capabilities = {
            "SIMULATE_API_VERSION_BIT", "SIMULATE_FEATURES_BIT", "SIMULATE_PROPERTIES_BIT", "SIMULATE_EXTENSIONS_BIT", "SIMULATE_FORMATS_BIT",
            "SIMULATE_QUEUE_FAMILY_PROPERTIES_BIT", "SIMULATE_VIDEO_CAPABILITIES_BIT", "SIMULATE_VIDEO_FORMATS_BIT"};
        const std::vector<const char*> debug_reports = {
            "DEBUG_REPORT_ERROR_BIT"};

        std::vector<VkLayerSettingEXT> settings = {
            {kLayerName, kLayerSettingsProfileFile, VK_LAYER_SETTING_TYPE_STRING_EXT, 1, &profile_file_data},
            {kLayerName, kLayerSettingsProfileName, VK_LAYER_SETTING_TYPE_STRING_EXT, 1, &profile_name_data},
            {kLayerName, kLayerSettingsSimulateCapabilities, VK_LAYER_SETTING_TYPE_STRING_EXT, static_cast<uint32_t>(simulate_capabilities.size()), &simulate_capabilities[0]},
            {kLayerName, kLayerSettingsDebugReports, VK_LAYER_SETTING_TYPE_STRING_EXT, static_cast<uint32_t>(debug_reports.size()), &debug_reports[0]}
        };

        err = inst_builder.init(settings);
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

'''


class TestsRegistry(LayerRegistry):
    """Lookups over VulkanObject for the data needed by the layer tests generator."""
    def __init__(self, vk: VulkanObject):
        super().__init__(vk)

        self.constants = { name: constant.value for name, constant in self.vk.constants.items() }

        self.specVersions = { extension.name: extension.specVersionValue for extension in self.vk.extensions.values() }

        # The structure names of all the structure aliases
        self.structNames = { name: struct.name for struct in self.vk.structs.values() for name in [ struct.name ] + struct.aliases }

        # The Vulkan required property values by structure and member name, from the VulkanObject property requirements
        # of the extensions and the versions. Requirements depending on an unsupported feature or extension ('!') are
        # the values when the capability is missing so they are skipped, and the values of the later Vulkan versions
        # replace the earlier ones.
        owners = list(self.vk.extensions.values()) + sorted(self.vk.versions.values(), key=lambda version: VK_VERSION.from_string(version.name))
        self.requiredPropertyValues = dict()
        # All the members with a property requirement, including the implementation-dependent ones without a required value
        self.listedProperties = set()
        for owner in owners:
            for requirement in getattr(owner, 'propertyRequirement', None) or []:
                self.listedProperties.add((self.structNames.get(requirement.struct, requirement.struct), requirement.name))
                if requirement.value is None or requirement.value == '???':
                    continue
                if requirement.depends is not None and '!' in requirement.depends:
                    continue
                self.requiredPropertyValues[(self.structNames.get(requirement.struct, requirement.struct), requirement.name)] = requirement.value

    def getRequiredPropertyValue(self, struct_name, member_name):
        return self.requiredPropertyValues.get((self.structNames.get(struct_name, struct_name), member_name))

    def hasPropertyRequirement(self, struct_name, member_name):
        return (self.structNames.get(struct_name, struct_name), member_name) in self.listedProperties

class ProfileGenerator():
    i = 1
    skipped_features = []
    skipped_members = ["sType", "pNext", "physicalDevices", "driverID", "copySrcLayoutCount", "copyDstLayoutCount"]
    skipped_properties_structs = ["VkPhysicalDeviceLineRasterizationPropertiesEXT", "VkPhysicalDeviceLayeredApiPropertiesListKHR", "VkPhysicalDeviceFragmentDensityMapOffsetPropertiesEXT", "VkPhysicalDeviceCopyMemoryIndirectPropertiesNV", "VkPhysicalDeviceMemoryDecompressionPropertiesNV", 'VkPhysicalDeviceGpaPropertiesAMD']
    # VkPhysicalDeviceTileShadingPropertiesQCOM currently crashes in a driver
    # VkPhysicalDeviceRobustness2PropertiesEXT is an alias of VkPhysicalDeviceRobustness2PropertiesKHR
    skipped_test_structs = ["VkPhysicalDeviceTileShadingPropertiesQCOM", "VkPhysicalDeviceRobustness2PropertiesEXT"]

    # The maximum format features, set depending on the vk.xml <format> definitions
    # Buffer features of all formats, only set in bufferFeatures
    buffer_format_features = [
        "VK_FORMAT_FEATURE_2_UNIFORM_TEXEL_BUFFER_BIT",
        "VK_FORMAT_FEATURE_2_STORAGE_TEXEL_BUFFER_BIT",
        "VK_FORMAT_FEATURE_2_STORAGE_TEXEL_BUFFER_ATOMIC_BIT",
        "VK_FORMAT_FEATURE_2_VERTEX_BUFFER_BIT",
        "VK_FORMAT_FEATURE_2_ACCELERATION_STRUCTURE_VERTEX_BUFFER_BIT_KHR"]
    # Image features of all formats, only set in linearTilingFeatures and optimalTilingFeatures as the following features
    image_format_features = [
        "VK_FORMAT_FEATURE_2_SAMPLED_IMAGE_BIT",
        "VK_FORMAT_FEATURE_2_STORAGE_IMAGE_BIT",
        "VK_FORMAT_FEATURE_2_STORAGE_IMAGE_ATOMIC_BIT",
        "VK_FORMAT_FEATURE_2_BLIT_SRC_BIT",
        "VK_FORMAT_FEATURE_2_BLIT_DST_BIT",
        "VK_FORMAT_FEATURE_2_SAMPLED_IMAGE_FILTER_LINEAR_BIT",
        "VK_FORMAT_FEATURE_2_SAMPLED_IMAGE_FILTER_CUBIC_BIT",
        "VK_FORMAT_FEATURE_2_TRANSFER_SRC_BIT",
        "VK_FORMAT_FEATURE_2_TRANSFER_DST_BIT",
        "VK_FORMAT_FEATURE_2_SAMPLED_IMAGE_FILTER_MINMAX_BIT",
        "VK_FORMAT_FEATURE_2_STORAGE_READ_WITHOUT_FORMAT_BIT",
        "VK_FORMAT_FEATURE_2_STORAGE_WRITE_WITHOUT_FORMAT_BIT",
        "VK_FORMAT_FEATURE_2_FRAGMENT_DENSITY_MAP_BIT_EXT",
        "VK_FORMAT_FEATURE_2_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR"]
    # Formats with a depth or a stencil component
    depth_stencil_format_features = [
        "VK_FORMAT_FEATURE_2_DEPTH_STENCIL_ATTACHMENT_BIT",
        "VK_FORMAT_FEATURE_2_SAMPLED_IMAGE_DEPTH_COMPARISON_BIT"]
    # Formats without a depth or a stencil component
    color_format_features = [
        "VK_FORMAT_FEATURE_2_COLOR_ATTACHMENT_BIT",
        "VK_FORMAT_FEATURE_2_COLOR_ATTACHMENT_BLEND_BIT"]
    # Formats with planes
    ycbcr_format_features = [
        "VK_FORMAT_FEATURE_2_MIDPOINT_CHROMA_SAMPLES_BIT",
        "VK_FORMAT_FEATURE_2_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT",
        "VK_FORMAT_FEATURE_2_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT",
        "VK_FORMAT_FEATURE_2_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT",
        "VK_FORMAT_FEATURE_2_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT",
        "VK_FORMAT_FEATURE_2_DISJOINT_BIT",
        "VK_FORMAT_FEATURE_2_COSITED_CHROMA_SAMPLES_BIT"]
    # Video features of all formats
    video_format_features = [
        "VK_FORMAT_FEATURE_2_VIDEO_DECODE_OUTPUT_BIT_KHR",
        "VK_FORMAT_FEATURE_2_VIDEO_DECODE_DPB_BIT_KHR",
        "VK_FORMAT_FEATURE_2_VIDEO_ENCODE_INPUT_BIT_KHR",
        "VK_FORMAT_FEATURE_2_VIDEO_ENCODE_DPB_BIT_KHR"]

    def __init__(self, registry: TestsRegistry):
        self.registry = registry

    def generate_profile(self, outProfile):
        with open(outProfile, 'w') as f:
            f.write(self.gen_privateImpl())

    def gen_extensions(self, extensions):
        gen = ''
        first = True
        for name, info in extensions.items():
            if first:
                first = False
            else:
                gen += ','
            gen += '\n                \"'
            gen += name
            gen += '\": '
            gen += str(self.registry.specVersions[name])
        return gen

    # TODO: As with the legacy generator, this is never called as no structure name extending VkPhysicalDeviceFeatures2 contains
    # "VkPhysicalDeviceFeatures2", and the legacy registry has no "features" attribute
    def gen_physical_device_features(self):
        gen = "                    \"features\": {"
        first = True
        for feature in self.registry.features["VkPhysicalDeviceFeatures"]:
            if first:
                first = False
            else:
                gen += ','
            gen += '\n'
            gen += "                        \""
            gen += feature[1] + "\": true"
        gen += '\n'
        gen += "                    }"
        return gen

    def gen_features(self, extensions):
        gen = ''
        first = True
        self.test_features = list()
        for name, value in self.registry.structs.items():
            if ('VkPhysicalDeviceFeatures2' in value.extends and value.definedByExtensions):
                self.test_features.append(name)
                if (first):
                    first = False
                else:
                    gen += ','
                gen += '\n                \"'
                gen += name
                gen += '\": {'
                first_feature = True
                for feature in value.members:
                    feature_name = feature
                    if (feature_name == "sType" or feature_name == "pNext"):
                        continue
                    if first_feature:
                        first_feature = False
                    else:
                        gen += ','
                    gen += '\n'
                    if "VkPhysicalDeviceFeatures2" in name:
                        gen += self.gen_physical_device_features()
                    else:
                        gen += '                    \"'
                        gen += feature_name
                        gen += '\": true'
                gen += '\n'
                gen += '                }'
        return gen

    def get_random_enum_value(self, name):
        return self.registry.enums[name].values[self.i % len(self.registry.enums[name].values)]

    def get_enum(self, name, array):
        gen = ''
        if array:
            gen = '[ \"'
        else:
            gen = "\""
        enum = self.get_random_enum_value(name)
        gen += enum
        if array:
            gen += "\" ]"
        else:
            gen += "\""
        return gen, enum

    def gen_extend_2d(self):
        gen = '{\n'
        gen += '                        \"width\": '
        gen += str(self.i)
        gen += ',\n'
        gen += '                        \"height\": '
        gen += str(self.i + 1)
        gen += '\n                    }'
        return gen, (self.i, self.i + 1)

    def gen_extend_3d(self):
        gen = '{\n'
        gen += '                        \"width\": '
        gen += str(self.i)
        gen += ',\n'
        gen += '                        \"height\": '
        gen += str(self.i + 1)
        gen += ',\n'
        gen += '                        \"depth\": '
        gen += str(self.i + 2)
        gen += '\n                    }'
        return gen, (self.i, self.i + 1, self.i + 2)

    def get_conformance_version(self):
        gen = '{\n'
        gen += '                        \"major\": '
        gen += str(self.i)
        gen += ',\n'
        gen += '                        \"minor\": '
        gen += str(self.i + 1)
        gen += ',\n'
        gen += '                        \"subminor\": '
        gen += str(self.i + 2)
        gen += ',\n'
        gen += '                        \"patch\": '
        gen += str(self.i + 3)
        gen += '\n                    }'
        return gen, (self.i, self.i + 1, self.i + 2, self.i + 3)

    def gen_required_property(self, name, property_name, property_type, property_size):
        # Returns the JSON value and the test value of a property set to its Vulkan required value,
        # or None when the property has no Vulkan required value or the type is not handled
        value = self.registry.getRequiredPropertyValue(name, property_name)
        if value is None:
            return None
        values = [ element.strip() for element in value.split(',') ]

        if property_type == "VkBool32":
            if value not in ('true', 'false'):
                return None
            return value, 'VK_TRUE' if value == 'true' else 'VK_FALSE'
        elif property_type in ("uint32_t", "uint64_t", "int64_t", "VkDeviceSize", "size_t", "float"):
            convert = float if property_type == "float" else int
            if len(values) == 1:
                values = values * property_size
            if len(values) != property_size:
                return None
            try:
                test_value = [ convert(element) for element in values ]
            except ValueError:
                # Required values expressed relative to other members, e.g. '96 * samplerDescriptorSize'
                return None
            gen = ', '.join(str(element) for element in test_value)
            if property_size > 1:
                gen = '[ ' + gen + ' ]'
            return gen, test_value
        elif property_type == "VkExtent2D":
            if len(values) != 2:
                return None
            gen = '{\n'
            gen += '                        \"width\": ' + values[0] + ',\n'
            gen += '                        \"height\": ' + values[1] + '\n'
            gen += '                    }'
            return gen, (int(values[0]), int(values[1]))
        elif property_type.endswith("Flags") or property_type.endswith("FlagsEXT"):
            flags = [ flag.strip() for flag in value.split('|') if flag.strip() != '0' ]
            gen = '[ ' + ', '.join('\"' + flag + '\"' for flag in flags) + ' ]' if flags else '[]'
            return gen, ' | '.join(flags) if flags else '0'
        elif property_type in self.registry.enums and value.startswith('VK_'):
            return '\"' + value + '\"', value
        return None

    def gen_properties(self, extensions):
        gen = ''
        first = True
        self.test_values = dict()
        missing_requirements = list()
        for name, value in self.registry.structs.items():
            if ('VkPhysicalDeviceProperties2' in value.extends and value.definedByExtensions):
                if (name in self.skipped_properties_structs):
                    continue
                self.test_values[name] = dict()
                if first:
                    first = False
                else:
                    gen += ','
                gen += '\n                \"'
                gen += name
                gen += '\": {'
                first_property = True
                for property in value.members:
                    member = value.members[property]
                    property_type = member.type
                    property_name = member.name
                    property_size = 1
                    if (member.arraySize):
                        if (isinstance(member.arraySize, str)):
                            property_size = int(self.registry.constants[member.arraySize])
                        else:
                            property_size = member.arraySize
                    skip = False
                    for skipped in self.skipped_members:
                        if (property_name.startswith(skipped)):
                            skip = True
                            break
                    if skip:
                        continue
                    if first_property:
                        first_property = False
                    else:
                        gen += ','
                    gen += '\n'
                    gen += '                    \"'
                    gen += property_name
                    gen += '\": '
                    # Members without a property requirement come from a new vk.xml, the counter values would hide them
                    if not self.registry.hasPropertyRequirement(name, property_name):
                        missing_requirements.append(name + '::' + property_name)
                    required_property = self.gen_required_property(name, property_name, property_type, property_size)
                    if required_property is not None:
                        gen += required_property[0]
                        self.test_values[name][property] = required_property[1]
                    elif property_type == "VkBool32":
                        gen += "true"
                        self.test_values[name][property] = 'VK_TRUE'
                    elif property_type == "uint8_t":
                        self.test_values[name][property] = list()
                        if property_size > 1:
                            gen += "[ "
                        first = True
                        for i in range(property_size):
                            if first:
                                first = False
                            else:
                                gen += ", "
                            gen += str(self.i % 255)
                            self.test_values[name][property].append(self.i % 255)
                        if property_size > 1:
                            gen += " ]"
                        self.i += 1
                    elif property_type == "uint32_t" or property_type == "uint64_t" or property_type == "int64_t" or property_type == "VkDeviceSize" or property_type == "size_t":
                        self.test_values[name][property] = list()
                        if property_size > 1:
                            gen += "[ "
                        first = True
                        for i in range(property_size):
                            if first:
                                first = False
                            else:
                                gen += ", "
                            gen += str(self.i)
                            self.test_values[name][property].append(self.i)
                        if property_size > 1:
                            gen += " ]"
                        self.i += 1
                    elif property_type == "float":
                        self.test_values[name][property] = list()
                        if property_size > 1:
                            gen += "[ "
                        first = True
                        for i in range(property_size):
                            if first:
                                first = False
                            else:
                                gen += ", "
                            gen += str(float(self.i))
                            self.test_values[name][property].append(float(self.i))
                        if property_size > 1:
                            gen += " ]"
                        self.i += 1
                    elif property_type == "VkSampleCountFlags":
                        enum = self.get_enum('VkSampleCountFlagBits', True)
                        gen += enum[0]
                        self.test_values[name][property] = enum[1]
                        self.i += 1
                    elif property_type == "VkToolPurposeFlags":
                        enum = self.get_enum('VkToolPurposeFlagBits', True)
                        gen += enum[0]
                        self.test_values[name][property] = enum[1]
                        self.i += 1
                    elif property_type == "VkShaderFloatControlsIndependence":
                        enum = self.get_enum(property_type, False)
                        gen += enum[0]
                        self.test_values[name][property] = enum[1]
                        self.i += 1
                    elif property_type == "VkResolveModeFlags":
                        enum = self.get_enum('VkResolveModeFlagBits', True)
                        gen += enum[0]
                        self.test_values[name][property] = enum[1]
                        self.i += 1
                    elif property_type == "VkQueueFlags":
                        enum = self.get_enum('VkQueueFlagBits', True)
                        gen += enum[0]
                        self.test_values[name][property] = enum[1]
                        self.i += 1
                    elif property_type == "VkImageLayout":
                        enum = self.get_enum('VkImageLayout', True)
                        gen += enum[0]
                        self.test_values[name][property] = enum[1]
                        self.i += 1
                    elif property_type == "VkMemoryDecompressionMethodFlagsNV":
                        enum = self.get_enum('VkMemoryDecompressionMethodFlagBitsNV', True)
                        gen += enum[0]
                        self.test_values[name][property] = enum[1]
                        self.i += 1
                    elif property_type == "VkOpticalFlowGridSizeFlagsNV":
                        enum = self.get_enum('VkOpticalFlowGridSizeFlagBitsNV', True)
                        gen += enum[0]
                        self.test_values[name][property] = enum[1]
                        self.i += 1
                    elif property_type == "VkShaderStageFlags":
                        enum = self.get_enum('VkShaderStageFlagBits', True)
                        gen += enum[0]
                        self.test_values[name][property] = enum[1]
                        self.i += 1
                    elif property_type == "VkSampleCountFlagBits":
                        enum = self.get_enum(property_type, False)
                        gen += enum[0]
                        self.test_values[name][property] = enum[1]
                        self.i += 1
                    elif property_type == "VkShaderCorePropertiesFlagsAMD":
                        gen += "[]"
                        self.test_values[name][property] = None
                    elif property_type == "VkSubgroupFeatureFlags":
                        enum = self.get_enum('VkSubgroupFeatureFlagBits', True)
                        gen += enum[0]
                        self.test_values[name][property] = enum[1]
                        self.i += 1
                    elif property_type == "VkExtent2D":
                        property_value = self.gen_extend_2d()
                        gen += property_value[0]
                        self.test_values[name][property] = property_value[1]
                        self.i += 2
                    elif property_type == "VkExtent3D":
                        property_value = self.gen_extend_3d()
                        gen += property_value[0]
                        self.test_values[name][property] = property_value[1]
                        self.i += 3
                    elif property_type == "VkConformanceVersion":
                        property_value = self.get_conformance_version()
                        gen += property_value[0]
                        self.test_values[name][property] = property_value[1]
                        self.i += 4
                    elif property_type == "VkPointClippingBehavior":
                        enum = self.get_enum('VkPointClippingBehavior', False)
                        gen += enum[0]
                        self.test_values[name][property] = enum[1]
                        self.i += 1
                    elif property_type == "VkPipelineRobustnessBufferBehaviorEXT":
                        enum = self.get_enum('VkPipelineRobustnessBufferBehaviorEXT', False)
                        gen += enum[0]
                        self.test_values[name][property] = enum[1]
                        self.i += 1
                    elif property_type == "VkPipelineRobustnessImageBehaviorEXT":
                        enum = self.get_enum('VkPipelineRobustnessImageBehaviorEXT', False)
                        gen += enum[0]
                        self.test_values[name][property] = enum[1]
                        self.i += 1
                    elif property_type == "VkRayTracingInvocationReorderModeNV":
                        enum = self.get_enum('VkRayTracingInvocationReorderModeNV', False)
                        gen += enum[0]
                        self.test_values[name][property] = enum[1]
                        self.i += 1
                    elif property_type == "VkRayTracingInvocationReorderModeEXT":
                        enum = self.get_enum('VkRayTracingInvocationReorderModeEXT', False)
                        gen += enum[0]
                        self.test_values[name][property] = enum[1]
                        self.i += 1
                    elif property_type == "VkLayeredDriverUnderlyingApiMSFT":
                        enum = self.get_enum('VkLayeredDriverUnderlyingApiMSFT', False)
                        gen += enum[0]
                        self.test_values[name][property] = enum[1]
                        self.i += 1
                    elif property_type == "VkChromaLocation":
                        enum = self.get_enum('VkChromaLocation', False)
                        gen += enum[0]
                        self.test_values[name][property] = enum[1]
                        self.i += 1
                    elif property_type == "VkImageUsageFlags":
                        enum = self.get_enum('VkImageUsageFlagBits', True)
                        gen += enum[0]
                        self.test_values[name][property] = enum[1]
                        self.i += 1
                    elif property_type == "VkBufferUsageFlags":
                        enum = self.get_enum('VkBufferUsageFlagBits', True)
                        gen += enum[0]
                        self.test_values[name][property] = enum[1]
                        self.i += 1
                    elif property_type == "VkPhysicalDeviceSchedulingControlsFlagsARM":
                        enum = self.get_enum('VkPhysicalDeviceSchedulingControlsFlagBitsARM', True)
                        gen += enum[0]
                        self.test_values[name][property] = enum[1]
                        self.i += 1
                    elif property_type == "VkMemoryDecompressionMethodFlagsEXT":
                        enum = self.get_enum('VkMemoryDecompressionMethodFlagBitsEXT', True)
                        gen += enum[0]
                        self.test_values[name][property] = enum[1]
                        self.i += 1
                    elif property_type == "VkIndirectCommandsInputModeFlagsEXT":
                        enum = self.get_enum('VkIndirectCommandsInputModeFlagBitsEXT', True)
                        gen += enum[0]
                        self.test_values[name][property] = enum[1]
                        self.i += 1
                    elif property_type == "VkPipelineRobustnessBufferBehavior":
                        enum = self.get_enum('VkPipelineRobustnessBufferBehavior', False)
                        gen += enum[0]
                        self.test_values[name][property] = enum[1]
                        self.i += 1
                    elif property_type == "VkPipelineRobustnessImageBehavior":
                        enum = self.get_enum('VkPipelineRobustnessImageBehavior', False)
                        gen += enum[0]
                        self.test_values[name][property] = enum[1]
                        self.i += 1
                    elif property_type == "VkDefaultVertexAttributeValueKHR":
                        enum = self.get_enum('VkDefaultVertexAttributeValueKHR', False)
                        gen += enum[0]
                        self.test_values[name][property] = enum[1]
                        self.i += 1
                    elif property_type == "char":
                        gen += "\""
                        gen += property_name
                        gen += "\""
                        self.test_values[name][property] = '\"' + property_name + '\"'
                        self.i += 1
                    else:
                        gen += property_type
                gen += '\n'
                gen += '                }'
        if missing_requirements:
            raise RuntimeError(
                f'{len(missing_requirements)} property structure members have no default property requirement, '
                'add them to DEFAULT_CORE_PROPERTY_REQUIREMENTS or DEFAULT_EXTENSION_PROPERTY_REQUIREMENTS:\n'
                + '\n'.join(f'    {member}' for member in missing_requirements))
        return gen

    def get_max_format_features(self, format: Format) -> tuple[list[str], list[str], list[str]]:
        # Returns all the linear tiling, optimal tiling and buffer features a format could have, from its vk.xml <format> definition
        image_features = list(self.image_format_features)
        if any(component.type in ('D', 'S') for component in format.components):
            image_features += self.depth_stencil_format_features
        else:
            image_features += self.color_format_features
        if format.planes:
            image_features += self.ycbcr_format_features
        image_features += self.video_format_features
        return image_features, image_features, list(self.buffer_format_features)

    def gen_format_features(self, features: list[str]) -> str:
        return ', '.join('\"{0}\"'.format(feature) for feature in features) + ' ' if features else ''

    def gen_formats(self):
        gen = ''
        first = True
        self.test_features = dict()
        for name in self.registry.enums['VkFormat'].values:
            # The format aliases and VK_FORMAT_UNDEFINED have no vk.xml <format> definition
            format = self.registry.vk.formats.get(name)
            if format is None:
                continue
            # List for each of linear, optimal and buffer features
            self.test_features[name] = list(self.get_max_format_features(format))
            if (first):
                first = False
            else:
                gen += ','
            gen += '\n                \"'
            gen += name
            gen += '\": {\n'
            gen += '                    "VkFormatProperties3": {\n'
            gen += '                        "linearTilingFeatures": [ '
            gen += self.gen_format_features(self.test_features[name][0])
            gen += '],\n'
            gen += '                        "optimalTilingFeatures": [ '
            gen += self.gen_format_features(self.test_features[name][1])
            gen += '],\n'
            gen += '                        "bufferFeatures": [ '
            gen += self.gen_format_features(self.test_features[name][2])
            gen += ']\n'
            gen += '                    }\n'
            gen += '                }'
        return gen

    def gen_privateImpl(self):
        gen = '{\n'
        gen += '    "$schema": "https://schema.khronos.org/vulkan/profiles-0.8.0-204.json#",\n'
        gen += '    "capabilities": {\n'
        gen += '        "baseline": {\n'
        gen += '            "extensions": {'
        gen += self.gen_extensions(self.registry.extensions)
        gen += '\n'
        gen += '            },\n'
        gen += '            "features": {'
        gen += self.gen_features(self.registry.extensions)
        gen += '\n            },\n'
        gen += '            "properties": {'
        gen += self.gen_properties(self.registry.extensions)
        gen += '\n            },\n'
        gen += '            "formats": {'
        gen += self.gen_formats()
        gen += '\n            }\n'
        gen += '        }\n'
        gen += '    },'
        gen += PROFILES
        gen += '}\n'
        return gen

    def generate_tests(self, outTests):
        with open(outTests, 'w') as f:
            f.write(self.gen_tests())

    def gen_tests(self):
        gen = TESTS_HEADER

        for name, value in self.registry.structs.items():
            if ('VkPhysicalDeviceProperties2' in value.extends):
                gen += self.gen_properties_test(name, value)
            if ('VkPhysicalDeviceFeatures2' in value.extends):
                if name in self.skipped_features:
                    continue
                gen += self.gen_features_test(name, value)

        for name in self.test_features:
            gen += self.gen_format_test(name, self.test_features[name])

        return gen

    def gen_properties_test(self, name, value):
        gen = ''
        if name in self.skipped_test_structs:
            return gen
        if not hasattr(self, 'test_values'):
            return gen
        # No members to test in property
        if not name in self.test_values:
            return gen
        gen += 'TEST_F(TestsCapabilitiesGenerated, Test' + name[16:] + ') {\n'
        for ext in value.definedByExtensions:
            gen += '#ifdef ' + self.registry.extensions[ext].name + '\n'

        gen += '    bool supported = true;\n'
        for ext in value.definedByExtensions:
            gen += '    supported = supported && IsSupported(gpu_profile, "' + self.registry.extensions[ext].name + '");\n\n'

        var_name = self.create_var_name(name)
        gen += '    ' + name + ' ' + var_name + '_native' + '{};\n'
        gen += '    ' + var_name + '_native' + '.sType = ' + value.sType + ';\n\n'

        gen += '    VkPhysicalDeviceProperties2 gpu_props_native{};\n'
        gen += '    gpu_props_native.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;\n'
        gen += '    gpu_props_native.pNext = &' + var_name + '_native' + ';\n'
        gen += '    vkGetPhysicalDeviceProperties2(gpu_native, &gpu_props_native);\n\n'

        gen += '    ' + name + ' ' + var_name + '_profile' + '{};\n'
        gen += '    ' + var_name + '_profile' + '.sType = ' + value.sType + ';\n\n'

        gen += '    VkPhysicalDeviceProperties2 gpu_props_profile{};\n'
        gen += '    gpu_props_profile.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;\n'
        gen += '    gpu_props_profile.pNext = &' + var_name + '_profile' + ';\n'
        gen += '    vkGetPhysicalDeviceProperties2(gpu_profile, &gpu_props_profile);\n\n'

        for member in value.members:
            if member in self.test_values[name]:
                property_value = self.test_values[name][member]
                if (property_value):
                    if (self.registry.structs[name].members[member].limittype == 'exact' or self.registry.structs[name].members[member].limittype == 'noauto'):
                        member_type = self.registry.structs[name].members[member].type
                        # VkConformanceVersion is noauto and unmodified
                        if 'VkConformanceVersion' in member_type:
                            continue

                        gen += '    if (!supported) {\n'
                        if type(property_value) is list:
                            if (len(property_value) > 1):
                                for i in range(len(property_value)):
                                    gen += '    EXPECT_EQ(' + var_name + '_profile' + '.' + member + '[' + str(i) + '], ' + var_name + '_native' + '.' + member + '[' + str(i) + ']);\n'
                            else:
                                gen += '    EXPECT_EQ(' + var_name + '_profile' + '.' + member + ', ' + var_name + '_native' + '.' + member + ');\n'
                        elif type(property_value) is tuple:
                            if (member_type == 'VkExtent2D'):
                                gen += '    EXPECT_EQ(' + var_name + '_profile' + '.' + member + '.width, ' + var_name + '_native' + '.' + member + '.width);\n'
                                gen += '    EXPECT_EQ(' + var_name + '_profile' + '.' + member + '.height, ' + var_name + '_native' + '.' + member + '.height);\n'
                            elif (member_type == 'VkExtent3D'):
                                gen += '    EXPECT_EQ(' + var_name + '_profile' + '.' + member + '.width, ' + var_name + '_native' + '.' + member + '.width);\n'
                                gen += '    EXPECT_EQ(' + var_name + '_profile' + '.' + member + '.height, ' + var_name + '_native' + '.' + member + '.height);\n'
                                gen += '    EXPECT_EQ(' + var_name + '_profile' + '.' + member + '.depth, ' + var_name + '_native' + '.' + member + '.depth);\n'
                            else:
                                print('ERROR: unknown tuple type from ' + name + '.' + member)
                        elif self.registry.structs[name].members[member].type == 'char':
                            gen += '    EXPECT_EQ(0, strncmp(' + var_name + '_profile' + '.' + member + ', ' + var_name + '_native' + '.' + member + ', ' + str(len(property_value)) + '));\n'
                        else:
                            gen += '    EXPECT_EQ(' + var_name + '_profile' + '.' + member + ', ' + var_name + '_native' + '.' + member + ');\n'
                        gen += '    }\n\n'
                    else:
                        if type(property_value) is list:
                            if (len(property_value) > 1):
                                for i in range(len(property_value)):
                                    gen += '    EXPECT_EQ(' + var_name + '_profile' + '.' + member + '[' + str(i) + '], ' + str(property_value[i]) + ');\n'
                            else:
                                gen += '    EXPECT_EQ(' + var_name + '_profile' + '.' + member + ', ' + str(property_value[0]) + ');\n'
                        elif type(property_value) is tuple:
                            member_type = self.registry.structs[name].members[member].type
                            if (member_type == 'VkExtent2D'):
                                gen += '    EXPECT_EQ(' + var_name + '_profile' + '.' + member + '.width, ' + str(property_value[0]) + ');\n'
                                gen += '    EXPECT_EQ(' + var_name + '_profile' + '.' + member + '.height, ' + str(property_value[1]) + ');\n'
                            elif (member_type == 'VkExtent3D'):
                                gen += '    EXPECT_EQ(' + var_name + '_profile' + '.' + member + '.width, ' + str(property_value[0]) + ');\n'
                                gen += '    EXPECT_EQ(' + var_name + '_profile' + '.' + member + '.height, ' + str(property_value[1]) + ');\n'
                                gen += '    EXPECT_EQ(' + var_name + '_profile' + '.' + member + '.depth, ' + str(property_value[2]) + ');\n'
                            else:
                                print('ERROR: unknown tuple type from ' + name + '.' + member)
                        elif self.registry.structs[name].members[member].type == 'char':
                            gen += '    EXPECT_EQ(0, strncmp(' + var_name + '_profile' + '.' + member + ', ' + property_value + ', ' + str(len(property_value)) + '));\n'
                        else:
                            gen += '    EXPECT_EQ(' + var_name + '_profile' + '.' + member + ', ' + property_value + ');\n'

        for ext in value.definedByExtensions:
            gen += '#endif\n'
        gen += '}\n\n'
        return gen

    def gen_features_test(self, name, value):
        gen = ''
        if (not name in self.test_features):
            return gen
        gen += 'TEST_F(TestsCapabilitiesGenerated, Test' + name[16:] + ') {\n'
        for ext in value.definedByExtensions:
            gen += '#ifdef ' + self.registry.extensions[ext].name + '\n'

        var_name = self.create_var_name(name)
        gen += '    ' + name + ' ' + var_name + '{};\n'
        gen += '    ' + var_name + '.sType = ' + value.sType + ';\n\n'

        gen += '    VkPhysicalDeviceFeatures2 features;\n'
        gen += '    features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;\n'
        gen += '    features.pNext = &' + var_name + ';\n'
        gen += '    vkGetPhysicalDeviceFeatures2(gpu_profile, &features);\n\n'

        for member in value.members:
            gen += '    EXPECT_EQ(' + var_name + '.' + member + ', VK_TRUE);\n'

        for ext in value.definedByExtensions:
            gen += '#endif\n'
        gen += '}\n\n'
        return gen

    def gen_format_test(self, name, values):
        gen = 'TEST_F(TestsCapabilitiesGenerated, Test' + name[2:] + ') {\n'
        gen += '    VkFormat format = ' + name + ';\n'
        # The format features are VkFormatFeatureFlags2 values, including bits beyond the 32 bits of VkFormatFeatureFlags
        gen += '    VkFormatProperties3 format_properties_3{VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_3};\n'
        gen += '    VkFormatProperties2 format_properties{VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2, &format_properties_3};\n'
        gen += '    vkGetPhysicalDeviceFormatProperties2(gpu_profile, format, &format_properties);\n\n'

        for variable, features in zip(['linear_tiling_features', 'optimal_tiling_features', 'buffer_features'], values):
            gen += '    VkFormatFeatureFlags2 ' + variable + ' = ' + (' | '.join(features) if features else '0') + ';\n'

        gen += '    EXPECT_EQ(format_properties_3.linearTilingFeatures & linear_tiling_features, linear_tiling_features);\n'
        gen += '    EXPECT_EQ(format_properties_3.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);\n'
        gen += '    EXPECT_EQ(format_properties_3.bufferFeatures & buffer_features, buffer_features);\n'

        gen += '}\n\n'
        return gen

    def create_var_name(self, struct):
        var_name = ''
        while struct[-1].isupper():
            struct = struct[:-1]
        c = 0
        while c < len(struct):
            upper = struct[c].isupper()
            number = struct[c].isdigit()
            if upper or number:
                var_name += '_'
                while c < len(struct) and (upper and struct[c].isupper() or number and struct[c].isdigit()):
                    var_name += struct[c].lower()
                    c += 1
            else:
                var_name += struct[c]
                c += 1
        var_name = var_name[20:]
        if (var_name[0].isdigit()):
            var_name = 'f_' + var_name
        return var_name


def generate_tests(vk: VulkanObject, output_profile: str, output_cpp: str | None):
    """Generates the layer tests profile and, when output_cpp is set, the layer tests source file."""
    registry = TestsRegistry(vk)
    generator = ProfileGenerator(registry)
    generator.generate_profile(output_profile)
    if output_cpp is not None:
        generator.generate_tests(output_cpp)
