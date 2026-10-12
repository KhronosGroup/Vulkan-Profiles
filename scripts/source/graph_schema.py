#!/usr/bin/python3
#
# Copyright (c) 2026-2026 Google, Inc.
# Copyright (C) 2026-2026 Valve Corporation
# Copyright (c) 2026-2026 LunarG, Inc.
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
# - Christophe Riccio <christophe@lunarg.com>

GRAPH_JSON_SCHEMA = {
    "$schema": "http://json-schema.org/draft-07/schema#",
    "$id": "https://schema.khronos.org/vulkan/profiles-graph.json#",
    "title": "Vulkan Profiles Operation Graph Schema",
    "description": "Schema for vkprofiles execution graph files.",
    "type": "object",
    "required": ["nodes"],
    "properties": {
        "$schema": { "type": "string" },
        "variables": {
            "type": "object",
            "additionalProperties": { "type": "string" }
        },
        "nodes": {
            "type": "array",
            "items": {
                "type": "object",
                "required": ["id", "command", "args"],
                "properties": {
                    "id": { "type": "string" },
                    "description": { "type": "string" },
                    "ignore": { "type": "boolean" },
                    "depends_on": {
                        "type": "array",
                        "items": { "type": "string" }
                    },
                    "command": {
                        "type": "string",
                        "enum": [
                            "schema", "transform", "combine", "validate",
                            "library", "doc", "extract", "min-api-version",
                            "summary", "layer", "tests"
                        ]
                    },
                    "args": { "type": "object" }
                },
                "additionalProperties": False,
                "oneOf": [
                    {
                        "properties": {
                            "command": { "const": "schema" },
                            "args": { "$ref": "#/definitions/schema_args" }
                        }
                    },
                    {
                        "properties": {
                            "command": { "const": "transform" },
                            "args": { "$ref": "#/definitions/transform_args" }
                        }
                    },
                    {
                        "properties": {
                            "command": { "const": "combine" },
                            "args": { "$ref": "#/definitions/combine_args" }
                        }
                    },
                    {
                        "properties": {
                            "command": { "const": "validate" },
                            "args": { "$ref": "#/definitions/validate_args" }
                        }
                    },
                    {
                        "properties": {
                            "command": { "const": "library" },
                            "args": { "$ref": "#/definitions/library_args" }
                        }
                    },
                    {
                        "properties": {
                            "command": { "const": "doc" },
                            "args": { "$ref": "#/definitions/doc_args" }
                        }
                    },
                    {
                        "properties": {
                            "command": { "const": "extract" },
                            "args": { "$ref": "#/definitions/extract_args" }
                        }
                    },
                    {
                        "properties": {
                            "command": { "const": "min-api-version" },
                            "args": { "$ref": "#/definitions/min_api_version_args" }
                        }
                    },
                    {
                        "properties": {
                            "command": { "const": "summary" },
                            "args": { "$ref": "#/definitions/summary_args" }
                        }
                    },
                    {
                        "properties": {
                            "command": { "const": "layer" },
                            "args": { "$ref": "#/definitions/layer_args" }
                        }
                    },
                    {
                        "properties": {
                            "command": { "const": "tests" },
                            "args": { "$ref": "#/definitions/tests_args" }
                        }
                    }
                ]
            }
        }
    },
    "additionalProperties": False,
    "definitions": {
        "output_format": {
            "type": "string",
            "enum": ["pretty", "flatten", "compact"]
        },
        "pull_bits": {
            "type": "string",
            "enum": [
                "required-capabilities",
                "promoted-extensions",
                "aliases",
                "ignore-extension-versions",
                "ignore-unsupported",
                "ignore-subcore-capabilities",
                "ignore-unratified"
            ]
        },
        "strip_bits": {
            "type": "string",
            "enum": [
                "helper-values",
                "duplication",
                "promoted-extensions",
                "unratified"
            ]
        },
        "analyse_bits": {
            "type": "string",
            "enum": [
                "api-version",
                "extensions",
                "aliases",
                "unratified"
            ]
        },
        "common_args": {
            "properties": {
                "registry": { "type": "string" },
                "verbose": { "type": "boolean" },
                "quiet": { "type": "boolean" },
                "log": {
                    "type": "array",
                    "items": {
                        "type": "string",
                        "enum": ["none", "all", "info", "warning", "error", "critical"]
                    }
                }
            }
        },
        "schema_args": {
            "type": "object",
            "properties": {
                "registry": { "type": "string" },
                "input": { "type": "string" },
                "output": { "type": "string" },
                "upgrade": { "type": "boolean" },
                "format": { "$ref": "#/definitions/output_format" },
                "api": { "type": "string", "enum": ["vulkan"] },
                "verbose": { "type": "boolean" },
                "quiet": { "type": "boolean" },
                "log": { "$ref": "#/definitions/common_args/properties/log" }
            },
            "additionalProperties": False
        },
        "transform_args": {
            "type": "object",
            "properties": {
                "registry": { "type": "string" },
                "input": { "type": "string" },
                "input-profiles": { "type": "string" },
                "output": { "type": "string" },
                "format": { "$ref": "#/definitions/output_format" },
                "pull": {
                    "type": "array",
                    "items": { "$ref": "#/definitions/pull_bits" }
                },
                "consolidate": { "type": "boolean" },
                "strip": {
                    "type": "array",
                    "items": { "$ref": "#/definitions/strip_bits" }
                },
                "sort": { "type": "boolean" },
                "api": { "type": "string", "enum": ["vulkan"] },
                "verbose": { "type": "boolean" },
                "quiet": { "type": "boolean" },
                "log": { "$ref": "#/definitions/common_args/properties/log" }
            },
            "additionalProperties": False
        },
        "combine_args": {
            "type": "object",
            "properties": {
                "registry": { "type": "string" },
                "input": { "type": "string" },
                "input-profiles": { "type": "string" },
                "output": { "type": "string" },
                "profile-name": { "type": "string" },
                "profile-version": { "type": ["string", "integer"] },
                "profile-label": { "type": "string" },
                "profile-desc": { "type": "string" },
                "profile-date": { "type": "string" },
                "profile-api-version": { "type": "string" },
                "profile-stage": { "type": "string", "enum": ["ALPHA", "BETA", "STABLE"] },
                "profile-required-profiles": { "type": "string" },
                "mode": { "type": "string", "enum": ["intersection", "union", "difference", "limittype"] },
                "format": { "$ref": "#/definitions/output_format" },
                "contributors": { "type": "object" },
                "history": { "type": ["array", "object", "string"] },
                "profiles": { "type": "object" },
                "api": { "type": "string", "enum": ["vulkan"] },
                "verbose": { "type": "boolean" },
                "quiet": { "type": "boolean" },
                "log": { "$ref": "#/definitions/common_args/properties/log" }
            },
            "additionalProperties": False
        },
        "validate_args": {
            "type": "object",
            "properties": {
                "registry": { "type": "string" },
                "input": { "type": "string" },
                "schema": { "type": "string" },
                "analyse": {
                    "type": "array",
                    "items": { "$ref": "#/definitions/analyse_bits" }
                },
                "api": { "type": "string", "enum": ["vulkan"] },
                "verbose": { "type": "boolean" },
                "quiet": { "type": "boolean" },
                "log": { "$ref": "#/definitions/common_args/properties/log" }
            },
            "additionalProperties": False
        },
        "library_args": {
            "type": "object",
            "properties": {
                "registry": { "type": "string" },
                "input": { "type": "string" },
                "input-profiles": { "type": "string" },
                "output": { "type": "string" },
                "output-inc": { "type": "string" },
                "output-src": { "type": "string" },
                "output-filename": { "type": "string" },
                "mode": {
                    "type": "array",
                    "items": { "type": "string", "enum": ["header-only", "header+source"] }
                },
                "strip": { "type": "boolean" },
                "intermediate": { "type": "string" },
                "debug": { "type": "boolean" },
                "config": { "type": "string", "enum": ["release", "debug"] },
                "include-header": { "type": "string" },
                "api": { "type": "string", "enum": ["vulkan"] },
                "verbose": { "type": "boolean" },
                "quiet": { "type": "boolean" },
                "log": { "$ref": "#/definitions/common_args/properties/log" }
            },
            "additionalProperties": False
        },
        "doc_args": {
            "type": "object",
            "properties": {
                "registry": { "type": "string" },
                "input": { "type": "string" },
                "input-profiles": { "type": "string" },
                "output": { "type": "string" },
                "api": { "type": "string", "enum": ["vulkan"] },
                "verbose": { "type": "boolean" },
                "quiet": { "type": "boolean" },
                "log": { "$ref": "#/definitions/common_args/properties/log" }
            },
            "additionalProperties": False
        },
        "extract_args": {
            "type": "object",
            "properties": {
                "registry": { "type": "string" },
                "input": { "type": "string" },
                "output": { "type": "string" },
                "input-profiles": { "type": "string" },
                "mode": { "type": "string", "enum": ["reference-required-profiles", "pull-required-profiles"] },
                "format": { "$ref": "#/definitions/output_format" },
                "verbose": { "type": "boolean" },
                "quiet": { "type": "boolean" },
                "log": { "$ref": "#/definitions/common_args/properties/log" }
            },
            "additionalProperties": False
        },
        "min_api_version_args": {
            "type": "object",
            "properties": {
                "registry": { "type": "string" },
                "input": { "type": "string" },
                "input-profiles": { "type": "string" },
                "output": { "type": "string" },
                "schemas": { "type": "string" },
                "mode": { "type": "string", "enum": ["display", "evaluate"] },
                "format": { "$ref": "#/definitions/output_format" },
                "verbose": { "type": "boolean" },
                "quiet": { "type": "boolean" },
                "log": { "$ref": "#/definitions/common_args/properties/log" }
            },
            "additionalProperties": False
        },
        "summary_args": {
            "type": "object",
            "properties": {
                "registry": { "type": "string" },
                "input": { "type": "string" },
                "input-profiles": { "type": "string" },
                "output": { "type": "string" },
                "verbose": { "type": "boolean" },
                "quiet": { "type": "boolean" },
                "log": { "$ref": "#/definitions/common_args/properties/log" }
            },
            "additionalProperties": False
        },
        "layer_args": {
            "type": "object",
            "properties": {
                "registry": { "type": "string" },
                "output": { "type": "string" },
                "out-layer": { "type": "string" },
                "api": { "type": "string", "enum": ["vulkan"] },
                "verbose": { "type": "boolean" },
                "quiet": { "type": "boolean" },
                "log": { "$ref": "#/definitions/common_args/properties/log" }
            },
            "additionalProperties": False
        },
        "tests_args": {
            "type": "object",
            "properties": {
                "registry": { "type": "string" },
                "output-profile": { "type": "string" },
                "output-cpp": { "type": "string" },
                "api": { "type": "string", "enum": ["vulkan"] },
                "verbose": { "type": "boolean" },
                "quiet": { "type": "boolean" },
                "log": { "$ref": "#/definitions/common_args/properties/log" }
            },
            "additionalProperties": False
        }
    }
}