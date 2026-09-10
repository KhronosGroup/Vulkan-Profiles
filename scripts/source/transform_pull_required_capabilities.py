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

from typing import Any

from source.vulkan_object_utils import (
    VulkanObject, 
    VK_VERSION, 
    gatherDependentExtensions, 
    gatherCapabilityAliases,
    gatherSatisfiedCoreRequiredFeaturesForVersion,
    gatherSatisfiedCoreRequiredPropertiesForVersion,
    gatherSatisfiedExtensionRequiredFeatures,
    StructCapabilityAlias
)
from source.profiles_json_utils import (
    collect_block_names,
    collect_profile_capabilities,
    get_profile_and_file_data,
    get_primary_capability_block,
    get_topologically_sorted_file_keys,
    collect_required_profiles_capabilities_recursive,
    deep_merge_dict
)
from source.transform_utils import (
    canonicalize_capabilities_for_version,
    filter_features_against_context,
    filter_properties_against_context,
    update_context_from_capabilities
)


def pull_extension_dependencies_capabilities_block(
    vk: VulkanObject, 
    version: VK_VERSION, 
    ignore_extension_versions: bool, 
    json_profiles_capabilities_block: dict,
    context_extensions: set[str] = None,
    context_features: set[tuple[str, str]] = None
):
    if "extensions" not in json_profiles_capabilities_block:
        return

    context_extensions = context_extensions or set()
    context_features = context_features or set()

    curr_exts = dict(json_profiles_capabilities_block["extensions"])
    while True:
        raw_deps = gatherDependentExtensions(
            vk, version, ignore_extension_versions, curr_exts
        )
        if len(raw_deps) == len(curr_exts):
            break
        curr_exts = raw_deps

    block_exts = json_profiles_capabilities_block["extensions"]
    original_extensions = set(block_exts.keys()) if isinstance(block_exts, dict) else set(block_exts)
    original_order = list(block_exts.keys()) if isinstance(block_exts, dict) else list(block_exts)

    filtered_deps = {}

    for ext_name in original_order:
        if ext_name in raw_deps:
            if ext_name in original_extensions or ext_name not in context_extensions:
                filtered_deps[ext_name] = 1 if ignore_extension_versions else raw_deps[ext_name]

    for ext_name, ext_ver in raw_deps.items():
        if ext_name not in filtered_deps:
            if ext_name in original_extensions or ext_name not in context_extensions:
                filtered_deps[ext_name] = 1 if ignore_extension_versions else ext_ver

    json_profiles_capabilities_block["extensions"] = filtered_deps

    profile_enabled_exts = context_extensions | set(filtered_deps.keys())
    
    block_features = json_profiles_capabilities_block.get("features", {})
    enabled_features = set(context_features)
    if isinstance(block_features, dict):
        for s_name, members in block_features.items():
            if isinstance(members, dict):
                for m_name, val in members.items():
                    if val:
                        enabled_features.add((s_name, m_name))

    for ext_name in filtered_deps.keys():
        ext_satisfied, _ = gatherSatisfiedExtensionRequiredFeatures(
            vk, ext_name, version, profile_enabled_exts, enabled_features
        )
        if ext_satisfied:
            filtered_ext_satisfied = {}
            for s_name, members in ext_satisfied.items():
                if not isinstance(members, dict):
                    continue
                new_members = {}
                for m_name, val in members.items():
                    if not val:
                        continue

                    query_id = StructCapabilityAlias(s_name, m_name)
                    aliases = [query_id] + gatherCapabilityAliases(vk, query_id)

                    is_in_context = False
                    for alias in aliases:
                        if isinstance(alias, StructCapabilityAlias):
                            if (alias.struct, alias.member) in context_features:
                                is_in_context = True
                                break

                    if not is_in_context:
                        new_members[m_name] = val

                if new_members:
                    filtered_ext_satisfied[s_name] = new_members

            if filtered_ext_satisfied:
                features_dict = json_profiles_capabilities_block.setdefault("features", {})
                deep_merge_dict(features_dict, filtered_ext_satisfied)


