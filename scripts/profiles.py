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
from datetime import datetime
from enum import StrEnum
from pathlib import Path
import argparse
import sys
from vulkan_object import get_vulkan_object
from vulkan_object import VulkanObject
from source.profiles_parsing import load_profiles_jsons
from source.profiles_parsing import save_profiles_jsons
from source.profiles_parsing import validate_profiles_json
from source.profiles_parsing import OutputFormatType
from source.vk_xml_parsing import find_dependent_extensions
from source.expression_parsing import VK_VERSION

class ConvertMode(StrEnum):
    STRIP_DUPLICATION = 'strip-duplication'
    PULL_DEPENDENCES = 'pull-dependences'

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
    
    extensions = find_dependent_extensions(vk, version, ignore_extension_versions, json_profiles_capabilities_block["extensions"])
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
    vk = get_vulkan_object(args.registry or None)

    for version in vk.versions.values():
        logging.debug(version.name)
    
    json_files_dict = load_profiles_jsons(Path(args.input))
    #save_profiles_jsons(json_files_dict, Path(args.format))
    
    require_promoted_extensions = False
    if args.require_promoted_extensions is not None:
        require_promoted_extensions = True
        
    ignore_extension_versions = False
    if args.ignore_extension_versions is not None:
        ignore_extension_versions = True
        
    mode_enums = [ConvertMode(m) for m in args.mode]
    
    if ConvertMode.PULL_DEPENDENCES in mode_enums:
        pull_profiles_files_dependencies(vk, require_promoted_extensions, ignore_extension_versions, json_files_dict)

    if ConvertMode.STRIP_DUPLICATION in mode_enums:
        strip_profiles_files_capabilities_duplication(json_files_dict)

    save_profiles_jsons(json_files_dict, Path(args.output), OutputFormatType(args.format))


def main_validate(args):
    validate_profiles_json(Path(args.input), Path(args.schema))

 
def main(argv):
    logging.basicConfig(level=logging.DEBUG, format='%(levelname)s: %(message)s')
    
    parser = argparse.ArgumentParser(description='Convert Vulkan profile JSON file')
    subparsers = parser.add_subparsers(dest='command', required=True)
    
    convert_parser = subparsers.add_parser('convert', help='Convert an implicit profile to an explicit profile by pulling Vulkan capabilities dependencies from vk.xml.')
    convert_parser.add_argument('--require-promoted-extensions', action='store_true', help='Require all extensions promoted to a core version.')
    convert_parser.add_argument('--ignore-extension-versions', action='store_true', help='Set all required extensions to version 1, ignoring extension versions.')
    convert_parser.add_argument('--registry', '-r', action='store', help='Use a specific Vulkan registry file (vk.xml).')
    convert_parser.add_argument('--input', '-i', action='store', required=True, help='Path to the input profiles files.')
    convert_parser.add_argument('--output', '-o', action='store', required=True, help='Path to the output profiles files.')
    convert_parser.add_argument('--format', action='store', choices=list(OutputFormatType), default=OutputFormatType.FLATTEN, help='Formatting style for the profiles files (default: flatten).')
    convert_parser.add_argument('--mode', nargs='*',action='store', choices=list(ConvertMode), default=list(ConvertMode), help='List of conversion capabilities')

    validate_parser = subparsers.add_parser('validate', help='Validate a profile file against a profile schema.')
    validate_parser.add_argument('--schema', '-s', action='store', required=True, help='Use a specific Vulkan registry file (vk.xml).')
    validate_parser.add_argument('--input', '-i', action='store', required=True, help='Path to the input profiles files.')

    args = parser.parse_args(argv)

    if args.command == 'convert':
        main_convert(args)
    elif args.command == 'validate':
        main_validate(args)
    else:
        parser.print_help()


if __name__ == '__main__':
    print(sys.executable)
    
    sys.exit(main(sys.argv[1:]))

