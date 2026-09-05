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

import argparse
import copy
import logging
import re
from pathlib import Path
from enum import Enum
from typing import Any

from source.main_validate import main_validate
from source.vulkan_object_version import (
    BUNDLE_STRUCT_VERSIONS,
    is_bundle_structure, 
    get_bundle_structure_core_version,
    get_active_feature_bundles,
    get_active_property_bundles
)
from source.vulkan_object_utils import (
    VulkanObject, 
    initVulkanObject, 
    VK_VERSION, 
    gatherDependentExtensions, 
    gatherCapabilityAliases,
    gatherDependentCapabilityAliases, 
    gatherPromotedExtensionsForExactVersion,
    gatherSatisfiedCoreRequiredFeaturesForVersion,
    gatherSatisfiedCoreRequiredPropertiesForVersion,
    gatherSatisfiedExtensionRequiredFeatures,
    isStructExtensionEnabled,
    getStructByName,
    getStructCoreVersion,
    getExtensionPromotedTo,
    getStructDefiningExtensions,
    is_extension_struct_name,
    are_structs_aliases_for_version,
    get_struct_rank,
    get_required_extensions_for_struct,
    is_struct_covered_by_bundle,
    is_property_struct_covered_by_bundle,
    should_remove_struct_a_in_favor_of_b,
    is_extension_promoted_to_version,
    StructCapabilityAlias, 
    ExtensionCapabilityAlias, 
    CapabilityAlias
)
from source.profiles_json_utils import (
    load_profiles_jsons, 
    save_profiles_jsons, 
    OutputFormatType,
    collect_block_names,
    parse_profile_capabilities,
    collect_profile_capabilities,
    get_profile_and_file_data,
    get_primary_capability_block,
    get_topologically_sorted_file_keys,
    collect_required_profiles_capabilities_recursive,
    strip_dict_duplication,
    deep_merge_dict,
    is_property_satisfied
)
from source.format_flag_converter import FormatFeatureFlagConverter 


class ConvertBits(str, Enum):
    PULL_REQUIRED_CAPABILITIES = 'pull-required-capabilities'  # Evaluates extension dependencies and pulls satisfied core/extension feature and property requirements into capability blocks.
    PULL_PROMOTED_EXTENSIONS = 'pull-promoted-extensions'      # Requires all extensions promoted to core up to the profile's target Vulkan version.
    IGNORE_EXTENSION_VERSIONS = 'ignore-extension-versions'    # Sets all required extension versions to 1, overriding specific extension spec versions.
    PULL_ALIASES = 'pull-aliases'                              # Resolves and populates all equivalent capability aliases across core structures and extensions.
    CONSOLIDATE = 'consolidate'                                # Merges all mandatory capability blocks into a single consolidated requirements block per profile.
    STRIP_DUPLICATION = 'strip-duplication'                    # Removes redundant duplicate features, properties, and extension requirements across inheritance trees and within blocks.
    STRIP_PROMOTED_EXTENSIONS = 'strip-promoted-extensions'    # Removes extensions that are already promoted to the profile's target core Vulkan version.
    SORT = 'sort'                                              # Sorts capability blocks, structures, and extension lists into canonical Vulkan order.


