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

from source.main_validate import main_validate
from source.vulkan_object_version import is_bundle_structure, get_bundle_structure_core_version
from source.vulkan_object_utils import (
    VulkanObject, 
    initVulkanObject, 
    VK_VERSION, 
    gatherDependentExtensions, 
    gatherCapabilityAliases,
    gatherDependentCapabilityAliases, 
    gatherPromotedExtensionsForExactVersion,
    gatherSatisfiedCoreRequiredFeaturesForVersion,
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
from source.profiles_parsing import (
    load_profiles_jsons, 
    save_profiles_jsons, 
    OutputFormatType,
    collect_block_names,
    parse_profile_capabilities,
    collect_profile_capabilities,
    get_profile_and_file_data,
    deep_merge_dict
)
from source.format_flag_converter import FormatFeatureFlagConverter 


class ConvertBits(str, Enum):
    PULL_EXTENSION_DEPENDENCIES = 'pull-extension-dependencies'
    PULL_REQUIRED_CAPABILITIES = 'pull-required-capabilities'  # Evaluate & pull satisfied required features into capability blocks.
    PULL_PROMOTED_EXTENSIONS = 'pull-promoted-extensions'      # Require all extensions promoted to a core version.
    IGNORE_EXTENSION_VERSIONS = 'ignore-extension-versions'    # Set all required extensions to version 1, ignoring extension versions.
    PULL_ALIASES = 'pull-aliases'
    STRIP_DUPLICATION = 'strip-duplication'
    CONSOLIDATE = 'consolidate'                                # Consolidate all mandatory capability blocks into a single block per profile.


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


def should_remove_struct_a_in_favor_of_b(vk: VulkanObject, version: VK_VERSION, struct_a: str, struct_b: str) -> bool:
    rank_a = get_struct_rank(vk, version, struct_a)
    rank_b = get_struct_rank(vk, version, struct_b)

    if rank_a < rank_b:
        return True
    elif rank_a > rank_b:
        return False

    is_ext_a = is_extension_struct_name(vk, struct_a)
    is_ext_b = is_extension_struct_name(vk, struct_b)
    if is_ext_a and not is_ext_b:
        return True
    if not is_ext_a and is_ext_b:
        return False

    return struct_a > struct_b


# -----------------------------------------------------------------------------
# Phase 1: Extension Dependencies ('pull-extension-dependencies')
# -----------------------------------------------------------------------------

def pull_capabilities_block_dependencies(
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


def pull_profiles_file_dependencies(
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
                pull_capabilities_block_dependencies(
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


def pull_profiles_files_dependencies(vk: VulkanObject, ignore_extension_versions: bool, json_files_dict: dict):
    """Process extension dependencies across all profile files in topological order."""
    if not isinstance(json_files_dict, dict):
        return

    if "profiles" in json_files_dict or "capabilities" in json_files_dict:
        pull_profiles_file_dependencies(vk, ignore_extension_versions, json_files_dict, None)
        return

    sorted_file_keys = get_topologically_sorted_file_keys(json_files_dict)
    for file_key in sorted_file_keys:
        json_file_data = json_files_dict[file_key]
        if isinstance(json_file_data, dict):
            pull_profiles_file_dependencies(vk, ignore_extension_versions, json_file_data, json_files_dict)


# -----------------------------------------------------------------------------
# Phase 2: Required Capabilities Evaluation & Transition Blocks ('pull-required-capabilities')
# -----------------------------------------------------------------------------

def pull_required_capabilities_profiles_file(vk: VulkanObject, json_files_dict: dict, json_file_data: dict):
    profiles_data = json_file_data.get("profiles", {})
    capabilities_dict = json_file_data.setdefault("capabilities", {})

    for key, profile_obj in profiles_data.items():
        api_version = VK_VERSION.from_string(profile_obj["api-version"])
        profile_caps_list = profile_obj.setdefault("capabilities", [])

        # 1. Process Parent Profile API Version Upgrade Transition Blocks
        required_parent_profiles = profile_obj.get("profiles", [])
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
                for ver in VK_VERSION.core_versions():
                    if ver != VK_VERSION.NONE and parent_api_version < ver <= api_version:
                        satisfied = gatherSatisfiedCoreRequiredFeaturesForVersion(
                            vk, ver, api_version, all_exts, enabled_features_set
                        )
                        if satisfied:
                            deep_merge_dict(transition_features, satisfied)

                parent_features_dict = parent_inherited_caps.get("features", {})
                parent_enabled_tuples: set[tuple[str, str]] = set()
                for s_name, members in parent_features_dict.items():
                    if isinstance(members, dict):
                        for m_name, val in members.items():
                            if val:
                                parent_enabled_tuples.add((s_name, m_name))

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
                                if (alias.struct, alias.member) in parent_enabled_tuples:
                                    is_in_parent = True
                                    break

                        if not is_in_parent:
                            new_members[m_name] = val

                    if new_members:
                        filtered_transition_features[s_name] = new_members

                transition_features = filtered_transition_features

                if transition_features:
                    ver_tuple = api_version.as_tuple()
                    transition_block_name = f"{parent_pname}_to_vulkan{ver_tuple[0]}{ver_tuple[1]}"

                    trans_block = capabilities_dict.setdefault(transition_block_name, {})
                    trans_features = trans_block.setdefault("features", {})
                    deep_merge_dict(trans_features, transition_features)

                    if transition_block_name in profile_caps_list:
                        profile_caps_list.remove(transition_block_name)
                    profile_caps_list.insert(0, transition_block_name)

        # 2. Process Extension-satisfied requirements in existing blocks
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


pull_required_features_profiles_files = pull_required_capabilities_profiles_files


# -----------------------------------------------------------------------------
# Phase 3: Promoted Extensions ('pull-promoted-extensions')
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

        for ver in VK_VERSION.core_versions():
            if ver != VK_VERSION.NONE and api_version != VK_VERSION.NONE and ver <= api_version:
                promoted_exts = gatherPromotedExtensionsForExactVersion(vk, ver)
                for ext_name, ext_ver in promoted_exts.items():
                    if ext_name not in ext_dict and ext_name not in context_extensions:
                        ext_dict[ext_name] = 1 if ignore_extension_versions else ext_ver

        block_names = collect_block_names(profile_obj.get("capabilities", []))
        for block_name in block_names:
            if block_name in capabilities_dict:
                block = capabilities_dict[block_name]
                pull_capabilities_block_dependencies(
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
# Phase 4: Capability Aliases ('pull-aliases')
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


# -----------------------------------------------------------------------------
# Phase 5: Deep Duplication Stripping ('strip-duplication')
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


def strip_intra_block_feature_duplication(vk: VulkanObject, version: VK_VERSION, json_block: dict, context_features: dict):
    if "features" not in json_block or not isinstance(json_block["features"], dict):
        return

    block_features = json_block["features"]

    all_features = {}
    deep_merge_dict(all_features, context_features)
    deep_merge_dict(all_features, block_features)

    structs_to_remove = set()

    active_bundles = [
        b for b in ("VkPhysicalDeviceVulkan11Features", "VkPhysicalDeviceVulkan12Features",
                    "VkPhysicalDeviceVulkan13Features", "VkPhysicalDeviceVulkan14Features")
        if b in all_features and version >= get_bundle_structure_core_version(b)
    ]

    for struct_name in list(block_features.keys()):
        if is_bundle_structure(struct_name):
            continue

        if active_bundles and any(is_struct_covered_by_bundle(vk, bundle, struct_name) for bundle in active_bundles):
            structs_to_remove.add(struct_name)
            continue

        for other_struct in all_features.keys():
            if other_struct != struct_name and are_structs_aliases_for_version(vk, version, struct_name, other_struct):
                if should_remove_struct_a_in_favor_of_b(vk, version, struct_name, other_struct):
                    structs_to_remove.add(struct_name)
                    break

    for s in structs_to_remove:
        if s in block_features:
            del block_features[s]

    if not block_features:
        del json_block["features"]


def strip_capabilities_block_duplication(vk: VulkanObject, json_files_dict, version: VK_VERSION, json_profiles_capabilities_block: dict, collected_capabilities: dict):
    context_features = collected_capabilities.get("features", {})
    strip_intra_block_feature_duplication(vk, version, json_profiles_capabilities_block, context_features)

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


def strip_profiles_file_capabilities_duplication(vk: VulkanObject, json_files_dict, json_file_data):
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
                    strip_capabilities_block_duplication(
                        vk, json_files_dict, version, json_profiles_capabilities[item], collected_capabilities
                    )
                    deep_merge_dict(collected_capabilities, json_profiles_capabilities[item])

            elif isinstance(item, list):
                for alt_block_name in item:
                    if alt_block_name in json_profiles_capabilities:
                        strip_capabilities_block_duplication(
                            vk, json_files_dict, version, json_profiles_capabilities[alt_block_name], collected_capabilities
                        )


# -----------------------------------------------------------------------------
# Phase 6: Consolidation ('consolidate')
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
    
    sorted_file_keys = get_topologically_sorted_file_keys(json_files_dict)

    # Phase 1: Pull Extension Dependencies
    if ConvertBits.PULL_EXTENSION_DEPENDENCIES in mode_enums:
        logging.debug("Phase 1: Pulling extension dependencies...")
        pull_profiles_files_dependencies(vk, ignore_extension_versions, json_files_dict)

    # Phase 2: Pull Required Capabilities (Core & Extension Feature Requirements)
    if ConvertBits.PULL_REQUIRED_CAPABILITIES in mode_enums:
        logging.debug("Phase 2: Evaluating and pulling required capabilities...")
        pull_required_capabilities_profiles_files(vk, json_files_dict)

    # Phase 3: Pull Promoted Extensions
    if ConvertBits.PULL_PROMOTED_EXTENSIONS in mode_enums:
        logging.debug("Phase 3: Pulling promoted extensions for core versions...")
        pull_promoted_extensions_profiles_files(vk, ignore_extension_versions, json_files_dict)

    # Phase 4: Pull Capability Aliases
    if ConvertBits.PULL_ALIASES in mode_enums:
        logging.debug("Phase 4: Pulling capability aliases...")
        for file_key in sorted_file_keys:
            pull_aliases_profiles_file(vk, require_promoted_extensions, json_files_dict, json_files_dict[file_key])

    # Phase 5: Strip Duplication
    if ConvertBits.STRIP_DUPLICATION in mode_enums:
        logging.debug("Phase 5: Stripping capabilities duplication...")
        for file_key in sorted_file_keys:
            strip_profiles_file_capabilities_duplication(vk, json_files_dict, json_files_dict[file_key])

    # Phase 6: Consolidate
    if ConvertBits.CONSOLIDATE in mode_enums:
        logging.debug("Phase 6: Consolidating profile capability blocks...")
        for file_key in sorted_file_keys:
            consolidate_profiles_file(json_files_dict, json_files_dict[file_key])

    save_profiles_jsons(json_files_dict, Path(args.output), OutputFormatType(args.format))
    