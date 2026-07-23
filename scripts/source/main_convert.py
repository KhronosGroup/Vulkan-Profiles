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

from source.vulkan_object_utils import VulkanObject, initVulkanObject, VK_VERSION, gatherDependentExtensions, gatherDependentCapabilityAliases
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


def pull_aliases_capabilities_block(vk: VulkanObject, version: VK_VERSION, ignore_extension_versions: bool, json_profiles_capabilities_block):
    if "features" in json_profiles_capabilities_block:
        for struct_name, members in json_profiles_capabilities_block["features"].items():
            #aliases = gatherDependentCapabilityAliases(vk, version)
            continue # TODO

    if "properties" in json_profiles_capabilities_block:
        for struct_name, members in json_profiles_capabilities_block["properties"].items():
            #aliases = gatherDependentCapabilityAliases(vk, version)
            continue # TODO
   
    return


def pull_aliases_profiles_file(vk: VulkanObject, require_promoted_extensions: bool, ignore_extension_versions: bool, json_file_data):
    profiles_data = json_file_data["profiles"]
    json_profiles_capabilities = json_file_data["capabilities"]

    for key, value in profiles_data.items():
        version = VK_VERSION.NONE
        if not require_promoted_extensions:
            version = VK_VERSION.from_string(value["api-version"])

        block_names = collect_block_names(value["capabilities"])
        
        for block_name in block_names:
            pull_aliases_capabilities_block(vk, version, ignore_extension_versions, json_profiles_capabilities[block_name])


def pull_aliases_profiles_files(vk: VulkanObject, require_promoted_extensions: bool, ignore_extension_versions: bool, json_files_dict):
    for key, value in json_files_dict.items():
        logging.debug(f"Fill capabilities aliases for: {key}")
        pull_aliases_profiles_file(vk, require_promoted_extensions, ignore_extension_versions, value)


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
        
        version = VK_VERSION.from_string(value["api-version"])

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
        pull_aliases_profiles_files(vk, require_promoted_extensions, ignore_extension_versions, json_files_dict)

    if ConvertBits.STRIP_DUPLICATION in mode_enums:
        strip_profiles_files_capabilities_duplication(json_files_dict)

    save_profiles_jsons(json_files_dict, Path(args.output), OutputFormatType(args.format))


