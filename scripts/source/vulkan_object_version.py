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

from vulkan_object import VulkanObject
from enum import Enum
    
class VK_VERSION(Enum):
    NONE = "VK_NONE"
    V1_0 = "VK_VERSION_1_0"
    V1_1 = "VK_VERSION_1_1"
    V1_2 = "VK_VERSION_1_2"
    V1_3 = "VK_VERSION_1_3"
    V1_4 = "VK_VERSION_1_4"

    @classmethod
    def from_string(cls, version_str: str) -> "VK_VERSION":
        # 1. Split the version string (e.g., "1.3.276" -> ["1", "3", "276"])
        parts = version_str.split('.')
        
        # 2. Extract major and minor, ignoring the revision
        major, minor = parts[0], parts[1]
        
        # 3. Construct the matching Enum value string (e.g., "VK_VERSION_1_3")
        target_value = f"VK_VERSION_{major}_{minor}"
        
        return cls(target_value)

# # A dummy placeholder
# class VK_VERSION(Enum):
#     NONE = "VK_NONE"

# def buildVulkanVersionEnum2(vk: VulkanObject):
#     # (Note: base_generator.py explicitly skips '1.0' in parsing, so we hardcode it as the floor)
#     enum_members = {
#         "NONE": "VK_NONE",
#         "V1_0": "VK_VERSION_1_0"
#     }
    
#     # vk.versions keys look like: "VK_VERSION_1_1", "VK_VERSION_1_2", etc.
#     for version_name in sorted(vk.versions.keys()):
#         # Transform "VK_VERSION_1_1" -> "V1_1"
#         parts = version_name.split('_')
#         if len(parts) >= 4 and parts[1] == 'VERSION':
#             enum_key = f"V{parts[2]}_{parts[3]}"
#             enum_members[enum_key] = version_name

#     @classmethod
#     def from_string(cls, version_str: str) -> "VK_VERSION":
#         if not version_str or '.' not in version_str:
#             return cls.NONE
        
#         parts = version_str.split('.')
#         major, minor = parts[0], parts[1]
#         target_value = f"VK_VERSION_{major}_{minor}"
        
#         try:
#             return cls(target_value)
#         except ValueError:
#             # Fallback if a weird or unparsed version string is passed
#             return cls.NONE

#     DynamicEnum = Enum("VK_VERSION", enum_members)
    
#     DynamicEnum.from_string = from_string
    
#     return DynamicEnum

# def buildVulkanVersionEnum(vk: VulkanObject):
# # Base members
#     raw_versions = ["VK_NONE", "VK_VERSION_1_0"]
    
#     # Gather other versions dynamically parsed from vk.xml
#     for version_name in vk.versions.keys():
#         if version_name not in raw_versions:
#             raw_versions.append(version_name)
            
#     # Parse version strings into integers for pristine semantic sorting
#     def sem_ver_key(name):
#         if name == "VK_NONE":
#             return (-1, -1)
#         parts = name.split('_')
#         if len(parts) >= 4 and parts[1] == 'VERSION':
#             return (int(parts[2]), int(parts[3]))
#         return (0, 0)
        
#     # Sort them natively by actual version numbers
#     sorted_versions = sorted(raw_versions, key=sem_ver_key)
    
#     enum_members = {}
#     for version_name in sorted_versions:
#         if version_name == "VK_NONE":
#             enum_members["NONE"] = version_name
#         else:
#             parts = version_name.split('_')
#             enum_key = f"V{parts[2]}_{parts[3]}"
#             enum_members[enum_key] = version_name

#     # --- Custom Methods ---
#     @classmethod
#     def from_string(cls, version_str: str) -> "VK_VERSION":
#         if not version_str or '.' not in version_str:
#             return cls.NONE
#         parts = version_str.split('.')
#         return cls(f"VK_VERSION_{parts[0]}_{parts[1]}")

#     # Converts "VK_VERSION_1_3" -> (1, 3) for clean comparison operations
#     def as_tuple(self):
#         if self.value == "VK_NONE":
#             return (-1, -1)
#         parts = self.value.split('_')
#         return (int(parts[2]), int(parts[3]))

#     # Build the Enum Class
#     DynamicEnum = Enum("VK_VERSION", enum_members)
#     DynamicEnum.from_string = from_string
#     DynamicEnum.as_tuple = as_tuple  # Attach the tuple method
    
#     return DynamicEnum