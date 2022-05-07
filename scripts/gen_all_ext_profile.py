#!/usr/bin/python3
#
# Copyright (c) 2021-2022 LunarG, Inc.
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
# Author: Ziga Markus <ziga@lunarg.com>

import genvp
import gen_layer
import xml.etree.ElementTree as etree
import math
import argparse
from fractions import Fraction

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

TESTS_HEADER = '''/*
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

#ifdef _WIN32
#ifdef _DEBUG
static const char* CONFIG_PATH = "bin/Debug";
#else
static const char* CONFIG_PATH = "bin/Release";
#endif
#else
static const char* CONFIG_PATH = "lib";
#endif

static VkInstance instance;
static VkPhysicalDevice gpu;
static profiles_test::VulkanInstanceBuilder inst_builder;


class TestsCapabilitiesGenerated : public VkTestFramework {
  public:
    TestsCapabilitiesGenerated(){};
    ~TestsCapabilitiesGenerated(){};

    static void SetUpTestSuite() {
        VkResult err = VK_SUCCESS;

        const std::string layer_path = std::string(TEST_BINARY_PATH) + CONFIG_PATH;
        profiles_test::setEnvironmentSetting("VK_LAYER_PATH", layer_path.c_str());

        inst_builder.addLayer("VK_LAYER_KHRONOS_profiles");

        VkProfileLayerSettingsEXT settings;
        settings.profile_file = JSON_TEST_FILES_PATH "VP_LUNARG_test_api_generated.json";
        settings.emulate_portability = true;
        settings.profile_name = "VP_LUNARG_test_api";
        settings.simulate_capabilities = SimulateCapabilityFlag::SIMULATE_ALL_CAPABILITIES;
        settings.debug_reports = DEBUG_REPORT_ERROR_BIT;
        err = inst_builder.makeInstance(&settings);

        instance = inst_builder.getInstance();
        err = inst_builder.getPhysicalDevice(&gpu);
    };

    static void TearDownTestSuite() {
        if (instance != VK_NULL_HANDLE) {
            vkDestroyInstance(instance, nullptr);
            instance = VK_NULL_HANDLE;
        }
    };

};

'''

