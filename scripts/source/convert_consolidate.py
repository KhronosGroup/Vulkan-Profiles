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

from source.profiles_json_utils import (
    parse_profile_capabilities, 
    deep_merge_dict,
    collect_required_profiles_capabilities,
    get_topologically_sorted_file_keys
)


def consolidate_profiles_file(json_files_dict: dict, json_file_data: dict):
    profiles_data = json_file_data.get("profiles", {})
    capabilities_dict = json_file_data.setdefault("capabilities", {})

    for profile_name, profile_obj in profiles_data.items():
        required_profile_names = profile_obj.get("profiles", [])
        consolidated_caps = collect_required_profiles_capabilities(json_files_dict, required_profile_names)

        parsed_caps = parse_profile_capabilities(profile_obj.get("capabilities", []))
        optional_blocks = []

        for item in parsed_caps:
            if isinstance(item, str):
                if item in capabilities_dict:
                    deep_merge_dict(consolidated_caps, capabilities_dict[item])
            elif isinstance(item, list):
                optional_blocks.append(item)

        if consolidated_caps:
            consolidated_block_name = f"{profile_name}_requirements"
            capabilities_dict[consolidated_block_name] = consolidated_caps

            profile_obj["capabilities"] = [consolidated_block_name] + optional_blocks
            profile_obj["profiles"] = []

    referenced_blocks = set()
    for prof in profiles_data.values():
        for item in prof.get("capabilities", []):
            if isinstance(item, str):
                referenced_blocks.add(item)
            elif isinstance(item, list):
                referenced_blocks.update(item)

    for block_name in list(capabilities_dict.keys()):
        if block_name not in referenced_blocks:
            del capabilities_dict[block_name]


def consolidate_profiles_files(json_files_dict: dict):
    if not isinstance(json_files_dict, dict):
        return

    if "profiles" in json_files_dict or "capabilities" in json_files_dict:
        consolidate_profiles_file(json_files_dict, json_files_dict)
        return

    sorted_file_keys = get_topologically_sorted_file_keys(json_files_dict)
    for file_key in sorted_file_keys:
        consolidate_profiles_file(json_files_dict, json_files_dict[file_key])