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
from source.vulkan_object_utils import initVulkanObject, VK_VERSION, getStructByName
from source.generate_profiles_schema import VulkanProfilesSchemaGenerator2


def _get_extension_promoted_to(vk, ext_name: str) -> list[str]:
    if hasattr(vk, 'extensions') and ext_name in vk.extensions:
        ext_obj = vk.extensions[ext_name]
        promoted = getattr(ext_obj, 'promotedTo', None) or getattr(ext_obj, 'promotedto', None)
        if isinstance(promoted, list):
            return [p for p in promoted if p]
        elif isinstance(promoted, str) and promoted:
            return [p.strip() for p in promoted.split(',') if p.strip()]
    return []


def _get_struct_defining_extensions(vk, struct_name: str) -> list[str]:
    exts = set()

    struct_obj = vk.structs.get(struct_name) or getStructByName(vk.structs, struct_name)
    if struct_obj:
        if hasattr(struct_obj, 'definingRequirements') and struct_obj.definingRequirements:
            exts.update(struct_obj.definingRequirements.keys())
        if hasattr(struct_obj, 'extensions') and struct_obj.extensions:
            exts.update(struct_obj.extensions)

    if hasattr(vk, 'aliasTypeRequirements') and struct_name in vk.aliasTypeRequirements:
        exts.update(vk.aliasTypeRequirements[struct_name].keys())

    if hasattr(vk, 'extensions'):
        for e_name, e_obj in vk.extensions.items():
            e_structs = getattr(e_obj, 'structs', {})
            if isinstance(e_structs, (dict, list, set)) and struct_name in e_structs:
                exts.add(e_name)
            e_features = getattr(e_obj, 'features', {})
            if isinstance(e_features, (dict, list, set)) and struct_name in e_features:
                exts.add(e_name)

    return [e for e in exts if hasattr(vk, 'extensions') and e in vk.extensions]


def _get_struct_core_version(vk, struct_name: str) -> VK_VERSION:
    if struct_name in ("VkPhysicalDeviceVulkan11Features", "VkPhysicalDeviceVulkan11Properties"):
        return VK_VERSION.V1_2
    if struct_name in ("VkPhysicalDeviceVulkan12Features", "VkPhysicalDeviceVulkan12Properties"):
        return VK_VERSION.V1_2
    if struct_name in ("VkPhysicalDeviceVulkan13Features", "VkPhysicalDeviceVulkan13Properties"):
        return VK_VERSION.V1_3
    if struct_name in ("VkPhysicalDeviceVulkan14Features", "VkPhysicalDeviceVulkan14Properties"):
        return VK_VERSION.V1_4

    struct_obj = vk.structs.get(struct_name) or getStructByName(vk.structs, struct_name)
    if struct_obj:
        def_ver = getattr(struct_obj, 'definedByVersion', None)
        if def_ver is not None and def_ver != VK_VERSION.NONE:
            return def_ver

        def_reqs = getattr(struct_obj, 'definingRequirements', {})
        for req in def_reqs.keys():
            ver = VK_VERSION.from_string(req)
            if ver != VK_VERSION.NONE:
                return ver

    return VK_VERSION.NONE


def _collect_block_names(json_capabilities):
    block_names = []
    for value in json_capabilities:
        if isinstance(value, str):
            block_names.append(value)
        elif isinstance(value, list):
            for val in value:
                if isinstance(val, str):
                    block_names.append(val)
    return block_names