class ProfileGenerator():
    i = 1
    # Currently a bug in vk.xml, can be removed once pNext is no longer const
    skipped_features = ["VkPhysicalDeviceRasterizationOrderAttachmentAccessFeaturesARM"]
    skipped_members = ["sType", "pNext", "physicalDevices", "driverID"]

    def generate_profile(self, outProfile, registry):
        with open(outProfile, 'w') as f:
            f.write(self.gen_privateImpl(registry))

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
            gen += str(info.spec_version)
        return gen

    def gen_physical_device_features(self, registry):
        gen = "                    \"features\": {"
        first = True
        for feature in registry.features["VkPhysicalDeviceFeatures"]:
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

    def gen_features(self, extensions, registry):
        gen = ''
        first = True
        self.test_features = list()
        for name, value  in registry.structs.items():
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
                    if "VkPhysicalDeviceFeatures2"in name:
                        gen += self.gen_physical_device_features(registry)
                    else:
                        gen += '                    \"'
                        gen += feature_name
                        gen += '\": true'
                gen += '\n'
                gen += '                }'
        return gen

    def get_random_enum_value(self, name):
        return registry.enums[name].values[self.i % len(registry.enums[name].values)]

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
        return gen, (self.i, self.i + 1, self.i + 2, self.i +3)

    def gen_properties(self, extensions, registry):
        gen = ''
        first = True
        self.test_values = dict()
        for name, value  in registry.structs.items():
            if ('VkPhysicalDeviceProperties2' in value.extends and value.definedByExtensions):
                skip = False
                for skipped in gen_layer.VulkanProfilesLayerGenerator.non_modifiable_structs:
                    if (name.startswith(skipped)):
                        skip = True
                        break
                if skip:
                    continue
                if gen_layer.VulkanProfilesLayerGenerator.from_skipped_extension(gen_layer.VulkanProfilesLayerGenerator(), name, registry):
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
                            property_size = int(registry.constants[member.arraySize])
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
                    if property_type == "VkBool32":
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
                    elif property_type == "uint32_t" or property_type == "uint64_t" or property_type == "int64_t" or property_type == "VkDeviceSize":
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
                    elif property_type == "char":
                        gen += "\""
                        gen += property_name
                        gen += "\""
                        self.test_values[name][property] = '\"' + property_name + '\"'
                    else:
                        gen += property_type
                gen += '\n'
                gen += '                }'
        return gen

    def pseudo_random(self, s):
        return Fraction(math.sin(s * 12.9898) * 43758.5453123)

    def gen_random_format_features(self, registry, name, list_index):
        # Choose from 0 to including 3 number of format features
        num = int(self.pseudo_random(self.i) * 1000) % 5
        self.i += 1
        gen = ''
        first = True
        used = list()
        for j in range(num):
            index = int(self.pseudo_random(self.i) * 1000) % len(registry.enums['VkFormatFeatureFlagBits'].values)
            if (index in used):
                continue
            feature = registry.enums['VkFormatFeatureFlagBits'].values[index]
            if feature in registry.betaFormatFeatures:
                continue
            if first:
                first = False
            else:
                gen += ', '
            self.i += 1
            gen += '\"'
            gen += feature
            gen += '\"'
            self.test_features[name][list_index].append(feature)
            used.append(index)

        if num > 0:
            gen += ' '

        return gen

    def gen_formats(self, registry):
        gen = ''
        first = True
        self.test_features = dict()
        for name in registry.enums['VkFormat'].values:
            if name in registry.aliasFormats:
                continue
            self.test_features[name] = list()
            # List for each of linear, optimal and buffer features
            self.test_features[name].append(list())
            self.test_features[name].append(list())
            self.test_features[name].append(list())
            if (first):
                first = False
            else:
                gen += ','
            gen += '\n                \"'
            gen += name
            gen += '\": {\n'
            gen += '                    "VkFormatProperties": {\n'
            gen += '                        "linearTilingFeatures": [ '
            gen += self.gen_random_format_features(registry, name, 0)
            gen += '],\n'
            gen += '                        "optimalTilingFeatures": [ '
            gen += self.gen_random_format_features(registry, name, 1)
            gen += '],\n'
            gen += '                        "bufferFeatures": [ '
            gen += self.gen_random_format_features(registry, name, 2)
            gen += ']\n'
            gen += '                    }\n'
            gen += '                }'
        return gen

    def gen_privateImpl(self, registry):
        gen = '{\n'
        gen += '    "$schema": "https://schema.khronos.org/vulkan/profiles-0.8.0-204.json#",\n'
        gen += '    "capabilities": {\n'
        gen += '        "baseline": {\n'
        gen += '            "extensions": {'
        gen += self.gen_extensions(registry.extensions)
        gen += '\n'
        gen += '            },\n'
        gen += '            "features": {'
        gen += self.gen_features(registry.extensions, registry)
        gen += '\n            },\n'
        gen += '            "properties": {'
        gen += self.gen_properties(registry.extensions, registry)
        gen += '\n            },\n'
        gen += '            "formats": {'
        gen += self.gen_formats(registry)
        gen += '\n            }\n'
        gen += '        }\n'
        gen += '    },'
        gen += PROFILES
        gen += '}\n'
        return gen

    def generate_tests(self, outTests, registry):
        with open(outTests, 'w') as f:
            f.write(self.gen_tests(registry))

    def gen_tests(self, registry):
        gen = TESTS_HEADER

        for name, value  in registry.structs.items():
            if ('VkPhysicalDeviceProperties2' in value.extends):
                if name in gen_layer.VulkanProfilesLayerGenerator.non_modifiable_structs:
                    continue
                if gen_layer.VulkanProfilesLayerGenerator.from_skipped_extension(gen_layer.VulkanProfilesLayerGenerator(), name, registry):
                    continue
                gen += self.gen_properties_test(registry, name, value)
            if ('VkPhysicalDeviceFeatures2' in value.extends):
                if name in self.skipped_features:
                    continue
                gen += self.gen_features_test(registry, name, value)

        for name in self.test_features:
            gen += self.gen_format_test(name, self.test_features[name])

        return gen

    def gen_properties_test(self, registry, name, value):
        gen = ''
        if not hasattr(self, 'test_values'):
            return gen
        # No members to test in property
        if not name in self.test_values:
            return gen
        gen += 'TEST_F(TestsCapabilitiesGenerated, Test' + name[16:] + ') {\n'
        for ext in value.definedByExtensions:
            gen += '#ifdef ' + registry.extensions[ext].name + '\n'

        var_name = self.create_var_name(name)
        gen += '    ' + name + ' ' + var_name + '{};\n'
        gen += '    ' + var_name + '.sType = ' + value.sType + ';\n\n'

        gen += '    VkPhysicalDeviceProperties2 gpu_props{};\n'
        gen += '    gpu_props.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;\n'
        gen += '    gpu_props.pNext = &' + var_name + ';\n'
        gen += '    vkGetPhysicalDeviceProperties2(gpu, &gpu_props);\n\n'

        for member in value.members:
            if member in self.test_values[name]:
                property_value = self.test_values[name][member]
                if (property_value):
                    if type(property_value) is list:
                        if (len(property_value) > 1):
                            for i in range(len(property_value)):
                                gen += '    EXPECT_EQ(' + var_name + '.' + member + '[' + str(i) + '], ' + str(property_value[i]) + ');\n'
                        else:
                            gen += '    EXPECT_EQ(' + var_name + '.' + member + ', ' + str(property_value[0]) + ');\n'
                    elif type(property_value) is tuple:
                        member_type = registry.structs[name].members[member].type
                        if (member_type == 'VkExtent2D'):
                            gen += '    EXPECT_EQ(' + var_name + '.' + member + '.width, ' + str(property_value[0]) + ');\n'
                            gen += '    EXPECT_EQ(' + var_name + '.' + member + '.height, ' + str(property_value[1]) + ');\n'
                        elif (member_type == 'VkExtent3D'):
                            gen += '    EXPECT_EQ(' + var_name + '.' + member + '.width, ' + str(property_value[0]) + ');\n'
                            gen += '    EXPECT_EQ(' + var_name + '.' + member + '.height, ' + str(property_value[1]) + ');\n'
                            gen += '    EXPECT_EQ(' + var_name + '.' + member + '.depth, ' + str(property_value[2]) + ');\n'
                    else:
                        gen += '    EXPECT_EQ(' + var_name + '.' + member + ', ' + property_value + ');\n'

        for ext in value.definedByExtensions:
            gen += '#endif\n'
        gen += '}\n\n'
        return gen

    def gen_features_test(self, registry, name, value):
        gen = ''
        if (not name in self.test_features):
            return gen
        gen += 'TEST_F(TestsCapabilitiesGenerated, Test' + name[16:] + ') {\n'
        for ext in value.definedByExtensions:
            gen += '#ifdef ' + registry.extensions[ext].name + '\n'

        var_name = self.create_var_name(name)
        gen += '    ' + name + ' ' + var_name + '{};\n'
        gen += '    ' + var_name + '.sType = ' + value.sType + ';\n\n'

        gen += '    VkPhysicalDeviceFeatures2 features;\n'
        gen += '    features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;\n'
        gen += '    features.pNext = &' + var_name + ';\n'
        gen += '    vkGetPhysicalDeviceFeatures2(gpu, &features);\n\n'
        
        for member in value.members:
            gen += '    EXPECT_EQ(' + var_name + '.' + member + ', VK_TRUE);\n'

        for ext in value.definedByExtensions:
            gen += '#endif\n'
        gen += '}\n\n'
        return gen

    def gen_format_test(self, name, values):
        gen = 'TEST_F(TestsCapabilitiesGenerated, Test' + name[2:] + ') {\n'
        gen += '    VkFormat format = ' + name + ';\n'
        gen += '    VkFormatProperties format_properties;\n'
        gen += '    vkGetPhysicalDeviceFormatProperties(gpu, format, &format_properties);\n\n'

        gen += '    VkFormatFeatureFlags linear_tiling_features = '
        first = True
        for feature in values[0]:
            if first:
                first = False
            else:
                gen += ' | '
            gen += feature
        if first:
            gen += '0'
        gen += ';\n'
        
        gen += '    VkFormatFeatureFlags optimal_tiling_features = '
        first = True
        for feature in values[1]:
            if first:
                first = False
            else:
                gen += ' | '
            gen += feature
        if first:
            gen += '0'
        gen += ';\n'
        
        gen += '    VkFormatFeatureFlags buffer_features = '
        first = True
        for feature in values[2]:
            if first:
                first = False
            else:
                gen += ' | '
            gen += feature
        if first:
            gen += '0'
        gen += ';\n'

        gen += '    EXPECT_EQ(format_properties.linearTilingFeatures & linear_tiling_features, linear_tiling_features);\n'
        gen += '    EXPECT_EQ(format_properties.optimalTilingFeatures & optimal_tiling_features, optimal_tiling_features);\n'
        gen += '    EXPECT_EQ(format_properties.bufferFeatures & buffer_features, buffer_features);\n'

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

if __name__ == '__main__':
    parser = argparse.ArgumentParser()

    parser.add_argument('-registry', action='store',
                        help='Use specified registry file instead of vk.xml')
    parser.add_argument('-outProfile', action='store',
                        help='Output profiles file')
    parser.add_argument('-outTests', action='store',
                        help='Output tests file')

    args = parser.parse_args()

    if args.registry is None or args.outProfile is None:
        parser.print_help()
        exit()

    registry = genvp.VulkanRegistry(args.registry)
    generator = ProfileGenerator()
    generator.generate_profile(args.outProfile, registry)
    generator.generate_tests(args.outTests, registry)
