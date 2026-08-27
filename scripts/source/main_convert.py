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
    collect_required_profiles_capabilities,
    get_profile_and_file_data,
    deep_merge_dict
)
from source.format_flag_converter import FormatFeatureFlagConverter 


class ConvertBits(str, Enum):
    PULL_EXTENSION_DEPENDENCIES = 'pull-extension-dependencies'
    PULL_PROMOTED_EXTENSIONS = 'pull-promoted-extensions'  # Require all extensions promoted to a core version.
    IGNORE_EXTENSION_VERSIONS = 'ignore-extension-versions'  # Set all required extensions to version 1, ignoring extension versions.
    PULL_REQUIRED_CAPABILITIES = 'pull-required-capabilities'  # Evaluate & pull satisfied required features into capability blocks.
    PULL_ALIASES = 'pull-aliases'
    STRIP_DUPLICATION = 'strip-duplication'
    CONSOLIDATE = 'consolidate'  # Consolidate all mandatory capability blocks into a single block per profile.


def are_structs_aliases_for_version(vk: VulkanObject, version: VK_VERSION, struct1: str, struct2: str) -> bool:
    """
    Checks if struct1 and struct2 are valid capability aliases in the target API version,
    filtering out aliases whose core version exceeds the profile's api-version.
    """
    if struct1 == struct2:
        return True

    if not hasattr(vk, '_struct_aliases_for_ver_cache'):
        vk._struct_aliases_for_ver_cache = {}

    cache_key = (version, struct1, struct2)
    if cache_key in vk._struct_aliases_for_ver_cache:
        return vk._struct_aliases_for_ver_cache[cache_key]

    # Fast direct alias checks
    s1_obj = vk.structs.get(struct1) or getStructByName(vk.structs, struct1)
    if s1_obj and hasattr(s1_obj, 'aliases') and struct2 in getattr(s1_obj, 'aliases', []):
        vk._struct_aliases_for_ver_cache[cache_key] = True
        return True

    s2_obj = vk.structs.get(struct2) or getStructByName(vk.structs, struct2)
    if s2_obj and hasattr(s2_obj, 'aliases') and struct1 in getattr(s2_obj, 'aliases', []):
        vk._struct_aliases_for_ver_cache[cache_key] = True
        return True

    # Representative sample check: checking the first member determines structural aliasing
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
    """
    Returns the priority rank of a structure for a given target API version.
    """
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
    """
    Returns extension names required by struct_name (or its aliases)
    that are NOT core in the target Vulkan API version.
    """
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


# -----------------------------------------------------------------------------
# Phase 1: Extension Dependencies
# -----------------------------------------------------------------------------

def pull_capabilities_block_dependencies(
    vk: VulkanObject, 
    version: VK_VERSION, 
    ignore_extension_versions: bool, 
    json_profiles_capabilities_block: dict
):
    if "extensions" not in json_profiles_capabilities_block:
        return

    extensions = gatherDependentExtensions(vk, version, ignore_extension_versions, json_profiles_capabilities_block["extensions"])
    json_profiles_capabilities_block["extensions"] = extensions


def pull_profiles_file_dependencies(vk: VulkanObject, ignore_extension_versions: bool, json_file_data: dict):
    profiles_data = json_file_data.get("profiles", {})
    json_profiles_capabilities = json_file_data.get("capabilities", {})

    for key, value in profiles_data.items():
        api_version = VK_VERSION.from_string(value["api-version"])
        block_names = collect_block_names(value["capabilities"])
        
        for block_name in block_names:
            if block_name in json_profiles_capabilities:
                pull_capabilities_block_dependencies(
                    vk, api_version, ignore_extension_versions, json_profiles_capabilities[block_name]
                )


def pull_profiles_files_dependencies(vk: VulkanObject, ignore_extension_versions: bool, json_files_dict: dict):
    for key, value in json_files_dict.items():
        logging.debug(f"Fill capabilities dependencies for: {key}")
        pull_profiles_file_dependencies(vk, ignore_extension_versions, value)


# -----------------------------------------------------------------------------
# Phase 2: Promoted Extensions
# -----------------------------------------------------------------------------

