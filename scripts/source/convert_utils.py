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

from enum import IntEnum
from typing import Any

from source.vulkan_object_version import (
    is_bundle_structure, 
    get_active_feature_bundles,
    get_active_property_bundles
)
from source.vulkan_object_utils import (
    VulkanObject, 
    VK_VERSION, 
    gatherCapabilityAliases,
    getStructByName,
    getStructCoreVersion,
    is_extension_struct_name,
    is_struct_covered_by_bundle,
    is_property_struct_covered_by_bundle,
    StructCapabilityAlias
)


class VendorPriority(IntEnum):
    """
    Defines the precedence order for structure vendor authorities during sorting and deduplication (lower value = higher priority).
    """
    CORE = 0    # Standard Vulkan core structures. Example: 'VkPhysicalDeviceFeatures', 'VkPhysicalDeviceProperties'
    KHR = 1     # Khronos-ratified extension structures (*KHR). Example: 'VkPhysicalDeviceBufferDeviceAddressFeaturesKHR'
    EXT = 2     # Multivendor extension structures (*EXT). Example: 'VkPhysicalDeviceCustomBorderColorFeaturesEXT'
    VENDOR = 3  # Single-vendor extension structures (*NV, *AMD, *INTEL, etc.). Example: 'VkPhysicalDeviceMeshShaderFeaturesNV'


class CoreStructTier(IntEnum):
    """
    Categorizes core structures by structural group type when sorting capability blocks.
    """
    BUNDLE = 0  # Aggregated versioned bundle structures encompassing core capability sets. Example: 'VkPhysicalDeviceVulkan12Features', 'VkPhysicalDeviceVulkan13Properties'
    SPLIT = 1   # Individual split/fine-grained core structures. Example: 'VkPhysicalDeviceSamplerYcbcrConversionFeatures', 'VkPhysicalDeviceTimelineSemaphoreProperties'


class StructRank(IntEnum):
    """
    Ranks structure promotion status relative to the profile's target Vulkan API version (higher value = higher precedence).
    """
    FUTURE_CORE = 1        # Core structures introduced in a Vulkan version newer than the target API version. Example: 'VkPhysicalDeviceVulkan13Features' when targeting a Vulkan 1.2 profile
    EXTENSION_OR_ALIAS = 2 # Extension structures or structure aliases. Example: 'VkPhysicalDeviceBufferDeviceAddressFeaturesKHR'
    TARGET_CORE = 3        # Core structures introduced in or before the profile's target Vulkan version. Example: 'VkPhysicalDeviceVulkan11Features' when targeting a Vulkan 1.2 profile


def get_struct_rank(vk: VulkanObject, version: VK_VERSION, struct_name: str) -> int:
    if not hasattr(vk, '_struct_rank_cache'):
        vk._struct_rank_cache = {}

    cache_key = (version, struct_name)
    if cache_key in vk._struct_rank_cache:
        return vk._struct_rank_cache[cache_key]

    struct_obj = vk.structs.get(struct_name) or getStructByName(vk.structs, struct_name)

    is_alias = False
    if struct_obj and hasattr(struct_obj, 'name') and struct_name != struct_obj.name:
        is_alias = True

    core_ver = getStructCoreVersion(vk, struct_name)
    rank = StructRank.EXTENSION_OR_ALIAS
    if core_ver != VK_VERSION.NONE and not is_alias:
        if version != VK_VERSION.NONE and core_ver <= version:
            rank = StructRank.TARGET_CORE
        else:
            rank = StructRank.FUTURE_CORE

    vk._struct_rank_cache[cache_key] = rank
    return rank


def get_struct_tier(vk: VulkanObject, struct_name: str) -> int:
    if not is_extension_struct_name(vk, struct_name):
        return VendorPriority.CORE
    if struct_name.endswith("KHR"):
        return VendorPriority.KHR
    if struct_name.endswith("EXT"):
        return VendorPriority.EXT
    return VendorPriority.VENDOR


def should_remove_struct_a_in_favor_of_b(vk: VulkanObject, version: VK_VERSION, struct_a: str, struct_b: str) -> bool:
    rank_a = get_struct_rank(vk, version, struct_a)
    rank_b = get_struct_rank(vk, version, struct_b)

    if rank_a < rank_b:
        return True
    elif rank_a > rank_b:
        return False

    tier_a = get_struct_tier(vk, struct_a)
    tier_b = get_struct_tier(vk, struct_b)

    if tier_a > tier_b:
        return True
    elif tier_a < tier_b:
        return False

    return struct_a > struct_b


def canonicalize_capabilities_for_version(
    vk: VulkanObject, 
    api_version: VK_VERSION, 
    features_dict: dict[str, dict[str, bool]], 
    properties_dict: dict[str, Any]
) -> tuple[dict[str, dict[str, bool]], dict[str, Any]]:
    new_features: dict[str, dict[str, bool]] = {}
    new_properties: dict[str, Any] = {}

    active_feature_bundles = get_active_feature_bundles(api_version)
    active_property_bundles = get_active_property_bundles(api_version)

    for struct_name, members in features_dict.items():
        if not isinstance(members, dict):
            continue

        is_covered = False
        if active_feature_bundles and not is_bundle_structure(struct_name):
            for bundle in active_feature_bundles:
                if is_struct_covered_by_bundle(vk, bundle, struct_name):
                    is_covered = True
                    for member_name, val in members.items():
                        if val:
                            new_features.setdefault(bundle, {})[member_name] = True
                    break

        if not is_covered:
            for member_name, val in members.items():
                if val:
                    new_features.setdefault(struct_name, {})[member_name] = True

    for struct_name, prop_data in properties_dict.items():
        is_covered = False
        if active_property_bundles and not is_bundle_structure(struct_name):
            for bundle in active_property_bundles:
                if is_property_struct_covered_by_bundle(vk, bundle, struct_name):
                    is_covered = True
                    break

        if not is_covered:
            new_properties[struct_name] = prop_data

    return new_features, new_properties


def get_parent_property_value(parent_props_dict: dict, struct_name: str, prop_name: str, vk: VulkanObject) -> tuple[bool, Any]:
    query_id = StructCapabilityAlias(struct_name, prop_name)
    aliases = [query_id] + gatherCapabilityAliases(vk, query_id)

    for alias in aliases:
        if not isinstance(alias, StructCapabilityAlias):
            continue
        a_struct = alias.struct
        a_member = alias.member

        if a_struct in parent_props_dict:
            s_data = parent_props_dict[a_struct]
            if isinstance(s_data, dict):
                if a_struct == "VkPhysicalDeviceProperties":
                    if "limits" in s_data and isinstance(s_data["limits"], dict) and a_member in s_data["limits"]:
                        return (True, s_data["limits"][a_member])
                    if "sparseProperties" in s_data and isinstance(s_data["sparseProperties"], dict) and a_member in s_data["sparseProperties"]:
                        return (True, s_data["sparseProperties"][a_member])
                else:
                    if a_member in s_data:
                        return (True, s_data[a_member])

    return (False, None)