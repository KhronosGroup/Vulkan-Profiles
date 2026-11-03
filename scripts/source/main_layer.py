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

from source.generate_layer import generate_layer
from source.vulkan_object_utils import initVulkanObject

def main_layer(args):
    registry_path = getattr(args, 'registry', None)
    if registry_path is None or not Path(registry_path).is_file():
        logging.error(f"Registry file '{registry_path}' not found, use --registry to specify the path to vk.xml")
        sys.exit(1)

    output_path = "../layer/profiles_generated.cpp"
    out_val = getattr(args, 'output', None) or getattr(args, 'out_layer', None)
    if out_val is not None:
        output_path = out_val
    Path(output_path).parent.mkdir(parents=True, exist_ok=True)

    api = getattr(args, 'api', 'vulkan') or 'vulkan'

    if getattr(args, 'legacy', False):
        main_layer_legacy(registry_path, api, output_path)
        return

    vk = initVulkanObject(api, registry_path, video=True)
    generate_layer(vk, output_path)


def main_layer_legacy(registry_path, api, output_path):
    # Legacy code path loading vk.xml with gen_profiles_solution.VulkanRegistry
    import gen_profiles_solution
    import gen_profiles_layer

    registry = gen_profiles_solution.VulkanRegistry(registry_path, api)

    generator = gen_profiles_layer.VulkanProfilesLayerGenerator()
    generator.generate(output_path, registry)
    