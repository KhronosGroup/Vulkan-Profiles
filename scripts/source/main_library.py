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
import sys
from pathlib import Path

import gen_profiles_solution
from source.vulkan_object_utils import initVulkanObject
from source.profiles_json_utils import load_profiles_jsons, save_profiles_jsons, OutputFormatType
from source.transform_utils import PullBits, StripBits
from source.main_transform import transform_profiles_files
from source.main_validate import main_validate


def main_library(args):
    input_path = Path(args.input) if getattr(args, 'input', None) else None
    output_inc_path = Path(args.output) if getattr(args, 'output', None) else None
    output_src_path = Path(args.output_src) if getattr(args, 'output_src', None) else None
    registry_path = getattr(args, 'registry', None)
    intermediate_path = Path(args.intermediate) if getattr(args, 'intermediate', None) else None
    api = getattr(args, 'api', 'vulkan') or 'vulkan'

    if not registry_path or not input_path:
        logging.error("Generating library requires specifying --registry and --input")
        sys.exit(1)

    strip = getattr(args, 'strip', False)

    if getattr(args, 'validate', None) is not None:
        main_validate(args)

    input_profiles = None
    if getattr(args, 'input_profiles', None):
        input_profiles = [p.strip() for p in args.input_profiles.split(',') if p.strip()]

    json_files_dict = load_profiles_jsons(input_path)

    if input_profiles and json_files_dict:
        all_available_profiles = {
            p_name
            for data in json_files_dict.values()
            if isinstance(data, dict) and 'profiles' in data and isinstance(data['profiles'], dict)
            for p_name in data['profiles'].keys()
        }
        missing_profiles = [p for p in input_profiles if p not in all_available_profiles]
        if missing_profiles:
            logging.error(f"Profile(s) specified in '--input-profiles' not found in database: {', '.join(missing_profiles)}")
            sys.exit(1)

        filtered_dict = {}
        for path_key, data in json_files_dict.items():
            if isinstance(data, dict) and 'profiles' in data and isinstance(data['profiles'], dict):
                matching_profiles = {
                    p_name: p_val for p_name, p_val in data['profiles'].items() if p_name in input_profiles
                }
                if matching_profiles:
                    new_data = dict(data)
                    new_data['profiles'] = matching_profiles
                    filtered_dict[path_key] = new_data
        json_files_dict = filtered_dict

    if not json_files_dict:
        logging.error(f"No profile JSON files loaded from '{input_path}'")
        sys.exit(1)

    # Validate that all required parent profile dependencies are present in the filtered dataset
    active_profile_names = {
        p_name
        for data in json_files_dict.values()
        if isinstance(data, dict) and 'profiles' in data and isinstance(data['profiles'], dict)
        for p_name in data['profiles'].keys()
    }

    missing_parents = {}
    for data in json_files_dict.values():
        if isinstance(data, dict) and 'profiles' in data and isinstance(data['profiles'], dict):
            for p_name, p_val in data['profiles'].items():
                if isinstance(p_val, dict):
                    req_parents = p_val.get('profiles', [])
                    for parent in req_parents:
                        if parent not in active_profile_names:
                            missing_parents.setdefault(p_name, []).append(parent)

    if missing_parents:
        for p_name, parents in missing_parents.items():
            logging.error(
                f"Profile '{p_name}' requires parent profile(s) '{', '.join(parents)}', "
                f"which were omitted from '--input-profiles'."
            )
        sys.exit(1)

    vk = initVulkanObject(api, registry_path)

    if strip:
        transform_profiles_files(
            vk,
            json_files_dict,
            pull_modes=[PullBits.ALIASES],
            strip_modes=[StripBits.DUPLICATION, StripBits.HELPER_VALUES]
        )
        if intermediate_path:
            save_profiles_jsons(json_files_dict, intermediate_path, OutputFormatType.PRETTY)

    registry = gen_profiles_solution.VulkanRegistry(registry_path, api)

    input_profiles_files = gen_profiles_solution.VulkanProfilesFiles.__new__(gen_profiles_solution.VulkanProfilesFiles)
    input_profiles_files.profiles = dict()
    input_profiles_files.json_profiles_database = gen_profiles_solution.VulkanProfilesDatabase()
    input_profiles_files.json_profiles_database.json_files = list(json_files_dict.values())
    for json_file_data in input_profiles_files.json_profiles_database.json_files:
        input_profiles_files.parseProfiles(registry, json_file_data['profiles'], json_file_data['capabilities'])

    output_filename = getattr(args, 'output_filename', 'vulkan_profiles') or 'vulkan_profiles'
    is_debug = getattr(args, 'debug', False) or getattr(args, 'config', 'release') == 'debug'
    include_header = getattr(args, 'include_header', None)

    raw_modes = getattr(args, 'mode', ['header-only', 'header+source']) or ['header-only', 'header+source']
    
    out_inc_str = str(output_inc_path) if output_inc_path else None
    out_src_str = str(output_src_path) if ('header+source' in raw_modes and output_src_path) else (out_inc_str if 'header+source' in raw_modes else None)

    generator = gen_profiles_solution.VulkanProfilesLibraryGenerator(
        registry,
        input_profiles_files,
        output_filename,
        is_debug,
        include_header
    )
    generator.generate(out_inc_str, out_src_str)
    logging.info(f"Generated C/C++ Vulkan Profiles library files ('{output_filename}')")