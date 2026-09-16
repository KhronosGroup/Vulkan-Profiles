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

import json
import logging
from enum import Enum
from pathlib import Path

from source.profiles_json_utils import (
    load_profiles_jsons,
    save_profiles_jsons,
    get_profile_and_file_data,
    collect_profile_capabilities,
    OutputFormatType
)
from source.main_min_api_version import (
    load_available_schemas,
    find_min_schema_for_profile,
    get_schema_api_version
)


class ExtractMode(str, Enum):
    REFERENCE = 'reference-required-profiles'
    PULL = 'pull-required-profiles'


def extract_profile(json_files_dict: dict, profile_name: str, mode: ExtractMode) -> dict | None:
    profile_obj, source_file_data = get_profile_and_file_data(json_files_dict, profile_name)
    if not profile_obj or not source_file_data:
        logging.error(f"Profile '{profile_name}' not found in input files.")
        return None

    new_file_data = {
        "$schema": source_file_data.get("$schema", "https://schema.khronos.org/vulkan/profiles-0.8.2-303.json#"),
        "profiles": {
            profile_name: json.loads(json.dumps(profile_obj))
        },
        "capabilities": {}
    }

    caps_dict = source_file_data.get("capabilities", {})
    if mode == ExtractMode.PULL:
        combined_caps = collect_profile_capabilities(json_files_dict, source_file_data, profile_obj)
        block_name = f"{profile_name}_block"
        new_file_data["capabilities"][block_name] = combined_caps
        new_file_data["profiles"][profile_name]["capabilities"] = [block_name]
        new_file_data["profiles"][profile_name]["profiles"] = []
    else:
        local_caps = profile_obj.get("capabilities", [])
        for cap_entry in local_caps:
            if isinstance(cap_entry, str) and cap_entry in caps_dict:
                new_file_data["capabilities"][cap_entry] = json.loads(json.dumps(caps_dict[cap_entry]))
            elif isinstance(cap_entry, list):
                for sub_cap in cap_entry:
                    if isinstance(sub_cap, str) and sub_cap in caps_dict:
                        new_file_data["capabilities"][sub_cap] = json.loads(json.dumps(caps_dict[sub_cap]))

    return new_file_data


def main_extract(args):
    input_path = Path(args.input)
    output_path = Path(args.output)
    profile_name = args.profile_name
    mode = getattr(args, 'mode', ExtractMode.REFERENCE) or ExtractMode.REFERENCE
    format_type = getattr(args, 'format', OutputFormatType.PRETTY)
    schemas_dir = getattr(args, 'schemas', None)

    json_files_dict = load_profiles_jsons(input_path)
    if not json_files_dict:
        logging.error(f"No profile files loaded from {input_path}")
        return

    extracted_data = extract_profile(json_files_dict, profile_name, mode)
    if not extracted_data:
        return

    schemas = load_available_schemas(schemas_dir)
    if schemas:
        min_header_ver, min_schema = find_min_schema_for_profile(extracted_data, schemas, profile_name=profile_name)
        api_ver_str = get_schema_api_version(min_schema)
        if api_ver_str and profile_name in extracted_data.get("profiles", {}):
            extracted_data["profiles"][profile_name]["api-version"] = api_ver_str
        if min_header_ver:
            extracted_data["$schema"] = f"https://schema.khronos.org/vulkan/profiles-0.8.2-{min_header_ver}.json#"

    save_profiles_jsons({output_path: extracted_data}, output_path, format_type)
    logging.info(f"Extracted profile '{profile_name}' saved to {output_path}")