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


def collect_required_profiles_capabilities_recursive(json_files_dict: dict, profile_names: list, visited: set = None) -> dict:
    if visited is None:
        visited = set()

    aggregated_caps = {}
    for pname in profile_names:
        if pname in visited:
            continue
        visited.add(pname)

        p_obj, p_file_data = get_profile_and_file_data(json_files_dict, pname)
        if not p_obj or not p_file_data:
            continue

        parent_profiles = p_obj.get("profiles", [])
        if parent_profiles:
            parent_caps = collect_required_profiles_capabilities_recursive(json_files_dict, parent_profiles, visited)
            deep_merge_dict(aggregated_caps, parent_caps)

        direct_caps = collect_profile_capabilities(json_files_dict, p_file_data, p_obj)
        deep_merge_dict(aggregated_caps, direct_caps)

    return aggregated_caps


def get_topologically_sorted_file_keys(json_files_dict: dict) -> list:
    profile_to_file = {}
    for file_key, file_data in json_files_dict.items():
        if isinstance(file_data, dict) and "profiles" in file_data:
            for profile_name in file_data["profiles"].keys():
                profile_to_file[profile_name] = file_key

    adj = {fk: set() for fk in json_files_dict.keys()}
    in_degree = {fk: 0 for fk in json_files_dict.keys()}

    for file_key, file_data in json_files_dict.items():
        if not isinstance(file_data, dict) or "profiles" not in file_data:
            continue
        for profile_obj in file_data["profiles"].values():
            req_profiles = profile_obj.get("profiles", [])
            for parent_pname in req_profiles:
                parent_fk = profile_to_file.get(parent_pname)
                if parent_fk and parent_fk != file_key and file_key not in adj[parent_fk]:
                    adj[parent_fk].add(file_key)
                    in_degree[file_key] += 1

    queue = [fk for fk, deg in in_degree.items() if deg == 0]
    sorted_keys = []

    while queue:
        curr = queue.pop(0)
        sorted_keys.append(curr)
        for neighbor in adj[curr]:
            in_degree[neighbor] -= 1
            if in_degree[neighbor] == 0:
                queue.append(neighbor)

    for fk in json_files_dict.keys():
        if fk not in sorted_keys:
            sorted_keys.append(fk)

    return sorted_keys


def get_primary_capability_block(profile_obj: dict, capabilities_dict: dict) -> dict | None:
    caps = profile_obj.get("capabilities", [])
    for cap_item in caps:
        if isinstance(cap_item, str) and cap_item in capabilities_dict:
            return capabilities_dict[cap_item]
        elif isinstance(cap_item, list) and cap_item:
            if cap_item[0] in capabilities_dict:
                return capabilities_dict[cap_item[0]]
    return None


def are_structs_aliases_for_version(vk: VulkanObject, version: VK_VERSION, struct1: str, struct2: str) -> bool:
    if struct1 == struct2:
        return True

    if not hasattr(vk, '_struct_aliases_for_ver_cache'):
        vk._struct_aliases_for_ver_cache = {}

    cache_key = (version, struct1, struct2)
    if cache_key in vk._struct_aliases_for_ver_cache:
        return vk._struct_aliases_for_ver_cache[cache_key]

    s1_obj = vk.structs.get(struct1) or getStructByName(vk.structs, struct1)
    if s1_obj and hasattr(s1_obj, 'aliases') and struct2 in getattr(s1_obj, 'aliases', []):
        vk._struct_aliases_for_ver_cache[cache_key] = True
        return True

    s2_obj = vk.structs.get(struct2) or getStructByName(vk.structs, struct2)
    if s2_obj and hasattr(s2_obj, 'aliases') and struct1 in getattr(s2_obj, 'aliases', []):
        vk._struct_aliases_for_ver_cache[cache_key] = True
        return True

    res = False
    if s1_obj and hasattr(s1_obj, 'members') and s1_obj.members:
        sample_member = s1_obj.members[0].name
        valid_aliases = gatherDependentCapabilityAliases(vk, version, StructCapabilityAlias(struct1, sample_member))
        if any(isinstance(alias, StructCapabilityAlias) and alias.struct == struct2 for alias in valid_aliases):
            res = True

    if not res and s2_obj and hasattr(s2_obj, 'members') and s2_obj.members:
        sample_member = s2_obj.members[0].name
        valid_aliases = gatherDependentCapabilityAliases(vk, version, StructCapabilityAlias(struct2, sample_member))
        if any(isinstance(alias, StructCapabilityAlias) and alias.struct == struct1 for alias in valid_aliases):
            res = True

    vk._struct_aliases_for_ver_cache[cache_key] = res
    return res