def pull_extension_dependencies_profiles_file(
    vk: VulkanObject, 
    ignore_extension_versions: bool, 
    json_file_data: dict, 
    json_files_dict: dict = None
):
    profiles_data = json_file_data.get("profiles", {})
    json_profiles_capabilities = json_file_data.get("capabilities", {})

    for profile_key, profile_obj in profiles_data.items():
        api_version = VK_VERSION.from_string(profile_obj.get("api-version", "1.0.0"))

        context_extensions = set()
        context_features = set()
        parent_profiles = profile_obj.get("profiles", [])
        if json_files_dict and parent_profiles:
            parent_caps = collect_required_profiles_capabilities_recursive(json_files_dict, parent_profiles)
            context_extensions.update(parent_caps.get("extensions", {}).keys())
            parent_features_dict = parent_caps.get("features", {})
            for s_name, members in parent_features_dict.items():
                if isinstance(members, dict):
                    for m_name, val in members.items():
                        if val:
                            context_features.add((s_name, m_name))

        block_names = collect_block_names(profile_obj.get("capabilities", []))
        for block_name in block_names:
            if block_name in json_profiles_capabilities:
                block = json_profiles_capabilities[block_name]
                pull_extension_dependencies_capabilities_block(
                    vk, api_version, ignore_extension_versions, block, context_extensions, context_features
                )
                if "extensions" in block and isinstance(block["extensions"], dict):
                    context_extensions.update(block["extensions"].keys())
                if "features" in block and isinstance(block["features"], dict):
                    for s_name, members in block["features"].items():
                        if isinstance(members, dict):
                            for m_name, val in members.items():
                                if val:
                                    context_features.add((s_name, m_name))


def pull_extension_dependencies_profiles_files(vk: VulkanObject, ignore_extension_versions: bool, json_files_dict: dict):
    if not isinstance(json_files_dict, dict):
        return

    if "profiles" in json_files_dict or "capabilities" in json_files_dict:
        pull_extension_dependencies_profiles_file(vk, ignore_extension_versions, json_files_dict, None)
        return

    sorted_file_keys = get_topologically_sorted_file_keys(json_files_dict)
    for file_key in sorted_file_keys:
        json_file_data = json_files_dict[file_key]
        if isinstance(json_file_data, dict):
            pull_extension_dependencies_profiles_file(vk, ignore_extension_versions, json_file_data, json_files_dict)


def process_or_disjunction(
    vk: VulkanObject,
    profile_obj: dict,
    capabilities_dict: dict,
    block_name: str,
    disjunction: dict
):
    struct_name = disjunction["struct"]
    fields = disjunction["fields"]
    ext_name = disjunction.get("ext")

    parent_block = capabilities_dict.get(block_name, {})
    ext_ver_val = 1

    if ext_name and "extensions" in parent_block and isinstance(parent_block["extensions"], dict):
        if ext_name in parent_block["extensions"]:
            ext_ver_val = parent_block["extensions"].pop(ext_name)
            if not parent_block["extensions"]:
                del parent_block["extensions"]

    variant_block_names = []
    for field_name in fields:
        var_block_name = f"{block_name}_{field_name}"
        variant_block_names.append(var_block_name)

        var_block = capabilities_dict.setdefault(var_block_name, {})

        if ext_name:
            var_exts = var_block.setdefault("extensions", {})
            var_exts[ext_name] = ext_ver_val

        var_feats = var_block.setdefault("features", {})
        var_struct = var_feats.setdefault(struct_name, {})
        var_struct[field_name] = True

    profile_caps = profile_obj.setdefault("capabilities", [])
    if variant_block_names not in profile_caps:
        if block_name in profile_caps:
            idx = profile_caps.index(block_name)
            profile_caps.insert(idx + 1, variant_block_names)
        else:
            profile_caps.append(variant_block_names)


