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
import logging
from pathlib import Path
from enum import Enum

from source.vulkan_object_utils import (
    VulkanObject, 
    initVulkanObject, 
    VK_VERSION, 
    gatherDependentExtensions, 
    gatherDependentCapabilityAliases, 
    getStructByName,
    StructCapabilityAlias, 
    ExtensionCapabilityAlias, 
    CapabilityAlias
)
from source.profiles_parsing import load_profiles_jsons, save_profiles_jsons, OutputFormatType


class ConvertBits(str, Enum):
    STRIP_DUPLICATION = 'strip-duplication'
    PULL_DEPENDENCES = 'pull-dependences'
    PULL_ALIASES = 'pull-aliases'
    PULL_PROMOTED_EXTENSIONS = 'pull-promoted-extensions' # Require all extensions promoted to a core version.
    IGNORE_EXTENSION_VERSIONS = 'ignore-extension-versions' # Set all required extensions to version 1, ignoring extension versions.


FORMAT_STRUCTS_32 = [
    "VkFormatProperties",
]

FORMAT_STRUCTS_64 = [
    "VkFormatProperties3",
    "VkFormatProperties3KHR",
]

FORMAT_STRUCTS_4KHR = [
    "VkFormatProperties4KHR",
]


class FormatFeatureFlagConverter:
    """
    Bidirectionally maps 32-bit VkFormatFeatureFlagBits <-> 64-bit VkFormatFeatureFlagBits2 / VkFormatFeatureFlagBits4KHR
    by matching bitpos values directly from vk.xml.
    """
    def __init__(self, vk: VulkanObject):
        self.flag32_to_flag64: dict[str, str] = {}
        self.flag64_to_flag32: dict[str, str] = {}
        self.flag32_to_flag4khr: dict[str, str] = {}
        self.flag4khr_to_flag32: dict[str, str] = {}
        self._build_maps(vk)

    def _build_maps(self, vk: VulkanObject):
        bitmask_32 = vk.bitmasks.get("VkFormatFeatureFlagBits")
        bitmask_64 = vk.bitmasks.get("VkFormatFeatureFlagBits2")
        bitmask_4khr = vk.bitmasks.get("VkFormatFeatureFlagBits4KHR")

        if bitmask_32 and bitmask_64:
            bitpos_to_flag64 = {
                flag.bitpos: flag.name 
                for flag in bitmask_64.flags 
                if flag.bitpos is not None
            }
            for flag32 in bitmask_32.flags:
                if flag32.bitpos is not None and flag32.bitpos in bitpos_to_flag64:
                    flag64_name = bitpos_to_flag64[flag32.bitpos]
                    self.flag32_to_flag64[flag32.name] = flag64_name
                    self.flag64_to_flag32[flag64_name] = flag32.name
                    for alias32 in getattr(flag32, 'aliases', []):
                        self.flag32_to_flag64[alias32] = flag64_name

        if bitmask_32 and bitmask_4khr:
            bitpos_to_flag4khr = {
                flag.bitpos: flag.name 
                for flag in bitmask_4khr.flags 
                if flag.bitpos is not None
            }
            for flag32 in bitmask_32.flags:
                if flag32.bitpos is not None and flag32.bitpos in bitpos_to_flag4khr:
                    flag4_name = bitpos_to_flag4khr[flag32.bitpos]
                    self.flag32_to_flag4khr[flag32.name] = flag4_name
                    self.flag4khr_to_flag32[flag4_name] = flag32.name
                    for alias32 in getattr(flag32, 'aliases', []):
                        self.flag32_to_flag4khr[alias32] = flag4_name

    def to_flag64_list(self, flags32: list[str]) -> list[str]:
        result = []
        for f32 in flags32:
            if f32 in self.flag32_to_flag64:
                result.append(self.flag32_to_flag64[f32])
            elif f32.startswith("VK_FORMAT_FEATURE_2_"):
                result.append(f32)  # Already a 64-bit flag
        return result

    def to_flag4khr_list(self, flags32: list[str]) -> list[str]:
        result = []
        for f32 in flags32:
            if f32 in self.flag32_to_flag4khr:
                result.append(self.flag32_to_flag4khr[f32])
            elif f32 in self.flag32_to_flag64:
                result.append(self.flag32_to_flag64[f32])  # Fallback to flag64 if bitpos aligns
            elif f32.startswith("VK_FORMAT_FEATURE_4_") or f32.startswith("VK_FORMAT_FEATURE_2_"):
                result.append(f32)
        return result

    def to_flag32_list(self, flags64: list[str]) -> list[str]:
        result = []
        for f64 in flags64:
            if f64 in self.flag64_to_flag32:
                result.append(self.flag64_to_flag32[f64])
            elif f64 in self.flag4khr_to_flag32:
                result.append(self.flag4khr_to_flag32[f64])
            elif f64.startswith("VK_FORMAT_FEATURE_") and not f64.startswith("VK_FORMAT_FEATURE_2_") and not f64.startswith("VK_FORMAT_FEATURE_4_"):
                result.append(f64)  # Already a 32-bit flag
        return result


