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

    @classmethod
    def from_string(cls, version_str: str) -> "VK_VERSION":
        if not version_str or version_str in ("NONE", "VK_NONE"):
            return cls.NONE
        
        # Match enum value string directly if passed (e.g., "VK_VERSION_1_3")
        try:
            return cls(version_str)
        except ValueError:
            pass

        # Extract major/minor digits from version strings (e.g., "1.3.276" -> ["1", "3"])
        parts = re.findall(r'\d+', str(version_str))
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

    def as_tuple(self) -> tuple[int, int]:
        """Converts enum value (e.g., 'VK_VERSION_1_3') to numeric tuple (1, 3) for comparisons."""
        if self == VK_VERSION.NONE:
            return (-1, -1)
        parts = str(self.value).split('_')
        if len(parts) >= 4:
            return (int(parts[2]), int(parts[3]))
        return (-1, -1)

    # Native Rich Comparisons
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
    