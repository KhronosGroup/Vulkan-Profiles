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

from pathlib import Path
from enum import Enum

from source.vulkan_object_utils import VulkanObject, initVulkanObject, VK_VERSION, gatherDependentExtensions, gatherDependentCapabilityAliases, StructCapabilityAlias, ExtensionCapabilityAlias, CapabilityAlias
from source.profiles_parsing import load_profiles_jsons
from source.profiles_parsing import save_profiles_jsons
from source.profiles_parsing import OutputFormatType


class ConvertBits(str, Enum):
    STRIP_DUPLICATION = 'strip-duplication'
    PULL_DEPENDENCES = 'pull-dependences'
    PULL_ALIASES = 'pull-aliases'
    PULL_PROMOTED_EXTENSIONS = 'pull-promoted-extensions' # Require all extensions promoted to a core version.
    IGNORE_EXTENSION_VERSIONS = 'ignore-extension-versions' # Set all required extensions to version 1, ignoring extension versions.


# A Profiles Json capabilities element containts block names. Collect all the names
# "capabilities": [
#    "MUST",
#    ["multisampledToSingleSampled", "shaderStencilExport"],
#    ["wideLinesEnabledConstrained", "wideLinesDisabledUnconstrained"]
# ]
def collect_block_names(json_capabilities):
    block_names = []
    
    for value in json_capabilities:
        if isinstance(value, str):
            block_names.append(value)
        elif isinstance(value, list):
            names = value
            for value in names:
                block_names.append(value)
        
    return block_names


def pull_capabilities_block_dependencies(vk: VulkanObject, version: VK_VERSION, ignore_extension_versions: bool, json_profiles_capabilities_block):
    if "extensions" not in json_profiles_capabilities_block:
        return
    
    extensions = gatherDependentExtensions(vk, version, ignore_extension_versions, json_profiles_capabilities_block["extensions"])
    json_profiles_capabilities_block["extensions"] = extensions
    
    return


# TODO: Add Vulkan version and other profiles? (Maybe not necessary)
def pull_profiles_file_dependencies(vk: VulkanObject, require_promoted_extensions: bool, ignore_extension_versions: bool, json_file_data):
    profiles_data = json_file_data["profiles"]
    json_profiles_capabilities = json_file_data["capabilities"]

    for key, value in profiles_data.items():
        version = VK_VERSION.NONE
        if not require_promoted_extensions:
            version = VK_VERSION.from_string(value["api-version"])

        block_names = collect_block_names(value["capabilities"])
        
        for block_name in block_names:
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

    # 1. Collect required extensions / core versions for the struct
    if hasattr(vk, 'aliasTypeRequirements') and struct_name in vk.aliasTypeRequirements:
        ext_names.update(vk.aliasTypeRequirements[struct_name].keys())
    elif struct_name in vk.structs:
        struct_obj = vk.structs[struct_name]
        if getattr(struct_obj, 'definingRequirements', None):
            ext_names.update(struct_obj.definingRequirements.keys())
        elif getattr(struct_obj, 'extensions', None):
            ext_names.update(struct_obj.extensions)
    else:
        struct_obj = getStructByName(vk.structs, struct_name) if 'getStructByName' in globals() else None
        if struct_obj:
            if getattr(struct_obj, 'definingRequirements', None):
                ext_names.update(struct_obj.definingRequirements.keys())
            elif getattr(struct_obj, 'extensions', None):
                ext_names.update(struct_obj.extensions)

    # 2. Filter down to actual extension names (excluding Vulkan core versions)
    required_exts = {ext for ext in ext_names if ext in vk.extensions}

    # 3. If the struct is defined by extension(s), ensure at least one defining extension is present
    if required_exts and not required_exts.intersection(enabled_exts):
        return False

    return True


def pull_aliases_capabilities_block(vk: VulkanObject, version: VK_VERSION, json_profiles_capabilities_block: dict) -> dict:
    # Build a set of currently enabled extension names from the JSON block
    ext_block = json_profiles_capabilities_block.get("extensions", {})
    enabled_exts = set(ext_block.keys()) if isinstance(ext_block, dict) else set(ext_block)

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

                        # Skip aliases if their defining extension is not present in "extensions"
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
                        # Register new extension alias if "extensions" exists in the JSON block
                        if "extensions" in json_profiles_capabilities_block:
                            target_ext_block = json_profiles_capabilities_block["extensions"]
                            if isinstance(target_ext_block, dict):
                                target_ext_block[alias.name] = 1
                                enabled_exts.add(alias.name)
                            elif isinstance(target_ext_block, list) and alias.name not in target_ext_block:
                                target_ext_block.append(alias.name)
                                enabled_exts.add(alias.name)

        json_profiles_capabilities_block[category] = new_category_block

    return json_profiles_capabilities_block


def pull_aliases_profiles_file(vk: VulkanObject, require_promoted_extensions: bool, json_file_data):
    profiles_data = json_file_data["profiles"]
    json_profiles_capabilities = json_file_data["capabilities"]

    for key, value in profiles_data.items():
        version = VK_VERSION.from_string(value["api-version"])
        #if not require_promoted_extensions:

        block_names = collect_block_names(value["capabilities"])
        
        for block_name in block_names:
            pull_aliases_capabilities_block(vk, version, json_profiles_capabilities[block_name])


def pull_aliases_profiles_files(vk: VulkanObject, require_promoted_extensions: bool, json_files_dict):
    for key, value in json_files_dict.items():
        logging.debug(f"Fill capabilities aliases for: {key}")
        pull_aliases_profiles_file(vk, require_promoted_extensions, value)


def strip_capabilities_block_duplication(json_files_dict, json_profiles_capabilities_block, collected_extension_names: set[str]):
    if "extensions" not in json_profiles_capabilities_block:
        return
    
    stripped_extensions: dict[str, int] = {}
    
    for extension_name, extension_version in json_profiles_capabilities_block["extensions"].items():
        if extension_name in collected_extension_names:
            continue # The extension was already listed, it's a duplicate.
        collected_extension_names.add(extension_name)
        stripped_extensions[extension_name] = extension_version
    
    json_profiles_capabilities_block["extensions"] = stripped_extensions


def strip_profiles_file_capabilities_duplication(json_files_dict, json_file_data):
    profiles_data = json_file_data["profiles"]
    json_profiles_capabilities = json_file_data["capabilities"]

    for key, value in profiles_data.items():
        collected_extension_names: set[str] = set()
        
        version = VK_VERSION.from_string(value["api-version"]) # TODO 

        block_names = collect_block_names(value["capabilities"]) # Here, it collects all the block names but some blocks are OR
        
        for block_name in block_names:
            strip_capabilities_block_duplication(json_files_dict, json_profiles_capabilities[block_name], collected_extension_names)
    
    return


def strip_profiles_files_capabilities_duplication(json_files_dict):
    for key, value in json_files_dict.items():
        logging.debug(f"Strip duplicated capabilities for: {key}")
        strip_profiles_file_capabilities_duplication(json_files_dict, value)


def main_convert(args):
    vk = initVulkanObject('vulkan', args.registry or None)

    for version in vk.versions.values():
        logging.debug(version.name)
    
    json_files_dict = load_profiles_jsons(Path(args.input))
    #save_profiles_jsons(json_files_dict, Path(args.format))

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