def canonicalize_capabilities_for_version(
    vk: VulkanObject, 
    api_version: VK_VERSION, 
    features_dict: dict[str, dict[str, bool]], 
    properties_dict: dict[str, Any]
) -> tuple[dict[str, dict[str, bool]], dict[str, Any]]:
    """
    Remaps split structure capabilities into active bundle structures for api_version
    and removes redundant split structures covered by active bundle structures.
    """
    new_features: dict[str, dict[str, bool]] = {}
    new_properties: dict[str, Any] = {}

    active_feature_bundles = get_active_feature_bundles(api_version)
    active_property_bundles = get_active_property_bundles(api_version)

    def _is_bundle_or_main_core(s: str) -> bool:
        return is_bundle_structure(s) or s in ("VkPhysicalDeviceFeatures", "VkPhysicalDeviceProperties")

    # 1. Features remapping and deduplication
    sorted_feature_structs = sorted(
        features_dict.keys(),
        key=lambda s: (0 if _is_bundle_or_main_core(s) else 1)
    )

    for struct_name in sorted_feature_structs:
        members = features_dict[struct_name]
        if not isinstance(members, dict):
            continue

        is_covered = False
        if active_feature_bundles and not is_bundle_structure(struct_name):
            for bundle in active_feature_bundles:
                if is_struct_covered_by_bundle(vk, bundle, struct_name):
                    is_covered = True
                    for member_name, val in members.items():
                        if val and member_name not in new_features.setdefault(bundle, {}):
                            new_features[bundle][member_name] = True
                    break

        if not is_covered:
            for member_name, val in members.items():
                new_features.setdefault(struct_name, {})[member_name] = val

    # Re-order members of feature bundle structures according to C struct definition order in vk.xml[cite: 13, 15]
    for struct_name, members in new_features.items():
        if is_bundle_structure(struct_name) and isinstance(members, dict):
            struct_obj = vk.structs.get(struct_name) or getStructByName(vk.structs, struct_name)
            if struct_obj and hasattr(struct_obj, 'members'):
                member_idx_map = {m.name: idx for idx, m in enumerate(struct_obj.members)}
                new_features[struct_name] = dict(
                    sorted(members.items(), key=lambda item: member_idx_map.get(item[0], 9999))
                )

    # 2. Properties remapping and deduplication
    sorted_property_structs = sorted(
        properties_dict.keys(),
        key=lambda s: (0 if _is_bundle_or_main_core(s) else 1)
    )

    for struct_name in sorted_property_structs:
        prop_data = properties_dict[struct_name]
        is_covered = False
        if active_property_bundles and not is_bundle_structure(struct_name):
            for bundle in active_property_bundles:
                if is_property_struct_covered_by_bundle(vk, bundle, struct_name):
                    is_covered = True
                    break

        if not is_covered:
            new_properties[struct_name] = prop_data

    # Re-order members of property bundle structures according to C struct definition order in vk.xml[cite: 13, 15]
    for struct_name, prop_data in new_properties.items():
        if is_bundle_structure(struct_name) and isinstance(prop_data, dict):
            struct_obj = vk.structs.get(struct_name) or getStructByName(vk.structs, struct_name)
            if struct_obj and hasattr(struct_obj, 'members'):
                member_idx_map = {m.name: idx for idx, m in enumerate(struct_obj.members)}
                new_properties[struct_name] = dict(
                    sorted(prop_data.items(), key=lambda item: member_idx_map.get(item[0], 9999))
                )

    return new_features, new_properties


def get_parent_property_value(parent_props_dict: dict, struct_name: str, prop_name: str, vk: VulkanObject) -> tuple[bool, Any]:
    """
    Looks up parent_props_dict for struct_name::prop_name or any of its capability aliases.
    Returns (True, parent_val) if found, or (False, None) if not found in parent.
    """
    query_id = StructCapabilityAlias(struct_name, prop_name)
    aliases = [query_id] + gatherCapabilityAliases(vk, query_id)

    for alias in aliases:
        if not isinstance(alias, StructCapabilityAlias):
            continue
        a_struct = alias.struct
        a_member = alias.member

        if a_struct in parent_props_dict:
            s_data = parent_props_dict[a_struct]
            if isinstance(s_data, dict):
                if a_struct == "VkPhysicalDeviceProperties":
                    if "limits" in s_data and isinstance(s_data["limits"], dict) and a_member in s_data["limits"]:
                        return (True, s_data["limits"][a_member])
                    if "sparseProperties" in s_data and isinstance(s_data["sparseProperties"], dict) and a_member in s_data["sparseProperties"]:
                        return (True, s_data["sparseProperties"][a_member])
                else:
                    if a_member in s_data:
                        return (True, s_data[a_member])

    return (False, None)