def get_struct_rank(vk: VulkanObject, version: VK_VERSION, struct_name: str) -> int:
    if not hasattr(vk, '_struct_rank_cache'):
        vk._struct_rank_cache = {}

    cache_key = (version, struct_name)
    if cache_key in vk._struct_rank_cache:
        return vk._struct_rank_cache[cache_key]

    struct_obj = vk.structs.get(struct_name) or getStructByName(vk.structs, struct_name)

    is_alias = False
    if struct_obj and hasattr(struct_obj, 'name') and struct_name != struct_obj.name:
        is_alias = True

    core_ver = getStructCoreVersion(vk, struct_name)
    rank = 2
    if core_ver != VK_VERSION.NONE and not is_alias:
        if version != VK_VERSION.NONE and core_ver <= version:
            rank = 3
        else:
            rank = 1

    vk._struct_rank_cache[cache_key] = rank
    return rank


def get_struct_tier(vk: VulkanObject, struct_name: str) -> int:
    """Returns lower priority tier number (0: Core, 1: KHR, 2: EXT, 3: Vendor)."""
    if not is_extension_struct_name(vk, struct_name):
        return 0
    if struct_name.endswith("KHR"):
        return 1
    if struct_name.endswith("EXT"):
        return 2
    return 3


def get_required_extensions_for_struct(vk: VulkanObject, struct_name: str, version: VK_VERSION) -> set[str]:
    if not hasattr(vk, '_req_exts_for_struct_cache'):
        vk._req_exts_for_struct_cache = {}

    cache_key = (struct_name, version)
    if cache_key in vk._req_exts_for_struct_cache:
        return vk._req_exts_for_struct_cache[cache_key]

    req_exts = set()

    struct_names = [struct_name]
    struct_obj = vk.structs.get(struct_name) or getStructByName(vk.structs, struct_name)
    if struct_obj and hasattr(struct_obj, 'aliases'):
        for alias in struct_obj.aliases:
            if alias not in struct_names:
                struct_names.append(alias)

    for s_name in struct_names:
        s_obj = vk.structs.get(s_name) or getStructByName(vk.structs, s_name)
        if s_obj:
            def_ver = getattr(s_obj, 'definedByVersion', None)
            if def_ver is not None and def_ver != VK_VERSION.NONE and version != VK_VERSION.NONE and def_ver <= version:
                continue

        def_exts = getStructDefiningExtensions(vk, s_name)
        for ext_name in def_exts:
            promoted_targets = getExtensionPromotedTo(vk, ext_name)
            promoted_to_core = False
            for target in promoted_targets:
                p_ver = VK_VERSION.from_string(target)
                if p_ver != VK_VERSION.NONE and version != VK_VERSION.NONE and p_ver <= version:
                    promoted_to_core = True
                    break

            if not promoted_to_core:
                req_exts.add(ext_name)

    vk._req_exts_for_struct_cache[cache_key] = req_exts
    return req_exts


def is_struct_covered_by_bundle(vk: VulkanObject, bundle_name: str, struct_name: str) -> bool:
    if struct_name == bundle_name or is_bundle_structure(struct_name):
        return False

    bundle_obj = vk.structs.get(bundle_name) or getStructByName(vk.structs, bundle_name)
    struct_obj = vk.structs.get(struct_name) or getStructByName(vk.structs, struct_name)

    if not bundle_obj or not struct_obj:
        return False

    bundle_members = {m.name for m in getattr(bundle_obj, 'members', [])}
    struct_members = {m.name for m in getattr(struct_obj, 'members', []) if m.name not in ('sType', 'pNext')}

    if not struct_members:
        return False

    return struct_members.issubset(bundle_members)


