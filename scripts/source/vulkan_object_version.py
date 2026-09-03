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

from enum import Enum
from typing import Any


class VK_VERSION(Enum):
    NONE = "VK_VERSION_NONE"
    V1_0 = "VK_VERSION_1_0"
    V1_1 = "VK_VERSION_1_1"
    V1_2 = "VK_VERSION_1_2"
    V1_3 = "VK_VERSION_1_3"
    V1_4 = "VK_VERSION_1_4"

    @classmethod
    def all_versions(cls) -> list['VK_VERSION']:
        return [cls.V1_0, cls.V1_1, cls.V1_2, cls.V1_3, cls.V1_4]

    @classmethod
    def core_versions(cls) -> list['VK_VERSION']:
        return [cls.V1_1, cls.V1_2, cls.V1_3, cls.V1_4]

    @classmethod
    def from_string(cls, ver_str: str | Any) -> 'VK_VERSION':
        if not ver_str:
            return cls.NONE
        if isinstance(ver_str, cls):
            return ver_str
        s = str(ver_str).upper().strip()
        if "1.4" in s or "VK_VERSION_1_4" in s or "VK_API_VERSION_1_4" in s:
            return cls.V1_4
        elif "1.3" in s or "VK_VERSION_1_3" in s or "VK_API_VERSION_1_3" in s:
            return cls.V1_3
        elif "1.2" in s or "VK_VERSION_1_2" in s or "VK_API_VERSION_1_2" in s:
            return cls.V1_2
        elif "1.1" in s or "VK_VERSION_1_1" in s or "VK_API_VERSION_1_1" in s:
            return cls.V1_1
        elif "1.0" in s or "VK_VERSION_1_0" in s or "VK_API_VERSION_1_0" in s:
            return cls.V1_0
        return cls.NONE

    @classmethod
    def from_tuple(cls, ver_tuple: tuple[int, int]) -> 'VK_VERSION':
        if ver_tuple == (1, 0):
            return cls.V1_0
        elif ver_tuple == (1, 1):
            return cls.V1_1
        elif ver_tuple == (1, 2):
            return cls.V1_2
        elif ver_tuple == (1, 3):
            return cls.V1_3
        elif ver_tuple == (1, 4):
            return cls.V1_4
        return cls.NONE

    def as_tuple(self) -> tuple[int, int]:
        if self == VK_VERSION.V1_0:
            return (1, 0)
        elif self == VK_VERSION.V1_1:
            return (1, 1)
        elif self == VK_VERSION.V1_2:
            return (1, 2)
        elif self == VK_VERSION.V1_3:
            return (1, 3)
        elif self == VK_VERSION.V1_4:
            return (1, 4)
        return (-1, -1)

    def _resolve_other(self, other: Any) -> 'VK_VERSION':
        if isinstance(other, VK_VERSION):
            return other
        if isinstance(other, str):
            return VK_VERSION.from_string(other)
        return VK_VERSION.NONE

    def __eq__(self, other: Any) -> bool:
        if isinstance(other, str):
            return self == VK_VERSION.from_string(other)
        return super().__eq__(other)

    def __hash__(self) -> int:
        return super().__hash__()

    def __ge__(self, other: Any) -> bool:
        other_ver = self._resolve_other(other)
        return self.as_tuple() >= other_ver.as_tuple()

    def __gt__(self, other: Any) -> bool:
        other_ver = self._resolve_other(other)
        return self.as_tuple() > other_ver.as_tuple()

    def __le__(self, other: Any) -> bool:
        other_ver = self._resolve_other(other)
        return self.as_tuple() <= other_ver.as_tuple()

    def __lt__(self, other: Any) -> bool:
        other_ver = self._resolve_other(other)
        return self.as_tuple() < other_ver.as_tuple()


BUNDLE_STRUCT_VERSIONS: dict[str, tuple[int, int]] = {
    "VkPhysicalDeviceFeatures": (1, 0),
    "VkPhysicalDeviceProperties": (1, 0),
    "VkPhysicalDeviceVulkan11Features": (1, 2),
    "VkPhysicalDeviceVulkan11Properties": (1, 2),
    "VkPhysicalDeviceVulkan12Features": (1, 2),
    "VkPhysicalDeviceVulkan12Properties": (1, 2),
    "VkPhysicalDeviceVulkan13Features": (1, 3),
    "VkPhysicalDeviceVulkan13Properties": (1, 3),
    "VkPhysicalDeviceVulkan14Features": (1, 4),
    "VkPhysicalDeviceVulkan14Properties": (1, 4),
}


def is_bundle_structure(struct_name: str) -> bool:
    """Returns True if struct_name is a Vulkan core bundle structure."""
    return struct_name in BUNDLE_STRUCT_VERSIONS


def get_bundle_structure_core_version(struct_name: str) -> VK_VERSION:
    """Returns the VK_VERSION for a bundle structure, or VK_VERSION.NONE if not a bundle."""
    if struct_name in BUNDLE_STRUCT_VERSIONS:
        return VK_VERSION.from_tuple(BUNDLE_STRUCT_VERSIONS[struct_name])
    return VK_VERSION.NONE


def get_feature_bundle_structures() -> list[str]:
    """Returns all feature bundle structure names introduced in Vulkan 1.1+."""
    return [
        s for s, ver in BUNDLE_STRUCT_VERSIONS.items()
        if s.endswith("Features") and ver > (1, 0)
    ]


def get_property_bundle_structures() -> list[str]:
    """Returns all property bundle structure names introduced in Vulkan 1.1+."""
    return [
        s for s, ver in BUNDLE_STRUCT_VERSIONS.items()
        if s.endswith("Properties") and ver > (1, 0)
    ]


def get_active_feature_bundles(api_version: VK_VERSION) -> list[str]:
    """Returns active feature bundle structure names for the given API version."""
    return [
        b for b in get_feature_bundle_structures()
        if api_version >= get_bundle_structure_core_version(b)
    ]


def get_active_property_bundles(api_version: VK_VERSION) -> list[str]:
    """Returns active property bundle structure names for the given API version."""
    return [
        b for b in get_property_bundle_structures()
        if api_version >= get_bundle_structure_core_version(b)
    ]


def get_version_string() -> str:
    """
    Returns the vkprofiles version string based on the baked build version.
    """
    try:
        from source.baked_version import BAKED_VERSION
        if BAKED_VERSION:
            return f"vkprofiles version {BAKED_VERSION} (ALPHA)"
    except ImportError:
        pass

    return "vkprofiles version unknown"


def main_version(args=None) -> str:
    """
    Handler for the 'version' subcommand and top-level --version option.
    """
    version_str = get_version_string()

    if args is not None:
        print(version_str)

    return version_str