# -----------------------------------------------------------------------------
# Extension Dependencies
# -----------------------------------------------------------------------------

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

    filtered_deps = {}
    for ext_name, ext_ver in raw_deps.items():
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


# -----------------------------------------------------------------------------
# Required Capabilities Evaluation & Transition Blocks
# -----------------------------------------------------------------------------

def pull_required_capabilities_profiles_file(vk: VulkanObject, json_files_dict: dict, json_file_data: dict):
    profiles_data = json_file_data.get("profiles", {})
    capabilities_dict = json_file_data.setdefault("capabilities", {})

    for key, profile_obj in profiles_data.items():
        api_version = VK_VERSION.from_string(profile_obj["api-version"])
        profile_caps_list = profile_obj.setdefault("capabilities", [])

        required_parent_profiles = profile_obj.get("profiles", [])

        # 1. Process Parent Profile API Version Upgrade Transition Blocks
        if required_parent_profiles:
            for parent_pname in required_parent_profiles:
                parent_obj, _ = get_profile_and_file_data(json_files_dict, parent_pname)
                if not parent_obj:
                    continue

                parent_api_version = VK_VERSION.from_string(parent_obj.get("api-version", "1.0.0"))
                if api_version > parent_api_version:
                    parent_inherited_caps = collect_required_profiles_capabilities_recursive(json_files_dict, [parent_pname])
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

                    # Filter Transition Features against Parent Profile
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

                    # Filter Transition Properties against Parent Profile
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

        # 2. Standalone profile: pull core required features and properties directly into primary block
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

        # 3. Process Extension-satisfied requirements in existing blocks
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


# -----------------------------------------------------------------------------
# Promoted Extensions
# -----------------------------------------------------------------------------

def pull_promoted_extensions_profiles_file(
    vk: VulkanObject, 
    ignore_extension_versions: bool, 
    json_file_data: dict, 
    json_files_dict: dict = None
):
    profiles_data = json_file_data.get("profiles", {})
    capabilities_dict = json_file_data.setdefault("capabilities", {})

    for key, profile_obj in profiles_data.items():
        api_version = VK_VERSION.from_string(profile_obj["api-version"])

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

        primary_block = get_primary_capability_block(profile_obj, capabilities_dict)
        if primary_block is None:
            continue

        ext_dict = primary_block.setdefault("extensions", {})

        for ver in VK_VERSION.versions():
            if ver != VK_VERSION.NONE and api_version != VK_VERSION.NONE and ver <= api_version:
                promoted_exts = gatherPromotedExtensionsForExactVersion(vk, ver)
                for ext_name, ext_ver in promoted_exts.items():
                    if ext_name not in ext_dict and ext_name not in context_extensions:
                        ext_dict[ext_name] = 1 if ignore_extension_versions else ext_ver

        block_names = collect_block_names(profile_obj.get("capabilities", []))
        for block_name in block_names:
            if block_name in capabilities_dict:
                block = capabilities_dict[block_name]
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


def pull_promoted_extensions_profiles_files(vk: VulkanObject, ignore_extension_versions: bool, json_files_dict: dict):
    if not isinstance(json_files_dict, dict):
        return

    if "profiles" in json_files_dict or "capabilities" in json_files_dict:
        pull_promoted_extensions_profiles_file(vk, ignore_extension_versions, json_files_dict, None)
        return

    sorted_file_keys = get_topologically_sorted_file_keys(json_files_dict)
    for file_key in sorted_file_keys:
        pull_promoted_extensions_profiles_file(vk, ignore_extension_versions, json_files_dict[file_key], json_files_dict)


# -----------------------------------------------------------------------------
# Capability Aliases ('pull-aliases')
# -----------------------------------------------------------------------------

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
        category_block = {}
        if category in inherited_caps:
            deep_merge_dict(category_block, inherited_caps[category])
        if category in json_profiles_capabilities_block:
            deep_merge_dict(category_block, json_profiles_capabilities_block[category])

        if not category_block:
            continue

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
            json_profiles_capabilities_block[category] = new_category_block

    formats_block = {}
    if "formats" in inherited_caps:
        deep_merge_dict(formats_block, inherited_caps["formats"])
    if "formats" in json_profiles_capabilities_block:
        deep_merge_dict(formats_block, json_profiles_capabilities_block["formats"])

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


