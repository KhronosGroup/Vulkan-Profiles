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
from typing import OrderedDict

from source.vulkan_registry import VulkanRegistry, struct_with_valid_dynamic_array, struct_with_dynamic_array_size_cap
from source.log import Log

class VulkanProfilesSchemaGenerator():
    def __init__(self, registry):
        self.registry = registry
        self.schema = self.gen_schema()

    def validate(self):
        try:
            import jsonschema
            Log.i("Validating JSON profiles schema...")
            jsonschema.Draft7Validator.check_schema(self.schema)
        except ModuleNotFoundError:
            Log.w("`jsonschema` module is not installed, schema validation skip")

    def generate(self, outSchema):
        Log.i("Generating '{0}'...".format(outSchema))
        with open(outSchema, 'w') as f:
            f.write(json.dumps(self.schema, indent=4))


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
        versionStr = str(self.registry.headerVersionNumber)

        return OrderedDict({
            "$schema": "http://json-schema.org/draft-07/schema#",
            "$id": "https://schema.khronos.org/vulkan/profiles-0.8.2-{0}.json#".format(str(self.registry.headerVersionNumber.patch)),
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
                            })
                        })
                    })
                })
            })
        })


    def gen_baseDefinitions(self):
        gen = OrderedDict({
            "status": OrderedDict({
                "description": "The development status of the setting. When missing, this property is inherited from parent nodes. If no parent node defines it, the default value is 'STABLE'.",
                "type": "string",
                "enum": [ "ALPHA", "BETA", "STABLE", "DEPRECATED" ]
            }),
            "contributor": OrderedDict({
                "type": "object",
                "additionalProperties": False,
                "required": [
                    "company"
                ],
                "properties": OrderedDict({
                    "company": OrderedDict({
                        "type": "string"
                    }),
                    "email": OrderedDict({
                        "type": "string",
                        "pattern": "^[A-Za-z0-9_.]+@[a-zA-Z0-9-].[a-zA-Z0-9-.]+$"
                    }),
                    "github": OrderedDict({
                        "type": "string",
                        "pattern": "^[A-Za-z0-9_-]+$"
                    }),
                    "contact": OrderedDict({
                        "type": "boolean"
                    })
                })
            }),
            "uint8_t": OrderedDict({
                "type": "integer",
                "minimum": 0,
                "maximum": 255
            }),
            "int32_t": OrderedDict({
                "type": "integer",
                "minimum": -2147483648,
                "maximum": 2147483647
            }),
            "uint32_t": OrderedDict({
                "type": "integer",
                "minimum": 0,
                "maximum": 4294967295
            }),
            "int64_t": OrderedDict({
                "type": "integer"
            }),
            "uint64_t": OrderedDict({
                "type": "integer",
                "minimum": 0
            }),
            "VkDeviceSize": OrderedDict({
                "type": "integer",
                "minimum": 0
            }),
            "char": {
                "type": "string"
            },
            "float": {
                "type": "number"
            },
            "size_t": OrderedDict({
                "type": "integer",
                "minimum": 0
            })
        })
        return gen


    def gen_extensions(self):
        gen = OrderedDict()
        for extName in sorted(self.registry.extensions.keys()):
            gen[extName] = { "type": "integer" }
        return gen


    def gen_type(self, type, definitions):
        if type == 'VkBool32':
            # Simple boolean
            gen = { "type": "boolean" }
        else:
            # All other types are referenced
            gen = { "$ref": "#/definitions/" + type }

        if gen.get("$ref") != None:
            # Generate referenced type, if needed
            if type in definitions:
                # Nothing to do, already defined
                pass
            elif type in self.registry.structs:
                # Generate structure definition
                self.gen_struct(type, definitions)
            elif type in self.registry.enums:
                # Generate enum definition
                self.gen_enum(type, definitions)
            elif type in self.registry.bitmasks:
                # Generate bitmask definition
                self.gen_bitmask(type, definitions)
            else:
                Log.f("Unknown type '{0}'".format(type))

        return gen


    def gen_array(self, type, size, sizeCap, definitions):
        arraySize = self.registry.evalArraySize(size)
        if isinstance(arraySize, list) and len(arraySize) == 1:
            # This is the last dimension of a multi-dimensional array
            # Treat it as one-dimensional from here on
            arraySize = arraySize[0]

        if type == 'char':
            # Character arrays should be handled as strings
            # We assume all are null-terminated, even though the vk.xml doesn't specify that
            # everywhere, but that's probably a bug rather than intentional
            return OrderedDict({
                "type": "string",
                "maxLength": arraySize - 1
            })
        elif isinstance(arraySize, list):
            # Multi-dimensional array
            return OrderedDict({
                "type": "array",
                "items": self.gen_array(type, arraySize[1:], None, definitions),
                "uniqueItems": False,
                # We don't have information from vk.xml to be able to tell what's the minimum
                # number of items that may need to be specified
                # "minItems": arraySize[0],
                "maxItems": arraySize[0]
            })
        else:
            # One-dimensional array
            return OrderedDict({
                "type": "array",
                "items": self.gen_type(type, definitions),
                "uniqueItems": False,
                # We don't have information from vk.xml to be able to tell what's the minimum
                # number of items that may need to be specified
                # "minItems": arraySize,
                "maxItems": arraySize if arraySize is not None else sizeCap
            })


    def gen_enum(self, name, definitions):
        enumDef = self.registry.enums[name]

        if len(enumDef.values) > 0:
            values = sorted(enumDef.values)
        else:
            # If the enum has no values then we must add a dummy one
            # in order to produce a valid JSON schema
            values = [ 0 ]

        # Generate definition
        definitions[name] = OrderedDict({
            "enum": values
        })


    def gen_bitmask(self, name, definitions):
        bitmaskDef = self.registry.bitmasks[name]

        if bitmaskDef.bitsType != None:
            # Also generate corresponding bits enum
            self.gen_enum(bitmaskDef.bitsType.name, definitions)
            itemType = { "$ref": "#/definitions/" + bitmaskDef.bitsType.name }
        else:
            # If the bitmask has no bits type then we must add a dummy
            # item type with a single dummy value
            itemType = { "enum": [ 0 ] }

        # Generate definition
        definitions[name] = OrderedDict({
            "type": "array",
            "items": itemType,
            "uniqueItems": True
        })


    def gen_struct(self, name, definitions):
        structDef = self.registry.structs[name]

        # Generate member data
        members = OrderedDict()
        for memberName in sorted(structDef.members.keys()):
            memberDef = structDef.members[memberName]

            if memberDef.type in self.registry.externalTypes and not memberDef.type in definitions:
                # Members with types defined externally and aren't manually defined are ignored
                Log.w("Ignoring member '{0}' in struct '{1}' with external type '{2}'".format(memberName, name, memberDef.type))
                continue

            if memberDef.isArray:
                if memberDef.arraySizeMember != None and name not in struct_with_valid_dynamic_array and name not in struct_with_dynamic_array_size_cap:
                    # This array is a dynamic one (count + pointer to array) which is not allowed
                    # for return structures. Such structures hence are ill-formed and shouldn't
                    # be included in the schema
                    Log.w("Ignoring member '{0}' in struct '{1}' containing ill-formed pointer to array".format(memberName, name))
                else:
                    if memberDef.arraySizeMember != None and name in struct_with_dynamic_array_size_cap:
                        Log.w("Member '{0}' in struct '{1}' is a pointer to array with a known maximum size, it will be ignored in the API library, but supported in the layer".format(memberName, name))
                    members[memberDef.name] = self.gen_array(memberDef.type, memberDef.arraySize, memberDef.arraySizeCap, definitions)
            else:
                members[memberDef.name] = self.gen_type(memberDef.type, definitions)

        # Generate definition
        definitions[name] = OrderedDict({
            "type": "object",
            "additionalProperties": False,
            "properties": members
        })


    def gen_structChainDefinitions(self, basename, definitions):
        structNames = [ basename ]
        if basename + '2' in self.registry.structs:
            # Structure has version 2 which is extensible
            basename += '2'
            structNames.append(basename)

        # Collect unique chainable structures (ignoring aliases)
        for structName in sorted(self.registry.structs.keys()):
            structDef = self.registry.structs[structName]
            if not structDef.isAlias and basename in structDef.extends:
                structNames.append(structName)

        # Generate structure definitions and references
        gen = OrderedDict()
        for structName in structNames:
            # Add structure definition and reference
            self.gen_struct(structName, definitions)
            gen[structName] = { "$ref": "#/definitions/" + structName }

            # Add structure references for all alises
            for alias in self.registry.structs[structName].aliases:
                if alias != structName:
                    gen[alias] = gen[structName]

        return gen


    def gen_features(self, definitions):
        return self.gen_structChainDefinitions("VkPhysicalDeviceFeatures", definitions)


    def gen_properties(self, definitions):
        return self.gen_structChainDefinitions("VkPhysicalDeviceProperties", definitions)


    def gen_formats(self, definitions):
        # Add definition for format properties
        definitions['formatProperties'] = OrderedDict({
            "type": "object",
            "additionalProperties": False,
            "properties": self.gen_structChainDefinitions("VkFormatProperties", definitions)
        })

        # Generate references to the format properties definition for each format
        gen = OrderedDict()
        for format in sorted(self.registry.enums['VkFormat'].values):
            gen[format] = OrderedDict({
                "$ref": "#/definitions/formatProperties"
            })
        return gen


    def gen_queueFamilies(self, definitions):
        return self.gen_structChainDefinitions("VkQueueFamilyProperties", definitions)


    def gen_videoProfiles(self, definitions):
        # We do not want to include usage hint structures in the schema
        # as those are only usage scenario customizations and do not affect capabilities
        excludedSet = {
            "VkVideoDecodeUsageInfoKHR",
            "VkVideoEncodeUsageInfoKHR"
        }
        videoProfiles = self.gen_structChainDefinitions("VkVideoProfileInfoKHR", definitions)
        for excluded in excludedSet:
            excluded = self.registry.getNonAliasTypeName(excluded, self.registry.structs)
            if excluded in videoProfiles:
                del videoProfiles[excluded]
            # Check also any aliases
            for alias in self.registry.structs[excluded].aliases:
                if alias in videoProfiles:
                    del videoProfiles[excluded]

        return videoProfiles


    def gen_videoCapabilities(self, definitions):
        return self.gen_structChainDefinitions("VkVideoCapabilitiesKHR", definitions)


    def gen_videoFormats(self, definitions):
        return self.gen_structChainDefinitions("VkVideoFormatPropertiesKHR", definitions)
