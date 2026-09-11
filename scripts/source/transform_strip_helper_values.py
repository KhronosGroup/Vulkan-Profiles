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
from source.vulkan_object_utils import VulkanObject


def get_bitmask_helper_values(vk: VulkanObject) -> set[str]:
    """Extracts non-bitpos helper enums (composites and zero/none flags) from bitmask enums in vk.xml."""
    helpers = set()
    if not hasattr(vk, 'enums'):
        return helpers

    for enum_name, enum_obj in vk.enums.items():
        if getattr(enum_obj, 'type', None) == 'bitmask':
            for elem in getattr(enum_obj, 'values', []):
                # Entries with 'value' attribute rather than 'bitpos' or 'alias' are helper values
                if hasattr(elem, 'value') and not getattr(elem, 'bitpos', None) and not getattr(elem, 'alias', None):
                    helpers.add(elem.name)

    return helpers


def _strip_helpers_from_dict(data: dict, helpers: set[str]):
    for key, val in list(data.items()):
        if isinstance(val, dict):
            _strip_helpers_from_dict(val, helpers)
        elif isinstance(val, list):
            filtered_list = [item for item in val if item not in helpers]
            if filtered_list != val:
                data[key] = filtered_list


def strip_helper_values_profiles_files(vk: VulkanObject, json_files_dict: dict):
    """Strips non-bitpos bitmask helper values from all loaded profile JSON files."""
    helpers = get_bitmask_helper_values(vk)
    if not helpers:
        return

    for file_data in json_files_dict.values():
        if isinstance(file_data, dict) and "capabilities" in file_data:
            for cap_block in file_data["capabilities"].values():
                if isinstance(cap_block, dict):
                    _strip_helpers_from_dict(cap_block, helpers)
                    