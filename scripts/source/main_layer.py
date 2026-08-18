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

import gen_profiles_solution
import gen_profiles_layer

def main_layer(args):
    registry_path = '../external/Debug/Vulkan-Headers/build/install/share/vulkan/registry/vk.xml'
    if getattr(args, 'registry', None) is not None:
        registry_path = args.registry

    output_path = "../layer/profiles_generated.cpp"
    out_val = getattr(args, 'output', None) or getattr(args, 'out_layer', None)
    if out_val is not None:
        output_path = out_val

    api = getattr(args, 'api', 'vulkan') or 'vulkan'
    registry = gen_profiles_solution.VulkanRegistry(registry_path, api)

    generator = gen_profiles_layer.VulkanProfilesLayerGenerator()
    generator.generate(output_path, registry)
    