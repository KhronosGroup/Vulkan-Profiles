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

import argparse
import sys
import re
import json
import os
import tempfile

import gen_profiles_solution
import gen_profiles_file
from source.main_validate import main_validate
from source.main_convert import main_convert, OutputFormatType

def main_merge(args):
    validate_val = getattr(args, 'validate', None)
    if validate_val:
        validate_modes = validate_val if isinstance(validate_val, list) else ['schema', 'analysis']
        
        # Determine the input path for validation
        input_path = getattr(args, 'input', None)
        if not input_path and getattr(args, 'config', None):
            input_path = os.path.dirname(args.config)

        if input_path:
            validate_args = argparse.Namespace(
                registry=getattr(args, 'registry', None),
                input=input_path,
                schema=getattr(args, 'schema', None),
                api=getattr(args, 'api', 'vulkan') or 'vulkan',
                mode=validate_modes
            )
            main_validate(validate_args)

    if args.registry is None:
        gen_profiles_solution.Log.e('Merging the profiles requires specifying --registry')
        sys.exit(1)

    registry = gen_profiles_solution.VulkanRegistry(args.registry)

    if args.mode.lower() not in ('union', 'intersection'):
        gen_profiles_solution.Log.e('Mode must be either union or intersection')
        sys.exit(1)

    if args.input_profiles is not None:
        input_profile_names = args.input_profiles.split(',')
    else:
        input_profile_names = list()

    profile_file = gen_profiles_file.ProfileFile()
    profile_configs = list()

    if args.config is None:
        profile_config = gen_profiles_file.ProfileConfig(
            args.input, input_profile_names, args.profile_api_version, args.mode
        )

        if args.profile_name is not None:
            if not re.match('^VP_[A-Z0-9]+[A-Za-z0-9]+', args.profile_name):
                gen_profiles_solution.Log.e('Invalid profile_name, must follow regex pattern ^VP_[A-Z0-9]+[A-Za-z0-9]+')
                sys.exit(1)
            else:
                profile_config.name = args.profile_name
        elif args.output_profile is not None:
            if not re.match('^VP_[A-Z0-9]+[A-Za-z0-9]+', args.output_profile):
                gen_profiles_solution.Log.e('Invalid output_profile, must follow regex pattern ^VP_[A-Z0-9]+[A-Za-z0-9]+')
                sys.exit(1)
            else:
                profile_config.name = args.output_profile

        if args.profile_version is not None:
            profile_config.version = int(args.profile_version)

        if args.profile_label is not None:
            profile_config.label = args.profile_label

        if args.profile_desc is not None:
            profile_config.description = args.profile_desc

        if args.profile_stage is not None:
            profile_config.stage = args.profile_stage

        if args.profile_date is not None:
            profile_config.date = args.profile_date

        if args.profile_required_profiles is not None:
            profile_config.required_profiles = args.profile_required_profiles.split(',')

        profile_configs.append(profile_config)

    else:
        currentdir = os.path.dirname(args.config)

        with open(args.config, "r") as json_file:
            json_data = json.load(json_file)

        if json_data.get("contributors"):
            profile_file.set_contributors(json_data["contributors"])
        if json_data.get("history"):
            profile_file.set_history(json_data["history"])

        for profile_name in json_data.get("profiles", {}):
            profile_value = json_data["profiles"][profile_name]
            profile_config = gen_profiles_file.ProfileConfig(
                os.path.join(currentdir, profile_value["input"]),
                list(),
                profile_value["api-version"],
                args.mode
            )
            profile_config.apply_json_value(profile_name, profile_value)
            profile_configs.append(profile_config)

    for config in profile_configs:
        profile_merger = gen_profiles_file.ProfileMerger(registry)
        profile_merger.merge(
            config,
            profile_file,
            args.mode,
            False
        )

    format_type = getattr(args, 'format', None) or OutputFormatType.PRETTY
    convert_mode = getattr(args, 'convert', None)

    if convert_mode is not None:
        api = getattr(args, 'api', 'vulkan') or 'vulkan'
        with tempfile.TemporaryDirectory() as temp_dir:
            temp_merged_path = os.path.join(temp_dir, "merged.json")
            profile_file.dump(temp_merged_path, format_type)

            convert_args = argparse.Namespace(
                registry=args.registry,
                input=temp_merged_path,
                output=args.output,
                mode=convert_mode,
                format=format_type,
                api=api,
                validate=False
            )
            main_convert(convert_args)
    else:
        profile_file.dump(args.output, format_type)
        