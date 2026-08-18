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
import gen_profiles_solution


def main_library(args):
    if not args.registry or not args.input:
        gen_profiles_solution.Log.e("Generating the profile library requires specifying --registry and --input")
        sys.exit(1)

    api = getattr(args, 'api', 'vulkan') or 'vulkan'
    registry = gen_profiles_solution.VulkanRegistry(args.registry, api)

    validate = getattr(args, 'validate', False)
    schema = None
    if validate:
        schema_gen = gen_profiles_solution.VulkanProfilesSchemaGenerator(registry)
        schema_gen.validate()
        schema = schema_gen.schema

    profiles_filenames = []
    input_filenames = getattr(args, 'input_filenames', None)
    if input_filenames:
        profiles_filenames = input_filenames.split(',')

    input_profiles_files = gen_profiles_solution.VulkanProfilesFiles(
        registry, args.input, profiles_filenames, validate, schema
    )

    debug = getattr(args, 'debug', False) or (getattr(args, 'config', 'release').lower() == 'debug')
    output_filename = getattr(args, 'output_filename', None) or 'vulkan_profiles'

    out_inc = getattr(args, 'output_inc', None)
    out_src = getattr(args, 'output_src', None)

    if not out_inc and not out_src:
        gen_profiles_solution.Log.e("At least one output directory (--output-inc or --output-src) must be provided")
        sys.exit(1)

    generator = gen_profiles_solution.VulkanProfilesLibraryGenerator(
        registry, input_profiles_files, output_filename, debug
    )
    generator.generate(out_inc, out_src)
