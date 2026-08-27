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
import re


class VK_VERSION(Enum):
    NONE = "VK_NONE"
    V1_0 = "VK_VERSION_1_0"
    V1_1 = "VK_VERSION_1_1"
    V1_2 = "VK_VERSION_1_2"
    V1_3 = "VK_VERSION_1_3"
    V1_4 = "VK_VERSION_1_4"

    def __hash__(self) -> int:
        return hash(self.value)

    @classmethod
    def from_string(cls, version_str: str) -> "VK_VERSION":
        if not version_str or version_str in ("NONE", "VK_NONE"):
            return cls.NONE
        
        # 1. Direct enum value match
        try:
            return cls(version_str)
        except ValueError:
            pass

        s = str(version_str).strip()

        # 2. Strict guard: MUST start with VK_VERSION_, VK_API_VERSION_, or a version digit
        if s.startswith("VK_VERSION_") or s.startswith("VK_API_VERSION_") or (s and s[0].isdigit()):
            parts = re.findall(r'\d+', s)
            if len(parts) >= 2:
                try:
                    return cls(f"VK_VERSION_{parts[0]}_{parts[1]}")
                except ValueError:
                    return cls.NONE
            elif len(parts) == 1:
                try:
                    return cls(f"VK_VERSION_{parts[0]}_0")
                except ValueError:
                    return cls.NONE

        return cls.NONE

    @classmethod
    def core_versions(cls) -> list["VK_VERSION"]:
        """Returns all Vulkan core versions that promote extensions (> Vulkan 1.0)."""
        return [v for v in cls if v > cls.V1_0]

    def as_tuple(self) -> tuple[int, int]:
        """Converts enum value (e.g., 'VK_VERSION_1_3') to numeric tuple (1, 3) for comparisons."""
        if self == VK_VERSION.NONE:
            return (-1, -1)
        parts = str(self.value).split('_')
        if len(parts) >= 4:
            return (int(parts[2]), int(parts[3]))
        return (-1, -1)

    # Native Rich Comparisons
    def __eq__(self, other) -> bool:
        if isinstance(other, VK_VERSION):
            return self.value == other.value
        if isinstance(other, str):
            other_ver = VK_VERSION.from_string(other)
            if self == VK_VERSION.NONE or other_ver == VK_VERSION.NONE:
                return self.value == other_ver.value
            return self.as_tuple() == other_ver.as_tuple()
        return False

    def __lt__(self, other) -> bool:
        other_ver = other if isinstance(other, VK_VERSION) else VK_VERSION.from_string(other)
        return self.as_tuple() < other_ver.as_tuple()

    def __le__(self, other) -> bool:
        other_ver = other if isinstance(other, VK_VERSION) else VK_VERSION.from_string(other)
        return self.as_tuple() <= other_ver.as_tuple()

    def __gt__(self, other) -> bool:
        other_ver = other if isinstance(other, VK_VERSION) else VK_VERSION.from_string(other)
        return self.as_tuple() > other_ver.as_tuple()

    def __ge__(self, other) -> bool:
        other_ver = other if isinstance(other, VK_VERSION) else VK_VERSION.from_string(other)
        return self.as_tuple() >= other_ver.as_tuple()


BUNDLE_STRUCTURE_VERSIONS = {
    "VkPhysicalDeviceVulkan11Features": VK_VERSION.V1_2,
    "VkPhysicalDeviceVulkan11Properties": VK_VERSION.V1_2,
    "VkPhysicalDeviceVulkan12Features": VK_VERSION.V1_2,
    "VkPhysicalDeviceVulkan12Properties": VK_VERSION.V1_2,
    "VkPhysicalDeviceVulkan13Features": VK_VERSION.V1_3,
    "VkPhysicalDeviceVulkan13Properties": VK_VERSION.V1_3,
    "VkPhysicalDeviceVulkan14Features": VK_VERSION.V1_4,
    "VkPhysicalDeviceVulkan14Properties": VK_VERSION.V1_4,
}


def is_bundle_structure(struct_name: str) -> bool:
    """Checks whether a structure is a core bundle structure (e.g. VkPhysicalDeviceVulkan11Features)."""
    return struct_name in BUNDLE_STRUCTURE_VERSIONS or bool(re.match(r"^VkPhysicalDeviceVulkan1\d(Features|Properties)$", struct_name))


def get_bundle_structure_core_version(struct_name: str) -> VK_VERSION:
    """Returns the VK_VERSION where the bundle structure was introduced, or VK_VERSION.NONE if not a bundle."""
    return BUNDLE_STRUCTURE_VERSIONS.get(struct_name, VK_VERSION.NONE)
