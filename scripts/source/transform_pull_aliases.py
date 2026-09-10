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

from source.vulkan_object_utils import (
    VulkanObject, 
    VK_VERSION, 
    gatherDependentCapabilityAliases,
    StructCapabilityAlias, 
    ExtensionCapabilityAlias
)
from source.profiles_json_utils import (
    collect_block_names,
    collect_profile_capabilities,
    get_topologically_sorted_file_keys,
    collect_required_profiles_capabilities_recursive,
    deep_merge_dict
)
from source.format_flag_converter import FormatFeatureFlagConverter 
from source.transform_utils import (
    _restore_member_orders,
    isStructExtensionEnabled
)


def pull_aliases_capabilities_block(
    vk: VulkanObject, 
    version: VK_VERSION, 
    json_profiles_capabilities_block: dict, 
    inherited_caps: dict = None,
    profile_enabled_exts: set[str] = None
) -> dict:
    inherited_caps = inherited_caps or {}
    ext_block = json_profiles_capabilities_block.get("extensions", {})
    block_exts = set(ext_block.keys()) if isinstance(ext_block, dict) else set(ext_block)
    inherited_exts = set(inherited_caps.get("extensions", {}).keys())

    enabled_exts = profile_enabled_exts if profile_enabled_exts is not None else (block_exts | inherited_exts)

    for category in ("features", "properties"):
        category_block = json_profiles_capabilities_block.get(category, {})

        if not category_block:
            continue

        original_member_orders = {
            s: list(m.keys()) for s, m in category_block.items()
            if isinstance(m, dict)
        }

        new_category_block = {}

        for struct_name, members in category_block.items():
            is_dict = isinstance(members, dict)

            for member in members:
                val = members[member] if is_dict else None

                query_id = StructCapabilityAlias(struct_name, member)
                dependent_aliases = gatherDependentCapabilityAliases(vk, version, query_id)

                all_aliases = [query_id] + dependent_aliases

                for alias in all_aliases:
                    if isinstance(alias, StructCapabilityAlias):
                        target_struct = alias.struct
                        target_member = alias.member

                        if not isStructExtensionEnabled(vk, target_struct, version, enabled_exts):
                            continue

                        if is_dict:
                            if target_struct not in new_category_block:
                                new_category_block[target_struct] = {}
                            new_category_block[target_struct][target_member] = val
                        else:
                            if target_struct not in new_category_block:
                                new_category_block[target_struct] = []
                            if target_member not in new_category_block[target_struct]:
                                new_category_block[target_struct].append(target_member)

                    elif isinstance(alias, ExtensionCapabilityAlias):
                        if alias.name in enabled_exts and "extensions" in json_profiles_capabilities_block:
                            target_ext_block = json_profiles_capabilities_block["extensions"]
                            if isinstance(target_ext_block, dict):
                                target_ext_block[alias.name] = 1
                            elif isinstance(target_ext_block, list) and alias.name not in target_ext_block:
                                target_ext_block.append(alias.name)

        if new_category_block:
            _restore_member_orders(new_category_block, original_member_orders)
            json_profiles_capabilities_block[category] = new_category_block

    formats_block = json_profiles_capabilities_block.get("formats", {})

    if formats_block:
        new_formats_block = {}
        flag_converter = FormatFeatureFlagConverter(vk)

        for format_name, structs_dict in formats_block.items():
            if not isinstance(structs_dict, dict):
                continue

            new_structs_dict = {}

            for src_struct_name, members_dict in structs_dict.items():
                if isinstance(members_dict, dict):
                    expanded = flag_converter.expand_format_struct(
                        vk, src_struct_name, members_dict, version, enabled_exts
                    )
                    deep_merge_dict(new_structs_dict, expanded)

            if new_structs_dict:
                new_formats_block[format_name] = new_structs_dict

        if new_formats_block:
            json_profiles_capabilities_block["formats"] = new_formats_block

    return json_profiles_capabilities_block


def pull_aliases_profiles_file(vk: VulkanObject, require_promoted_extensions: bool, json_files_dict, json_file_data):
    profiles_data = json_file_data["profiles"]
    json_profiles_capabilities = json_file_data["capabilities"]

    for key, value in profiles_data.items():
        version = VK_VERSION.from_string(value["api-version"])

        required_profile_names = value.get("profiles", [])
        inherited_caps = collect_required_profiles_capabilities_recursive(json_files_dict, required_profile_names)

        profile_caps = collect_profile_capabilities(json_files_dict, json_file_data, value)
        profile_enabled_exts = set(profile_caps.get("extensions", {}).keys())

        block_names = collect_block_names(value["capabilities"])
        
        for block_name in block_names:
            if block_name in json_profiles_capabilities:
                pull_aliases_capabilities_block(
                    vk, version, json_profiles_capabilities[block_name], inherited_caps, profile_enabled_exts
                )


def pull_aliases_profiles_files(vk: VulkanObject, require_promoted_extensions: bool, json_files_dict: dict):
    if not isinstance(json_files_dict, dict):
        return

    if "profiles" in json_files_dict or "capabilities" in json_files_dict:
        pull_aliases_profiles_file(vk, require_promoted_extensions, json_files_dict, json_files_dict)
        return

    sorted_file_keys = get_topologically_sorted_file_keys(json_files_dict)
    for file_key in sorted_file_keys:
        pull_aliases_profiles_file(vk, require_promoted_extensions, json_files_dict, json_files_dict[file_key])
        