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
import tempfile
from enum import Enum
from pathlib import Path

from source.vulkan_object_utils import initVulkanObject
from source.generate_profiles_combine import VulkanProfilesCombineGenerator
from source.main_validate import main_validate
from source.main_transform import main_transform
from source.profiles_json_utils import (
    load_profiles_jsons,
    save_profiles_jsons,
    strip_dict_duplication,
    OutputFormatType
)
from source.json_config import JsonConfig


class CombineMode(str, Enum):
    INTERSECTION = 'intersection'  # Keeps capabilities supported by all input profiles.
    UNION = 'union'                # Keeps capabilities supported by any input profile.
    DIFFERENCE = 'difference'      # Removes common capabilities, leaving only unique profile differences.


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

    if not args.registry:
        logging.error("Combining profiles requires specifying --registry")
        sys.exit(1)

    vk = initVulkanObject(api, args.registry)

    config_path = getattr(args, 'config', None)
    input_dir = getattr(args, 'input', None)
    input_profiles = getattr(args, 'input_profiles', None)

    if not config_path and not input_dir:
        logging.error("Combining profiles requires specifying either --config or --input")
        sys.exit(1)

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
        json_files_dict = load_profiles_jsons(Path(input_dir) if input_dir else Path(config_path).parent)

        generator = VulkanProfilesCombineGenerator(vk)
        all_jsons = list(json_files_dict.values())
        all_profile_names = []
        for file_data in all_jsons:
            all_profile_names.extend(list(file_data.get("profiles", {}).keys()))

        common_caps = generator.combine_capabilities(all_jsons, all_profile_names, (1, 0))

        diff_files_dict = {}
        for file_key, file_data in json_files_dict.items():
            diff_file_data = json.loads(json.dumps(file_data))
            for cap_block in diff_file_data.get("capabilities", {}).values():
                strip_dict_duplication(cap_block, common_caps, strip_list_elements=True)
            diff_files_dict[file_key] = diff_file_data

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
    profile_configs = []

    if config_path:
        current_dir = os.path.dirname(os.path.abspath(config_path))
        with open(config_path, 'r', encoding='utf-8') as f:
            json_data = json.load(f)

        if json_data.get("contributors"):
            combined_json["contributors"] = json_data["contributors"]
        if json_data.get("history"):
            combined_json["history"] = json_data["history"]

        for p_name, p_val in json_data.get("profiles", {}).items():
            in_dir = os.path.join(current_dir, p_val["input"])
            p_config = JsonConfig(in_dir, [], p_val.get("api-version"), mode.value)
            p_config.apply_json_value(p_name, p_val)
            profile_configs.append(p_config)
    else:
        input_profile_names = []
        if input_profiles:
            input_profile_names = [p.strip() for p in input_profiles.split(',') if p.strip()]

        p_config = JsonConfig(input_dir, input_profile_names, getattr(args, 'profile_api_version', None), mode.value)

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

        profile_configs.append(p_config)

    for cfg in profile_configs:
        combiner = VulkanProfilesCombineGenerator(vk)
        combiner.combine(cfg, combined_json, mode.value)

    output_dir = output_path.parent
    if output_dir:
        os.makedirs(output_dir, exist_ok=True)

    transform_mode = getattr(args, 'transform', None)

    if transform_mode:
        with tempfile.TemporaryDirectory() as temp_dir:
            temp_combined_path = Path(temp_dir) / "combined.json"
            save_profiles_jsons({temp_combined_path: combined_json}, temp_combined_path, format_type)

            transform_args = argparse.Namespace(
                registry=args.registry,
                input=str(temp_combined_path),
                output=str(output_path),
                mode=transform_mode,
                format=format_type,
                api=api
            )
            main_transform(transform_args)
    else:
        save_profiles_jsons({output_path: combined_json}, output_path, format_type)