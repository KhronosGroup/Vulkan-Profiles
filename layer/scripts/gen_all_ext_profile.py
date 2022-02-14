import os
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

class ProfileGenerator():
    i = 1
    skipped_properties = ["VkPhysicalDeviceMemoryProperties", "VkPhysicalDevicePointClippingProperties", "VkPhysicalDeviceToolPropertiesEXT", "VkPhysicalDeviceGroupPropertiesKHR"]
    skipped_members = ["sType", "pNext", "physicalDevices", "driverID"]

    def generate(self, outProfile, registry):
        with open(outProfile, 'w') as f:
            f.write(self.gen_privateImpl(registry))

    def gen_extensions(self, extensions, first):
        gen = ''
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

    def gen_features(self, extensions, registry, first):
        gen = ''
        for name, info in extensions.items():
            if (info.feature and info.feature in registry.features):
                if (first):
                    first = False
                else:
                    gen += ','
                gen += '\n                \"'
                gen += info.feature
                gen += '\": {'
                first_feature = True
                for feature in registry.features[info.feature]:
                    feature_name = feature[1]
                    if (feature_name == "sType" or feature_name == "pNext"):
                        continue
                    if first_feature:
                        first_feature = False
                    else:
                        gen += ','
                    gen += '\n'
                    if "VkPhysicalDeviceFeatures2"in info.feature:
                        gen += self.gen_physical_device_features(registry)
                    else:
                        gen += '                    \"'
                        gen += feature_name
                        gen += '\": true'
                gen += '\n'
                gen += '                }'
        return gen

    def get_sample_count_flags(self, registry):
        gen = '[ \"'
        gen += registry.sample_count_flags[self.i % len(registry.sample_count_flags)]
        gen += "\" ]"
        return gen

    def get_shader_float_controls_independence(self, registry):
        gen = "\""
        gen += registry.shader_float_controls_independence[self.i % len(registry.shader_float_controls_independence)]
        gen += "\""
        return gen

    def get_resolve_mode_flags(self, registry):
        gen = "[ \""
        gen += registry.resolve_mode_flags[self.i % len(registry.resolve_mode_flags)]
        gen += "\" ]"
        return gen

    def get_shader_stage_flags(self, registry):
        gen = "[ \""
        gen += registry.shader_stage_flags[self.i % len(registry.shader_stage_flags)]
        gen += "\" ]"
        return gen

    def get_sample_count_flags_bit(self, registry):
        gen = "\""
        gen += registry.sample_count_flags[self.i % len(registry.sample_count_flags)]
        gen += "\""
        return gen

    def gen_extend_2d(self):
        gen = '{\n'
        gen += '                        \"width\": '
        gen += str(self.i)
        gen += ',\n'
        gen += '                        \"height\": '
        gen += str(self.i + 1)
        gen += '\n                    }'
        return gen

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
        return gen

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
        return gen

    def gen_properties(self, extensions, first, registry):
        gen = ''
        for name, info in extensions.items():
            if (info.property and info.property in registry.properties):
                skip = False
                for skipped in self.skipped_properties:
                    if (info.property.startswith(skipped)):
                        skip = True
                        break
                if skip:
                    continue
                if first:
                    first = False
                else:
                    gen += ','
                gen += '\n                \"'
                gen += info.property
                gen += '\": {'
                first_property = True
                for property in registry.properties[info.property]:
                    property_type = property[0]
                    property_name = property[1]
                    property_size_str = property[2]
                    property_size = 1
                    if (property_size_str.isdigit()):
                        property_size = int(property_size_str)
                    elif property_size_str:
                        property_size = int(registry.constants[property_size_str][1])
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
                    elif property_type == "uint8_t":
                        if property_size > 1:
                            gen += "[ "
                        first = True
                        for i in range(property_size):
                            if first:
                                first = False
                            else:
                                gen += ", "
                            gen += str(self.i % 255)
                        if property_size > 1:
                            gen += " ]"
                        self.i += 1
                    elif property_type == "uint32_t" or property_type == "uint64_t" or property_type == "int64_t" or property_type == "VkDeviceSize":
                        if property_size > 1:
                            gen += "[ "
                        first = True
                        for i in range(property_size):
                            if first:
                                first = False
                            else:
                                gen += ", "
                            gen += str(self.i)
                        if property_size > 1:
                            gen += " ]"
                        self.i += 1
                    elif property_type == "float":
                        if property_size > 1:
                            gen += "[ "
                        first = True
                        for i in range(property_size):
                            if first:
                                first = False
                            else:
                                gen += ", "
                            gen += str(float(self.i))
                        if property_size > 1:
                            gen += " ]"
                        self.i += 1
                    elif property_type == "VkSampleCountFlags":
                        gen += self.get_sample_count_flags(registry)
                        self.i += 1
                    elif property_type == "VkShaderFloatControlsIndependence":
                        gen += self.get_shader_float_controls_independence(registry)
                        self.i += 1
                    elif property_type == "VkResolveModeFlags":
                        gen += self.get_resolve_mode_flags(registry)
                        self.i += 1
                    elif property_type == "VkShaderStageFlags":
                        gen += self.get_shader_stage_flags(registry)
                        self.i += 1
                    elif property_type == "VkSampleCountFlagBits":
                        gen += self.get_sample_count_flags_bit(registry)
                        self.i += 1
                    elif property_type == "VkShaderCorePropertiesFlagsAMD":
                        gen += "[]"
                    elif property_type == "VkExtent2D":
                        gen += self.gen_extend_2d()
                        self.i += 2
                    elif property_type == "VkExtent3D":
                        gen += self.gen_extend_3d()
                        self.i += 3
                    elif property_type == "VkConformanceVersion":
                        gen += self.get_conformance_version()
                        self.i += 4
                    elif property_type == "char":
                        gen += "\""
                        gen += property_name
                        gen += "\""
                    else:
                        gen += property_type
                gen += '\n'
                gen += '                }'
        return gen

    def pseudo_random(self, s):
        return Fraction(math.sin(s * 12.9898) * 43758.5453123)

    def gen_random_format_features(self, registry):
        # Choose from 0 to including 3 number of format features
        num = int(self.pseudo_random(self.i) * 1000) % 4
        self.i += 1
        gen = ''
        first = True
        for i in range(num):
            if first:
                first = False
            else:
                gen += ', '
            index = int(self.pseudo_random(i) * 1000) % len(registry.format_features)
            gen += '\"'
            gen += registry.format_features[index]
            gen += '\"'

        if num > 0:
            gen += ' '

        return gen

    def gen_formats(self, registry):
        gen = ''
        first = True
        for format in registry.formats:
            if (first):
                first = False
            else:
                gen += ','
            gen += '\n                \"'
            gen += format
            gen += '\": {\n'
            gen += '                    "VkFormatProperties": {\n'
            gen += '                        "linearTilingFeatures": [ '
            gen += self.gen_random_format_features(registry)
            gen += '],\n'
            gen += '                        "optimalTilingFeatures": [ '
            gen += self.gen_random_format_features(registry)
            gen += '],\n'
            gen += '                        "bufferFeatures": [ '
            gen += self.gen_random_format_features(registry)
            gen += ']\n'
            gen += '                    }\n'
            gen += '                }'
        return gen

    def gen_privateImpl(self, registry):
        gen = '{\n'
        gen += '    "$schema": "https://schema.khronos.org/vulkan/profiles-1.3.204.json#",\n'
        gen += '    "capabilities": {\n'
        gen += '        "baseline": {\n'
        gen += '            "extensions": {'
        gen += self.gen_extensions(registry.instance_extensions, True)
        gen += self.gen_extensions(registry.device_extensions, not bool(registry.instance_extensions))
        gen += '\n'
        gen += '            },\n'
        gen += '            "features": {'
        gen += self.gen_features(registry.instance_extensions, registry, True)
        gen += self.gen_features(registry.device_extensions, registry, not bool(registry.instance_extensions))
        gen += '\n            },\n'
        gen += '            "properties": {'
        gen += self.gen_properties(registry.instance_extensions, True, registry)
        gen += self.gen_properties(registry.device_extensions, not bool(registry.instance_extensions), registry)
        gen += '\n            },\n'
        gen += '            "formats": {'
        gen += self.gen_formats(registry)
        gen += '\n            }\n'
        gen += '        }\n'
        gen += '    },'
        gen += PROFILES
        gen += '}\n'
        return gen

