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

import copy
import logging
from enum import Enum
from pathlib import Path

from source.profiles_json_utils import (
    load_profiles_jsons,
    save_profiles_jsons,
    get_profile_and_file_data,
    OutputFormatType
)


class ExtractMode(str, Enum):
    REFERENCE = 'reference-required-profiles'  # Keeps referencing required profiles from external files
    PULL = 'pull-required-profiles'            # Pulls required parent profiles and their capability blocks into the output file


def get_file_stem(file_key) -> str:
    """Extracts the file stem without extension from a file path key."""
    if isinstance(file_key, Path):
        return file_key.stem
    return Path(str(file_key)).stem


def find_capability_block_with_file(json_files_dict: dict, block_name: str, preferred_file_key=None) -> tuple[dict | None, Path | str | None]:
    """Finds a capability block by name across loaded files, returning (block_data, file_key)."""
    if preferred_file_key and preferred_file_key in json_files_dict:
        pref_data = json_files_dict[preferred_file_key]
        if isinstance(pref_data, dict) and "capabilities" in pref_data:
            if block_name in pref_data["capabilities"]:
                return pref_data["capabilities"][block_name], preferred_file_key

    for file_key, file_data in json_files_dict.items():
        if isinstance(file_data, dict) and "capabilities" in file_data:
            if block_name in file_data["capabilities"]:
                return file_data["capabilities"][block_name], file_key

    return None, None


def rename_block_references_in_list(cap_list: list, rename_map: dict) -> list:
    """Updates capability block names in a capabilities or optionals reference list."""
    new_list = []
    for item in cap_list:
        if isinstance(item, str):
            new_list.append(rename_map.get(item, item))
        elif isinstance(item, list):
            new_sub_list = []
            for sub_item in item:
                if isinstance(sub_item, str):
                    new_sub_list.append(rename_map.get(sub_item, sub_item))
                else:
                    new_sub_list.append(sub_item)
            new_list.append(new_sub_list)
        else:
            new_list.append(item)
    return new_list


def extract_profile(json_files_dict: dict, target_profile_name: str, mode: ExtractMode = ExtractMode.REFERENCE) -> dict | None:
    """Extracts a target profile and its required capability blocks and parent profiles."""
    target_pobj, target_file_data = get_profile_and_file_data(json_files_dict, target_profile_name)
    if not target_pobj or not target_file_data:
        return None

    target_file_key = None
    for fk, fdata in json_files_dict.items():
        if fdata is target_file_data:
            target_file_key = fk
            break

    pull_parents = (mode == ExtractMode.PULL)

    included_profiles = {}
    extracted_capabilities = {}

    to_process = [target_profile_name]
    visited_profiles = set()
    profiles_to_include = []

    # Step 1: Collect profiles to include
    while to_process:
        pname = to_process.pop(0)
        if pname in visited_profiles:
            continue
        visited_profiles.add(pname)

        p_obj, p_file_data = get_profile_and_file_data(json_files_dict, pname)
        if not p_obj:
            continue

        p_file_key = None
        for fk, fdata in json_files_dict.items():
            if fdata is p_file_data:
                p_file_key = fk
                break

        profiles_to_include.append((pname, copy.deepcopy(p_obj), p_file_key))

        if pull_parents:
            for parent_name in p_obj.get("profiles", []):
                if parent_name not in visited_profiles:
                    to_process.append(parent_name)

    # Step 2: Extract capability blocks and rename blocks originating from parent/external files to prevent collisions
    for pname, p_obj, p_file_key in profiles_to_include:
        rename_map = {}

        for cap_section in ("capabilities", "optionals"):
            if cap_section not in p_obj:
                continue

            raw_blocks = []
            for cap_item in p_obj[cap_section]:
                if isinstance(cap_item, str):
                    raw_blocks.append(cap_item)
                elif isinstance(cap_item, list):
                    for sub_cap in cap_item:
                        if isinstance(sub_cap, str):
                            raw_blocks.append(sub_cap)

            for block_name in raw_blocks:
                if block_name in rename_map:
                    continue

                block_data, b_file_key = find_capability_block_with_file(
                    json_files_dict, block_name, preferred_file_key=p_file_key
                )
                if block_data is None:
                    continue

                b_file_stem = get_file_stem(b_file_key)

                # Prefix block name with originating filename stem if block is from external/parent file
                if b_file_key == target_file_key:
                    new_block_name = block_name
                else:
                    if block_name.startswith(b_file_stem):
                        new_block_name = block_name
                    else:
                        new_block_name = f"{b_file_stem}_{block_name}"

                rename_map[block_name] = new_block_name
                extracted_capabilities[new_block_name] = copy.deepcopy(block_data)

            p_obj[cap_section] = rename_block_references_in_list(p_obj[cap_section], rename_map)

        included_profiles[pname] = p_obj

    return {
        "$schema": target_file_data.get("$schema", "https://schema.khronos.org/vulkan/profiles-0.8-latest.json#"),
        "profiles": included_profiles,
        "capabilities": extracted_capabilities
    }


def main_extract(args):
    input_path = Path(args.input)
    output_file_path = Path(args.output)
    target_profile_name = args.profile_name
    mode = ExtractMode(getattr(args, 'mode', ExtractMode.REFERENCE))
    format_type = getattr(args, 'format', OutputFormatType.PRETTY)

    json_files_dict = load_profiles_jsons(input_path)
    if not json_files_dict:
        logging.error(f"No profile files loaded from {input_path}")
        return

    extracted_data = extract_profile(json_files_dict, target_profile_name, mode)
    if not extracted_data:
        logging.error(f"Profile '{target_profile_name}' not found in input.")
        return

    save_profiles_jsons({output_file_path: extracted_data}, output_file_path, format_type)
    logging.info(f"Successfully extracted profile '{target_profile_name}' ({mode.value} mode) to {output_file_path}")
    