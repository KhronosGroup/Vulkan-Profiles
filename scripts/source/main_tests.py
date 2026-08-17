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
import gen_profiles_tests

def main_tests(args):
    if args.registry is None or args.out_profile is None:
        print('Error: --registry and --out-profile are required for test generation.')
        sys.exit(1)

    registry = gen_profiles_solution.VulkanRegistry(args.registry, args.api)
    generator = gen_profiles_tests.ProfileGenerator()
    generator.generate_profile(args.out_profile, registry)
    if args.out_tests is not None:
        generator.generate_tests(args.out_tests, registry)
        
        