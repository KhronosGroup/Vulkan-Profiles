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

import copy

from source.vulkan_object_utils import (
    VulkanObject, 
    VK_VERSION, 
    gatherDependentCapabilityAliases,
    is_member_unsupported_default,
    getStructDefiningExtensions,
    StructCapabilityAlias, 
    ExtensionCapabilityAlias
)
from source.profiles_json_utils import (
    collect_block_names,
    collect_profile_capabilities,
    get_topologically_sorted_file_keys,
    collect_required_profiles_capabilities_recursive,
    deep_merge_dict,
    get_profile_and_file_data,
    merge_capability_value
)
from source.format_flag_converter import FormatFeatureFlagConverter 
from source.transform_utils import (
    PullBits,
    _restore_member_orders,
    isStructExtensionEnabled
)


def pull_aliases_capabilities_block(
    vk: VulkanObject, 
    version: VK_VERSION, 
    json_profiles_capabilities_block: dict, 
    inherited_caps: dict = None,
    profile_enabled_exts: set[str] = None,
    pull_bits: list[PullBits] = None
) -> dict:
    pull_bits = pull_bits or []
    ignore_unratified = PullBits.IGNORE_UNRATIFIED in pull_bits

    inherited_caps = inherited_caps or {}
    ext_block = json_profiles_capabilities_block.get("extensions", {})
    block_exts = set(ext_block.keys()) if isinstance(ext_block, dict) else set(ext_block)
    inherited_exts = set(inherited_caps.get("extensions", {}).keys())

    enabled_exts = profile_enabled_exts if profile_enabled_exts is not None else (block_exts | inherited_exts)
    if ignore_unratified:
        enabled_exts = {ext for ext in enabled_exts if ext in vk.extensions and vk.extensions[ext].ratified}

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

                        if ignore_unratified:
                            def_exts = getStructDefiningExtensions(vk, target_struct)
                            if def_exts and any(ext in vk.extensions and not vk.extensions[ext].ratified for ext in def_exts):
                                continue

                        if is_dict:
                            if target_struct not in new_category_block:
                                new_category_block[target_struct] = {}
                            if target_member in new_category_block[target_struct]:
                                new_category_block[target_struct][target_member] = merge_capability_value(
                                    target_member, new_category_block[target_struct][target_member], val
                                )
                            else:
                                new_category_block[target_struct][target_member] = copy.deepcopy(val)
                        else:
                            if target_struct not in new_category_block:
                                new_category_block[target_struct] = []
                            if target_member not in new_category_block[target_struct]:
                                new_category_block[target_struct].append(target_member)

                    elif isinstance(alias, ExtensionCapabilityAlias):
                        if ignore_unratified and alias.name in vk.extensions and not vk.extensions[alias.name].ratified:
                            continue
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


def pull_inherited_aliases_profile(
    vk: VulkanObject,
    json_files_dict: dict,
    json_file_data: dict,
    profile_name: str,
    profile_obj: dict,
    pull_bits: list[PullBits]
):
    """
    For a profile requiring profiles with a lower Vulkan version, pulls the aliases of the supported
    required profiles capabilities which only exist with the profile Vulkan version, e.g. the
    VkPhysicalDeviceVulkan11Features::multiview alias of a Vulkan 1.1 required profile
    VkPhysicalDeviceMultiviewFeatures::multiview, for a Vulkan 1.2 profile. The aliases are added to the
    '<required profile>_to_vulkan<version>' transition block of the profile.
    """
    ignore_unratified = PullBits.IGNORE_UNRATIFIED in pull_bits
    version = VK_VERSION.from_string(profile_obj["api-version"])
    capabilities_dict = json_file_data.setdefault("capabilities", {})
    profile_caps_list = profile_obj.setdefault("capabilities", [])

    for required_profile_name in profile_obj.get("profiles", []):
        required_obj, _ = get_profile_and_file_data(json_files_dict, required_profile_name)
        if not required_obj:
            continue
        required_version = VK_VERSION.from_string(required_obj.get("api-version", "1.0.0"))
        if version <= required_version:
            continue

        inherited_caps = collect_required_profiles_capabilities_recursive(json_files_dict, [required_profile_name])
        profile_caps = collect_profile_capabilities(json_files_dict, json_file_data, profile_obj)
        enabled_exts = set(profile_caps.get("extensions", {}).keys())
        if ignore_unratified:
            enabled_exts = {ext for ext in enabled_exts if ext in vk.extensions and vk.extensions[ext].ratified}

        inherited_aliases = {}
        for category in ("features", "properties"):
            for struct_name, members in inherited_caps.get(category, {}).items():
                if not isinstance(members, dict):
                    continue
                for member, val in members.items():
                    query_id = StructCapabilityAlias(struct_name, member)
                    required_version_aliases = gatherDependentCapabilityAliases(vk, required_version, query_id)
                    for alias in gatherDependentCapabilityAliases(vk, version, query_id):
                        if not isinstance(alias, StructCapabilityAlias) or alias in required_version_aliases:
                            continue
                        if not isStructExtensionEnabled(vk, alias.struct, version, enabled_exts):
                            continue
                        if ignore_unratified:
                            def_exts = getStructDefiningExtensions(vk, alias.struct)
                            if def_exts and any(ext in vk.extensions and not vk.extensions[ext].ratified for ext in def_exts):
                                continue
                        if alias.member in profile_caps.get(category, {}).get(alias.struct, {}):
                            continue
                        # The transition block only expresses supported capabilities: unsupported default values
                        # (e.g. false for a feature) are not requirements of the profile Vulkan version.
                        if is_member_unsupported_default(vk, alias.struct, alias.member, val):
                            continue

                        target_members = inherited_aliases.setdefault(category, {}).setdefault(alias.struct, {})
                        if alias.member in target_members:
                            target_members[alias.member] = merge_capability_value(alias.member, target_members[alias.member], val)
                        else:
                            target_members[alias.member] = copy.deepcopy(val)

        if not inherited_aliases:
            continue

        ver_tuple = version.as_tuple()
        transition_block_name = f"{required_profile_name}_to_vulkan{ver_tuple[0]}{ver_tuple[1]}"
        deep_merge_dict(capabilities_dict.setdefault(transition_block_name, {}), inherited_aliases)
        if transition_block_name not in collect_block_names(profile_caps_list):
            profile_caps_list.insert(0, transition_block_name)


def pull_aliases_profiles_file(vk: VulkanObject, pull_bits: list[PullBits], json_files_dict: dict, json_file_data: dict):
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
                    vk, version, json_profiles_capabilities[block_name], inherited_caps, profile_enabled_exts, pull_bits
                )

        pull_inherited_aliases_profile(vk, json_files_dict, json_file_data, key, value, pull_bits)


def pull_aliases_profiles_files(vk: VulkanObject, pull_bits: list[PullBits], json_files_dict: dict):
    if not isinstance(json_files_dict, dict):
        return

    if "profiles" in json_files_dict or "capabilities" in json_files_dict:
        pull_aliases_profiles_file(vk, pull_bits, json_files_dict, json_files_dict)
        return

    sorted_file_keys = get_topologically_sorted_file_keys(json_files_dict)
    for file_key in sorted_file_keys:
        pull_aliases_profiles_file(vk, pull_bits, json_files_dict, json_files_dict[file_key])