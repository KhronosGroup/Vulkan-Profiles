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

from datetime import datetime
from enum import Enum
from pathlib import Path
import argparse
import functools
import importlib.resources
import re
import tempfile
import sys
import collections
from vulkan_object import get_vulkan_object
import vulkan_object
from source.profiles_parsing import load_profiles_jsons
from source.profiles_parsing import save_profiles_jsons
from source.vk_xml_parsing import find_dependent_extensions
from source.expression_parsing import VK_VERSION


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

def pull_capabilities_block_dependencies(vk, version: VK_VERSION, json_profiles_capabilities_block):
    if "extensions" not in json_profiles_capabilities_block:
        return
    
    extensions = find_dependent_extensions(vk, version, json_profiles_capabilities_block["extensions"])
    json_profiles_capabilities_block["extensions"] = extensions
    
    return

def pull_profiles_file_dependencies(vk, require_promoted_extensions, json_file_data):
    profiles_data = json_file_data["profiles"]
    json_profiles_capabilities = json_file_data["capabilities"]

    for key, value in profiles_data.items():
        version = VK_VERSION.NONE
        if not require_promoted_extensions:
            version = VK_VERSION.from_string(value["api-version"])

        block_names = collect_block_names(value["capabilities"])
        
        for block_name in block_names:
            pull_capabilities_block_dependencies(vk, version, json_profiles_capabilities[block_name])

def pull_profiles_files_dependencies(vk, require_promoted_extensions, json_files_dict):
    for key, value in json_files_dict.items():
        print(key)
        pull_profiles_file_dependencies(vk, require_promoted_extensions, value)

def main_convert(args):
    vk = get_vulkan_object()

    for version in vk.versions.values():
        print(version.name)
    
    json_files_dict = load_profiles_jsons(Path(args.input))
    save_profiles_jsons(Path(args.format), json_files_dict)
    
    require_promoted_extensions = False
    if args.require_promoted_extensions is not None:
        require_promoted_extensions = True
    pull_profiles_files_dependencies(vk, require_promoted_extensions, json_files_dict)

    save_profiles_jsons(Path(args.output), json_files_dict)
 
def main(argv):
    parser = argparse.ArgumentParser(description='Convert Vulkan profile JSON file')
    parser.add_argument('convert', action='store',
                        help='Convert an implicit profile to an explicit profile by pulling Vulkan capabilities dependencies from vk.xml.')
    parser.add_argument('--registry', '-r', action='store', # required=True,
                        help='Use specified registry file instead of vk.xml.')
#    parser.add_argument('--config', '-c', action='store',
#                        help='Use specified a JSON merge config file path instead of using individual arguments.')
    parser.add_argument('--input', '-i', action='store', required=True, help='Path to the input profiles files.')
    parser.add_argument('--output', '-o', action='store', required=True, help='Path to the output profiles files.')
    parser.add_argument('--format', action='store', required=True, help='Path to the reformated profiles files.')
    parser.add_argument('--require-promoted-extensions', action='store_true', help='Require all extensions promoted to a core version.')
