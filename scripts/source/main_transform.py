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

from source.vulkan_object_utils import initVulkanObject, VulkanObject
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
from source.transform_pull_promoted_extensions import pull_promoted_extensions_profiles_files
from source.transform_pull_aliases import pull_aliases_profiles_files
from source.transform_consolidate import consolidate_profiles_files
from source.transform_strip_helper_values import strip_helper_values_profiles_files
from source.transform_strip_duplication import strip_duplication_profiles_files
from source.transform_strip_promoted_extensions import strip_promoted_extensions_profiles_files
from source.transform_sort import sort_profiles_files
from source.main_validate import main_validate


def transform_profiles_files(
    vk: VulkanObject,
    json_files_dict: dict,
    transform_modes: list[TransformBits]
):
    if not transform_modes:
        return

    ignore_ext_versions = TransformBits.IGNORE_EXTENSION_VERSIONS in transform_modes
    override_core_caps = TransformBits.OVERRIDE_CORE_CAPABILITIES in transform_modes

    if TransformBits.PULL_REQUIRED_CAPABILITIES in transform_modes:
        pull_extension_dependencies_profiles_files(
            vk, ignore_ext_versions, json_files_dict, override_core_capabilities=override_core_caps
        )
        pull_required_capabilities_profiles_files(
            vk, json_files_dict, override_core_capabilities=override_core_caps
        )

    if TransformBits.PULL_PROMOTED_EXTENSIONS in transform_modes:
        pull_promoted_extensions_profiles_files(vk, ignore_ext_versions, json_files_dict)

    if TransformBits.PULL_ALIASES in transform_modes:
        pull_aliases_profiles_files(vk, json_files_dict)

    if TransformBits.CONSOLIDATE in transform_modes:
        consolidate_profiles_files(vk, json_files_dict)

    if TransformBits.STRIP_HELPER_VALUES in transform_modes:
        strip_helper_values_profiles_files(vk, json_files_dict)

    if TransformBits.STRIP_DUPLICATION in transform_modes:
        strip_duplication_profiles_files(vk, json_files_dict)

    if TransformBits.STRIP_PROMOTED_EXTENSIONS in transform_modes:
        strip_promoted_extensions_profiles_files(vk, json_files_dict)

    if TransformBits.SORT in transform_modes:
        sort_profiles_files(vk, json_files_dict)


def main_transform(args):
    input_path = Path(args.input)
    output_path = Path(args.output)
    registry_path = getattr(args, 'registry', None)
    format_type = getattr(args, 'format', OutputFormatType.PRETTY) or OutputFormatType.PRETTY
    transform_modes = getattr(args, 'mode', []) or []

    if getattr(args, 'validate', None) is not None:
        main_validate(args)

    json_files_dict = load_profiles_jsons(input_path)
    if not json_files_dict:
        logging.error(f"No profiles JSON files loaded from '{input_path}'")
        return

    vk = initVulkanObject(getattr(args, 'api', 'vulkan'), registry_path)

    transform_profiles_files(vk, json_files_dict, transform_modes)

    save_profiles_jsons(json_files_dict, output_path, format_type)
    logging.info(f"Transformed profile files saved to {output_path}")