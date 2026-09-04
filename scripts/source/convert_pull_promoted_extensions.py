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

from source.vulkan_object_utils import VulkanObject, VK_VERSION, gatherPromotedExtensionsForExactVersion
from source.profiles_json_utils import (
    collect_block_names,
    collect_required_profiles_capabilities,
    get_topologically_sorted_file_keys,
    get_primary_capability_block
)
from source.convert_pull_required_capabilities import pull_extension_dependencies_capabilities_block


def pull_promoted_extensions_profiles_file(
    vk: VulkanObject, 
    ignore_extension_versions: bool, 
    json_file_data: dict, 
    json_files_dict: dict = None
):
    profiles_data = json_file_data.get("profiles", {})
    capabilities_dict = json_file_data.setdefault("capabilities", {})

    for key, profile_obj in profiles_data.items():
        api_version = VK_VERSION.from_string(profile_obj["api-version"])

        context_extensions = set()
        context_features = set()
        parent_profiles = profile_obj.get("profiles", [])
        if json_files_dict and parent_profiles:
            parent_caps = collect_required_profiles_capabilities(json_files_dict, parent_profiles)
            context_extensions.update(parent_caps.get("extensions", {}).keys())
            parent_features_dict = parent_caps.get("features", {})
            for s_name, members in parent_features_dict.items():
                if isinstance(members, dict):
                    for m_name, val in members.items():
                        if val:
                            context_features.add((s_name, m_name))

        primary_block = get_primary_capability_block(profile_obj, capabilities_dict)
        if primary_block is None:
            continue

        ext_dict = primary_block.setdefault("extensions", {})

        for ver in VK_VERSION.versions():
            if ver != VK_VERSION.NONE and api_version != VK_VERSION.NONE and ver <= api_version:
                promoted_exts = gatherPromotedExtensionsForExactVersion(vk, ver)
                for ext_name, ext_ver in promoted_exts.items():
                    if ext_name not in ext_dict and ext_name not in context_extensions:
                        ext_dict[ext_name] = 1 if ignore_extension_versions else ext_ver

        block_names = collect_block_names(profile_obj.get("capabilities", []))
        for block_name in block_names:
            if block_name in capabilities_dict:
                block = capabilities_dict[block_name]
                pull_extension_dependencies_capabilities_block(
                    vk, api_version, ignore_extension_versions, block, context_extensions, context_features
                )
                if "extensions" in block and isinstance(block["extensions"], dict):
                    context_extensions.update(block["extensions"].keys())
                if "features" in block and isinstance(block["features"], dict):
                    for s_name, members in block["features"].items():
                        if isinstance(members, dict):
                            for m_name, val in members.items():
                                if val:
                                    context_features.add((s_name, m_name))


def pull_promoted_extensions_profiles_files(vk: VulkanObject, ignore_extension_versions: bool, json_files_dict: dict):
    if not isinstance(json_files_dict, dict):
        return

    if "profiles" in json_files_dict or "capabilities" in json_files_dict:
        pull_promoted_extensions_profiles_file(vk, ignore_extension_versions, json_files_dict, None)
        return

    sorted_file_keys = get_topologically_sorted_file_keys(json_files_dict)
    for file_key in sorted_file_keys:
        pull_promoted_extensions_profiles_file(vk, ignore_extension_versions, json_files_dict[file_key], json_files_dict)