# -----------------------------------------------------------------------------
# Deep Duplication Stripping ('strip-duplication')
# -----------------------------------------------------------------------------

def strip_intra_block_feature_duplication(vk: VulkanObject, version: VK_VERSION, json_block: dict, context_features: dict):
    if "features" not in json_block or not isinstance(json_block["features"], dict):
        return

    block_features = json_block["features"]

    all_features = {}
    deep_merge_dict(all_features, context_features)
    deep_merge_dict(all_features, block_features)

    structs_to_remove = set()

    active_bundles = [
        b for b in get_active_feature_bundles(version)
        if b in all_features
    ]

    for struct_name in list(block_features.keys()):
        if is_bundle_structure(struct_name):
            continue

        if active_bundles and any(is_struct_covered_by_bundle(vk, bundle, struct_name) for bundle in active_bundles):
            structs_to_remove.add(struct_name)
            continue

        for other_struct in all_features.keys():
            if other_struct != struct_name and are_structs_aliases_for_version(vk, version, struct_name, other_struct):
                if other_struct in context_features:
                    structs_to_remove.add(struct_name)
                    break
                elif should_remove_struct_a_in_favor_of_b(vk, version, struct_name, other_struct):
                    structs_to_remove.add(struct_name)
                    break

    for s in structs_to_remove:
        if s in block_features:
            del block_features[s]

    if not block_features:
        del json_block["features"]


def strip_intra_block_property_duplication(vk: VulkanObject, version: VK_VERSION, json_block: dict, context_properties: dict):
    if "properties" not in json_block or not isinstance(json_block["properties"], dict):
        return

    block_properties = json_block["properties"]

    all_properties = {}
    deep_merge_dict(all_properties, context_properties)
    deep_merge_dict(all_properties, block_properties)

    structs_to_remove = set()

    active_bundles = [
        b for b in get_active_property_bundles(version)
        if b in all_properties
    ]

    for struct_name in list(block_properties.keys()):
        if is_bundle_structure(struct_name):
            continue

        if active_bundles and any(is_property_struct_covered_by_bundle(vk, bundle, struct_name) for bundle in active_bundles):
            structs_to_remove.add(struct_name)
            continue

        for other_struct in all_properties.keys():
            if other_struct != struct_name and are_structs_aliases_for_version(vk, version, struct_name, other_struct):
                if other_struct in context_properties:
                    structs_to_remove.add(struct_name)
                    break
                elif should_remove_struct_a_in_favor_of_b(vk, version, struct_name, other_struct):
                    structs_to_remove.add(struct_name)
                    break

    for s in structs_to_remove:
        if s in block_properties:
            del block_properties[s]

    if not block_properties:
        del json_block["properties"]


def strip_intra_block_format_duplication(vk: VulkanObject, version: VK_VERSION, json_block: dict, context_formats: dict):
    if "formats" not in json_block or not isinstance(json_block["formats"], dict):
        return

    block_formats = json_block["formats"]

    for fmt_name, structs_dict in list(block_formats.items()):
        if not isinstance(structs_dict, dict):
            continue

        ctx_structs = context_formats.get(fmt_name, {}) if isinstance(context_formats, dict) else {}
        all_structs = {}
        if isinstance(ctx_structs, dict):
            deep_merge_dict(all_structs, ctx_structs)
        deep_merge_dict(all_structs, structs_dict)

        structs_to_remove = set()
        for struct_name in list(structs_dict.keys()):
            for other_struct in all_structs.keys():
                if other_struct != struct_name and are_structs_aliases_for_version(vk, version, struct_name, other_struct):
                    if isinstance(ctx_structs, dict) and other_struct in ctx_structs:
                        structs_to_remove.add(struct_name)
                        break
                    elif should_remove_struct_a_in_favor_of_b(vk, version, struct_name, other_struct):
                        structs_to_remove.add(struct_name)
                        break

        for s in structs_to_remove:
            if s in structs_dict:
                del structs_dict[s]

        if not structs_dict:
            del block_formats[fmt_name]

    if not block_formats:
        del json_block["formats"]