def is_property_struct_covered_by_bundle(vk: VulkanObject, bundle_name: str, struct_name: str) -> bool:
    """Checks if all property members of a split structure are covered or aliased in bundle_name."""
    if is_struct_covered_by_bundle(vk, bundle_name, struct_name):
        return True

    struct_obj = vk.structs.get(struct_name) or getStructByName(vk.structs, struct_name)
    bundle_obj = vk.structs.get(bundle_name) or getStructByName(vk.structs, bundle_name)

    if not struct_obj or not bundle_obj:
        return False

    bundle_members = {m.name for m in getattr(bundle_obj, 'members', [])}
    for member in getattr(struct_obj, 'members', []):
        if member.name in ('sType', 'pNext'):
            continue

        if member.name in bundle_members:
            continue

        query_id = StructCapabilityAlias(struct_name, member.name)
        aliases = gatherCapabilityAliases(vk, query_id)
        is_aliased_in_bundle = any(
            isinstance(a, StructCapabilityAlias) and a.struct == bundle_name
            for a in aliases
        )
        if not is_aliased_in_bundle:
            return False

    return True


def should_remove_struct_a_in_favor_of_b(vk: VulkanObject, version: VK_VERSION, struct_a: str, struct_b: str) -> bool:
    rank_a = get_struct_rank(vk, version, struct_a)
    rank_b = get_struct_rank(vk, version, struct_b)

    if rank_a < rank_b:
        return True
    elif rank_a > rank_b:
        return False

    tier_a = get_struct_tier(vk, struct_a)
    tier_b = get_struct_tier(vk, struct_b)

    if tier_a > tier_b:
        return True
    elif tier_a < tier_b:
        return False

    return struct_a > struct_b


def is_extension_promoted_to_version(vk: VulkanObject, ext_name: str, version: VK_VERSION) -> bool:
    promoted_targets = getExtensionPromotedTo(vk, ext_name)
    for target in promoted_targets:
        p_ver = VK_VERSION.from_string(target)
        if p_ver != VK_VERSION.NONE and version != VK_VERSION.NONE and p_ver <= version:
            return True
    return False


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

    # 1. Features remapping and deduplication
    for struct_name, members in features_dict.items():
        if not isinstance(members, dict):
            continue

        is_covered = False
        if active_feature_bundles and not is_bundle_structure(struct_name):
            for bundle in active_feature_bundles:
                if is_struct_covered_by_bundle(vk, bundle, struct_name):
                    is_covered = True
                    for member_name, val in members.items():
                        if val:
                            new_features.setdefault(bundle, {})[member_name] = True
                    break

        if not is_covered:
            for member_name, val in members.items():
                if val:
                    new_features.setdefault(struct_name, {})[member_name] = True

    # 2. Properties remapping and deduplication
    for struct_name, prop_data in properties_dict.items():
        is_covered = False
        if active_property_bundles and not is_bundle_structure(struct_name):
            for bundle in active_property_bundles:
                if is_property_struct_covered_by_bundle(vk, bundle, struct_name):
                    is_covered = True
                    break

        if not is_covered:
            new_properties[struct_name] = prop_data

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
    """Gather dependent extensions and required features for a capability block."""
    if "extensions" not in json_profiles_capabilities_block:
        return

    context_extensions = context_extensions or set()
    context_features = context_features or set()

    # 1. Resolve extension dependencies iteratively
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

    # 2. Gather features required by extensions in this block
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
    """Process extension dependencies across blocks in sequential order, tracking precedent extensions and features."""
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
    """Process extension dependencies across all profile files in topological order."""
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


# Backward-compatibility aliases
pull_profiles_files_dependencies = pull_extension_dependencies_profiles_files
pull_profiles_file_dependencies = pull_extension_dependencies_profiles_file
pull_capabilities_block_dependencies = pull_extension_dependencies_capabilities_block


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


# Backward-compatibility alias
pull_required_features_profiles_files = pull_required_capabilities_profiles_files


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

