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
import gen_profiles_solution
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
    registry = gen_profiles_solution.VulkanRegistry(args.registry, api)

    input_profiles = None
    raw_profiles = getattr(args, 'input_profiles', None)
    if raw_profiles:
        input_profiles = [p.strip() for p in raw_profiles.split(',') if p.strip()]

    validate = getattr(args, 'validate', False)
    schema = None

    input_profiles_files = gen_profiles_solution.VulkanProfilesFiles(
        registry, args.input, input_profiles, validate, schema
    )

    if input_profiles:
        missing_profiles = [p for p in input_profiles if p not in input_profiles_files.profiles]
        if missing_profiles:
            logging.error(f"Profile(s) specified in '--input-profiles' not found in database: {', '.join(missing_profiles)}")
            sys.exit(1)

    generator = gen_profiles_solution.VulkanProfilesDocGenerator(registry, input_profiles_files)
    generator.generate(args.output)