def strip_duplication_capabilities_block(vk: VulkanObject, json_files_dict, version: VK_VERSION, json_profiles_capabilities_block: dict, collected_capabilities: dict):
    context_features = collected_capabilities.get("features", {})
    strip_intra_block_feature_duplication(vk, version, json_profiles_capabilities_block, context_features)

    context_properties = collected_capabilities.get("properties", {})
    strip_intra_block_property_duplication(vk, version, json_profiles_capabilities_block, context_properties)

    context_formats = collected_capabilities.get("formats", {})
    strip_intra_block_format_duplication(vk, version, json_profiles_capabilities_block, context_formats)

    for section in ("features", "properties", "formats"):
        if section in json_profiles_capabilities_block and section in collected_capabilities:
            strip_dict_duplication(json_profiles_capabilities_block[section], collected_capabilities[section])
            if not json_profiles_capabilities_block[section]:
                del json_profiles_capabilities_block[section]

    needed_extensions = set()
    for section in ("features", "properties"):
        if section in json_profiles_capabilities_block and isinstance(json_profiles_capabilities_block[section], dict):
            for struct_name in json_profiles_capabilities_block[section].keys():
                req_exts = get_required_extensions_for_struct(vk, struct_name, version)
                needed_extensions.update(req_exts)

    if "extensions" in json_profiles_capabilities_block and "extensions" in collected_capabilities:
        stripped_extensions: dict[str, int] = {}
        ref_extensions = collected_capabilities["extensions"]

        for extension_name, extension_version in json_profiles_capabilities_block["extensions"].items():
            if extension_name in ref_extensions and extension_name not in needed_extensions:
                continue
            stripped_extensions[extension_name] = extension_version

        if stripped_extensions:
            json_profiles_capabilities_block["extensions"] = stripped_extensions
        else:
            del json_profiles_capabilities_block["extensions"]


def strip_duplication_profiles_file(vk: VulkanObject, json_files_dict, json_file_data):
    profiles_data = json_file_data.get("profiles", {})
    json_profiles_capabilities = json_file_data.get("capabilities", {})

    for key, value in profiles_data.items():
        required_profile_names = value.get("profiles", [])
        collected_capabilities: dict = collect_required_profiles_capabilities_recursive(
            json_files_dict, required_profile_names
        )

        version = VK_VERSION.from_string(value.get("api-version", "1.0.0"))
        parsed_caps = parse_profile_capabilities(value.get("capabilities", []))

        for item in parsed_caps:
            if isinstance(item, str):
                if item in json_profiles_capabilities:
                    strip_duplication_capabilities_block(
                        vk, json_files_dict, version, json_profiles_capabilities[item], collected_capabilities
                    )
                    deep_merge_dict(collected_capabilities, json_profiles_capabilities[item])

            elif isinstance(item, list):
                for alt_block_name in item:
                    if alt_block_name in json_profiles_capabilities:
                        strip_duplication_capabilities_block(
                            vk, json_files_dict, version, json_profiles_capabilities[alt_block_name], collected_capabilities
                        )


def strip_duplication_profiles_files(vk: VulkanObject, json_files_dict: dict):
    if not isinstance(json_files_dict, dict):
        return

    if "profiles" in json_files_dict or "capabilities" in json_files_dict:
        strip_duplication_profiles_file(vk, json_files_dict, json_files_dict)
        return

    sorted_file_keys = get_topologically_sorted_file_keys(json_files_dict)
    for file_key in sorted_file_keys:
        strip_duplication_profiles_file(vk, json_files_dict, json_files_dict[file_key])


# -----------------------------------------------------------------------------
# Consolidation ('consolidate')
# -----------------------------------------------------------------------------