def pull_promoted_extensions_profiles_file(vk: VulkanObject, ignore_extension_versions: bool, json_file_data: dict):
    profiles_data = json_file_data.get("profiles", {})
    capabilities_dict = json_file_data.setdefault("capabilities", {})

    for key, profile_obj in profiles_data.items():
        api_version = VK_VERSION.from_string(profile_obj["api-version"])

        for ver in VK_VERSION.core_versions():
            if ver != VK_VERSION.NONE and api_version != VK_VERSION.NONE and ver <= api_version:
                promoted_exts = gatherPromotedExtensionsForExactVersion(vk, ver)
                if promoted_exts:
                    ver_tuple = ver.as_tuple()
                    block_name = f"vulkan{ver_tuple[0]}{ver_tuple[1]}pulledrequirements"

                    block = capabilities_dict.setdefault(block_name, {})
                    ext_dict = block.setdefault("extensions", {})

                    for ext_name, ext_ver in promoted_exts.items():
                        if ext_name not in ext_dict:
                            ext_dict[ext_name] = 1 if ignore_extension_versions else ext_ver

                    pull_capabilities_block_dependencies(vk, api_version, ignore_extension_versions, block)

                    profile_caps_list = profile_obj.setdefault("capabilities", [])
                    if block_name not in profile_caps_list:
                        profile_caps_list.append(block_name)


def pull_promoted_extensions_profiles_files(vk: VulkanObject, ignore_extension_versions: bool, json_files_dict: dict):
    for key, value in json_files_dict.items():
        logging.debug(f"Pulling promoted extensions for: {key}")
        pull_promoted_extensions_profiles_file(vk, ignore_extension_versions, value)


# -----------------------------------------------------------------------------
# Phase 3: Required Capabilities Evaluation
# -----------------------------------------------------------------------------

def pull_required_capabilities_profiles_file(vk: VulkanObject, json_files_dict: dict, json_file_data: dict):
    profiles_data = json_file_data.get("profiles", {})
    capabilities_dict = json_file_data.setdefault("capabilities", {})

    stale_pulled_blocks = [
        b for b, content in list(capabilities_dict.items())
        if isinstance(b, str) and b.endswith("pulledrequirements") and (not content or not any(content.values()))
    ]
    for block_name in stale_pulled_blocks:
        del capabilities_dict[block_name]
        for prof in profiles_data.values():
            caps = prof.get("capabilities", [])
            if block_name in caps:
                caps.remove(block_name)

    for key, profile_obj in profiles_data.items():
        api_version = VK_VERSION.from_string(profile_obj["api-version"])
        profile_caps_list = profile_obj.setdefault("capabilities", [])

        profile_caps = collect_profile_capabilities(json_files_dict, json_file_data, profile_obj)
        profile_enabled_exts = set(profile_caps.get("extensions", {}).keys())
        profile_features_block = profile_caps.get("features", {})

        enabled_features: set[tuple[str, str]] = set()
        for struct_name, members in profile_features_block.items():
            if isinstance(members, dict):
                for member_name, val in members.items():
                    if val:
                        enabled_features.add((struct_name, member_name))

        block_names = collect_block_names(profile_obj.get("capabilities", []))
        for block_name in block_names:
            if block_name in capabilities_dict:
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

        profile_caps = collect_profile_capabilities(json_files_dict, json_file_data, profile_obj)
        profile_features_block = profile_caps.get("features", {})
        enabled_features = set()
        for struct_name, members in profile_features_block.items():
            if isinstance(members, dict):
                for member_name, val in members.items():
                    if val:
                        enabled_features.add((struct_name, member_name))

        for ver in VK_VERSION.core_versions():
            if ver != VK_VERSION.NONE and api_version != VK_VERSION.NONE and ver <= api_version:
                satisfied_core_features = gatherSatisfiedCoreRequiredFeaturesForVersion(
                    vk, ver, api_version, profile_enabled_exts, enabled_features
                )

                if satisfied_core_features:
                    ver_tuple = ver.as_tuple()
                    core_block_name = f"vulkan{ver_tuple[0]}{ver_tuple[1]}pulledrequirements"

                    core_block = capabilities_dict.setdefault(core_block_name, {})
                    core_features = core_block.setdefault("features", {})
                    deep_merge_dict(core_features, satisfied_core_features)

                    if core_block_name not in profile_caps_list:
                        profile_caps_list.append(core_block_name)


def pull_required_capabilities_profiles_files(vk: VulkanObject, json_files_dict: dict):
    for key, value in json_files_dict.items():
        logging.debug(f"Pulling satisfied required features for: {key}")
        pull_required_capabilities_profiles_file(vk, json_files_dict, value)


# Backward compatibility alias
pull_required_features_profiles_files = pull_required_capabilities_profiles_files


# -----------------------------------------------------------------------------
# Phase 4: Structural & Format Feature Aliases
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
        inherited_caps = collect_required_profiles_capabilities(json_files_dict, required_profile_names)

        profile_caps = collect_profile_capabilities(json_files_dict, json_file_data, value)
        profile_enabled_exts = set(profile_caps.get("extensions", {}).keys())

        block_names = collect_block_names(value["capabilities"])
        
        for block_name in block_names:
            if block_name in json_profiles_capabilities:
                pull_aliases_capabilities_block(
                    vk, version, json_profiles_capabilities[block_name], inherited_caps, profile_enabled_exts
                )


