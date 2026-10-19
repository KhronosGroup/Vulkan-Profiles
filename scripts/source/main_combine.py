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

import os
import sys
import json
import logging
import argparse
from enum import Enum
from pathlib import Path

from source.vulkan_object_utils import initVulkanObject
from source.generate_profiles_combine import VulkanProfilesCombineGenerator, CombineConfig
from source.main_validate import main_validate
from source.profiles_json_utils import (
    load_profiles_jsons,
    save_profiles_jsons,
    collect_block_names,
    deep_merge_dict,
    OutputFormatType
)


class CombineMode(str, Enum):
    INTERSECTION = 'intersection'  # Keeps capabilities supported by all input profiles.
    UNION = 'union'                # Keeps capabilities supported by any input profile.
    DIFFERENCE = 'difference'      # Removes common capabilities, leaving only unique profile differences.


CAPABILITY_SECTIONS = ('extensions', 'features', 'properties', 'formats')


def effective_profile_capabilities(file_data: dict, profile_name: str) -> dict:
    """Merges all capability blocks referenced by a profile into its effective capabilities."""
    effective = {section: {} for section in CAPABILITY_SECTIONS}
    capabilities = file_data.get('capabilities', {})
    profile_obj = file_data.get('profiles', {}).get(profile_name, {})
    for block_name in collect_block_names(profile_obj.get('capabilities', [])):
        block = capabilities.get(block_name, {})
        for section in CAPABILITY_SECTIONS:
            if isinstance(block.get(section), dict):
                deep_merge_dict(effective[section], block[section])
    return effective


def is_flag_list(value) -> bool:
    """Lists of names (e.g. format feature flags, shader stages) are compared element by element; numeric
    arrays (e.g. maxComputeWorkGroupSize) are compared as a single value."""
    return isinstance(value, list) and all(isinstance(x, str) for x in value)


def intersect_equal_values(a, b):
    """Returns the parts of a and b that are identical: common keys with equal values, common list elements."""
    if isinstance(a, dict) and isinstance(b, dict):
        result = {}
        for key in a.keys() & b.keys():
            common = intersect_equal_values(a[key], b[key])
            if common is not None:
                result[key] = common
        return result
    if isinstance(a, list) and isinstance(b, list):
        if is_flag_list(a) and is_flag_list(b):
            return [x for x in a if x in b]
        return a if a == b else None
    return a if a == b else None


def strip_common_capabilities(target: dict, commons: list):
    """
    Removes from target every member whose path is common in all the given common capability dicts,
    and every list element common to all of them. Emptied dicts and lists are removed.
    """
    for key in list(target.keys()):
        if not all(isinstance(c, dict) and key in c for c in commons):
            continue
        value = target[key]
        sub_commons = [c[key] for c in commons]
        if isinstance(value, dict):
            if all(isinstance(s, dict) for s in sub_commons):
                strip_common_capabilities(value, sub_commons)
                if not value:
                    del target[key]
            elif all(s == value for s in sub_commons):
                del target[key]
        elif is_flag_list(value) and all(is_flag_list(s) for s in sub_commons):
            target[key] = [x for x in value if not all(x in s for s in sub_commons)]
            if not target[key]:
                del target[key]
        else:
            # The profile effective value is the same in all compared files: this member adds no difference.
            del target[key]