def consolidate_profiles_file(json_files_dict: dict, json_file_data: dict):
    profiles_data = json_file_data.get("profiles", {})
    capabilities_dict = json_file_data.setdefault("capabilities", {})

    for profile_name, profile_obj in profiles_data.items():
        required_profile_names = profile_obj.get("profiles", [])
        consolidated_caps = collect_required_profiles_capabilities_recursive(json_files_dict, required_profile_names)

        parsed_caps = parse_profile_capabilities(profile_obj.get("capabilities", []))
        optional_blocks = []

        for item in parsed_caps:
            if isinstance(item, str):
                if item in capabilities_dict:
                    deep_merge_dict(consolidated_caps, capabilities_dict[item])
            elif isinstance(item, list):
                optional_blocks.append(item)

        if consolidated_caps:
            consolidated_block_name = f"{profile_name}_requirements"
            capabilities_dict[consolidated_block_name] = consolidated_caps

            profile_obj["capabilities"] = [consolidated_block_name] + optional_blocks
            profile_obj["profiles"] = []

    referenced_blocks = set()
    for prof in profiles_data.values():
        for item in prof.get("capabilities", []):
            if isinstance(item, str):
                referenced_blocks.add(item)
            elif isinstance(item, list):
                referenced_blocks.update(item)

    for block_name in list(capabilities_dict.keys()):
        if block_name not in referenced_blocks:
            del capabilities_dict[block_name]


def consolidate_profiles_files(json_files_dict: dict):
    if not isinstance(json_files_dict, dict):
        return

    if "profiles" in json_files_dict or "capabilities" in json_files_dict:
        consolidate_profiles_file(json_files_dict, json_files_dict)
        return

    sorted_file_keys = get_topologically_sorted_file_keys(json_files_dict)
    for file_key in sorted_file_keys:
        consolidate_profiles_file(json_files_dict, json_file_data=json_files_dict[file_key])


# -----------------------------------------------------------------------------
# Strip Promoted Extensions ('strip-promoted-extensions')
# -----------------------------------------------------------------------------

def strip_promoted_extensions_capabilities_block(
    vk: VulkanObject, version: VK_VERSION, json_profiles_capabilities_block: dict
):
    if "extensions" not in json_profiles_capabilities_block or not isinstance(json_profiles_capabilities_block["extensions"], dict):
        return

    block_exts = json_profiles_capabilities_block["extensions"]
    exts_to_remove = [
        ext_name for ext_name in block_exts.keys()
        if is_extension_promoted_to_version(vk, ext_name, version)
    ]

    for ext_name in exts_to_remove:
        del block_exts[ext_name]

    if not block_exts:
        del json_profiles_capabilities_block["extensions"]


def strip_promoted_extensions_profiles_file(vk: VulkanObject, json_file_data: dict):
    profiles_data = json_file_data.get("profiles", {})
    json_profiles_capabilities = json_file_data.get("capabilities", {})

    for profile_key, profile_obj in profiles_data.items():
        api_version = VK_VERSION.from_string(profile_obj.get("api-version", "1.0.0"))
        block_names = collect_block_names(profile_obj.get("capabilities", []))

        for block_name in block_names:
            if block_name in json_profiles_capabilities:
                strip_promoted_extensions_capabilities_block(
                    vk, api_version, json_profiles_capabilities[block_name]
                )


def strip_promoted_extensions_profiles_files(vk: VulkanObject, json_files_dict: dict):
    if not isinstance(json_files_dict, dict):
        return

    if "profiles" in json_files_dict or "capabilities" in json_files_dict:
        strip_promoted_extensions_profiles_file(vk, json_files_dict)
        return

    sorted_file_keys = get_topologically_sorted_file_keys(json_files_dict)
    for file_key in sorted_file_keys:
        strip_promoted_extensions_profiles_file(vk, json_files_dict[file_key])


# -----------------------------------------------------------------------------
# Sorting ('sort')
# -----------------------------------------------------------------------------