#    parser.add_argument('--output-profile', action='store',
#                        help='Profile name of the output profile. Deprecated, replaced by `--profile-name`.')
#    parser.add_argument('--profile-name', action='store',
#                        help='Profile name of the output profile. If the argument is not set, the value is generated.')
#    parser.add_argument('--profile-version', action='store',
#                        help='Override the Profile version of the generated profile. If the argument is not set, the value is 1.')
#    parser.add_argument('--profile-label', action='store',
#                        help='Override the Label of the generated profile. If the argument is not set, the value is generated.')
#    parser.add_argument('--profile-desc', action='store',
#                        help='Override the Description of the generated profile. If the argument is not set, the value is generated.')
#    parser.add_argument('--profile-date', action='store',
#                        help='Override the release date of the generated profile. If the argument is not set, the value is generated.')
#    parser.add_argument('--profile-api-version', action='store',
#                        help='Override the Vulkan API version of the generated profile. If the argument is not set, the value is generated.')
#    parser.add_argument('--profile-stage', action='store', choices=['ALPHA', 'BETA', 'STABLE'], default='STABLE',
#                        help='Override the development stage of the generated profile. If the argument is not set, the value is set to "stable".')
#    parser.add_argument('--profile-required-profiles', action='store',
#                        help='Comma separated list of required profiles by the generated profile.')
#    parser.add_argument('--mode', '-m', action='store', choices=['union', 'intersection'], default='intersection',
#                        help='Mode of profile combination. If the argument is not set, the value is set to "intersection".')
#    parser.add_argument('--strip-duplicate-structs', action='store_true',
#                        help='Strip the duplicated structures in the generated profiles file.')

    # parser.set_defaults(mode='intersection')

    args = parser.parse_args(argv)

    if args.convert is not None:
        main_convert(args)
        exit()
    else:
        parser.print_help()
        exit()

    # profile_configs = list()

    # if args.registry is None:
    #    gen_profiles_solution.Log.e('Merging the profiles requires specifying --registry')
    #    parser.print_help()
    #    exit()

    #registry = gen_profiles_solution.VulkanRegistry(args.registry)

    #if (args.mode.lower() != 'union' and args.mode.lower() != 'intersection'):
    #    gen_profiles_solution.Log.e('Mode must be either union or intersection')
    #    parser.print_help()
    #    exit()

    #if args.strip_duplicate_structs:
    #    gen_profiles_solution.Log.i('Stripping duplicated structures. `--strip-duplicate-structs` is set. Eg the output profiles file will contain VkPhysicalDeviceVulkan11Properties not VkPhysicalDeviceMultiviewPropertiesKHR.')
    #    strip_duplicate_struct = True
    #else:
    #    strip_duplicate_struct = False

    #if args.input_profiles is not None:
    #    input_profile_names = args.input_profiles.split(',')
    #else:
    #    input_profile_names = list()

    #profile_file = ProfileFile()

    #if args.config is None:
    #    profile_config = ProfileConfig(args.input, input_profile_names, args.profile_api_version, args.mode)

    #    if args.profile_name is not None:
    #        if not re.match('^VP_[A-Z0-9]+[A-Za-z0-9]+', args.profile_name):
    #            gen_profiles_solution.Log.e('Invalid profile_name, must follow regex pattern ^VP_[A-Z0-9]+[A-Za-z0-9]+')
    #            exit()
    #        else:
    #            profile_config.name = args.profile_name
    #    elif args.output_profile is not None:
    #        if not re.match('^VP_[A-Z0-9]+[A-Za-z0-9]+', args.output_profile):
    #            gen_profiles_solution.Log.e('Invalid output_profile, must follow regex pattern ^VP_[A-Z0-9]+[A-Za-z0-9]+')
    #            exit()
    #        else:
    #            profile_config.name = args.output_profile

    #    if args.profile_version is not None:
    #        profile_config.version = int(args.profile_version)

    #    if args.profile_label is not None:
    #        profile_config.label = args.profile_label

    #    if args.profile_desc is not None:
    #        profile_config.description = args.profile_desc

    #    if args.profile_stage is not None:
    #        profile_config.stage = args.profile_stage

    #    if args.profile_date is not None:
    #        profile_config.date = args.profile_date

    #    if args.profile_required_profiles is not None:
    #        profile_config.required_profiles = args.profile_required_profiles.split(',')

    #    profile_configs.append(profile_config)

    #else:
    #    currentdir = os.path.dirname(args.config)
        
    #    json_file = open(args.config, "r")
    #    json_data = json.load(json_file)

    #    if json_data["$schema"]:
    #        profile_file.set_schema(json_data["$schema"])
    #    if json_data["contributors"]:
    #        profile_file.set_contributors(json_data["contributors"])
    #    if json_data["history"]:
    #        profile_file.set_history(json_data["history"])

    #    for profile_name in json_data["profiles"]:
    #        profile_value = json_data["profiles"][profile_name]
    #        profile_config = ProfileConfig(currentdir + "/" +  profile_value["input"], list(), profile_value["api-version"], args.mode)
    #        profile_config.apply_json_value(profile_name, profile_value)
    #        profile_configs.append(profile_config)

    #for config in profile_configs:
    #    profile_merger = ProfileMerger(registry)
    #    profile_merger.merge(
    #        config,
    #        profile_file,
    #        args.mode,
    #        strip_duplicate_struct)

    #profile_file.dump(args.output_path)

if __name__ == '__main__':
    print(sys.executable)
    
    sys.exit(main(sys.argv[1:]))

