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

from source.vulkan_object_utils import (
    VulkanObject, 
    VK_VERSION, 
    is_extension_promoted_to_version
)
from source.profiles_json_utils import (
    collect_block_names, 
    get_topologically_sorted_file_keys
)


def strip_promoted_extensions_capabilities_block(
    vk: VulkanObject, version: VK_VERSION, json_profiles_capabilities_block: dict
):
    if "extensions" not in json_profiles_capabilities_block or not isinstance(json_profiles_capabilities_block["extensions"], dict):
        return

    block_exts = json_profiles_capabilities_block["extensions"]
    exts_to_remove = [
        ext_name for ext_name in block_exts.keys()
        if is_extension_promoted_to_version(vk, ext_name, version)
    ]

    for ext_name in exts_to_remove:
        del block_exts[ext_name]

    if not block_exts:
        del json_profiles_capabilities_block["extensions"]


def strip_promoted_extensions_profiles_file(vk: VulkanObject, json_file_data: dict):
    profiles_data = json_file_data.get("profiles", {})
    json_profiles_capabilities = json_file_data.get("capabilities", {})

    for profile_key, profile_obj in profiles_data.items():
        api_version = VK_VERSION.from_string(profile_obj.get("api-version", "1.0.0"))
        block_names = collect_block_names(profile_obj.get("capabilities", []))

        for block_name in block_names:
            if block_name in json_profiles_capabilities:
                strip_promoted_extensions_capabilities_block(
                    vk, api_version, json_profiles_capabilities[block_name]
                )


def strip_promoted_extensions_profiles_files(vk: VulkanObject, json_files_dict: dict):
    if not isinstance(json_files_dict, dict):
        return

    if "profiles" in json_files_dict or "capabilities" in json_files_dict:
        strip_promoted_extensions_profiles_file(vk, json_files_dict)
        return

    sorted_file_keys = get_topologically_sorted_file_keys(json_files_dict)
    for file_key in sorted_file_keys:
        strip_promoted_extensions_profiles_file(vk, json_files_dict[file_key])