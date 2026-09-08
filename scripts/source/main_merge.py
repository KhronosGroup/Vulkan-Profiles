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
from pathlib import Path

from source.vulkan_object_utils import initVulkanObject
from source.generate_profiles_merge import VulkanProfilesMergeGenerator
from source.main_validate import main_validate
from source.main_convert import main_convert
from source.profiles_json_utils import save_profiles_jsons, OutputFormatType
from source.json_config import JsonConfig


def main_merge(args):
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
        logging.error("Merging profiles requires specifying --registry")
        sys.exit(1)

    vk = initVulkanObject(api, args.registry)

    config_path = getattr(args, 'config', None)
    input_dir = getattr(args, 'input', None)
    input_profiles = getattr(args, 'input_profiles', None)

    if not config_path and not input_dir:
        logging.error("Merging profiles requires specifying either --config or --input")
        sys.exit(1)

    merged_json = {
        "$schema": "https://schema.khronos.org/vulkan/profiles-0.8-latest.json#",
        "capabilities": {},
        "profiles": {},
        "contributors": {},
        "history": []
    }
    profile_configs = []

    mode = getattr(args, 'mode', 'intersection')

    if config_path:
        current_dir = os.path.dirname(os.path.abspath(config_path))
        with open(config_path, 'r', encoding='utf-8') as f:
            json_data = json.load(f)

        if json_data.get("contributors"):
            merged_json["contributors"] = json_data["contributors"]
        if json_data.get("history"):
            merged_json["history"] = json_data["history"]

        for p_name, p_val in json_data.get("profiles", {}).items():
            in_dir = os.path.join(current_dir, p_val["input"])
            p_config = JsonConfig(in_dir, [], p_val.get("api-version"), mode)
            p_config.apply_json_value(p_name, p_val)
            profile_configs.append(p_config)
    else:
        input_profile_names = []
        if input_profiles:
            input_profile_names = [p.strip() for p in input_profiles.split(',') if p.strip()]

        p_config = JsonConfig(input_dir, input_profile_names, getattr(args, 'profile_api_version', None), mode)

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
        merger = VulkanProfilesMergeGenerator(vk)
        merger.merge(cfg, merged_json, mode)

    output_path = Path(args.output)
    output_dir = output_path.parent
    if output_dir:
        os.makedirs(output_dir, exist_ok=True)

    format_type = getattr(args, 'format', OutputFormatType.PRETTY)
    if isinstance(format_type, str):
        format_type = OutputFormatType(format_type)
    elif format_type is None:
        format_type = OutputFormatType.PRETTY

    convert_mode = getattr(args, 'convert', None)

    if convert_mode:
        with tempfile.TemporaryDirectory() as temp_dir:
            temp_merged_path = Path(temp_dir) / "merged.json"
            save_profiles_jsons({temp_merged_path: merged_json}, temp_merged_path, format_type)

            convert_args = argparse.Namespace(
                registry=args.registry,
                input=str(temp_merged_path),
                output=str(output_path),
                mode=convert_mode,
                format=format_type,
                api=api
            )
            main_convert(convert_args)
    else:
        save_profiles_jsons({output_path: merged_json}, output_path, format_type)
        