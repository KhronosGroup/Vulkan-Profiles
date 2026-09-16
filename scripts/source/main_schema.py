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

import logging
import re
from pathlib import Path

from source.vulkan_object_utils import initVulkanObject
from source.generate_profiles_schema import VulkanProfilesSchemaGenerator2
from source.profiles_json_utils import (
    load_schema_jsons,
    save_profiles_jsons,
    OutputFormatType
)


def upgrade_schema_json_data(schema_data: dict) -> dict:
    """Upgrades a Vulkan profile JSON Schema document from 0.8.0/0.8.1 to 0.8.2 layout."""
    if not isinstance(schema_data, dict):
        return schema_data

    # 1. Update $id URL: profiles-0.8.0 / profiles-0.8.1 -> profiles-0.8.2
    if "$id" in schema_data and isinstance(schema_data["$id"], str):
        schema_data["$id"] = re.sub(r'profiles-0\.8\.[01]', 'profiles-0.8.2', schema_data["$id"])

    definitions = schema_data.get("definitions", {})

    # 2. Upgrade primitive definitions to native integer types
    if "int64_t" in definitions:
        definitions["int64_t"] = {"type": "integer"}
    if "uint64_t" in definitions:
        definitions["uint64_t"] = {"type": "integer", "minimum": 0}
    if "VkDeviceSize" in definitions:
        definitions["VkDeviceSize"] = {"type": "integer", "minimum": 0}
    if "size_t" in definitions:
        definitions["size_t"] = {"type": "integer", "minimum": 0}

    # 3. Upgrade profile definition rules
    profiles_def = schema_data.get("properties", {}).get("profiles", {})
    pattern_props = profiles_def.get("patternProperties", {})

    for pattern, prof_schema in pattern_props.items():
        if not isinstance(prof_schema, dict):
            continue

        # Remove mandatory requirement for contributors and history
        if "required" in prof_schema and isinstance(prof_schema["required"], list):
            prof_schema["required"] = [
                r for r in prof_schema["required"] if r not in ("contributors", "history")
            ]

        props = prof_schema.get("properties", {})

        # Upgrade capabilities to support anyOf (string or string array)
        if "capabilities" in props and isinstance(props["capabilities"], dict):
            props["capabilities"]["items"] = {
                "anyOf": [
                    {"type": "string"},
                    {
                        "type": "array",
                        "uniqueItems": True,
                        "items": {"type": "string"}
                    }
                ]
            }

        # Ensure optionals property definition exists
        if "optionals" not in props:
            props["optionals"] = {
                "description": "The list of optional capability sets that can be referenced by a profile.",
                "type": "array",
                "uniqueItems": True,
                "items": {
                    "anyOf": [
                        {"type": "string"},
                        {
                            "type": "array",
                            "uniqueItems": True,
                            "items": {"type": "string"}
                        }
                    ]
                }
            }

        # Ensure profiles property definition exists
        if "profiles" not in props:
            props["profiles"] = {
                "description": "The list of required profiles by the profile.",
                "type": "array",
                "additionalProperties": False,
                "uniqueItems": True,
                "items": {
                    "type": "string"
                }
            }

    # Remove deprecated root properties
    root_props = schema_data.get("properties", {})
    root_props.pop("contributors", None)
    root_props.pop("history", None)

    return schema_data


def main_schema(args):
    input_path = getattr(args, 'input', None)
    is_upgrade = getattr(args, 'upgrade', False) or (input_path is not None)

    if is_upgrade and input_path:
        format_type = getattr(args, 'format', OutputFormatType.PRETTY)

        schema_files_dict = load_schema_jsons(Path(input_path))
        if not schema_files_dict:
            logging.error(f"No profile schema files found at {input_path}")
            return

        upgraded_dict = {}

        for file_key, file_data in schema_files_dict.items():
            new_filename = re.sub(r'profiles-0\.8\.[01]', 'profiles-0.8.2', file_key.name)
            print(f"Upgrading schema '{file_key.name}' to '{new_filename}'...")
            
            upgraded_data = upgrade_schema_json_data(file_data)
            new_file_key = file_key.parent / new_filename
            upgraded_dict[new_file_key] = upgraded_data

        save_profiles_jsons(upgraded_dict, Path(args.output), format_type)
        logging.info(f"Upgraded {len(upgraded_dict)} profile schema(s) saved to {args.output}")
    else:
        if args.registry is None:
            vk = initVulkanObject(args.api)
        else:
            vk = initVulkanObject(args.api, args.registry, True)
        generator = VulkanProfilesSchemaGenerator2(vk)
        generator.generate(args.output)