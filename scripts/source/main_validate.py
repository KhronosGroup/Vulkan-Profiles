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

from source.vulkan_object_version import VK_VERSION, is_bundle_structure
from source.vulkan_object_utils import (
    VulkanObject,
    initVulkanObject,
    getStructCoreVersion,
    getStructDefiningExtensions,
    is_extension_struct_name,
    getExtensionPromotedTo,
    gatherCapabilityAliases,
    StructCapabilityAlias,
)
from source.profiles_json_utils import (
    load_profiles_jsons,
    validate_profiles_json,
    validate_profiles_jsons,
    validate_profiles_jsons_data,
    collect_profile_capabilities,
    get_profile_and_file_data,
)
from source.generate_profiles_schema import VulkanProfilesSchemaGenerator2


class VulkanProfilesDataValidation:
    def __init__(self, vk: VulkanObject):
        self.vk = vk

    def validate_required_profile_api_versions(
        self,
        json_files_dict: dict,
        profile_name: str,
        profile_obj: dict
    ) -> list[str]:
        """Analysis Case 1: Recursively checks that all inherited required profiles target an older or equal API version."""
        issues = []
        child_api_version = VK_VERSION.from_string(profile_obj.get("api-version", "1.0.0"))

        def check_parents_recursive(req_pnames: list[str], visited: set[str]):
            for req_pname in req_pnames:
                if req_pname in visited:
                    continue
                visited.add(req_pname)

                req_profile_obj, _ = get_profile_and_file_data(json_files_dict, req_pname)
                if not req_profile_obj:
                    continue

                req_api_version = VK_VERSION.from_string(req_profile_obj.get("api-version", "1.0.0"))
                if child_api_version < req_api_version:
                    issues.append(
                        f"Profile '{profile_name}' targets Vulkan {child_api_version} "
                        f"but inherits profile '{req_pname}' which targets newer Vulkan {req_api_version}"
                    )

                nested_parents = req_profile_obj.get("profiles", [])
                if nested_parents:
                    check_parents_recursive(nested_parents, visited)

        req_profiles = profile_obj.get("profiles", [])
        if req_profiles:
            check_parents_recursive(req_profiles, set())

        return issues

    def validate_core_structures_and_extensions(
        self,
        profile_name: str,
        api_version: VK_VERSION,
        category_dict: dict,
        enabled_exts: set[str]
    ) -> list[str]:
        """Analysis Case 2: Checks that required core structures and extension structures match declared profile API version and extensions."""
        issues = []

        for struct_name in category_dict.keys():
            core_ver = getStructCoreVersion(self.vk, struct_name)
            if core_ver != VK_VERSION.NONE and api_version < core_ver:
                ver_tuple = core_ver.as_tuple()
                issues.append(
                    f"Core structure '{struct_name}' requires Vulkan {ver_tuple[0]}.{ver_tuple[1]} "
                    f"but profile '{profile_name}' targets Vulkan {api_version}"
                )

            if is_extension_struct_name(self.vk, struct_name):
                def_exts = getStructDefiningExtensions(self.vk, struct_name)
                if def_exts:
                    declared = False
                    for ext_name in def_exts:
                        if ext_name in enabled_exts:
                            declared = True
                            break
                        promoted = getExtensionPromotedTo(self.vk, ext_name)
                        for p_target in promoted:
                            p_ver = VK_VERSION.from_string(p_target)
                            if p_ver != VK_VERSION.NONE and api_version >= p_ver:
                                declared = True
                                break
                        if declared:
                            break

                    if not declared:
                        issues.append(
                            f"Structure '{struct_name}' in profile '{profile_name}' "
                            f"belongs to extension '{def_exts[0]}' which is not declared in extensions"
                        )

        return issues

    def validate_aliased_capabilities(
        self,
        profile_name: str,
        category_dict: dict,
        checked_aliases: set[tuple[str, str]]
    ) -> list[str]:
        """Analysis Case 3: Checks that aliased structure members have consistent values across capability definitions."""
        issues = []

        def are_values_equal(val1, val2) -> bool:
            if isinstance(val1, list) and isinstance(val2, list):
                return sorted(val1) == sorted(val2)
            return val1 == val2

        for struct_name, members in category_dict.items():
            if not isinstance(members, dict):
                continue

            for member_name, val in members.items():
                pair_key = (struct_name, member_name)
                if pair_key in checked_aliases:
                    continue

                checked_aliases.add(pair_key)
                query_id = StructCapabilityAlias(struct_name, member_name)
                aliases = gatherCapabilityAliases(self.vk, query_id)

                for alias in aliases:
                    if isinstance(alias, StructCapabilityAlias):
                        checked_aliases.add((alias.struct, alias.member))
                        if alias.struct in category_dict:
                            alias_members = category_dict[alias.struct]
                            if isinstance(alias_members, dict) and alias.member in alias_members:
                                alias_val = alias_members[alias.member]
                                if not are_values_equal(val, alias_val):
                                    issues.append(
                                        f"Member '{member_name}' in structure '{struct_name}' (value: {val}) "
                                        f"has mismatching value with member '{alias.member}' in aliased structure '{alias.struct}' (value: {alias_val}) "
                                        f"in profile '{profile_name}'"
                                    )

        return issues

    def validate_data(self, json_files_dict: dict) -> list[str]:
        """Executes all data analysis validation cases across loaded profile files."""
        issues = []

        for file_path, json_file_data in json_files_dict.items():
            if not isinstance(json_file_data, dict) or "profiles" not in json_file_data:
                continue

            for profile_name, profile_obj in json_file_data["profiles"].items():
                api_version = VK_VERSION.from_string(profile_obj.get("api-version", "1.0.0"))
                profile_caps = collect_profile_capabilities(json_files_dict, json_file_data, profile_obj)
                enabled_exts = set(profile_caps.get("extensions", {}).keys())

                # Analysis Case 1: Recursive Required Profile API Version Inheritance Checks
                issues.extend(
                    self.validate_required_profile_api_versions(
                        json_files_dict, profile_name, profile_obj
                    )
                )

                for category in ("features", "properties"):
                    category_dict = profile_caps.get(category, {})
                    if not isinstance(category_dict, dict):
                        continue

                    # Analysis Case 2: Core Structure & Extension Declaration Checks
                    issues.extend(
                        self.validate_core_structures_and_extensions(
                            profile_name, api_version, category_dict, enabled_exts
                        )
                    )

                    # Analysis Case 3: Aliased Capability Value Mismatch Checks
                    checked_aliases = set()
                    issues.extend(
                        self.validate_aliased_capabilities(
                            profile_name, category_dict, checked_aliases
                        )
                    )

        return issues


def main_validate(args):
    input_path = Path(args.input)
    schema_path = Path(args.schema) if getattr(args, 'schema', None) else None

    modes = getattr(args, 'mode', ['schema', 'analysis'])
    if isinstance(modes, str):
        modes = [modes]

    vk = initVulkanObject(getattr(args, 'api', 'vulkan') or 'vulkan', getattr(args, 'registry', None))

    if 'schema' in modes:
        if schema_path is None:
            schema_gen = VulkanProfilesSchemaGenerator2(vk)
            schema_data = schema_gen.schema
            count = validate_profiles_jsons_data(input_path, schema_data)
            logging.info(f"Validated {count} file(s) against generated schema.")
        else:
            if input_path.is_file():
                validate_profiles_json(input_path, schema_path)
            else:
                count = validate_profiles_jsons(input_path, schema_path)
                logging.info(f"Validated {count} file(s) against schema {schema_path}.")

    if 'analysis' in modes:
        json_files_dict = load_profiles_jsons(input_path)
        validator = VulkanProfilesDataValidation(vk)
        issues = validator.validate_data(json_files_dict)
        if issues:
            for issue in issues:
                logging.warning(issue)
