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
import re
import sys
from enum import Enum
from pathlib import Path

from source.profiles_json_utils import (
    load_profiles_jsons,
    save_profiles_jsons,
    get_profile_and_file_data,
    collect_profile_capabilities,
    OutputFormatType
)


class ExtractMode(str, Enum):
    REFERENCE = 'reference-required-profiles'
    PULL = 'pull-required-profiles'


def parse_schema_version(schema_str: str) -> tuple[int, ...]:
    """Extracts major, minor, patch, and header version numbers from a schema URI for version comparison."""
    if not schema_str:
        return (0, 0, 0, 0)
    match = re.search(r'profiles-(\d+)\.(\d+)(?:\.(\d+))?-(?:(\d+)|latest)\.json', schema_str)
    if match:
        major = int(match.group(1))
        minor = int(match.group(2))
        patch = int(match.group(3)) if match.group(3) else 0
        header_str = match.group(4)
        header = int(header_str) if header_str else 999999
        return (major, minor, patch, header)
    nums = [int(n) for n in re.findall(r'\d+', schema_str)]
    if nums:
        return tuple(nums)
    return (0, 0, 0, 0)


def extract_profile(
    json_files_dict: dict, 
    profile_names: list[str] | str, 
    mode: ExtractMode = ExtractMode.REFERENCE,
    contributors: dict | None = None,
    history: list | None = None
) -> dict | None:
    """
    Extracts one or multiple profiles from loaded JSON profile datasets into a single JSON profile structure.
    Capability block names are namespaced with `${profile_name}_` to prevent namespace collisions.
    The schema used in the output JSON is selected from the input file with the newest Vulkan Header/schema version.
    Optionally includes 'contributors' and 'history' metadata.
    """
    if isinstance(profile_names, str):
        profile_names = [profile_names]

    all_profiles = {}
    for file_key, file_data in json_files_dict.items():
        if isinstance(file_data, dict) and "profiles" in file_data and isinstance(file_data["profiles"], dict):
            for p_name, p_val in file_data["profiles"].items():
                all_profiles[p_name] = (p_val, file_data)

    extracted_profiles = {}
    extracted_capabilities = {}
    candidate_schemas = []

    collected_contributors = {}
    collected_history = []
    seen_history = set()

    for p_name in profile_names:
        if p_name not in all_profiles:
            logging.error(f"Profile '{p_name}' not found in input files.")
            return None

        profile_obj, source_file_data = all_profiles[p_name]
        schema_uri = source_file_data.get("$schema")
        if schema_uri:
            candidate_schemas.append(schema_uri)

        # Collect source contributors if not explicitly provided
        if contributors is None and "contributors" in source_file_data and isinstance(source_file_data["contributors"], dict):
            for c_name, c_val in source_file_data["contributors"].items():
                collected_contributors[c_name] = c_val

        # Collect source history entries if not explicitly provided
        if history is None and "history" in source_file_data and isinstance(source_file_data["history"], list):
            for h_entry in source_file_data["history"]:
                h_str = json.dumps(h_entry, sort_keys=True)
                if h_str not in seen_history:
                    seen_history.add(h_str)
                    collected_history.append(h_entry)

        caps_dict = source_file_data.get("capabilities", {})
        if mode == ExtractMode.PULL:
            combined_caps = collect_profile_capabilities(json_files_dict, source_file_data, profile_obj)
            block_name = f"{p_name}_block"
            extracted_capabilities[block_name] = combined_caps

            new_p_obj = json.loads(json.dumps(profile_obj))
            new_p_obj["capabilities"] = [block_name]
            new_p_obj["profiles"] = []
            extracted_profiles[p_name] = new_p_obj
        else:
            # REFERENCE mode: retain external required-profile references and prefix local capability blocks to avoid namespace collisions
            new_p_obj = json.loads(json.dumps(profile_obj))
            new_caps_refs = []

            for cap_entry in profile_obj.get("capabilities", []):
                if isinstance(cap_entry, str):
                    new_block_name = cap_entry if cap_entry.startswith(f"{p_name}_") else f"{p_name}_{cap_entry}"
                    new_caps_refs.append(new_block_name)
                    if cap_entry in caps_dict:
                        extracted_capabilities[new_block_name] = json.loads(json.dumps(caps_dict[cap_entry]))
                elif isinstance(cap_entry, list):
                    sub_refs = []
                    for sub_cap in cap_entry:
                        if isinstance(sub_cap, str):
                            new_sub_name = sub_cap if sub_cap.startswith(f"{p_name}_") else f"{p_name}_{sub_cap}"
                            sub_refs.append(new_sub_name)
                            if sub_cap in caps_dict:
                                extracted_capabilities[new_sub_name] = json.loads(json.dumps(caps_dict[sub_cap]))
                    new_caps_refs.append(sub_refs)

            new_p_obj["capabilities"] = new_caps_refs
            extracted_profiles[p_name] = new_p_obj

    newest_schema = max(candidate_schemas, key=parse_schema_version) if candidate_schemas else "https://schema.khronos.org/vulkan/profiles-0.8.2-303.json#"

    result = {
        "$schema": newest_schema,
        "profiles": extracted_profiles,
        "capabilities": extracted_capabilities
    }

    final_contributors = contributors if contributors is not None else collected_contributors
    if final_contributors:
        result["contributors"] = final_contributors

    final_history = history if history is not None else collected_history
    if final_history:
        result["history"] = final_history

    return result


def main_extract(args):
    input_path = Path(args.input)
    output_path = Path(args.output)

    raw_profiles = getattr(args, 'input_profiles', None)
    if not raw_profiles:
        logging.error("Extract command requires '--input-profiles'")
        sys.exit(1)

    target_profiles = [p.strip() for p in raw_profiles.split(',') if p.strip()]

    mode = getattr(args, 'mode', ExtractMode.REFERENCE) or ExtractMode.REFERENCE
    format_type = getattr(args, 'format', OutputFormatType.PRETTY) or OutputFormatType.PRETTY

    raw_contributors = getattr(args, 'contributors', None)
    contributors = None
    if raw_contributors:
        if isinstance(raw_contributors, str):
            try:
                contributors = json.loads(raw_contributors)
            except Exception as e:
                logging.error(f"Failed to parse '--contributors' JSON string: {e}")
                sys.exit(1)
        elif isinstance(raw_contributors, dict):
            contributors = raw_contributors

    raw_history = getattr(args, 'history', None)
    history = None
    if raw_history:
        if isinstance(raw_history, str):
            try:
                history = json.loads(raw_history)
            except Exception as e:
                logging.error(f"Failed to parse '--history' JSON string: {e}")
                sys.exit(1)
        elif isinstance(raw_history, list):
            history = raw_history

    json_files_dict = load_profiles_jsons(input_path)
    if not json_files_dict:
        logging.error(f"No profile files loaded from {input_path}")
        sys.exit(1)

    extracted_data = extract_profile(
        json_files_dict, 
        target_profiles, 
        mode, 
        contributors=contributors, 
        history=history
    )
    if not extracted_data:
        sys.exit(1)

    save_profiles_jsons({output_path: extracted_data}, output_path, format_type)
    logging.info(f"Extracted profile(s) ({', '.join(target_profiles)}) saved to {output_path}")