def collect_block_names(json_capabilities):
    block_names = []
    
    for value in json_capabilities:
        if isinstance(value, str):
            block_names.append(value)
        elif isinstance(value, list):
            names = value
            for val in names:
                block_names.append(val)
        
    return block_names


def parse_profile_capabilities(json_capabilities: list) -> list:
    """
    Parses a profile's capabilities list while preserving structure:
    - str: Mandatory (AND) capability block name.
    - list[str]: Alternative (OR) capability block names.
    """
    parsed = []
    for entry in json_capabilities:
        if isinstance(entry, str):
            parsed.append(entry)
        elif isinstance(entry, list):
            parsed.append([item for item in entry if isinstance(item, str)])
    return parsed


def pull_capabilities_block_dependencies(vk: VulkanObject, version: VK_VERSION, ignore_extension_versions: bool, json_profiles_capabilities_block):
    if "extensions" not in json_profiles_capabilities_block:
        return
    
    extensions = gatherDependentExtensions(vk, version, ignore_extension_versions, json_profiles_capabilities_block["extensions"])
    json_profiles_capabilities_block["extensions"] = extensions


def pull_profiles_file_dependencies(vk: VulkanObject, require_promoted_extensions: bool, ignore_extension_versions: bool, json_file_data):
    profiles_data = json_file_data["profiles"]
    json_profiles_capabilities = json_file_data["capabilities"]

    for key, value in profiles_data.items():
        version = VK_VERSION.NONE
        if not require_promoted_extensions:
            version = VK_VERSION.from_string(value["api-version"])

        block_names = collect_block_names(value["capabilities"])
        
        for block_name in block_names:
            if block_name in json_profiles_capabilities:
                pull_capabilities_block_dependencies(vk, version, ignore_extension_versions, json_profiles_capabilities[block_name])


def pull_profiles_files_dependencies(vk: VulkanObject, require_promoted_extensions: bool, ignore_extension_versions: bool, json_files_dict):
    for key, value in json_files_dict.items():
        logging.debug(f"Fill capabilities dependencies for: {key}")
        pull_profiles_file_dependencies(vk, require_promoted_extensions, ignore_extension_versions, value)


def _is_struct_extension_enabled(vk: VulkanObject, struct_name: str, enabled_exts: set[str]) -> bool:
    """
    Checks whether a structure's defining extension requirements are satisfied
    by the set of enabled extensions in the profile block.
    """
    ext_names = set()

    if hasattr(vk, 'aliasTypeRequirements') and struct_name in vk.aliasTypeRequirements:
        ext_names.update(vk.aliasTypeRequirements[struct_name].keys())
    elif struct_name in vk.structs:
        struct_obj = vk.structs[struct_name]
        if getattr(struct_obj, 'definingRequirements', None):
            ext_names.update(struct_obj.definingRequirements.keys())
        elif getattr(struct_obj, 'extensions', None):
            ext_names.update(struct_obj.extensions)
    else:
        struct_obj = getStructByName(vk.structs, struct_name)
        if struct_obj:
            if getattr(struct_obj, 'definingRequirements', None):
                ext_names.update(struct_obj.definingRequirements.keys())
            elif getattr(struct_obj, 'extensions', None):
                ext_names.update(struct_obj.extensions)

    required_exts = {ext for ext in ext_names if ext in vk.extensions}

    if required_exts and not required_exts.intersection(enabled_exts):
        return False

    return True