def strip_dict_duplication(target: dict, reference: dict):
    keys_to_delete = []

    for key, value in list(target.items()):
        if key in reference:
            ref_value = reference[key]

            if isinstance(value, dict) and isinstance(ref_value, dict):
                strip_dict_duplication(value, ref_value)
                if not value:
                    keys_to_delete.append(key)

            elif value == ref_value:
                keys_to_delete.append(key)

            elif isinstance(value, list) and isinstance(ref_value, list):
                try:
                    if sorted(value) == sorted(ref_value):
                        keys_to_delete.append(key)
                except TypeError:
                    if value == ref_value:
                        keys_to_delete.append(key)

    for key in keys_to_delete:
        del target[key]


def is_feature_member_covered(
    vk: VulkanObject,
    version: VK_VERSION,
    struct_name: str,
    m_name: str,
    val: bool,
    ref_features: dict
) -> bool:
    """Checks if (struct_name, m_name, val) is covered in ref_features across any alias or bundle structure."""
    if not ref_features or not val:
        return False

    query_id = StructCapabilityAlias(struct_name, m_name)
    aliases = [query_id] + gatherCapabilityAliases(vk, query_id)

    for alias in aliases:
        if not isinstance(alias, StructCapabilityAlias):
            continue
        a_struct = alias.struct
        a_member = alias.member

        if a_struct in ref_features and isinstance(ref_features[a_struct], dict):
            if ref_features[a_struct].get(a_member) == val:
                return True

    return False


def is_feature_struct_members_covered_by_bundle(vk: VulkanObject, bundle_name: str, struct_name: str, members: dict, all_features: dict) -> bool:
    if bundle_name not in all_features or not isinstance(all_features[bundle_name], dict):
        return False

    bundle_members = all_features[bundle_name]
    for m_name, val in members.items():
        if not val:
            continue
        query_id = StructCapabilityAlias(struct_name, m_name)
        aliases = [query_id] + gatherCapabilityAliases(vk, query_id)
        found = False
        for alias in aliases:
            if isinstance(alias, StructCapabilityAlias) and alias.struct == bundle_name:
                if bundle_members.get(alias.member) == val:
                    found = True
                    break
        if not found:
            return False
    return True


def is_feature_struct_covered_by_alias(vk: VulkanObject, struct_a: str, members_a: dict, struct_b: str, all_features: dict) -> bool:
    if struct_b not in all_features or not isinstance(all_features[struct_b], dict):
        return False
    members_b = all_features[struct_b]
    for m_name, val in members_a.items():
        query_id = StructCapabilityAlias(struct_a, m_name)
        aliases = [query_id] + gatherCapabilityAliases(vk, query_id)
        found = False
        for alias in aliases:
            if isinstance(alias, StructCapabilityAlias) and alias.struct == struct_b:
                if members_b.get(alias.member) == val:
                    found = True
                    break
        if not found:
            return False
    return True


def strip_intra_block_feature_duplication(vk: VulkanObject, version: VK_VERSION, json_block: dict, context_features: dict):
    if "features" not in json_block or not isinstance(json_block["features"], dict):
        return

    block_features = json_block["features"]

    # Pass 1: Strip members/structs already covered in context_features (parent profile / previous blocks)
    if context_features:
        for struct_name in list(block_features.keys()):
            members = block_features[struct_name]
            if not isinstance(members, dict):
                continue

            for m_name in list(members.keys()):
                val = members[m_name]
                if is_feature_member_covered(vk, version, struct_name, m_name, val, context_features):
                    del members[m_name]

            if not members:
                del block_features[struct_name]

    if not block_features:
        del json_block["features"]
        return

    # Pass 2: Intra-block deduplication among structures within block_features itself
    active_bundles = [
        b for b in get_active_feature_bundles(version)
        if b in block_features
    ]

    structs_to_remove = set()

    for struct_name, members in list(block_features.items()):
        if is_bundle_structure(struct_name) or not isinstance(members, dict):
            continue

        if active_bundles:
            covered_by_bundle = False
            for bundle in active_bundles:
                if is_feature_struct_members_covered_by_bundle(vk, bundle, struct_name, members, block_features):
                    covered_by_bundle = True
                    break
            if covered_by_bundle:
                structs_to_remove.add(struct_name)
                continue

        for other_struct in block_features.keys():
            if other_struct != struct_name and are_structs_aliases_for_version(vk, version, struct_name, other_struct):
                if should_remove_struct_a_in_favor_of_b(vk, version, struct_name, other_struct):
                    if is_feature_struct_covered_by_alias(vk, struct_name, members, other_struct, block_features):
                        structs_to_remove.add(struct_name)
                        break

    for s in structs_to_remove:
        if s in block_features:
            del block_features[s]

    if not block_features:
        del json_block["features"]


