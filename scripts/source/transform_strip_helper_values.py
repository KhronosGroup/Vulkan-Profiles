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
    """
    Extracts bitmask enum values from VulkanObject.bitmasks where the flag is 
    defined by a 'value' attribute (bitpos is None) rather than a 'bitpos'.
    """
    helpers = set()
    if not hasattr(vk, 'bitmasks') or not isinstance(vk.bitmasks, dict):
        return helpers

    for bitmask in vk.bitmasks.values():
        for flag in getattr(bitmask, 'flags', []):
            # Flag objects with bitpos == None were created from XML value attributes 
            # (zero constants or multi-bit composite masks)
            if flag.bitpos is None or getattr(flag, 'multiBit', False) or getattr(flag, 'zero', False):
                helpers.add(flag.name)
                if hasattr(flag, 'aliases') and flag.aliases:
                    helpers.update(flag.aliases)

    return helpers


def _strip_helpers_from_dict(data: dict, helpers: set[str]):
    """Recursively strips identified helper enum strings from lists or dictionaries."""
    if not isinstance(data, dict):
        return

    keys_to_delete = []
    for key, val in list(data.items()):
        if isinstance(val, dict):
            _strip_helpers_from_dict(val, helpers)
        elif isinstance(val, list):
            filtered = [item for item in val if item not in helpers]
            data[key] = filtered
        elif isinstance(val, str) and val in helpers:
            keys_to_delete.append(key)

    for k in keys_to_delete:
        del data[k]


def strip_helper_values_profiles_files(vk: VulkanObject, json_files_dict: dict):
    """Strips non-bitpos bitmask helper values from loaded profile JSON structures."""
    helpers = get_bitmask_helper_values(vk)
    if not helpers:
        return

    for file_data in json_files_dict.values():
        if isinstance(file_data, dict):
            _strip_helpers_from_dict(file_data, helpers)
        elif hasattr(file_data, 'dict'):
            _strip_helpers_from_dict(file_data.dict, helpers)
            