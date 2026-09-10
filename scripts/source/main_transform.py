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

from source.main_validate import main_validate
from source.vulkan_object_utils import initVulkanObject
from source.profiles_json_utils import (
    load_profiles_jsons, 
    save_profiles_jsons, 
    OutputFormatType
)

from source.transform_utils import TransformBits
from source.transform_pull_required_capabilities import (
    pull_extension_dependencies_profiles_files,
    pull_required_capabilities_profiles_files
)
from source.transform_pull_promoted_extensions import (
    pull_promoted_extensions_profiles_files
)
from source.transform_pull_aliases import (
    pull_aliases_profiles_files
)
from source.transform_consolidate import (
    consolidate_profiles_files
)
from source.transform_strip_duplication import (
    strip_duplication_profiles_files
)
from source.transform_strip_promoted_extensions import (
    strip_promoted_extensions_profiles_files
)
from source.transform_sort import (
    sort_profiles_files
)


def main_transform(args):
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
    mode_enums = [TransformBits(m) for m in raw_modes]
    
    require_promoted_extensions = TransformBits.PULL_PROMOTED_EXTENSIONS in mode_enums
    ignore_extension_versions = TransformBits.IGNORE_EXTENSION_VERSIONS in mode_enums

    # Pull Required Capabilities (Dependencies + Core/Extension Requirements)
    if TransformBits.PULL_REQUIRED_CAPABILITIES in mode_enums:
        logging.debug("Pulling extension dependencies...")
        pull_extension_dependencies_profiles_files(vk, ignore_extension_versions, json_files_dict)

        logging.debug("Evaluating and pulling required capabilities...")
        pull_required_capabilities_profiles_files(vk, json_files_dict)

    # Pull Promoted Extensions
    if TransformBits.PULL_PROMOTED_EXTENSIONS in mode_enums:
        logging.debug("Pulling promoted extensions for core versions...")
        pull_promoted_extensions_profiles_files(vk, ignore_extension_versions, json_files_dict)

    # Pull Capability Aliases
    if TransformBits.PULL_ALIASES in mode_enums:
        logging.debug("Pulling capability aliases...")
        pull_aliases_profiles_files(vk, require_promoted_extensions, json_files_dict)

    # Consolidate
    if TransformBits.CONSOLIDATE in mode_enums:
        logging.debug("Consolidating profile capability blocks...")
        consolidate_profiles_files(json_files_dict)

    # Strip Duplication
    if TransformBits.STRIP_DUPLICATION in mode_enums:
        logging.debug("Stripping capabilities duplication...")
        strip_duplication_profiles_files(vk, json_files_dict)

    # Strip Promoted Extensions
    if TransformBits.STRIP_PROMOTED_EXTENSIONS in mode_enums:
        logging.debug("Stripping extensions promoted to profile core version...")
        strip_promoted_extensions_profiles_files(vk, json_files_dict)

    # Sort
    if TransformBits.SORT in mode_enums:
        logging.debug("Sorting capability blocks and extensions...")
        sort_profiles_files(vk, json_files_dict)

    save_profiles_jsons(json_files_dict, Path(args.output), OutputFormatType(args.format))
    