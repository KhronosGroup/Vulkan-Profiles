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

from pathlib import Path

from source.vulkan_object_utils import initVulkanObject
from source.generate_profiles_schema import VulkanProfilesSchemaGenerator2
from source.profiles_parsing import validate_profiles_json, validate_profiles_jsons_data
from source.log import Log

def main_validate(args):
    if args.schema is None:
        if args.registry is None:
            Log.e("`--schema` or `--registry` are required to validate profile files")
        else:
            vk = initVulkanObject(args.api, args.registry, True)
            generator2 = VulkanProfilesSchemaGenerator2(vk)
            validate_profiles_jsons_data(Path(args.input), generator2.schema)
    else:
        validate_profiles_json(Path(args.input), Path(args.schema))