def pull_aliases_profiles_files(vk: VulkanObject, require_promoted_extensions: bool, json_files_dict):
    for key, value in json_files_dict.items():
        logging.debug(f"Fill capabilities aliases for: {key}")
        pull_aliases_profiles_file(vk, require_promoted_extensions, json_files_dict, value)


# -----------------------------------------------------------------------------
# Phase 5: Deep Duplication Stripping & Profile Inheritance Traversal
# -----------------------------------------------------------------------------

def deep_merge_dict(target: dict, source: dict):
    for key, value in source.items():
        if key in target and isinstance(target[key], dict) and isinstance(value, dict):
            deep_merge_dict(target[key], value)
        else:
            target[key] = copy.deepcopy(value)


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


def get_profile_and_file_data(json_files_dict, profile_name: str):
    for file_path, json_file_data in json_files_dict.items():
        if isinstance(json_file_data, dict) and "profiles" in json_file_data:
            if profile_name in json_file_data["profiles"]:
                return json_file_data["profiles"][profile_name], json_file_data
    return None, None


def collect_required_profiles_capabilities(json_files_dict, required_profile_names: list[str], visited_profiles: set[str] = None) -> dict:
    if visited_profiles is None:
        visited_profiles = set()

    collected_capabilities: dict = {}

    for profile_name in required_profile_names:
        if profile_name in visited_profiles:
            continue
        visited_profiles.add(profile_name)

        profile_obj, json_file_data = get_profile_and_file_data(json_files_dict, profile_name)
        if not profile_obj or not json_file_data:
            logging.error(f"Required parent profile '{profile_name}' not found in loaded JSON files!")
            continue

        ancestor_profiles = profile_obj.get("profiles", [])
        if ancestor_profiles:
            ancestor_caps = collect_required_profiles_capabilities(json_files_dict, ancestor_profiles, visited_profiles)
            deep_merge_dict(collected_capabilities, ancestor_caps)

        capabilities_dict = json_file_data.get("capabilities", {})
        parsed_caps = parse_profile_capabilities(profile_obj.get("capabilities", []))

        for item in parsed_caps:
            if isinstance(item, str):
                if item in capabilities_dict:
                    deep_merge_dict(collected_capabilities, capabilities_dict[item])

    return collected_capabilities


