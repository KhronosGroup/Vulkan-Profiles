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

import re
from typing import Any


class _VK_VERSION_Meta(type):
    """Metaclass enabling iteration (e.g. list(VK_VERSION)) and dictionary indexing for VK_VERSION."""

    def __iter__(cls):
        return iter(cls.versions())

    def __len__(cls):
        return len(cls.versions())

    def __getitem__(cls, item: str):
        if isinstance(item, str) and hasattr(cls, item):
            return getattr(cls, item)
        ver = cls.from_string(item)
        if ver != cls.NONE or str(item).upper() in ("NONE", "VK_NONE", "VK_VERSION_NONE"):
            return ver
        raise KeyError(item)


class VK_VERSION(metaclass=_VK_VERSION_Meta):
    _instances: dict[tuple[int, int], 'VK_VERSION'] = {}

    def __new__(cls, *args, **kwargs):
        if len(args) == 1 and not kwargs:
            arg = args[0]
            if isinstance(arg, cls):
                return arg
            if isinstance(arg, tuple) and len(arg) == 2:
                major, minor = arg
            else:
                s_arg = str(arg).strip()
                ver = cls.from_string(s_arg)
                if ver == cls.NONE and s_arg.upper() not in ("NONE", "VK_NONE", "VK_VERSION_NONE", ""):
                    raise ValueError(f"'{arg}' is not a valid VK_VERSION")
                return ver
        elif len(args) >= 2:
            major = args[0]
            minor = args[1]
            name = args[2] if len(args) > 2 else kwargs.get('name')
        else:
            major = kwargs.get('major', -1)
            minor = kwargs.get('minor', -1)
            name = kwargs.get('name')

        key = (major, minor)
        if key in cls._instances:
            return cls._instances[key]

        instance = super().__new__(cls)
        instance.major = major
        instance.minor = minor
        if name:
            instance.name = name
        elif major < 0:
            instance.name = "VK_VERSION_NONE"
        else:
            instance.name = f"VK_VERSION_{major}_{minor}"
        instance.value = instance.name
        cls._instances[key] = instance
        return instance

    def as_tuple(self) -> tuple[int, int]:
        return (self.major, self.minor)

    @classmethod
    def from_tuple(cls, ver_tuple: tuple[int, int]) -> 'VK_VERSION':
        if ver_tuple == (-1, -1):
            return cls.NONE
        return cls(ver_tuple[0], ver_tuple[1])

    @classmethod
    def from_string(cls, ver_str: Any) -> 'VK_VERSION':
        if not ver_str:
            return cls.NONE
        if isinstance(ver_str, cls):
            return ver_str

        s = str(ver_str).upper().strip()
        if "NONE" in s or s == "" or s == "VK_NONE":
            return cls.NONE

        match = re.search(r'(\d+)\.(\d+)', s)
        if match:
            return cls(int(match.group(1)), int(match.group(2)))

        match_vk = re.search(r'VK_VERSION_(\d+)_(\d+)', s) or re.search(r'VK_API_VERSION_(\d+)_(\d+)', s)
        if match_vk:
            return cls(int(match_vk.group(1)), int(match_vk.group(2)))

        return cls.NONE

    def _resolve(self, other: Any) -> 'VK_VERSION':
        if isinstance(other, VK_VERSION):
            return other
        if isinstance(other, str):
            return VK_VERSION.from_string(other)
        return VK_VERSION.NONE

    def __eq__(self, other: Any) -> bool:
        other_ver = self._resolve(other)
        return self.as_tuple() == other_ver.as_tuple()

    def __ne__(self, other: Any) -> bool:
        return not (self == other)

    def __hash__(self) -> int:
        return hash(self.as_tuple())

    def __ge__(self, other: Any) -> bool:
        other_ver = self._resolve(other)
        return self.as_tuple() >= other_ver.as_tuple()

    def __gt__(self, other: Any) -> bool:
        other_ver = self._resolve(other)
        return self.as_tuple() > other_ver.as_tuple()

    def __le__(self, other: Any) -> bool:
        other_ver = self._resolve(other)
        return self.as_tuple() <= other_ver.as_tuple()

    def __lt__(self, other: Any) -> bool:
        other_ver = self._resolve(other)
        return self.as_tuple() < other_ver.as_tuple()

    def __repr__(self) -> str:
        return f"VK_VERSION.{self.name}"

    def __str__(self) -> str:
        return self.name

    @classmethod
    def versions(cls) -> list['VK_VERSION']:
        """Returns all registered valid Vulkan API versions (major >= 1) sorted by version tuple."""
        return [v for v in sorted(cls._instances.values(), key=lambda x: x.as_tuple()) if v.major >= 1]

    # Compatibility aliases
    all_versions = versions
    core_versions = versions


