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

from source.generate_tests import generate_tests
from source.vulkan_object_utils import initVulkanObject

def main_tests(args):
    registry_arg = getattr(args, 'registry', None)
    output_profile = getattr(args, 'output_profile', None)

    if registry_arg is None or output_profile is None:
        print('Error: --registry and --output-profile are required for test generation.')
        sys.exit(1)

    api = getattr(args, 'api', 'vulkan') or 'vulkan'
    output_cpp = getattr(args, 'output_cpp', None)

    if getattr(args, 'legacy', False):
        main_tests_legacy(registry_arg, api, output_profile, output_cpp)
        return

    vk = initVulkanObject(api, registry_arg, video=True)
    generate_tests(vk, output_profile, output_cpp)


def main_tests_legacy(registry_path, api, output_profile, output_cpp):
    # Legacy code path loading vk.xml with gen_profiles_solution.VulkanRegistry
    import gen_profiles_solution
    import gen_profiles_tests

    registry = gen_profiles_solution.VulkanRegistry(registry_path, api)

    generator = gen_profiles_tests.ProfileGenerator(registry)
    generator.generate_profile(output_profile)
    if output_cpp is not None:
        generator.generate_tests(output_cpp)
        