def strip_intra_block_feature_duplication(vk: VulkanObject, version: VK_VERSION, json_block: dict, context_features: dict):
    if "features" not in json_block or not isinstance(json_block["features"], dict):
        return

    block_features = json_block["features"]

    all_features = {}
    deep_merge_dict(all_features, context_features)
    deep_merge_dict(all_features, block_features)

    structs_to_remove = set()

    # Active core bundle structures in all_features
    active_bundles = [
        b for b in ("VkPhysicalDeviceVulkan11Features", "VkPhysicalDeviceVulkan12Features",
                    "VkPhysicalDeviceVulkan13Features", "VkPhysicalDeviceVulkan14Features")
        if b in all_features and version >= get_bundle_structure_core_version(b)
    ]

    for struct_name in list(block_features.keys()):
        if is_bundle_structure(struct_name):
            continue

        # 1. Strip split core/ext structures covered by an active core bundle in block/context
        if active_bundles and any(is_struct_covered_by_bundle(vk, bundle, struct_name) for bundle in active_bundles):
            structs_to_remove.add(struct_name)
            continue

        # 2. Structural alias deduplication using gatherDependentCapabilityAliases and rank preference
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
    # 1. Strip intra-block feature structure duplication (bundle coverage, aliases)
    context_features = collected_capabilities.get("features", {})
    strip_intra_block_feature_duplication(vk, version, json_profiles_capabilities_block, context_features)

    # 2. Strip duplicated features, properties, and formats across blocks / inheritance
    for section in ("features", "properties", "formats"):
        if section in json_profiles_capabilities_block and section in collected_capabilities:
            strip_dict_duplication(json_profiles_capabilities_block[section], collected_capabilities[section])
            if not json_profiles_capabilities_block[section]:
                del json_profiles_capabilities_block[section]

    # 3. Gather extensions required by structures that STILL REMAIN in this capability block for this API version
    needed_extensions = set()
    for section in ("features", "properties"):
        if section in json_profiles_capabilities_block and isinstance(json_profiles_capabilities_block[section], dict):
            for struct_name in json_profiles_capabilities_block[section].keys():
                req_exts = get_required_extensions_for_struct(vk, struct_name, version)
                needed_extensions.update(req_exts)

    # 4. Strip extensions if they are already in collected_capabilities AND not needed by any remaining struct in this block
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
        collected_capabilities: dict = collect_required_profiles_capabilities(
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


def strip_profiles_files_capabilities_duplication(vk: VulkanObject, json_files_dict):
    for key, value in json_files_dict.items():
        logging.debug(f"Strip duplicated capabilities for: {key}")
        strip_profiles_file_capabilities_duplication(vk, json_files_dict, value)


# -----------------------------------------------------------------------------
# Phase 6: Consolidation
# -----------------------------------------------------------------------------

def consolidate_profiles_file(json_files_dict: dict, json_file_data: dict):
    profiles_data = json_file_data.get("profiles", {})
    capabilities_dict = json_file_data.setdefault("capabilities", {})

    for profile_name, profile_obj in profiles_data.items():
        required_profile_names = profile_obj.get("profiles", [])
        consolidated_caps = collect_required_profiles_capabilities(json_files_dict, required_profile_names)

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
    for key, value in json_files_dict.items():
        logging.debug(f"Consolidating capabilities for: {key}")
        consolidate_profiles_file(json_files_dict, value)


# -----------------------------------------------------------------------------
# Cleanup and Sorting Helpers
# -----------------------------------------------------------------------------

def cleanup_and_sort_pulled_blocks(json_file_data: dict):
    capabilities_dict = json_file_data.get("capabilities", {})
    profiles_data = json_file_data.get("profiles", {})

    empty_blocks = []
    for block_name, block_content in capabilities_dict.items():
        if block_name.endswith("pulledrequirements"):
            if not block_content or not any(block_content.values()):
                empty_blocks.append(block_name)

    for block_name in empty_blocks:
        del capabilities_dict[block_name]
        for prof in profiles_data.values():
            caps = prof.get("capabilities", [])
            if block_name in caps:
                caps.remove(block_name)

    for prof in profiles_data.values():
        caps = prof.get("capabilities", [])
        pulled_blocks = [c for c in caps if isinstance(c, str) and c.endswith("pulledrequirements")]
        other_blocks = [c for c in caps if not (isinstance(c, str) and c.endswith("pulledrequirements"))]

        def get_ver_key(name: str):
            nums = re.findall(r'\d+', name)
            return int(nums[0]) if nums else 0

        pulled_blocks.sort(key=get_ver_key)
        prof["capabilities"] = other_blocks + pulled_blocks


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

    mode_enums = [ConvertBits(m) for m in args.mode]
    
    require_promoted_extensions = False
    if ConvertBits.PULL_PROMOTED_EXTENSIONS in mode_enums:
        require_promoted_extensions = True
        
    ignore_extension_versions = False
    if ConvertBits.IGNORE_EXTENSION_VERSIONS in mode_enums:
        ignore_extension_versions = True
    
    # Phase 1: Pull extension dependencies for existing capability blocks
    if ConvertBits.PULL_EXTENSION_DEPENDENCIES in mode_enums:
        pull_profiles_files_dependencies(vk, ignore_extension_versions, json_files_dict)

    # Phase 2: Pull promoted extensions into version-specific vulkan1Xpulledrequirements blocks
    if ConvertBits.PULL_PROMOTED_EXTENSIONS in mode_enums:
        pull_promoted_extensions_profiles_files(vk, ignore_extension_versions, json_files_dict)

    # Phase 3: Evaluate & pull satisfied required capabilities into capability blocks
    if ConvertBits.PULL_REQUIRED_CAPABILITIES in mode_enums:
        pull_required_capabilities_profiles_files(vk, json_files_dict)

    # Phase 4: Expand capability aliases (features, properties, format flags)
    if ConvertBits.PULL_ALIASES in mode_enums:
        pull_aliases_profiles_files(vk, require_promoted_extensions, json_files_dict)

    # Phase 5: Strip duplication across profile inheritance hierarchy and within blocks
    if ConvertBits.STRIP_DUPLICATION in mode_enums:
        strip_profiles_files_capabilities_duplication(vk, json_files_dict)

    # Phase 6: Consolidate mandatory capability blocks into a single unique block per profile
    if ConvertBits.CONSOLIDATE in mode_enums:
        consolidate_profiles_files(json_files_dict)

    # Cleanup empty pulled blocks & organize capability order
    for json_file in json_files_dict.values():
        cleanup_and_sort_pulled_blocks(json_file)

    save_profiles_jsons(json_files_dict, Path(args.output), OutputFormatType(args.format))
    