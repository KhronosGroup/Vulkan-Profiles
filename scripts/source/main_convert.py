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
import logging
from pathlib import Path
from enum import Enum

from source.main_validate import main_validate
from source.vulkan_object_utils import initVulkanObject
from source.profiles_json_utils import load_profiles_jsons, save_profiles_jsons, OutputFormatType

from source.convert_pull_required_capabilities import (
    pull_extension_dependencies_profiles_files,
    pull_required_capabilities_profiles_files
)
from source.convert_pull_promoted_extensions import pull_promoted_extensions_profiles_files
from source.convert_pull_aliases import pull_aliases_profiles_files
from source.convert_consolidate import consolidate_profiles_files
from source.convert_strip_duplication import strip_duplication_profiles_files
from source.convert_strip_promoted_extensions import strip_promoted_extensions_profiles_files
from source.convert_sort import sort_profiles_files


class ConvertBits(str, Enum):
    PULL_REQUIRED_CAPABILITIES = 'pull-required-capabilities'  # Evaluates extension dependencies and pulls satisfied core/extension feature and property requirements into capability blocks.
    PULL_PROMOTED_EXTENSIONS = 'pull-promoted-extensions'      # Requires all extensions promoted to core up to the profile's target Vulkan version.
    IGNORE_EXTENSION_VERSIONS = 'ignore-extension-versions'    # Sets all required extension versions to 1, overriding specific extension spec versions.
    PULL_ALIASES = 'pull-aliases'                              # Resolves and populates all equivalent capability aliases across core structures and extensions.
    CONSOLIDATE = 'consolidate'                                # Merges all mandatory capability blocks into a single consolidated requirements block per profile.
    STRIP_DUPLICATION = 'strip-duplication'                    # Removes redundant duplicate features, properties, and extension requirements across inheritance trees and within blocks.
    STRIP_PROMOTED_EXTENSIONS = 'strip-promoted-extensions'    # Removes extensions that are already promoted to the profile's target core Vulkan version.
    SORT = 'sort'                                              # Sorts capability blocks, structures, and extension lists into canonical Vulkan order.


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
    