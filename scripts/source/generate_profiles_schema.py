#!/usr/bin/python3
#
# Copyright (c) 2021-2026 LunarG, Inc.
# Copyright (c) 2023-2024 RasterGrid Kft.
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
# - Daniel Rakos <daniel.rakos@rastergrid.com>
# - Christophe Riccio <christophe@lunarg.com>

import json
import logging
from collections import OrderedDict

from source.vulkan_object_utils import gatherDynamicStructs

EXTERNAL_TYPES = {
    "Display", "VisualID", "Window", "ANativeWindow", "wl_display", "wl_surface",
    "_XDisplay", "HINSTANCE", "HWND", "HANDLE", "DWORD", "LPCWSTR", "zx_handle_t",
    "GgpStreamDescriptor", "GgpFrameToken", "CAMetalLayer", "SECURITY_ATTRIBUTES"
}

class VulkanProfilesSchemaGenerator2():
    def __init__(self, vk):
        """
        :param vk: An instance of VulkanObject from vulkan_object.py
        """
        self.vk = vk
        
        # Call the global discovery helper passing the VulkanObject parameter
        self.valid_dynamic_structs = gatherDynamicStructs(vk)
        self.schema = self.gen_schema()

    def validate(self):
        try:
            import jsonschema
            logging.info("Validating JSON profiles schema...")
            jsonschema.Draft7Validator.check_schema(self.schema)
        except ModuleNotFoundError:
            logging.warning("`jsonschema` module is not installed, schema validation skip")

    def generate(self, outSchema):
        logging.info("Generating '{0}'...".format(outSchema))
        with open(outSchema, 'w') as f:
            f.write(json.dumps(self.schema, indent=4))

    def getNonAliasTypeName(self, name):
        if name in self.vk.structs:
            return name
        for struct_name, struct_def in self.vk.structs.items():
            if name in struct_def.aliases:
                return struct_name
        return name

    def evalArraySize(self, size):
        if size is None:
            return None
        if isinstance(size, list):
            return [self.evalArraySize(s) for s in size]
        if isinstance(size, int):
            return size
        if isinstance(size, str):
            if size in self.vk.constants:
                val = self.vk.constants[size].value
                try:
                    return int(val)
                except (ValueError, TypeError):
                    return val
            try:
                clean_str = size.rstrip('UuLl')
                if clean_str.startswith('0x') or clean_str.startswith('0X'):
                    return int(clean_str, 16)
                return int(clean_str)
            except ValueError:
                return size
        return size

    def gen_schema(self):
        definitions = self.gen_baseDefinitions()
        extensions = self.gen_extensions()
        features = self.gen_features(definitions)
        properties = self.gen_properties(definitions)
        formats = self.gen_formats(definitions)
        queueFamilies = self.gen_queueFamilies(definitions)
        videoProfiles = self.gen_videoProfiles(definitions)
        videoCapabilities = self.gen_videoCapabilities(definitions)
        videoFormats = self.gen_videoFormats(definitions)
        
        versionStr = self.vk.headerVersionComplete

        return OrderedDict({
            "$schema": "http://json-schema.org/draft-07/schema#",
            "$id": "https://schema.khronos.org/vulkan/profiles-0.8.2-{0}.json#".format(self.vk.headerVersion),
            "title": "Vulkan Profiles Schema for Vulkan {0}".format(versionStr),
            "additionalProperties": True,
            "required": [
                "capabilities",
                "profiles"
            ],
            "definitions": definitions,
            "properties": OrderedDict({
                "capabilities": OrderedDict({
                    "description": "The block that specifies the list of capabilities sets.",
                    "type": "object",
                    "additionalProperties": OrderedDict({
                        "type": "object",
                        "additionalProperties": False,
                        "properties": OrderedDict({
                            "extensions": OrderedDict({
                                "description": "The block that stores required extensions.",
                                "type": "object",
                                "additionalProperties": False,
                                "properties": extensions
                            }),
                            "features": OrderedDict({
                                "description": "The block that stores features requirements.",
                                "type": "object",
                                "additionalProperties": False,
                                "properties": features
                            }),
                            "properties": OrderedDict({
                                "description": "The block that stores properties requirements.",
                                "type": "object",
                                "additionalProperties": False,
                                "properties": properties
                            }),
                            "formats": OrderedDict({
                                "description": "The block that store formats capabilities definitions.",
                                "type": "object",
                                "additionalProperties": False,
                                "properties": formats
                            }),
                            "queueFamiliesProperties": OrderedDict({
                                "type": "array",
                                "uniqueItems": True,
                                "items": OrderedDict({
                                    "type": "object",
                                    "additionalProperties": False,
                                    "properties": queueFamilies
                                })
                            }),
                            "videoProfiles": OrderedDict({
                                "type": "array",
                                "uniqueItems": True,
                                "items": OrderedDict({
                                    "type": "object",
                                    "additionalProperties": False,
                                    "properties": OrderedDict({
                                        "profile": OrderedDict({
                                            "type": "object",
                                            "additionalProperties": False,
                                            "properties": videoProfiles
                                        }),
                                        "capabilities": OrderedDict({
                                            "type": "object",
                                            "addationalProperties": False,
                                            "properties": videoCapabilities
                                        }),
                                        "formats": OrderedDict({
                                            "type": "array",
                                            "uniqueItems": True,
                                            "items": OrderedDict({
                                                "type": "object",
                                                "additionalProperties": False,
                                                "properties": videoFormats
                                            })
                                        })
                                    })
                                })
                            })
                        })
                    })
                }),
                "profiles": OrderedDict({
                    "description": "The list of profile definitions.",
                    "type": "object",
                    "additionalProperties": False,
                    "patternProperties": OrderedDict({
                        "^VP_[A-Z0-9]+_[A-Za-z0-9_]+": OrderedDict({
                            "type": "object",
                            "additionalProperties": False,
                            "required": [
                                "label",
                                "description",
                                "version",
                                "api-version",
                                "capabilities"
                            ],
                            "properties": OrderedDict({
                                "version": OrderedDict({
                                    "description": "The revision of the profile.",
                                    "type": "integer"
                                }),
                                "label": OrderedDict({
                                    "description": "The label used to present the profile to the Vulkan developer.",
                                    "type": "string"
                                }),
                                "description": OrderedDict({
                                    "description": "The description of the profile.",
                                    "type": "string"
                                }),
                                "status": OrderedDict({
                                    "description": "The developmet status of the profile: ALPHA, BETA, STABLE or DEPRECATED.",
                                    "$ref": "#/definitions/status"
                                }),
                                "api-version": OrderedDict({
                                    "description": "The Vulkan API version against which the profile is written.",
                                    "type": "string",
                                    "pattern": "^[0-9]+.[0-9]+.[0-9]+$"
                                }),
                                "contributors": OrderedDict({
                                    "type": "object",
                                    "description": "The list of contributors of the profile.",
                                    "additionalProperties": OrderedDict({
                                        "$ref": "#/definitions/contributor"
                                    })
                                }),
                                "history": OrderedDict({
                                    "description": "The version history of the profile file",
                                    "type": "array",
                                    "uniqueItems": True,
                                    "minItems": 1,
                                    "items": OrderedDict({
                                        "type": "object",
                                        "required": [
                                            "revision",
                                            "date",
                                            "author",
                                            "comment"
                                        ],
                                        "properties": OrderedDict({
                                            "revision": OrderedDict({
                                                "type": "integer"
                                            }),
                                            "date": OrderedDict({
                                                "type": "string",
                                                "pattern": "((?:19|20)\\d\\d)-(0?[1-9]|1[012])-([12][0-9]|3[01]|0?[1-9])"
                                            }),
                                            "author": OrderedDict({
                                                "type": "string"
                                            }),
                                            "comment": OrderedDict({
                                                "type": "string"
                                            })
                                        })
                                    })
                                }),
                                "profiles": OrderedDict({
                                    "description": "The list of required profiles by the profile.",
                                    "type": "array",
                                    "additionalProperties": False,
                                    "uniqueItems": True,
                                    "items": OrderedDict({
                                        "type": "string"
                                    })
                                }),
                                "capabilities": OrderedDict({
                                    "description": "The list of required capability sets that can be referenced by a profile.",
                                    "type": "array",
                                    "uniqueItems": True,
                                    "items": OrderedDict({
                                        "anyOf": [
                                            {
                                                "type": "string"
                                            },
                                            {
                                                "type": "array",
                                                "uniqueItems": True,
                                                "items": OrderedDict({
                                                    "type": "string"
                                                })
                                            }
                                        ]
                                    })
                                }),
                                "optionals": OrderedDict({
                                    "description": "The list of optional capability sets that can be referenced by a profile.",
                                    "type": "array",
                                    "uniqueItems": True,
                                    "items": OrderedDict({
                                        "anyOf": [
                                            {
                                                "type": "string"
                                            },
                                            {
                                                "type": "array",
                                                "uniqueItems": True,
                                                "items": OrderedDict({
                                                    "type": "string"
                                                })
                                            }
                                        ]
                                    })
                                }),
                                "fallback": OrderedDict({
                                    "description": "The list of profiles recommended if the checked profile is not supported by the platform.",
                                    "type": "array",
                                    "additionalProperties": False,
                                    "uniqueItems": True,
                                    "items": OrderedDict({
                                        "type": "string"
                                    })
                                })
                            })
                        })
                    })
                })
            })
        })

    def gen_baseDefinitions(self):
        return OrderedDict({
            "status": OrderedDict({
                "description": "The development status of the setting. When missing, this property is inherited from parent nodes. If no parent node defines it, the default value is 'STABLE'.",
                "type": "string",
                "enum": [ "ALPHA", "BETA", "STABLE", "DEPRECATED" ]
            }),
            "contributor": OrderedDict({
                "type": "object",
                "additionalProperties": False,
                "required": [ "company" ],
                "properties": OrderedDict({
                    "company": OrderedDict({ "type": "string" }),
                    "email": OrderedDict({ "type": "string", "pattern": "^[A-Za-z0-9_.]+@[a-zA-Z0-9-].[a-zA-Z0-9-.]+$" }),
                    "github": OrderedDict({ "type": "string", "pattern": "^[A-Za-z0-9_-]+$" }),
                    "contact": OrderedDict({ "type": "boolean" })
                })
            }),
            "uint8_t": OrderedDict({ "type": "integer", "minimum": 0, "maximum": 255 }),
            "int32_t": OrderedDict({ "type": "integer", "minimum": -2147483648, "maximum": 2147483647 }),
            "uint32_t": OrderedDict({ "type": "integer", "minimum": 0, "maximum": 4294967295 }),
            "int64_t": OrderedDict({ "type": "integer" }),
            "uint64_t": OrderedDict({ "type": "integer", "minimum": 0 }),
            "VkDeviceSize": OrderedDict({ "type": "integer", "minimum": 0 }),
            "char": { "type": "string" },
            "float": { "type": "number" },
            "size_t": OrderedDict({ "type": "integer", "minimum": 0 })
        })

    def gen_extensions(self):
        gen = OrderedDict()
        for extName in sorted(self.vk.extensions.keys()):
            gen[extName] = { "type": "integer" }
        return gen

    def gen_type(self, type_name, definitions):
        if type_name == 'VkBool32':
            return { "type": "boolean" }
            
        gen = { "$ref": "#/definitions/" + type_name }

        if type_name in definitions:
            pass
        elif type_name in self.vk.structs:
            self.gen_struct(type_name, definitions)
        elif self.vk.videoStd and type_name in self.vk.videoStd.structs:
            self.gen_struct(type_name, definitions, is_video_std=True)
        elif type_name in self.vk.enums:
            self.gen_enum(type_name, definitions)
        elif self.vk.videoStd and type_name in self.vk.videoStd.enums:
            self.gen_enum(type_name, definitions, is_video_std=True)
        elif type_name in self.vk.bitmasks or type_name in self.vk.flags:
            self.gen_bitmask(type_name, definitions)
        else:
            if type_name not in definitions:
                return { "type": "integer" }

        return gen

    def gen_array(self, type_name, size, sizeCap, definitions, len_attribute=None):
        arraySize = self.evalArraySize(size)
        if isinstance(arraySize, list) and len(arraySize) == 1:
            arraySize = arraySize[0]

        if type_name == 'char':
            max_len = arraySize - 1 if isinstance(arraySize, int) else 256
            return OrderedDict({
                "type": "string",
                "maxLength": max(0, max_len)
            })
        elif isinstance(arraySize, list):
            return OrderedDict({
                "type": "array",
                "items": self.gen_array(type_name, arraySize[1:], None, definitions, len_attribute),
                "uniqueItems": False,
                "maxItems": arraySize[0]
            })
        else:
            is_enum_group = (type_name in self.vk.enums or 
                             type_name in self.vk.bitmasks or 
                             type_name in self.vk.flags or 
                             (self.vk.videoStd and (type_name in self.vk.videoStd.enums)))
            
            res = OrderedDict({
                "type": "array",
                "items": self.gen_type(type_name, definitions),
                "uniqueItems": True if is_enum_group else False
            })

            max_items = arraySize if isinstance(arraySize, int) else sizeCap
            if max_items is not None and len_attribute is None:
                res["maxItems"] = max_items

            if len_attribute is not None:
                res["description"] = f"The number of items is determined by {len_attribute}"

            return res

    def gen_enum(self, name, definitions, is_video_std=False):
        if name in definitions:
            return
        enumDef = self.vk.videoStd.enums[name] if is_video_std else self.vk.enums[name]
        values = []
        for field in enumDef.fields:
            values.append(field.name)
            for alias in field.aliases:
                values.append(alias)
                
        values = sorted(list(set(values)))
        if len(values) == 0:
            values = [ 0 ]

        definitions[name] = OrderedDict({ "enum": values })

    def gen_bitmask_enum(self, name, definitions):
        if name in definitions:
            return
        bitmaskDef = self.vk.bitmasks[name]
        values = []
        for flag in bitmaskDef.flags:
            values.append(flag.name)
            for alias in flag.aliases:
                values.append(alias)
                
        values = sorted(list(set(values)))
        if len(values) == 0:
            values = [ 0 ]

        definitions[name] = OrderedDict({ "enum": values })

    def gen_bitmask(self, name, definitions):
        if name in definitions:
            return
            
        if name in self.vk.flags:
            bitmask_name = self.vk.flags[name].bitmaskName
            if bitmask_name and bitmask_name in self.vk.bitmasks:
                self.gen_bitmask_enum(bitmask_name, definitions)
                itemType = { "$ref": "#/definitions/" + bitmask_name }
            else:
                itemType = { "enum": [ 0 ] }

            definitions[name] = OrderedDict({
                "type": "array",
                "items": itemType,
                "uniqueItems": True
            })
        elif name in self.vk.bitmasks:
            self.gen_bitmask_enum(name, definitions)

    def gen_struct(self, name, definitions, is_video_std=False):
        if name in definitions:
            return
        structDef = self.vk.videoStd.structs[name] if is_video_std else self.vk.structs[name]
        
        # Parity Rule 1: Skip structures containing unhandled dynamic pointer arrays using our automated helper list
        for memberDef in structDef.members:
            if memberDef.length is not None and memberDef.pointer:
                if name not in self.valid_dynamic_structs:
                    return

        members = OrderedDict()
        for memberDef in sorted(structDef.members, key=lambda m: m.name):
            # Parity Rule 3: Skip structural runtime pointer metadata
            if memberDef.name in ['sType', 'pNext']:
                continue

            if memberDef.type in EXTERNAL_TYPES and not memberDef.type in definitions:
                continue

            # Parity Rule 2: Discern arrays accurately using vulkan_object definitions
            is_fixed_array = len(memberDef.fixedSizeArray) > 0
            is_dynamic_array = memberDef.length is not None and memberDef.pointer

            if is_fixed_array:
                size = memberDef.fixedSizeArray
                members[memberDef.name] = self.gen_array(memberDef.type, size, None, definitions)
            elif is_dynamic_array:
                array_size_member = memberDef.length
                size_cap = 1 if name in self.valid_dynamic_structs else None
                
                members[memberDef.name] = self.gen_array(
                    memberDef.type, 
                    array_size_member, 
                    size_cap, 
                    definitions, 
                    len_attribute=array_size_member
                )
            else:
                members[memberDef.name] = self.gen_type(memberDef.type, definitions)

        definitions[name] = OrderedDict({
            "type": "object",
            "additionalProperties": False,
            "properties": members
        })

    def gen_structChainDefinitions(self, basename, definitions):
        structNames = [ basename ]
        if basename + '2' in self.vk.structs:
            basename += '2'
            structNames.append(basename)

        for structName in sorted(self.vk.structs.keys()):
            structDef = self.vk.structs[structName]
            if basename in structDef.extends:
                structNames.append(structName)

        gen = OrderedDict()
        for structName in structNames:
            self.gen_struct(structName, definitions)
            if structName in definitions:
                gen[structName] = { "$ref": "#/definitions/" + structName }

                for alias in self.vk.structs[structName].aliases:
                    if alias != structName:
                        gen[alias] = gen[structName]

        return gen

    def gen_features(self, definitions):
        return self.gen_structChainDefinitions("VkPhysicalDeviceFeatures", definitions)

    def gen_properties(self, definitions):
        return self.gen_structChainDefinitions("VkPhysicalDeviceProperties", definitions)

    def gen_formats(self, definitions):
        definitions['formatProperties'] = OrderedDict({
            "type": "object",
            "additionalProperties": False,
            "properties": self.gen_structChainDefinitions("VkFormatProperties", definitions)
        })

        gen = OrderedDict()
        format_values = []
        if 'VkFormat' in self.vk.enums:
            for field in self.vk.enums['VkFormat'].fields:
                format_values.append(field.name)
                for alias in field.aliases:
                    format_values.append(alias)
                
        for format_name in sorted(list(set(format_values))):
            gen[format_name] = OrderedDict({
                "$ref": "#/definitions/formatProperties"
            })
        return gen

    def gen_queueFamilies(self, definitions):
        return self.gen_structChainDefinitions("VkQueueFamilyProperties", definitions)

    def gen_videoProfiles(self, definitions):
        excludedSet = { "VkVideoDecodeUsageInfoKHR", "VkVideoEncodeUsageInfoKHR" }
        videoProfiles = self.gen_structChainDefinitions("VkVideoProfileInfoKHR", definitions)
        for excluded in excludedSet:
            excluded = self.getNonAliasTypeName(excluded)
            if excluded in videoProfiles:
                del videoProfiles[excluded]
            if excluded in self.vk.structs:
                for alias in self.vk.structs[excluded].aliases:
                    if alias in videoProfiles:
                        del videoProfiles[alias]
        return videoProfiles

    def gen_videoCapabilities(self, definitions):
        return self.gen_structChainDefinitions("VkVideoCapabilitiesKHR", definitions)

    def gen_videoFormats(self, definitions):
        return self.gen_structChainDefinitions("VkVideoFormatPropertiesKHR", definitions)
    