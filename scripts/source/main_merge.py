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
import argparse
import tempfile
import gen_profiles_solution
import gen_profiles_file
from source.main_validate import main_validate
from source.main_convert import main_convert
from source.profiles_json_utils import OutputFormatType


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
        gen_profiles_solution.Log.e("Merging profiles requires specifying --registry")
        sys.exit(1)

    registry = gen_profiles_solution.VulkanRegistry(args.registry, api)

    config_path = getattr(args, 'config', None)
    input_dir = getattr(args, 'input', None)
    input_profiles = getattr(args, 'input_profiles', None)

    if not config_path and not input_dir:
        gen_profiles_solution.Log.e("Merging profiles requires specifying either --config or --input")
        sys.exit(1)

    profile_file = gen_profiles_file.ProfileFile()
    profile_configs = []

    mode = getattr(args, 'mode', 'intersection')
    strip_duplicate_struct = getattr(args, 'strip_duplicate_structs', False)

    if config_path:
        current_dir = os.path.dirname(os.path.abspath(config_path))
        with open(config_path, 'r', encoding='utf-8') as f:
            json_data = json.load(f)

        if json_data.get("contributors"):
            profile_file.set_contributors(json_data["contributors"])
        if json_data.get("history"):
            profile_file.set_history(json_data["history"])

        for p_name, p_val in json_data.get("profiles", {}).items():
            in_dir = os.path.join(current_dir, p_val["input"])
            p_config = gen_profiles_file.ProfileConfig(in_dir, [], p_val.get("api-version"), mode)
            p_config.apply_json_value(p_name, p_val)
            profile_configs.append(p_config)
    else:
        input_profile_names = []
        if input_profiles:
            input_profile_names = [p.strip() for p in input_profiles.split(',') if p.strip()]

        p_config = gen_profiles_file.ProfileConfig(input_dir, input_profile_names, getattr(args, 'profile_api_version', None), mode)

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
        merger = gen_profiles_file.ProfileMerger(registry)
        merger.merge(cfg, profile_file, mode, strip_duplicate_struct)

    output_path = args.output
    output_dir = os.path.dirname(os.path.abspath(output_path))
    if output_dir:
        os.makedirs(output_dir, exist_ok=True)

    convert_mode = getattr(args, 'convert', None)

    if convert_mode:
        with tempfile.TemporaryDirectory() as temp_dir:
            temp_merged_path = os.path.join(temp_dir, "merged.json")
            profile_file.dump(temp_merged_path)

            convert_args = argparse.Namespace(
                registry=args.registry,
                input=temp_merged_path,
                output=output_path,
                mode=convert_mode,
                format=getattr(args, 'format', OutputFormatType.PRETTY),
                api=api
            )
            main_convert(convert_args)
    else:
        profile_file.dump(output_path, getattr(args, 'format', None))
        