def is_property_member_covered(
    vk: VulkanObject,
    version: VK_VERSION,
    struct_name: str,
    prop_name: str,
    prop_val: Any,
    ref_properties: dict
) -> bool:
    if not ref_properties:
        return False

    found, parent_val = get_parent_property_value(ref_properties, struct_name, prop_name, vk)
    if found and parent_val == prop_val:
        return True

    return False


def is_property_struct_members_covered_by_bundle(vk: VulkanObject, bundle_name: str, struct_name: str, prop_data: dict, all_properties: dict) -> bool:
    if bundle_name not in all_properties or not isinstance(all_properties[bundle_name], dict):
        return False

    bundle_data = all_properties[bundle_name]
    if not isinstance(prop_data, dict):
        return False

    for prop_name, prop_val in prop_data.items():
        found = False
        query_id = StructCapabilityAlias(struct_name, prop_name)
        aliases = [query_id] + gatherCapabilityAliases(vk, query_id)

        for alias in aliases:
            if isinstance(alias, StructCapabilityAlias) and alias.struct == bundle_name:
                b_val = None
                if bundle_name == "VkPhysicalDeviceProperties":
                    if "limits" in bundle_data and isinstance(bundle_data["limits"], dict) and alias.member in bundle_data["limits"]:
                        b_val = bundle_data["limits"][alias.member]
                    elif "sparseProperties" in bundle_data and isinstance(bundle_data["sparseProperties"], dict) and alias.member in bundle_data["sparseProperties"]:
                        b_val = bundle_data["sparseProperties"][alias.member]
                else:
                    if alias.member in bundle_data:
                        b_val = bundle_data[alias.member]

                if b_val is not None and b_val == prop_val:
                    found = True
                    break

        if not found:
            return False
    return True


def is_property_struct_covered_by_alias(vk: VulkanObject, struct_a: str, prop_data_a: dict, struct_b: str, all_properties: dict) -> bool:
    if struct_b not in all_properties or not isinstance(all_properties[struct_b], dict):
        return False
    prop_data_b = all_properties[struct_b]
    if not isinstance(prop_data_a, dict):
        return False

    for prop_name, prop_val in prop_data_a.items():
        query_id = StructCapabilityAlias(struct_a, prop_name)
        aliases = [query_id] + gatherCapabilityAliases(vk, query_id)
        found = False
        for alias in aliases:
            if isinstance(alias, StructCapabilityAlias) and alias.struct == struct_b:
                b_val = prop_data_b.get(alias.member)
                if b_val is not None and b_val == prop_val:
                    found = True
                    break
        if not found:
            return False
    return True