def _is_format_struct_valid(vk: VulkanObject, struct_name: str, version: VK_VERSION, enabled_exts: set[str]) -> bool:
    """
    Determines if a format properties structure (32-bit or 64-bit) is valid for the target API version
    and enabled extensions.
    """
    if struct_name == "VkFormatProperties":
        return True

    if not _is_struct_extension_enabled(vk, struct_name, enabled_exts):
        return False

    target_ver_tuple = (
        int(version.name.split('V')[1].split('_')[0]), 
        int(version.name.split('V')[1].split('_')[1])
    ) if hasattr(version, 'name') and 'V' in version.name else (0, 0)

    if struct_name == "VkFormatProperties3":
        if target_ver_tuple >= (1, 3):
            return True
        return "VK_KHR_format_feature_flags2" in enabled_exts

    if struct_name == "VkFormatProperties3KHR":
        return "VK_KHR_format_feature_flags2" in enabled_exts

    if struct_name == "VkFormatProperties4KHR":
        return "VK_KHR_extended_flags" in enabled_exts

    return False


def pull_aliases_capabilities_block(vk: VulkanObject, version: VK_VERSION, json_profiles_capabilities_block: dict, inherited_exts: set[str] = None) -> dict:
    ext_block = json_profiles_capabilities_block.get("extensions", {})
    block_exts = set(ext_block.keys()) if isinstance(ext_block, dict) else set(ext_block)
    enabled_exts = block_exts | (inherited_exts or set())

    # 1. Process 2-level categories: "features" and "properties"
    for category in ("features", "properties"):
        if category not in json_profiles_capabilities_block:
            continue

        category_block = json_profiles_capabilities_block[category]
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

                        if not _is_struct_extension_enabled(vk, target_struct, enabled_exts):
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
                        if "extensions" in json_profiles_capabilities_block:
                            target_ext_block = json_profiles_capabilities_block["extensions"]
                            if isinstance(target_ext_block, dict):
                                target_ext_block[alias.name] = 1
                                enabled_exts.add(alias.name)
                            elif isinstance(target_ext_block, list) and alias.name not in target_ext_block:
                                target_ext_block.append(alias.name)
                                enabled_exts.add(alias.name)

        json_profiles_capabilities_block[category] = new_category_block

    # 2. Process 3-level category: "formats" using FormatFeatureFlagConverter
    if "formats" in json_profiles_capabilities_block:
        formats_block = json_profiles_capabilities_block["formats"]
        new_formats_block = {}
        flag_converter = FormatFeatureFlagConverter(vk)

        for format_name, structs_dict in formats_block.items():
            if not isinstance(structs_dict, dict):
                continue

            new_structs_dict = {}

            for src_struct_name, members_dict in structs_dict.items():
                if not isinstance(members_dict, dict):
                    continue

                src_is_64 = src_struct_name in FORMAT_STRUCTS_64 or src_struct_name in FORMAT_STRUCTS_4KHR
                members_32 = {}
                members_64 = {}
                members_4khr = {}

                for member_name, flag_list in members_dict.items():
                    if isinstance(flag_list, list):
                        if src_is_64:
                            flags_32 = flag_converter.to_flag32_list(flag_list)
                            members_32[member_name] = flags_32
                            members_64[member_name] = flag_converter.to_flag64_list(flags_32)
                            members_4khr[member_name] = flag_converter.to_flag4khr_list(flags_32)
                        else:
                            members_32[member_name] = flag_list
                            members_64[member_name] = flag_converter.to_flag64_list(flag_list)
                            members_4khr[member_name] = flag_converter.to_flag4khr_list(flag_list)
                    else:
                        members_32[member_name] = flag_list
                        members_64[member_name] = flag_list
                        members_4khr[member_name] = flag_list

                # Generate valid 32-bit format structures (VkFormatProperties)
                for target_32 in FORMAT_STRUCTS_32:
                    if _is_format_struct_valid(vk, target_32, version, enabled_exts):
                        if target_32 not in new_structs_dict:
                            new_structs_dict[target_32] = {}
                        for m_name, flags_32 in members_32.items():
                            new_structs_dict[target_32][m_name] = flags_32

                # Generate valid 64-bit format structures (VkFormatProperties3 / VkFormatProperties3KHR)
                for target_64 in FORMAT_STRUCTS_64:
                    if _is_format_struct_valid(vk, target_64, version, enabled_exts):
                        if target_64 not in new_structs_dict:
                            new_structs_dict[target_64] = {}
                        for m_name, flags_64 in members_64.items():
                            new_structs_dict[target_64][m_name] = flags_64

                # Generate valid VK_KHR_extended_flags format structures (VkFormatProperties4KHR)
                for target_4khr in FORMAT_STRUCTS_4KHR:
                    if _is_format_struct_valid(vk, target_4khr, version, enabled_exts):
                        if target_4khr not in new_structs_dict:
                            new_structs_dict[target_4khr] = {}
                        for m_name, flags_4khr in members_4khr.items():
                            new_structs_dict[target_4khr][m_name] = flags_4khr

            new_formats_block[format_name] = new_structs_dict

        json_profiles_capabilities_block["formats"] = new_formats_block

    return json_profiles_capabilities_block