def get_struct_sort_key(vk: VulkanObject, struct_name: str) -> tuple:
    """
    Computes a sorting key for structures:
    1. Core Bundle structures (Vulkan 1.0 -> 1.4)
    2. Core Split structures (Vulkan 1.0 -> 1.4, alphabetical per version)
    3. KHR extension structures (alphabetical)
    4. EXT extension structures (alphabetical)
    5. Vendor extension structures (alphabetical)
    """
    if struct_name in BUNDLE_STRUCT_VERSIONS:
        ver_val = BUNDLE_STRUCT_VERSIONS[struct_name]
        return (0, 0, ver_val, struct_name)

    is_ext = is_extension_struct_name(vk, struct_name)

    if not is_ext:
        core_ver = getStructCoreVersion(vk, struct_name)
        if core_ver == VK_VERSION.NONE:
            core_ver = VK_VERSION.V1_0

        ver_val = core_ver.as_tuple() if hasattr(core_ver, 'as_tuple') else (1, 0)
        return (0, 1, ver_val, struct_name)

    if struct_name.endswith("KHR"):
        return (1, (0, 0), struct_name)
    elif struct_name.endswith("EXT"):
        return (2, (0, 0), struct_name)
    else:
        return (3, (0, 0), struct_name)


def get_ext_priority_key(ext_name: str) -> tuple:
    """Priority order for tie-breaking independent extensions: KHR -> EXT -> Vendor."""
    if ext_name.startswith("VK_KHR_"):
        return (0, ext_name)
    elif ext_name.startswith("VK_EXT_"):
        return (1, ext_name)
    else:
        return (2, ext_name)


def sort_extensions(vk: VulkanObject, exts: dict | list) -> dict | list:
    """
    Sorts extensions ensuring dependent extensions are listed AFTER their prerequisite extensions,
    with priority tie-breaking: KHR -> EXT -> Vendor.
    """
    is_dict = isinstance(exts, dict)
    ext_names = list(exts.keys()) if is_dict else list(exts)
    ext_set = set(ext_names)

    prereqs = {e: set() for e in ext_names}
    for e in ext_names:
        deps = gatherDependentExtensions(vk, VK_VERSION.V1_0, True, {e: 1})
        for dep in deps:
            if dep != e and dep in ext_set:
                prereqs[e].add(dep)

    in_degree = {e: len(prereqs[e]) for e in ext_names}
    candidates = [e for e in ext_names if in_degree[e] == 0]

    sorted_exts = []
    while candidates:
        candidates.sort(key=get_ext_priority_key)
        curr = candidates.pop(0)
        sorted_exts.append(curr)

        for e in ext_names:
            if curr in prereqs[e]:
                prereqs[e].remove(curr)
                in_degree[e] -= 1
                if in_degree[e] == 0:
                    candidates.append(e)

    if len(sorted_exts) < len(ext_names):
        remaining = [e for e in ext_names if e not in sorted_exts]
        remaining.sort(key=get_ext_priority_key)
        sorted_exts.extend(remaining)

    if is_dict:
        return {e: exts[e] for e in sorted_exts}
    else:
        return sorted_exts


def sort_capabilities_block(vk: VulkanObject, json_block: dict):
    if not isinstance(json_block, dict):
        return

    if "extensions" in json_block:
        json_block["extensions"] = sort_extensions(vk, json_block["extensions"])

    for category in ("features", "properties"):
        if category in json_block and isinstance(json_block[category], dict):
            sorted_cat = {}
            sorted_struct_names = sorted(
                json_block[category].keys(), 
                key=lambda s: get_struct_sort_key(vk, s)
            )
            for s_name in sorted_struct_names:
                sorted_cat[s_name] = json_block[category][s_name]
            json_block[category] = sorted_cat

    if "formats" in json_block and isinstance(json_block["formats"], dict):
        sorted_formats = {}
        for fmt_name in sorted(json_block["formats"].keys()):
            fmt_structs = json_block["formats"][fmt_name]
            if isinstance(fmt_structs, dict):
                sorted_fmt_structs = {}
                for s_name in sorted(fmt_structs.keys(), key=lambda s: get_struct_sort_key(vk, s)):
                    sorted_fmt_structs[s_name] = fmt_structs[s_name]
                sorted_formats[fmt_name] = sorted_fmt_structs
            else:
                sorted_formats[fmt_name] = fmt_structs
        json_block["formats"] = sorted_formats


