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
import json
from pathlib import Path

import gen_profiles_solution
from source.vulkan_object_utils import initVulkanObject
from source.generate_profiles_schema import VulkanProfilesSchemaGenerator2


def main_validate(args):
    try:
        import jsonschema
    except ModuleNotFoundError:
        gen_profiles_solution.Log.w("`jsonschema` module is not installed, skipping schema validation.")
        return

    registry = getattr(args, 'registry', None)
    api = getattr(args, 'api', 'vulkan') or 'vulkan'

    if getattr(args, 'schema', None):
        try:
            with open(args.schema, 'r') as f:
                schema = json.load(f)
        except json.JSONDecodeError as e:
            gen_profiles_solution.Log.e(f"JSON Parse Error in schema file '{args.schema}':\n  {e}")
            sys.exit(1)
        except Exception as e:
            gen_profiles_solution.Log.e(f"Failed to load schema file '{args.schema}': {e}")
            sys.exit(1)
    else:
        vk = initVulkanObject(api, registry, video=True)
        schema_gen = VulkanProfilesSchemaGenerator2(vk)
        schema = schema_gen.schema

    input_path = Path(args.input)
    if input_path.is_file():
        json_files = [input_path]
    elif input_path.is_dir():
        json_files = list(input_path.glob('*.json'))
    else:
        gen_profiles_solution.Log.e(f"Input path '{args.input}' does not exist.")
        sys.exit(1)

    for json_file in json_files:
        try:
            with open(json_file, 'r') as f:
                data = json.load(f)
            jsonschema.validate(instance=data, schema=schema)
            gen_profiles_solution.Log.i(f"Validation successful: '{json_file.name}'")
        except json.JSONDecodeError as e:
            gen_profiles_solution.Log.e(f"JSON Parse Error in '{json_file.name}':\n  {e}")
            sys.exit(1)
        except jsonschema.exceptions.ValidationError as e:
            location = " -> ".join(str(p) for p in e.path) if e.path else "root"
            gen_profiles_solution.Log.e(
                f"Validation Error in '{json_file.name}':\n"
                f"  Location : {location}\n"
                f"  Message  : {e.message}"
            )
            sys.exit(1)
        except jsonschema.exceptions.SchemaError as e:
            gen_profiles_solution.Log.e(f"Schema Error:\n  {e.message}")
            sys.exit(1)