# Singleton instances for standard core versions
VK_VERSION.NONE = VK_VERSION(-1, -1, "VK_VERSION_NONE")
VK_VERSION.V1_0 = VK_VERSION(1, 0, "VK_VERSION_1_0")
VK_VERSION.V1_1 = VK_VERSION(1, 1, "VK_VERSION_1_1")
VK_VERSION.V1_2 = VK_VERSION(1, 2, "VK_VERSION_1_2")
VK_VERSION.V1_3 = VK_VERSION(1, 3, "VK_VERSION_1_3")
VK_VERSION.V1_4 = VK_VERSION(1, 4, "VK_VERSION_1_4")


def get_bundle_structure_core_version(struct_name: str) -> VK_VERSION:
    """Dynamically determines the VK_VERSION where a bundle structure was introduced into core Vulkan."""
    if struct_name in ("VkPhysicalDeviceFeatures", "VkPhysicalDeviceProperties"):
        return VK_VERSION.V1_0

    match = re.match(r"^VkPhysicalDeviceVulkan(\d)(\d+)(Features|Properties)$", struct_name)
    if match:
        major = int(match.group(1))
        minor = int(match.group(2))
        intro_tuple = max((1, 2), (major, minor))
        return VK_VERSION.from_tuple(intro_tuple)

    return VK_VERSION.NONE


def is_bundle_structure(struct_name: str) -> bool:
    """Returns True if struct_name is a Vulkan core bundle structure."""
    return get_bundle_structure_core_version(struct_name) != VK_VERSION.NONE


class _BundleStructVersionsDict(dict):
    def __contains__(self, key: object) -> bool:
        if super().__contains__(key):
            return True
        return is_bundle_structure(str(key))

    def __getitem__(self, key: str) -> tuple[int, int]:
        if super().__contains__(key):
            return super().__getitem__(key)
        ver = get_bundle_structure_core_version(key)
        if ver != VK_VERSION.NONE:
            return ver.as_tuple()
        raise KeyError(key)

    def get(self, key: str, default: Any = None) -> Any:
        if key in self:
            return self[key]
        return default


BUNDLE_STRUCT_VERSIONS: dict[str, tuple[int, int]] = _BundleStructVersionsDict()


def get_feature_bundle_structures(api_version: VK_VERSION = None, vk: Any = None) -> list[str]:
    """Returns feature bundle structure names introduced in Vulkan 1.1+ up to api_version."""
    bundles = []
    if vk and hasattr(vk, 'structs'):
        for s_name in vk.structs.keys():
            if is_bundle_structure(s_name) and s_name.endswith("Features") and s_name != "VkPhysicalDeviceFeatures":
                bundles.append(s_name)
    else:
        max_ver = api_version if api_version else VK_VERSION.V1_4
        max_minor = max(4, max_ver.minor if max_ver.major == 1 else 4)
        for minor in range(1, max_minor + 1):
            bundles.append(f"VkPhysicalDeviceVulkan1{minor}Features")
        if max_ver.major > 1:
            for maj in range(2, max_ver.major + 1):
                for min_v in range(0, (max_ver.minor + 1) if maj == max_ver.major else 5):
                    bundles.append(f"VkPhysicalDeviceVulkan{maj}{min_v}Features")
    return sorted(list(set(bundles)), key=lambda s: get_bundle_structure_core_version(s).as_tuple())


def get_property_bundle_structures(api_version: VK_VERSION = None, vk: Any = None) -> list[str]:
    """Returns property bundle structure names introduced in Vulkan 1.1+ up to api_version."""
    bundles = []
    if vk and hasattr(vk, 'structs'):
        for s_name in vk.structs.keys():
            if is_bundle_structure(s_name) and s_name.endswith("Properties") and s_name != "VkPhysicalDeviceProperties":
                bundles.append(s_name)
    else:
        max_ver = api_version if api_version else VK_VERSION.V1_4
        max_minor = max(4, max_ver.minor if max_ver.major == 1 else 4)
        for minor in range(1, max_minor + 1):
            bundles.append(f"VkPhysicalDeviceVulkan1{minor}Properties")
        if max_ver.major > 1:
            for maj in range(2, max_ver.major + 1):
                for min_v in range(0, (max_ver.minor + 1) if maj == max_ver.major else 5):
                    bundles.append(f"VkPhysicalDeviceVulkan{maj}{min_v}Properties")
    return sorted(list(set(bundles)), key=lambda s: get_bundle_structure_core_version(s).as_tuple())


def get_active_feature_bundles(api_version: VK_VERSION, vk: Any = None) -> list[str]:
    """Returns active feature bundle structure names for the given API version."""
    return [
        b for b in get_feature_bundle_structures(api_version, vk)
        if api_version >= get_bundle_structure_core_version(b)
    ]


def get_active_property_bundles(api_version: VK_VERSION, vk: Any = None) -> list[str]:
    """Returns active property bundle structure names for the given API version."""
    return [
        b for b in get_property_bundle_structures(api_version, vk)
        if api_version >= get_bundle_structure_core_version(b)
    ]

