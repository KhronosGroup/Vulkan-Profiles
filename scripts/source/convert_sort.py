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

from source.vulkan_object_version import get_bundle_structure_core_version, get_bundle_structures
from source.vulkan_object_utils import (
    VulkanObject, 
    VK_VERSION, 
    gatherDependentExtensions, 
    getStructCoreVersion,
    is_extension_struct_name
)
from source.convert_utils import VendorPriority, CoreStructTier


def get_struct_sort_key(vk: VulkanObject, struct_name: str) -> tuple:
    if struct_name in get_bundle_structures(vk=vk):
        ver_val = get_bundle_structure_core_version(struct_name).as_tuple()
        return (VendorPriority.CORE, CoreStructTier.BUNDLE, ver_val, struct_name)

    is_ext = is_extension_struct_name(vk, struct_name)

    if not is_ext:
        core_ver = getStructCoreVersion(vk, struct_name)
        if core_ver == VK_VERSION.NONE:
            core_ver = VK_VERSION.V1_0

        ver_val = core_ver.as_tuple() if hasattr(core_ver, 'as_tuple') else (1, 0)
        return (VendorPriority.CORE, CoreStructTier.SPLIT, ver_val, struct_name)

    if struct_name.endswith("KHR"):
        return (VendorPriority.KHR, (0, 0), struct_name)
    elif struct_name.endswith("EXT"):
        return (VendorPriority.EXT, (0, 0), struct_name)
    else:
        return (VendorPriority.VENDOR, (0, 0), struct_name)


def get_ext_priority_key(ext_name: str) -> tuple:
    if ext_name.startswith("VK_KHR_"):
        return (VendorPriority.KHR, ext_name)
    elif ext_name.startswith("VK_EXT_"):
        return (VendorPriority.EXT, ext_name)
    else:
        return (VendorPriority.VENDOR, ext_name)


def sort_extensions(vk: VulkanObject, exts: dict | list) -> dict | list:
    is_dict = isinstance(exts, dict)
    ext_names = list(exts.keys()) if is_dict else list(exts)
    ext_set = set(ext_names)

    prereqs = {e: set() for e in ext_names}
    for e in ext_names:
        deps = gatherDependentExtensions(vk, VK_VERSION.V1_0, True, {e: 1})
        for dep in deps:
            if dep != e and dep in ext_set:
                prereqs[e].add(dep)

    in_degree = {e: len(prereqs[e]) for e in ext_names}
    candidates = [e for e in ext_names if in_degree[e] == 0]

    sorted_exts = []
    while candidates:
        candidates.sort(key=get_ext_priority_key)
        curr = candidates.pop(0)
        sorted_exts.append(curr)

        for e in ext_names:
            if curr in prereqs[e]:
                prereqs[e].remove(curr)
                in_degree[e] -= 1
                if in_degree[e] == 0:
                    candidates.append(e)

    if len(sorted_exts) < len(ext_names):
        remaining = [e for e in ext_names if e not in sorted_exts]
        remaining.sort(key=get_ext_priority_key)
        sorted_exts.extend(remaining)

    if is_dict:
        return {e: exts[e] for e in sorted_exts}
    else:
        return sorted_exts


def sort_capabilities_block(vk: VulkanObject, json_block: dict):
    if not isinstance(json_block, dict):
        return

    if "extensions" in json_block:
        json_block["extensions"] = sort_extensions(vk, json_block["extensions"])

    for category in ("features", "properties"):
        if category in json_block and isinstance(json_block[category], dict):
            sorted_cat = {}
            sorted_struct_names = sorted(
                json_block[category].keys(), 
                key=lambda s: get_struct_sort_key(vk, s)
            )
            for s_name in sorted_struct_names:
                sorted_cat[s_name] = json_block[category][s_name]
            json_block[category] = sorted_cat

    if "formats" in json_block and isinstance(json_block["formats"], dict):
        sorted_formats = {}
        for fmt_name in sorted(json_block["formats"].keys()):
            fmt_structs = json_block["formats"][fmt_name]
            if isinstance(fmt_structs, dict):
                sorted_fmt_structs = {}
                for s_name in sorted(fmt_structs.keys(), key=lambda s: get_struct_sort_key(vk, s)):
                    sorted_fmt_structs[s_name] = fmt_structs[s_name]
                sorted_formats[fmt_name] = sorted_fmt_structs
            else:
                sorted_formats[fmt_name] = fmt_structs
        json_block["formats"] = sorted_formats


def sort_profiles_file(vk: VulkanObject, json_file_data: dict):
    capabilities_dict = json_file_data.get("capabilities", {})
    for block_name, block in capabilities_dict.items():
        sort_capabilities_block(vk, block)


def sort_profiles_files(vk: VulkanObject, json_files_dict: dict):
    if not isinstance(json_files_dict, dict):
        return

    if "profiles" in json_files_dict or "capabilities" in json_files_dict:
        sort_profiles_file(vk, json_files_dict)
        return

    for file_key, json_file_data in json_files_dict.items():
        if isinstance(json_file_data, dict):
            sort_profiles_file(vk, json_file_data)
            