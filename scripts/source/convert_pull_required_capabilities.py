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
    collect_required_profiles_capabilities,
    get_topologically_sorted_file_keys,
    get_primary_capability_block,
    get_profile_and_file_data,
    deep_merge_dict,
    is_property_satisfied
)
from source.convert_utils import (
    canonicalize_capabilities_for_version,
    get_parent_property_value
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
        ext_satisfied = gatherSatisfiedExtensionRequiredFeatures(
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
            parent_caps = collect_required_profiles_capabilities(json_files_dict, parent_profiles)
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


def pull_required_capabilities_profiles_file(vk: VulkanObject, json_files_dict: dict, json_file_data: dict):
    profiles_data = json_file_data.get("profiles", {})
    capabilities_dict = json_file_data.setdefault("capabilities", {})

    for key, profile_obj in profiles_data.items():
        api_version = VK_VERSION.from_string(profile_obj["api-version"])
        profile_caps_list = profile_obj.setdefault("capabilities", [])

        required_parent_profiles = profile_obj.get("profiles", [])

        if required_parent_profiles:
            for parent_pname in required_parent_profiles:
                parent_obj, _ = get_profile_and_file_data(json_files_dict, parent_pname)
                if not parent_obj:
                    continue

                parent_api_version = VK_VERSION.from_string(parent_obj.get("api-version", "1.0.0"))
                if api_version > parent_api_version:
                    parent_inherited_caps = collect_required_profiles_capabilities(json_files_dict, [parent_pname])
                    current_caps = collect_profile_capabilities(json_files_dict, json_file_data, profile_obj)

                    all_exts = set(parent_inherited_caps.get("extensions", {}).keys()) | set(current_caps.get("extensions", {}).keys())
                    all_features_dict = {}
                    deep_merge_dict(all_features_dict, parent_inherited_caps.get("features", {}))
                    deep_merge_dict(all_features_dict, current_caps.get("features", {}))

                    enabled_features_set: set[tuple[str, str]] = set()
                    for struct_name, members in all_features_dict.items():
                        if isinstance(members, dict):
                            for member_name, val in members.items():
                                if val:
                                    enabled_features_set.add((struct_name, member_name))

                    transition_features = {}
                    transition_properties = {}
                    for ver in VK_VERSION.versions():
                        if parent_api_version < ver <= api_version:
                            satisfied_feat = gatherSatisfiedCoreRequiredFeaturesForVersion(
                                vk, ver, api_version, all_exts, enabled_features_set
                            )
                            if satisfied_feat:
                                deep_merge_dict(transition_features, satisfied_feat)

                            satisfied_prop = gatherSatisfiedCoreRequiredPropertiesForVersion(
                                vk, ver, api_version, all_exts, enabled_features_set
                            )
                            if satisfied_prop:
                                deep_merge_dict(transition_properties, satisfied_prop)

                    parent_features_dict = parent_inherited_caps.get("features", {})
                    parent_enabled_features: set[tuple[str, str]] = set()
                    for s_name, members in parent_features_dict.items():
                        if isinstance(members, dict):
                            for m_name, val in members.items():
                                if val:
                                    parent_enabled_features.add((s_name, m_name))

                    filtered_transition_features = {}
                    for s_name, members in transition_features.items():
                        if not isinstance(members, dict):
                            continue
                        new_members = {}
                        for m_name, val in members.items():
                            if not val:
                                continue

                            query_id = StructCapabilityAlias(s_name, m_name)
                            aliases = [query_id] + gatherCapabilityAliases(vk, query_id)

                            is_in_parent = False
                            for alias in aliases:
                                if isinstance(alias, StructCapabilityAlias):
                                    if (alias.struct, alias.member) in parent_enabled_features:
                                        is_in_parent = True
                                        break

                            if not is_in_parent:
                                new_members[m_name] = val

                        if new_members:
                            filtered_transition_features[s_name] = new_members

                    transition_features = filtered_transition_features
                    parent_props_dict = parent_inherited_caps.get("properties", {})

                    filtered_transition_properties = {}
                    for s_name, p_data in transition_properties.items():
                        if not isinstance(p_data, dict):
                            continue

                        if s_name == "VkPhysicalDeviceProperties":
                            new_s_data = {}
                            for sub_group_name, sub_dict in p_data.items():
                                if not isinstance(sub_dict, dict):
                                    continue
                                new_sub_dict = {}
                                for prop_name, prop_val in sub_dict.items():
                                    found_in_parent, parent_val = get_parent_property_value(
                                        parent_props_dict, s_name, prop_name, vk
                                    )
                                    if not found_in_parent or not is_property_satisfied(parent_val, prop_val, prop_name):
                                        new_sub_dict[prop_name] = prop_val
                                if new_sub_dict:
                                    new_s_data[sub_group_name] = new_sub_dict
                            if new_s_data:
                                filtered_transition_properties[s_name] = new_s_data
                        else:
                            new_p_data = {}
                            for prop_name, prop_val in p_data.items():
                                found_in_parent, parent_val = get_parent_property_value(
                                    parent_props_dict, s_name, prop_name, vk
                                )
                                if not found_in_parent or not is_property_satisfied(parent_val, prop_val, prop_name):
                                    new_p_data[prop_name] = prop_val
                            if new_p_data:
                                filtered_transition_properties[s_name] = new_p_data

                    transition_properties = filtered_transition_properties

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

        else:
            primary_block = get_primary_capability_block(profile_obj, capabilities_dict)
            if primary_block is not None:
                profile_caps = collect_profile_capabilities(json_files_dict, json_file_data, profile_obj)
                profile_enabled_exts = set(profile_caps.get("extensions", {}).keys())
                profile_features_block = profile_caps.get("features", {})

                enabled_features_set: set[tuple[str, str]] = set()
                for struct_name, members in profile_features_block.items():
                    if isinstance(members, dict):
                        for member_name, val in members.items():
                            if val:
                                enabled_features_set.add((struct_name, member_name))

                core_satisfied_features = {}
                core_satisfied_properties = {}
                for ver in VK_VERSION.versions():
                    if ver <= api_version:
                        satisfied_feat = gatherSatisfiedCoreRequiredFeaturesForVersion(
                            vk, ver, api_version, profile_enabled_exts, enabled_features_set
                        )
                        if satisfied_feat:
                            deep_merge_dict(core_satisfied_features, satisfied_feat)

                        satisfied_prop = gatherSatisfiedCoreRequiredPropertiesForVersion(
                            vk, ver, api_version, profile_enabled_exts, enabled_features_set
                        )
                        if satisfied_prop:
                            deep_merge_dict(core_satisfied_properties, satisfied_prop)

                core_satisfied_features, core_satisfied_properties = canonicalize_capabilities_for_version(
                    vk, api_version, core_satisfied_features, core_satisfied_properties
                )

                if core_satisfied_features:
                    block_features = primary_block.setdefault("features", {})
                    deep_merge_dict(block_features, core_satisfied_features)

                if core_satisfied_properties:
                    block_properties = primary_block.setdefault("properties", {})
                    deep_merge_dict(block_properties, core_satisfied_properties)

        profile_caps = collect_profile_capabilities(json_files_dict, json_file_data, profile_obj)
        profile_enabled_exts = set(profile_caps.get("extensions", {}).keys())
        profile_features_block = profile_caps.get("features", {})

        enabled_features: set[tuple[str, str]] = set()
        for struct_name, members in profile_features_block.items():
            if isinstance(members, dict):
                for member_name, val in members.items():
                    if val:
                        enabled_features.add((struct_name, member_name))

        ver_tuple = api_version.as_tuple()
        curr_trans_suffix = f"_to_vulkan{ver_tuple[0]}{ver_tuple[1]}"

        block_names = collect_block_names(profile_obj.get("capabilities", []))
        for block_name in block_names:
            if block_name in capabilities_dict and not block_name.endswith(curr_trans_suffix):
                block = capabilities_dict[block_name]
                ext_dict = block.get("extensions", {})
                ext_list = list(ext_dict.keys()) if isinstance(ext_dict, dict) else (ext_dict if isinstance(ext_dict, list) else [])

                for ext_name in ext_list:
                    ext_satisfied = gatherSatisfiedExtensionRequiredFeatures(
                        vk, ext_name, api_version, profile_enabled_exts, enabled_features
                    )
                    if ext_satisfied:
                        block_features = block.setdefault("features", {})
                        deep_merge_dict(block_features, ext_satisfied)


def pull_required_capabilities_profiles_files(vk: VulkanObject, json_files_dict: dict):
    sorted_file_keys = get_topologically_sorted_file_keys(json_files_dict)
    for file_key in sorted_file_keys:
        pull_required_capabilities_profiles_file(vk, json_files_dict, json_files_dict[file_key])
        