def sort_profiles_file(vk: VulkanObject, json_file_data: dict):
    capabilities_dict = json_file_data.get("capabilities", {})
    for block_name, block in capabilities_dict.items():
        sort_capabilities_block(vk, block)


def sort_profiles_files(vk: VulkanObject, json_files_dict: dict):
    if not isinstance(json_files_dict, dict):
        return

    if "profiles" in json_files_dict or "capabilities" in json_files_dict:
        sort_profiles_file(vk, json_files_dict)
        return

    for file_key, json_file_data in json_files_dict.items():
        if isinstance(json_file_data, dict):
            sort_profiles_file(vk, json_file_data)


# -----------------------------------------------------------------------------
# Main Conversion Entry Point
# -----------------------------------------------------------------------------

def main_convert(args):
    validate_val = getattr(args, 'validate', None)
    if validate_val:
        validate_modes = validate_val if isinstance(validate_val, list) else ['schema', 'analysis']

        validate_args = argparse.Namespace(
            registry=getattr(args, 'registry', None),
            input=args.input,
            schema=getattr(args, 'schema', None),
            api=getattr(args, 'api', 'vulkan') or 'vulkan',
            mode=validate_modes
        )
        main_validate(validate_args)

    vk = initVulkanObject('vulkan', args.registry or None)

    for version in vk.versions.values():
        logging.debug(version.name)
    
    json_files_dict = load_profiles_jsons(Path(args.input))

    raw_modes = getattr(args, 'mode', None) or []
    mode_enums = [ConvertBits(m) for m in raw_modes]
    
    require_promoted_extensions = ConvertBits.PULL_PROMOTED_EXTENSIONS in mode_enums
    ignore_extension_versions = ConvertBits.IGNORE_EXTENSION_VERSIONS in mode_enums

    # Pull Required Capabilities (Dependencies + Core/Extension Requirements)
    if ConvertBits.PULL_REQUIRED_CAPABILITIES in mode_enums:
        logging.debug("Pulling extension dependencies...")
        pull_extension_dependencies_profiles_files(vk, ignore_extension_versions, json_files_dict)

        logging.debug("Evaluating and pulling required capabilities...")
        pull_required_capabilities_profiles_files(vk, json_files_dict)

    # Pull Promoted Extensions
    if ConvertBits.PULL_PROMOTED_EXTENSIONS in mode_enums:
        logging.debug("Pulling promoted extensions for core versions...")
        pull_promoted_extensions_profiles_files(vk, ignore_extension_versions, json_files_dict)

    # Pull Capability Aliases
    if ConvertBits.PULL_ALIASES in mode_enums:
        logging.debug("Pulling capability aliases...")
        pull_aliases_profiles_files(vk, require_promoted_extensions, json_files_dict)

    # Consolidate
    if ConvertBits.CONSOLIDATE in mode_enums:
        logging.debug("Consolidating profile capability blocks...")
        consolidate_profiles_files(json_files_dict)

    # Strip Duplication
    if ConvertBits.STRIP_DUPLICATION in mode_enums:
        logging.debug("Stripping capabilities duplication...")
        strip_duplication_profiles_files(vk, json_files_dict)

    # Strip Promoted Extensions
    if ConvertBits.STRIP_PROMOTED_EXTENSIONS in mode_enums:
        logging.debug("Stripping extensions promoted to profile core version...")
        strip_promoted_extensions_profiles_files(vk, json_files_dict)

    # Sort
    if ConvertBits.SORT in mode_enums:
        logging.debug("Sorting capability blocks and extensions...")
        sort_profiles_files(vk, json_files_dict)

    save_profiles_jsons(json_files_dict, Path(args.output), OutputFormatType(args.format))
    