class VulkanProfilesDataValidation:
    """
    Performs static analysis checks on Vulkan profile JSON data against the Vulkan registry.
    """
    def __init__(self, vk):
        self.vk = vk

    def validate_data(self, json_files_dict: dict) -> list[str]:
        issues = []

        for file_key, data in json_files_dict.items():
            filename = Path(file_key).name if isinstance(file_key, (str, Path)) else str(file_key)
            if not isinstance(data, dict):
                continue

            profiles = data.get('profiles', {})
            capabilities = data.get('capabilities', {})

            for profile_name, profile_obj in profiles.items():
                api_version_str = profile_obj.get('api-version', '1.0.0')
                api_version = VK_VERSION.from_string(api_version_str)

                block_names = _collect_block_names(profile_obj.get('capabilities', []))
                for block_name in block_names:
                    if block_name not in capabilities:
                        continue

                    block = capabilities[block_name]
                    block_exts = set(block.get('extensions', {}).keys()) if isinstance(block.get('extensions'), dict) else set()

                    for section in ('features', 'properties'):
                        section_dict = block.get(section, {})
                        if not isinstance(section_dict, dict):
                            continue

                        for struct_name in section_dict.keys():
                            # Issue 1: Capabilities block lists extension structure without declaring the extension
                            def_exts = _get_struct_defining_extensions(self.vk, struct_name)
                            for ext_name in def_exts:
                                promoted_targets = _get_extension_promoted_to(self.vk, ext_name)
                                promoted_to_core = False
                                for target in promoted_targets:
                                    p_ver = VK_VERSION.from_string(target)
                                    if p_ver != VK_VERSION.NONE and api_version != VK_VERSION.NONE and api_version >= p_ver:
                                        promoted_to_core = True
                                        break

                                if not promoted_to_core and ext_name not in block_exts:
                                    issues.append(
                                        f"Analysis Issue in '{filename}' (Profile '{profile_name}', Block '{block_name}'): "
                                        f"Structure '{struct_name}' belongs to extension '{ext_name}', but '{ext_name}' is not declared in 'extensions'."
                                    )

                            # Issue 2: Vulkan core structure used with an older Vulkan version
                            struct_core_ver = _get_struct_core_version(self.vk, struct_name)
                            if struct_core_ver != VK_VERSION.NONE and api_version != VK_VERSION.NONE and api_version < struct_core_ver:
                                ver_tuple = struct_core_ver.as_tuple()
                                ver_str = f"{ver_tuple[0]}.{ver_tuple[1]}"
                                issues.append(
                                    f"Analysis Issue in '{filename}' (Profile '{profile_name}', Block '{block_name}'): "
                                    f"Core structure '{struct_name}' requires Vulkan {ver_str}, but profile api-version is '{api_version_str}'."
                                )

        return issues

    def validate_files(self, json_files: list[Path]) -> bool:
        json_files_dict = {}
        for f in json_files:
            try:
                with open(f, 'r', encoding='utf-8') as file_obj:
                    json_files_dict[f] = json.load(file_obj)
            except Exception as e:
                gen_profiles_solution.Log.e(f"Failed to load profile file for analysis '{f.name}': {e}")
                return False

        issues = self.validate_data(json_files_dict)
        for issue in issues:
            gen_profiles_solution.Log.e(issue)

        return len(issues) == 0


def main_validate(args):
    modes = getattr(args, 'mode', None) or ['schema', 'analysis']

    registry = getattr(args, 'registry', None)
    api = getattr(args, 'api', 'vulkan') or 'vulkan'

    input_path = Path(args.input)
    if input_path.is_file():
        json_files = [input_path]
    elif input_path.is_dir():
        json_files = list(input_path.glob('*.json'))
    else:
        gen_profiles_solution.Log.e(f"Input path '{args.input}' does not exist.")
        sys.exit(1)

    vk = None
    if 'schema' in modes or 'analysis' in modes:
        vk = initVulkanObject(api, registry, video=True)

    # 1. Schema Validation Mode
    if 'schema' in modes:
        try:
            import jsonschema
        except ModuleNotFoundError:
            gen_profiles_solution.Log.w("`jsonschema` module is not installed, skipping schema validation.")
        else:
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
                schema_gen = VulkanProfilesSchemaGenerator2(vk)
                schema = schema_gen.schema

            for json_file in json_files:
                try:
                    with open(json_file, 'r') as f:
                        data = json.load(f)
                    jsonschema.validate(instance=data, schema=schema)
                    gen_profiles_solution.Log.i(f"Schema Validation successful: '{json_file.name}'")
                except json.JSONDecodeError as e:
                    gen_profiles_solution.Log.e(f"JSON Parse Error in '{json_file.name}':\n  {e}")
                    sys.exit(1)
                except jsonschema.exceptions.ValidationError as e:
                    location = " -> ".join(str(p) for p in e.path) if e.path else "root"
                    gen_profiles_solution.Log.e(
                        f"Schema Validation Error in '{json_file.name}':\n"
                        f"  Location : {location}\n"
                        f"  Message  : {e.message}"
                    )
                    sys.exit(1)
                except jsonschema.exceptions.SchemaError as e:
                    gen_profiles_solution.Log.e(f"Schema Error:\n  {e.message}")
                    sys.exit(1)

    # 2. Static Analysis Mode
    if 'analysis' in modes:
        analyzer = VulkanProfilesDataValidation(vk)
        success = analyzer.validate_files(json_files)
        if not success:
            sys.exit(1)