def compute_profile_differences(json_files_dict: dict) -> dict:
    """
    Strips the capabilities that are common to the compared profiles, leaving only their differences.

    Profiles sharing the same name across input files are compared with each other (e.g. a hand-written
    and a generated version of the same profile). When no profile name is shared, all profiles of all
    files are compared together. Only identical values are considered common.
    """
    profile_files = {}
    for file_key, file_data in json_files_dict.items():
        for profile_name in file_data.get('profiles', {}):
            profile_files.setdefault(profile_name, []).append(file_key)

    groups = [[(fk, name) for fk in files] for name, files in profile_files.items() if len(files) > 1]
    if not groups:
        groups = [[(fk, name) for name, files in profile_files.items() for fk in files]]

    common_by_profile = {}
    for group in groups:
        if not group:
            continue
        effectives = [effective_profile_capabilities(json_files_dict[fk], name) for fk, name in group]
        common = effectives[0]
        for other in effectives[1:]:
            common = intersect_equal_values(common, other)
        for key in group:
            common_by_profile[key] = common

    diff_files_dict = {}
    for file_key, file_data in json_files_dict.items():
        diff_file_data = json.loads(json.dumps(file_data))
        block_users = {}
        for profile_name, profile_obj in diff_file_data.get('profiles', {}).items():
            for block_name in collect_block_names(profile_obj.get('capabilities', [])):
                block_users.setdefault(block_name, []).append(profile_name)

        for block_name, block in diff_file_data.get('capabilities', {}).items():
            commons = [common_by_profile.get((file_key, name)) for name in block_users.get(block_name, [])]
            # A block shared by several profiles only loses what is common for every one of them.
            if not commons or any(c is None for c in commons):
                continue
            for section in CAPABILITY_SECTIONS:
                if isinstance(block.get(section), dict):
                    strip_common_capabilities(block[section], [c[section] for c in commons])
                    if not block[section]:
                        del block[section]
        diff_files_dict[file_key] = diff_file_data
    return diff_files_dict


def main_combine(args):
    api = getattr(args, 'api', 'vulkan') or 'vulkan'

    validate_val = getattr(args, 'validate', None)
    if validate_val and getattr(args, 'input', None):
        validate_modes = validate_val if isinstance(validate_val, list) else ['schema', 'analysis']
        validate_args = argparse.Namespace(
            registry=getattr(args, 'registry', None),
            input=args.input,
            schema=getattr(args, 'schema', None),
            api=api,
            mode=validate_modes
        )
        main_validate(validate_args)

    if not getattr(args, 'registry', None):
        logging.error("Combining profiles requires specifying --registry")
        sys.exit(1)

    input_dir = getattr(args, 'input', None)
    if not input_dir:
        logging.error("Combining profiles requires specifying --input")
        sys.exit(1)

    vk = initVulkanObject(api, args.registry)
    input_profiles = getattr(args, 'input_profiles', None)

    raw_mode = getattr(args, 'mode', CombineMode.INTERSECTION)
    mode = CombineMode(raw_mode)

    format_type = getattr(args, 'format', OutputFormatType.PRETTY)
    if isinstance(format_type, str):
        format_type = OutputFormatType(format_type)
    elif format_type is None:
        format_type = OutputFormatType.PRETTY

    output_path = Path(args.output)

    # Special processing for DIFFERENCE mode across input profile files
    if mode == CombineMode.DIFFERENCE:
        json_files_dict = load_profiles_jsons(Path(input_dir))
        diff_files_dict = compute_profile_differences(json_files_dict)
        save_profiles_jsons(diff_files_dict, output_path, format_type)
        return

    # Processing for UNION and INTERSECTION modes
    combined_json = {
        "$schema": "https://schema.khronos.org/vulkan/profiles-0.8-latest.json#",
        "capabilities": {},
        "profiles": {},
        "contributors": {},
        "history": []
    }

    input_profile_names = []
    if input_profiles:
        input_profile_names = [p.strip() for p in input_profiles.split(',') if p.strip()]

    p_config = CombineConfig(input_dir, input_profile_names, getattr(args, 'profile_api_version', None), mode.value)

    profile_name = getattr(args, 'profile_name', None) or getattr(args, 'output_profile', None)
    if profile_name:
        p_config.name = profile_name
    if getattr(args, 'profile_version', None) is not None:
        p_config.version = int(args.profile_version)
    if getattr(args, 'profile_label', None):
        p_config.label = args.profile_label
    if getattr(args, 'profile_desc', None):
        p_config.description = args.profile_desc
    if getattr(args, 'profile_stage', None):
        p_config.stage = args.profile_stage
    if getattr(args, 'profile_date', None):
        p_config.date = args.profile_date
    if getattr(args, 'profile_required_profiles', None):
        p_config.required_profiles = [p.strip() for p in args.profile_required_profiles.split(',') if p.strip()]

    combiner = VulkanProfilesCombineGenerator(vk)
    combiner.combine(p_config, combined_json, mode.value)

    output_dir = output_path.parent
    if output_dir:
        os.makedirs(output_dir, exist_ok=True)

    save_profiles_jsons({output_path: combined_json}, output_path, format_type)