def pull_required_capabilities_profiles_file(vk: VulkanObject, json_files_dict: dict, json_file_data: dict):
    profiles_data = json_file_data.get("profiles", {})
    capabilities_dict = json_file_data.setdefault("capabilities", {})

    for key, profile_obj in profiles_data.items():
        api_version = VK_VERSION.from_string(profile_obj["api-version"])
        profile_caps_list = profile_obj.setdefault("capabilities", [])

        required_parent_profiles = profile_obj.get("profiles", [])

        context_features: set[tuple[str, str]] = set()
        context_properties: dict[str, Any] = {}
        context_extensions: set[str] = set()

        if required_parent_profiles:
            inherited_caps = collect_required_profiles_capabilities_recursive(json_files_dict, required_parent_profiles)
            update_context_from_capabilities(inherited_caps, context_features, context_properties, context_extensions)

        if required_parent_profiles:
            for parent_pname in required_parent_profiles:
                parent_obj, _ = get_profile_and_file_data(json_files_dict, parent_pname)
                if not parent_obj:
                    continue

                parent_api_version = VK_VERSION.from_string(parent_obj.get("api-version", "1.0.0"))
                if api_version > parent_api_version:
                    current_caps = collect_profile_capabilities(json_files_dict, json_file_data, profile_obj)
                    all_exts = context_extensions | set(current_caps.get("extensions", {}).keys())

                    all_features_dict = {}
                    if inherited_caps:
                        deep_merge_dict(all_features_dict, inherited_caps.get("features", {}))
                    deep_merge_dict(all_features_dict, current_caps.get("features", {}))

                    all_enabled_features_set: set[tuple[str, str]] = set()
                    for struct_name, members in all_features_dict.items():
                        if isinstance(members, dict):
                            for member_name, val in members.items():
                                if val:
                                    all_enabled_features_set.add((struct_name, member_name))

                    transition_features = {}
                    transition_properties = {}
                    for ver in VK_VERSION.versions():
                        if parent_api_version < ver <= api_version:
                            satisfied_feat, core_disj = gatherSatisfiedCoreRequiredFeaturesForVersion(
                                vk, ver, api_version, all_exts, all_enabled_features_set
                            )
                            if satisfied_feat:
                                deep_merge_dict(transition_features, satisfied_feat)

                            satisfied_prop = gatherSatisfiedCoreRequiredPropertiesForVersion(
                                vk, ver, api_version, all_exts, all_enabled_features_set
                            )
                            if satisfied_prop:
                                deep_merge_dict(transition_properties, satisfied_prop)

                    transition_features = filter_features_against_context(vk, transition_features, context_features)
                    transition_properties = filter_properties_against_context(vk, transition_properties, context_properties)

                    transition_features, transition_properties = canonicalize_capabilities_for_version(
                        vk, api_version, transition_features, transition_properties
                    )

                    if transition_features or transition_properties:
                        ver_tuple = api_version.as_tuple()
                        transition_block_name = f"{parent_pname}_to_vulkan{ver_tuple[0]}{ver_tuple[1]}"

                        first_direct_cap = None
                        for cap_item in profile_caps_list:
                            if isinstance(cap_item, str) and cap_item != transition_block_name:
                                first_direct_cap = cap_item
                                break
                            elif isinstance(cap_item, list) and cap_item:
                                first_direct_cap = cap_item[0]
                                break

                        trans_block = capabilities_dict.get(transition_block_name)
                        if trans_block is None:
                            trans_block = {}
                            if first_direct_cap and first_direct_cap in capabilities_dict:
                                new_caps_dict = {}
                                for c_key, c_val in capabilities_dict.items():
                                    if c_key == first_direct_cap:
                                        new_caps_dict[transition_block_name] = trans_block
                                    new_caps_dict[c_key] = c_val
                                capabilities_dict.clear()
                                capabilities_dict.update(new_caps_dict)
                            else:
                                capabilities_dict[transition_block_name] = trans_block

                        if transition_features:
                            trans_features = trans_block.setdefault("features", {})
                            deep_merge_dict(trans_features, transition_features)
                        if transition_properties:
                            trans_properties = trans_block.setdefault("properties", {})
                            deep_merge_dict(trans_properties, transition_properties)

                        if transition_block_name in profile_caps_list:
                            profile_caps_list.remove(transition_block_name)
                        profile_caps_list.insert(0, transition_block_name)

                        update_context_from_capabilities(trans_block, context_features, context_properties, context_extensions)

        else:
            primary_block = get_primary_capability_block(profile_obj, capabilities_dict)
            if primary_block is not None:
                profile_caps = collect_profile_capabilities(json_files_dict, json_file_data, profile_obj)
                profile_enabled_exts = set(profile_caps.get("extensions", {}).keys())
                profile_features_block = profile_caps.get("features", {})

                all_enabled_features_set: set[tuple[str, str]] = set()
                for struct_name, members in profile_features_block.items():
                    if isinstance(members, dict):
                        for member_name, val in members.items():
                            if val:
                                all_enabled_features_set.add((struct_name, member_name))

                core_satisfied_features = {}
                core_satisfied_properties = {}
                for ver in VK_VERSION.versions():
                    if ver <= api_version:
                        satisfied_feat, core_disj = gatherSatisfiedCoreRequiredFeaturesForVersion(
                            vk, ver, api_version, profile_enabled_exts, all_enabled_features_set
                        )
                        if satisfied_feat:
                            deep_merge_dict(core_satisfied_features, satisfied_feat)

                        satisfied_prop = gatherSatisfiedCoreRequiredPropertiesForVersion(
                            vk, ver, api_version, profile_enabled_exts, all_enabled_features_set
                        )
                        if satisfied_prop:
                            deep_merge_dict(core_satisfied_properties, satisfied_prop)

                core_satisfied_features = filter_features_against_context(vk, core_satisfied_features, context_features)
                core_satisfied_properties = filter_properties_against_context(vk, core_satisfied_properties, context_properties)

                core_satisfied_features, core_satisfied_properties = canonicalize_capabilities_for_version(
                    vk, api_version, core_satisfied_features, core_satisfied_properties
                )

                if core_satisfied_features:
                    block_features = primary_block.setdefault("features", {})
                    deep_merge_dict(block_features, core_satisfied_features)

                if core_satisfied_properties:
                    block_properties = primary_block.setdefault("properties", {})
                    deep_merge_dict(block_properties, core_satisfied_properties)

                if "features" in primary_block or "properties" in primary_block:
                    p_feat, p_prop = canonicalize_capabilities_for_version(
                        vk, api_version,
                        primary_block.get("features", {}),
                        primary_block.get("properties", {})
                    )
                    if p_feat:
                        primary_block["features"] = p_feat
                    if p_prop:
                        primary_block["properties"] = p_prop

        block_names = collect_block_names(profile_obj.get("capabilities", []))
        for block_name in block_names:
            if block_name in capabilities_dict:
                block = capabilities_dict[block_name]
                ext_dict = block.get("extensions", {})
                ext_list = list(ext_dict.keys()) if isinstance(ext_dict, dict) else (ext_dict if isinstance(ext_dict, list) else [])

                profile_caps = collect_profile_capabilities(json_files_dict, json_file_data, profile_obj)
                profile_enabled_exts = set(profile_caps.get("extensions", {}).keys())

                current_enabled_features: set[tuple[str, str]] = set(context_features)
                if "features" in block and isinstance(block["features"], dict):
                    for struct_name, members in block["features"].items():
                        if isinstance(members, dict):
                            for member_name, val in members.items():
                                if val:
                                    current_enabled_features.add((struct_name, member_name))

                for ext_name in ext_list:
                    ext_satisfied, ext_or_disjunctions = gatherSatisfiedExtensionRequiredFeatures(
                        vk, ext_name, api_version, profile_enabled_exts, current_enabled_features
                    )
                    if ext_or_disjunctions:
                        for disj in ext_or_disjunctions:
                            process_or_disjunction(
                                vk, profile_obj, capabilities_dict, block_name, disj
                            )

                    if ext_satisfied:
                        filtered_ext_satisfied = filter_features_against_context(vk, ext_satisfied, context_features)
                        if filtered_ext_satisfied:
                            block_features = block.setdefault("features", {})
                            deep_merge_dict(block_features, filtered_ext_satisfied)

                update_context_from_capabilities(block, context_features, context_properties, context_extensions)


def pull_required_capabilities_profiles_files(vk: VulkanObject, json_files_dict: dict):
    sorted_file_keys = get_topologically_sorted_file_keys(json_files_dict)
    for file_key in sorted_file_keys:
        pull_required_capabilities_profiles_file(vk, json_files_dict, json_files_dict[file_key])
        