def pull_aliases_profiles_file(vk: VulkanObject, require_promoted_extensions: bool, json_files_dict, json_file_data):
    profiles_data = json_file_data["profiles"]
    json_profiles_capabilities = json_file_data["capabilities"]

    for key, value in profiles_data.items():
        version = VK_VERSION.from_string(value["api-version"])

        required_profile_names = value.get("profiles", [])
        inherited_caps = collect_required_profiles_capabilities(json_files_dict, required_profile_names)
        inherited_exts = set(inherited_caps.get("extensions", {}).keys())

        block_names = collect_block_names(value["capabilities"])
        
        for block_name in block_names:
            if block_name in json_profiles_capabilities:
                pull_aliases_capabilities_block(vk, version, json_profiles_capabilities[block_name], inherited_exts)


def pull_aliases_profiles_files(vk: VulkanObject, require_promoted_extensions: bool, json_files_dict):
    for key, value in json_files_dict.items():
        logging.debug(f"Fill capabilities aliases for: {key}")
        pull_aliases_profiles_file(vk, require_promoted_extensions, json_files_dict, value)


# -----------------------------------------------------------------------------
# Deep Duplication Stripping & Profile Inheritance Traversal
# -----------------------------------------------------------------------------

def deep_merge_dict(target: dict, source: dict):
    """
    Recursively merges `source` into `target`.
    """
    for key, value in source.items():
        if key in target and isinstance(target[key], dict) and isinstance(value, dict):
            deep_merge_dict(target[key], value)
        else:
            target[key] = copy.deepcopy(value)


def strip_dict_duplication(target: dict, reference: dict):
    """
    Recursively removes keys from `target` if their values match `reference`.
    Cleans up dictionary structures that become empty after stripping.
    """
    keys_to_delete = []

    for key, value in list(target.items()):
        if key in reference:
            ref_value = reference[key]

            # 1. Handle nested dictionaries (e.g. VkPhysicalDeviceProperties -> limits)
            if isinstance(value, dict) and isinstance(ref_value, dict):
                strip_dict_duplication(value, ref_value)
                if not value:  # Remove dictionary if empty after stripping
                    keys_to_delete.append(key)

            # 2. Handle identical primitive values (booleans, numbers, strings)
            elif value == ref_value:
                keys_to_delete.append(key)

            # 3. Handle list comparisons (e.g. subgroupSupportedOperations or format feature lists)
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
    """
    Finds a profile object and its corresponding JSON data across all loaded files.
    """
    for file_path, json_file_data in json_files_dict.items():
        if isinstance(json_file_data, dict) and "profiles" in json_file_data:
            if profile_name in json_file_data["profiles"]:
                return json_file_data["profiles"][profile_name], json_file_data
    return None, None


def collect_required_profiles_capabilities(json_files_dict, required_profile_names: list[str], visited_profiles: set[str] = None) -> dict:
    """
    Recursively collects all mandatory capability sections (extensions, features, properties, formats)
    required by parent profiles listed in the 'profiles' element. OR (alternative) blocks are excluded.
    """
    if visited_profiles is None:
        visited_profiles = set()

    collected_capabilities: dict = {}

    for profile_name in required_profile_names:
        if profile_name in visited_profiles:
            continue
        visited_profiles.add(profile_name)

        profile_obj, json_file_data = get_profile_and_file_data(json_files_dict, profile_name)
        if not profile_obj or not json_file_data:
            logging.error(f"Required parent profile '{profile_name}' not found in loaded JSON files! Stripping will fail for this dependency.")
            continue

        # 1. Recursively collect capabilities from ancestor profiles
        ancestor_profiles = profile_obj.get("profiles", [])
        if ancestor_profiles:
            ancestor_caps = collect_required_profiles_capabilities(json_files_dict, ancestor_profiles, visited_profiles)
            deep_merge_dict(collected_capabilities, ancestor_caps)

        # 2. Collect ONLY mandatory (string / AND) blocks from this parent profile
        capabilities_dict = json_file_data.get("capabilities", {})
        parsed_caps = parse_profile_capabilities(profile_obj.get("capabilities", []))

        for item in parsed_caps:
            if isinstance(item, str):  # Mandatory block
                if item in capabilities_dict:
                    deep_merge_dict(collected_capabilities, capabilities_dict[item])

    return collected_capabilities