class VulkanExtension():
    def __init__(self, spec_version, feature, property):
        self.spec_version = spec_version
        self.feature = feature
        self.property = property

class VulkanRegistry():
    def __init__(self, registryFile):
        xml = etree.parse(registryFile)
        self.parseAPIConstants(xml)
        self.parseStructInfo(xml)
        self.parseExtensionInfo(xml)

    def parseAPIConstants(self, xml):
        self.constants = dict()
        aliases = dict()
        for constant in xml.find("./enums[@name='API Constants']"):
            alias_name = constant.get('alias')
            name = constant.get('name')
            if alias_name:
                aliases[name] = alias_name
            else:
                type = constant.get('type')
                value = constant.get('value')
                self.constants[name] = ((type, value))
        
        for key, value in aliases.items():
            self.constants[key] = self.constants[value]

    def parseStructInfo(self, xml):
        self.features = dict()
        self.properties = dict()
        features_alias = dict()
        properties_alias = dict()
        for struct in xml.findall("./types/type[@category='struct']"):
            name = struct.get('name')
            extends = struct.get('structextends')
            if ("Features" in name):
                alias_name = struct.get('alias')
                if alias_name:
                    features_alias[name] = alias_name
                else:
                    if ("VkPhysicalDeviceFeatures" in name or (extends and "VkPhysicalDeviceFeatures2" in extends)):
                        self.features[name] = list()
                        for member in struct.findall('member'):
                            member_type = member.find('type').text
                            member_name = member.find('name').text
                            self.features[name].append((member_type, member_name))
            if ("Properties" in name):
                alias_name = struct.get('alias')
                if alias_name:
                    properties_alias[name] = alias_name
                else:
                    if ("VkPhysicalDeviceProperties" in name or (extends and "VkPhysicalDeviceProperties" in extends)):
                        self.properties[name] = list()
                        for member in struct.findall('member'):
                            member_type = member.find('type').text
                            member_name = member.find('name').text
                            member_size = ""
                            if (member.find('enum') != None):
                                member_size = member.find('enum').text
                            elif (member.find('name').tail != None):
                                member_size = member.find('name').tail[1:-1]
                            self.properties[name].append((member_type, member_name, member_size))

        for key, value in features_alias.items():
            if value in self.features:
                self.features[key] = self.features[value]
        for key, value in properties_alias.items():
            if value in self.properties:
                self.properties[key] = self.properties[value]

    def parseExtensionInfo(self, xml):
        self.instance_extensions = dict()
        self.device_extensions = dict()
        promoted_extensions = list()
        self.formats = list()
        self.format_features = list()
        self.sample_count_flags = list()
        self.shader_float_controls_independence = list()
        self.resolve_mode_flags = list()
        self.shader_stage_flags = list()

        for enum in xml.findall("./enums"):
            enum_name = enum.get('name')
            if (enum_name == "VkFormatFeatureFlagBits"):
                for value in enum.findall("enum"):
                    self.format_features.append(value.get('name'))
            if (enum_name == "VkSampleCountFlagBits"):
                for value in enum.findall("enum"):
                    self.sample_count_flags.append(value.get('name'))
            if (enum_name == "VkShaderFloatControlsIndependence"):
                for value in enum.findall("enum"):
                    self.shader_float_controls_independence.append(value.get('name'))
            if (enum_name == "VkResolveModeFlagBits"):
                for value in enum.findall("enum"):
                    self.resolve_mode_flags.append(value.get('name'))
            if (enum_name == "VkShaderStageFlagBits"):
                for value in enum.findall("enum"):
                    self.shader_stage_flags.append(value.get('name'))

        for ext in xml.findall("./extensions/extension"):
            # Only care about enabled device extensions
            if ext.get('supported') == "vulkan":
                name = ext.get('name')
                # Ignore NVX extensions
                if (name.find("NVX") != -1):
                    continue
                spec_version = 1
                feature = ""
                property = ""
                for e in ext.findall("./require/enum"):
                    if (e.get('name').endswith("SPEC_VERSION")):
                        spec_version = e.get('value')
                        break
                for e in ext.findall("./require/type"):
                    struct_name = e.get('name')
                    if (struct_name.startswith("VkPhysicalDevice") and "Features" in struct_name):
                        feature = struct_name
                    if (struct_name.startswith("VkPhysicalDevice") and "Properties" in struct_name):
                        property = struct_name

                for f in ext.findall("./require/enum"):
                    if f.get('extends') == "VkFormatFeatureFlagBits":
                        self.format_features.append(f.get('name'))

                if ext.get('promotedto'):
                    promoted_extensions.append(ext.get('promotedto'))
                if ext.get('type') == "instance":
                    self.instance_extensions[name] = VulkanExtension(spec_version, feature, property)
                elif ext.get('type') == "device":
                    self.device_extensions[name] = VulkanExtension(spec_version, feature, property)

        # Remove promoted extensions
        self.instance_extensions = {k:v for k,v in self.instance_extensions.items() if k not in promoted_extensions}
        self.device_extensions = {k:v for k,v in self.device_extensions.items() if k not in promoted_extensions}

        for format in xml.findall("./formats/format"):
            self.formats.append(format.get('name'))

if __name__ == '__main__':
    parser = argparse.ArgumentParser()

    parser.add_argument('-registry', action='store',
                        help='Use specified registry file instead of vk.xml')
    parser.add_argument('-outProfile', action='store',
                        help='Output profiles file')

    args = parser.parse_args()

    if args.registry is None or args.outProfile is None:
        parser.print_help()
        exit()

    registry = VulkanRegistry(args.registry)
    generator = ProfileGenerator()
    generator.generate(args.outProfile, registry)
