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


def main_doc(args):
    if not args.registry or not args.input or not args.output:
        gen_profiles_solution.Log.e("Generating markdown documentation requires specifying --registry, --input, and --output")
        sys.exit(1)

    api = getattr(args, 'api', 'vulkan') or 'vulkan'
    registry = gen_profiles_solution.VulkanRegistry(args.registry, api)

    profiles_filenames = []
    input_filenames = getattr(args, 'input_filenames', None)
    if input_filenames:
        profiles_filenames = input_filenames.split(',')

    validate = getattr(args, 'validate', False)
    schema = None

    input_profiles_files = gen_profiles_solution.VulkanProfilesFiles(
        registry, args.input, profiles_filenames, validate, schema
    )

    generator = gen_profiles_solution.VulkanProfilesDocGenerator(registry, input_profiles_files)
    generator.generate(args.output)
