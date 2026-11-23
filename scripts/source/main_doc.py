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

import sys
import logging
import argparse
from source.generate_profiles_doc import generate_profiles_doc
from source.vulkan_object_utils import initVulkanObject
from source.profiles_json_utils import load_profiles_jsons
from source.main_validate import main_validate


def main_doc(args):
    if not args.registry or not args.input or not args.output:
        logging.error("Generating markdown documentation requires specifying --registry, --input, and --output")
        sys.exit(1)

    if getattr(args, 'validate', False):
        validate_args = argparse.Namespace(
            registry=getattr(args, 'registry', None),
            input=args.input,
            schema=getattr(args, 'schema', None),
            api=getattr(args, 'api', 'vulkan') or 'vulkan'
        )
        main_validate(validate_args)

    api = getattr(args, 'api', 'vulkan') or 'vulkan'

    input_profiles = None
    raw_profiles = getattr(args, 'input_profiles', None)
    if raw_profiles:
        input_profiles = [p.strip() for p in raw_profiles.split(',') if p.strip()]

    # Only profiles files are loaded
    json_files = [ data for data in load_profiles_jsons(args.input).values() if 'capabilities' in data ]

    if input_profiles:
        available_profiles = { p_name for data in json_files for p_name in data['profiles'].keys() }
        missing_profiles = [p for p in input_profiles if p not in available_profiles]
        if missing_profiles:
            logging.error(f"Profile(s) specified in '--input-profiles' not found in database: {', '.join(missing_profiles)}")
            sys.exit(1)

    vk = initVulkanObject(api, args.registry, video=True)
    generate_profiles_doc(vk, json_files, input_profiles, args.output)
