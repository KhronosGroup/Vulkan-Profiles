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
                    "depends_on": {
                        "type": "array",
                        "items": { "type": "string" }
                    },
                    "command": {
                        "type": "string",
                        "enum": ["schema", "transform", "combine", "validate", "library", "doc", "extract", "min-api-version", "summary", "layer", "tests"]
                    },
                    "args": { "type": "object" }
                },
                "additionalProperties": False
            }
        }
    },
    "additionalProperties": False
}