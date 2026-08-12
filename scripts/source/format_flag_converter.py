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

from source.vulkan_object_utils import VulkanObject
from source.vulkan_object_version import VK_VERSION

FORMAT_STRUCTS_32 = [
    "VkFormatProperties",
]

FORMAT_STRUCTS_64 = [
    "VkFormatProperties3",
    "VkFormatProperties3KHR",
]

FORMAT_STRUCTS_4KHR = [
    "VkFormatProperties4KHR",
]


def isFormatStructValid(vk: VulkanObject, struct_name: str, version: VK_VERSION, enabled_exts: set[str]) -> bool:
    """
    Determines if a format properties structure (32-bit, 64-bit, or 4KHR) is valid 
    for the target API version and set of enabled extensions.
    """
    if struct_name == "VkFormatProperties":
        return True

    if struct_name in ("VkFormatProperties3", "VkFormatProperties3KHR"):
        if version >= VK_VERSION.V1_3:
            return True
        return "VK_KHR_format_feature_flags2" in enabled_exts

    if struct_name == "VkFormatProperties4KHR":
        return "VK_KHR_extended_flags" in enabled_exts

    return False


class FormatFeatureFlagConverter:
    """
    Bidirectionally maps 32-bit VkFormatFeatureFlagBits <-> 64-bit VkFormatFeatureFlagBits2 / VkFormatFeatureFlagBits4KHR
    by matching bitpos values directly from vk.xml.
    """
    def __init__(self, vk: VulkanObject):
        self.flag32_to_flag64: dict[str, str] = {}
        self.flag64_to_flag32: dict[str, str] = {}
        self.flag32_to_flag4khr: dict[str, str] = {}
        self.flag4khr_to_flag32: dict[str, str] = {}
        self._build_maps(vk)

    def _build_maps(self, vk: VulkanObject):
        bitmask_32 = vk.bitmasks.get("VkFormatFeatureFlagBits")
        bitmask_64 = vk.bitmasks.get("VkFormatFeatureFlagBits2")
        bitmask_4khr = vk.bitmasks.get("VkFormatFeatureFlagBits4KHR")

        if bitmask_32 and bitmask_64:
            bitpos_to_flag64 = {
                flag.bitpos: flag.name 
                for flag in bitmask_64.flags 
                if flag.bitpos is not None
            }
            for flag32 in bitmask_32.flags:
                if flag32.bitpos is not None and flag32.bitpos in bitpos_to_flag64:
                    flag64_name = bitpos_to_flag64[flag32.bitpos]
                    self.flag32_to_flag64[flag32.name] = flag64_name
                    self.flag64_to_flag32[flag64_name] = flag32.name
                    for alias32 in getattr(flag32, 'aliases', []):
                        self.flag32_to_flag64[alias32] = flag64_name

        if bitmask_32 and bitmask_4khr:
            bitpos_to_flag4khr = {
                flag.bitpos: flag.name 
                for flag in bitmask_4khr.flags 
                if flag.bitpos is not None
            }
            for flag32 in bitmask_32.flags:
                if flag32.bitpos is not None and flag32.bitpos in bitpos_to_flag4khr:
                    flag4_name = bitpos_to_flag4khr[flag32.bitpos]
                    self.flag32_to_flag4khr[flag32.name] = flag4_name
                    self.flag4khr_to_flag32[flag4_name] = flag32.name
                    for alias32 in getattr(flag32, 'aliases', []):
                        self.flag32_to_flag4khr[alias32] = flag4_name

    def to_flag64_list(self, flags32: list[str]) -> list[str]:
        result = []
        for f32 in flags32:
            if f32 in self.flag32_to_flag64:
                result.append(self.flag32_to_flag64[f32])
            elif f32.startswith("VK_FORMAT_FEATURE_2_"):
                result.append(f32)
        return result

    def to_flag4khr_list(self, flags32: list[str]) -> list[str]:
        result = []
        for f32 in flags32:
            if f32 in self.flag32_to_flag4khr:
                result.append(self.flag32_to_flag4khr[f32])
            elif f32 in self.flag32_to_flag64:
                result.append(self.flag32_to_flag64[f32])
            elif f32.startswith("VK_FORMAT_FEATURE_4_") or f32.startswith("VK_FORMAT_FEATURE_2_"):
                result.append(f32)
        return result

    def to_flag32_list(self, flags64: list[str]) -> list[str]:
        result = []
        for f64 in flags64:
            if f64 in self.flag64_to_flag32:
                result.append(self.flag64_to_flag32[f64])
            elif f64 in self.flag4khr_to_flag32:
                result.append(self.flag4khr_to_flag32[f64])
            elif f64.startswith("VK_FORMAT_FEATURE_") and not f64.startswith("VK_FORMAT_FEATURE_2_") and not f64.startswith("VK_FORMAT_FEATURE_4_"):
                result.append(f64)
        return result

    def expand_format_struct(
        self,
        vk: VulkanObject,
        src_struct_name: str,
        members_dict: dict,
        version: VK_VERSION,
        enabled_exts: set[str]
    ) -> dict[str, dict]:
        """
        Converts format property member flags and expands valid 32-bit, 64-bit, 
        and 4KHR target format structures based on API version and enabled extensions.
        """
        src_is_64 = src_struct_name in FORMAT_STRUCTS_64 or src_struct_name in FORMAT_STRUCTS_4KHR
        members_32, members_64, members_4khr = {}, {}, {}

        for member_name, flag_list in members_dict.items():
            if isinstance(flag_list, list):
                flags_32 = self.to_flag32_list(flag_list) if src_is_64 else flag_list
                members_32[member_name] = flags_32
                members_64[member_name] = self.to_flag64_list(flags_32)
                members_4khr[member_name] = self.to_flag4khr_list(flags_32)
            else:
                members_32[member_name] = members_64[member_name] = members_4khr[member_name] = flag_list

        target_groups = [
            (FORMAT_STRUCTS_32, members_32),
            (FORMAT_STRUCTS_64, members_64),
            (FORMAT_STRUCTS_4KHR, members_4khr),
        ]

        result_structs = {}
        for struct_list, members in target_groups:
            for target_struct in struct_list:
                if isFormatStructValid(vk, target_struct, version, enabled_exts):
                    result_structs.setdefault(target_struct, {}).update(members)

        return result_structs
    