def strip_capabilities_block_duplication(json_files_dict, version: VK_VERSION, json_profiles_capabilities_block: dict, collected_capabilities: dict):
    # 1. Strip duplicate extensions
    if "extensions" in json_profiles_capabilities_block and "extensions" in collected_capabilities:
        stripped_extensions: dict[str, int] = {}
        ref_extensions = collected_capabilities["extensions"]

        for extension_name, extension_version in json_profiles_capabilities_block["extensions"].items():
            if extension_name in ref_extensions:
                continue  # Duplicate extension from parent profile/block
            stripped_extensions[extension_name] = extension_version

        if stripped_extensions:
            json_profiles_capabilities_block["extensions"] = stripped_extensions
        else:
            del json_profiles_capabilities_block["extensions"]

    # 2. Strip duplicate features, properties, and formats
    for section in ("features", "properties", "formats"):
        if section in json_profiles_capabilities_block and section in collected_capabilities:
            strip_dict_duplication(json_profiles_capabilities_block[section], collected_capabilities[section])
            if not json_profiles_capabilities_block[section]:
                del json_profiles_capabilities_block[section]


def strip_profiles_file_capabilities_duplication(json_files_dict, json_file_data):
    profiles_data = json_file_data.get("profiles", {})
    json_profiles_capabilities = json_file_data.get("capabilities", {})

    for key, value in profiles_data.items():
        # Initialize baseline with mandatory capabilities inherited from parent profiles
        required_profile_names = value.get("profiles", [])
        collected_capabilities: dict = collect_required_profiles_capabilities(
            json_files_dict, required_profile_names
        )

        version = VK_VERSION.from_string(value.get("api-version", "1.0.0"))
        parsed_caps = parse_profile_capabilities(value.get("capabilities", []))

        for item in parsed_caps:
            if isinstance(item, str):
                # Mandatory (AND) block:
                # 1. Strip duplicates against accumulated baseline
                if item in json_profiles_capabilities:
                    strip_capabilities_block_duplication(
                        json_files_dict, version, json_profiles_capabilities[item], collected_capabilities
                    )
                    # 2. Merge remaining features into the baseline for subsequent blocks
                    deep_merge_dict(collected_capabilities, json_profiles_capabilities[item])

            elif isinstance(item, list):
                # Alternative (OR) group (e.g., ["multisampledToSingleSampled", "shaderStencilExport"]):
                # 1. Strip EACH alternative against the accumulated mandatory baseline
                # 2. Do NOT merge alternatives into each other or into the mandatory baseline
                for alt_block_name in item:
                    if alt_block_name in json_profiles_capabilities:
                        strip_capabilities_block_duplication(
                            json_files_dict, version, json_profiles_capabilities[alt_block_name], collected_capabilities
                        )


def strip_profiles_files_capabilities_duplication(json_files_dict):
    for key, value in json_files_dict.items():
        logging.debug(f"Strip duplicated capabilities for: {key}")
        strip_profiles_file_capabilities_duplication(json_files_dict, value)


def main_convert(args):
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
    
    if ConvertBits.PULL_DEPENDENCES in mode_enums:
        pull_profiles_files_dependencies(vk, require_promoted_extensions, ignore_extension_versions, json_files_dict)

    if ConvertBits.PULL_ALIASES in mode_enums:
        pull_aliases_profiles_files(vk, require_promoted_extensions, json_files_dict)

    if ConvertBits.STRIP_DUPLICATION in mode_enums:
        strip_profiles_files_capabilities_duplication(json_files_dict)

    save_profiles_jsons(json_files_dict, Path(args.output), OutputFormatType(args.format))