def strip_intra_block_property_duplication(vk: VulkanObject, version: VK_VERSION, json_block: dict, context_properties: dict):
    if "properties" not in json_block or not isinstance(json_block["properties"], dict):
        return

    block_properties = json_block["properties"]

    # Pass 1: Strip properties covered in context_properties
    if context_properties:
        for struct_name in list(block_properties.keys()):
            prop_data = block_properties[struct_name]
            if not isinstance(prop_data, dict):
                continue

            if struct_name == "VkPhysicalDeviceProperties":
                for sub_group in list(prop_data.keys()):
                    sub_dict = prop_data[sub_group]
                    if isinstance(sub_dict, dict):
                        for p_name in list(sub_dict.keys()):
                            p_val = sub_dict[p_name]
                            if is_property_member_covered(vk, version, struct_name, p_name, p_val, context_properties):
                                del sub_dict[p_name]
                        if not sub_dict:
                            del prop_data[sub_group]
            else:
                for p_name in list(prop_data.keys()):
                    p_val = prop_data[p_name]
                    if is_property_member_covered(vk, version, struct_name, p_name, p_val, context_properties):
                        del prop_data[p_name]

            if not prop_data:
                del block_properties[struct_name]

    if not block_properties:
        del json_block["properties"]
        return

    # Pass 2: Intra-block deduplication within block_properties
    active_bundles = [
        b for b in get_active_property_bundles(version)
        if b in block_properties
    ]

    structs_to_remove = set()

    for struct_name, prop_data in list(block_properties.items()):
        if is_bundle_structure(struct_name) or not isinstance(prop_data, dict):
            continue

        if active_bundles:
            covered_by_bundle = False
            for bundle in active_bundles:
                if is_property_struct_members_covered_by_bundle(vk, bundle, struct_name, prop_data, block_properties):
                    covered_by_bundle = True
                    break
            if covered_by_bundle:
                structs_to_remove.add(struct_name)
                continue

        for other_struct in block_properties.keys():
            if other_struct != struct_name and are_structs_aliases_for_version(vk, version, struct_name, other_struct):
                if should_remove_struct_a_in_favor_of_b(vk, version, struct_name, other_struct):
                    if is_property_struct_covered_by_alias(vk, struct_name, prop_data, other_struct, block_properties):
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

    # Pass 1: Strip format structs covered by context_formats
    if context_formats:
        for fmt_name in list(block_formats.keys()):
            structs_dict = block_formats[fmt_name]
            if not isinstance(structs_dict, dict):
                continue

            ctx_structs = context_formats.get(fmt_name, {}) if isinstance(context_formats, dict) else {}
            if isinstance(ctx_structs, dict) and ctx_structs:
                for struct_name in list(structs_dict.keys()):
                    members_dict = structs_dict[struct_name]
                    if not isinstance(members_dict, dict):
                        continue

                    covered_in_ctx = False
                    for ctx_struct_name, ctx_members in ctx_structs.items():
                        if are_structs_aliases_for_version(vk, version, struct_name, ctx_struct_name):
                            if isinstance(ctx_members, dict):
                                all_covered = True
                                for f_key, f_list in members_dict.items():
                                    if f_key in ctx_members and isinstance(f_list, list) and isinstance(ctx_members[f_key], list):
                                        if not set(f_list).issubset(set(ctx_members[f_key])):
                                            all_covered = False
                                            break
                                    elif f_key not in ctx_members:
                                        all_covered = False
                                        break
                                if all_covered:
                                    covered_in_ctx = True
                                    break
                    if covered_in_ctx:
                        del structs_dict[struct_name]

            if not structs_dict:
                del block_formats[fmt_name]

    if not block_formats:
        del json_block["formats"]
        return

    # Pass 2: Intra-block format deduplication within block_formats
    for fmt_name, structs_dict in list(block_formats.items()):
        if not isinstance(structs_dict, dict):
            continue

        structs_to_remove = set()
        for struct_name, members_dict in list(structs_dict.items()):
            if not isinstance(members_dict, dict):
                continue

            for other_struct in structs_dict.keys():
                if other_struct != struct_name and are_structs_aliases_for_version(vk, version, struct_name, other_struct):
                    if should_remove_struct_a_in_favor_of_b(vk, version, struct_name, other_struct):
                        other_members = structs_dict[other_struct]
                        if isinstance(other_members, dict):
                            all_covered = True
                            for f_key, f_list in members_dict.items():
                                if f_key in other_members and isinstance(f_list, list) and isinstance(other_members[f_key], list):
                                    if not set(f_list).issubset(set(other_members[f_key])):
                                        all_covered = False
                                        break
                                elif f_key not in other_members:
                                    all_covered = False
                                    break
                            if all_covered:
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


# Backward-compatibility aliases
strip_profiles_files_capabilities_duplication = strip_duplication_profiles_files
strip_profiles_file_capabilities_duplication = strip_duplication_profiles_file
strip_capabilities_block_duplication = strip_duplication_capabilities_block


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
        consolidate_profiles_file(json_files_dict, json_files_dict[file_key])


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
    