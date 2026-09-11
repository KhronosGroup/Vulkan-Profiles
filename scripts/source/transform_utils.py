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

from enum import Enum, IntEnum
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
from source.profiles_json_utils import (
    deep_merge_dict,
    is_property_satisfied
)


class TransformBits(str, Enum):
    PULL_REQUIRED_CAPABILITIES = 'pull-required-capabilities'  # Evaluates extension dependencies and pulls satisfied core/extension feature and property requirements into capability blocks.
    PULL_PROMOTED_EXTENSIONS = 'pull-promoted-extensions'      # Requires all extensions promoted to core up to the profile's target Vulkan version.
    IGNORE_EXTENSION_VERSIONS = 'ignore-extension-versions'    # Sets all required extension versions to 1, overriding specific extension spec versions.
    PULL_ALIASES = 'pull-aliases'                              # Resolves and populates all equivalent capability aliases across core structures and extensions.
    CONSOLIDATE = 'consolidate'                                # Merges all mandatory capability blocks into a single consolidated requirements block per profile.
    STRIP_HELPER_VALUES = 'strip-helper-values'                # Removes non-bitpos bitmask helper values (composites, all-flags, and zero/none constants) from capability blocks.
    STRIP_DUPLICATION = 'strip-duplication'                    # Removes redundant duplicate features, properties, and extension requirements across inheritance trees and within blocks.
    STRIP_PROMOTED_EXTENSIONS = 'strip-promoted-extensions'    # Removes extensions that are already promoted to the profile's target core Vulkan version.
    SORT = 'sort'                                              # Sorts capability blocks, structures, and extension lists into canonical Vulkan order.


class CategoryPriority(IntEnum):
    """Defines structure category sorting precedence during capability block sorting (lower value = higher priority)."""
    CORE = 0    # Standard Vulkan core structures (version bundle and split core).
    KHR = 1     # Khronos-ratified extension structures (*KHR).
    EXT = 2     # Multivendor extension structures (*EXT).
    VENDOR = 3  # Single-vendor extension structures (*NV, *AMD, *QCOM, *ARM, etc.).


class CoreStructTier(IntEnum):
    """Categorizes core structures by structural group type when sorting capability blocks."""
    BUNDLE = 0  # Aggregated versioned bundle structures (e.g., 'VkPhysicalDeviceFeatures', 'VkPhysicalDeviceVulkan12Features').
    SPLIT = 1   # Fine-grained split core structures (e.g., 'VkPhysicalDeviceMultiviewFeatures').


class ExtensionPriority(IntEnum):
    """Priority order for tie-breaking independent extensions when sorting extension lists."""
    KHR = 0     # Khronos-ratified extensions (VK_KHR_*).
    EXT = 1     # Multivendor extensions (VK_EXT_*).
    VENDOR = 2  # Single-vendor extensions (VK_VENDOR_*).


class MemberSortFallback(IntEnum):
    """Fallback index position for structure members not found in C struct definitions."""
    UNKNOWN_MEMBER_INDEX = 9999  # Members missing from the vk.xml struct definition are sorted to the end of the dictionary.


BUNDLE_FEATURE_ORDER = [
    'VkPhysicalDeviceFeatures',
    'VkPhysicalDeviceVulkan11Features',
    'VkPhysicalDeviceVulkan12Features',
    'VkPhysicalDeviceVulkan13Features',
    'VkPhysicalDeviceVulkan14Features'
]

BUNDLE_PROPERTY_ORDER = [
    'VkPhysicalDeviceProperties',
    'VkPhysicalDeviceVulkan11Properties',
    'VkPhysicalDeviceVulkan12Properties',
    'VkPhysicalDeviceVulkan13Properties',
    'VkPhysicalDeviceVulkan14Properties'
]


def _restore_member_orders(target_dict: dict[str, dict], original_orders: dict[str, list[str]]):
    """Restores pre-existing member key insertion sequence for non-bundle structures."""
    for struct_name, members in target_dict.items():
        if isinstance(members, dict) and struct_name in original_orders:
            orig_order = original_orders[struct_name]
            reordered = {}
            for m_name in orig_order:
                if m_name in members:
                    reordered[m_name] = members[m_name]
            for m_name, val in members.items():
                if m_name not in reordered:
                    reordered[m_name] = val
            target_dict[struct_name] = reordered


def canonicalize_capabilities_for_version(
    vk: VulkanObject, 
    api_version: VK_VERSION, 
    features_dict: dict[str, dict[str, bool]], 
    properties_dict: dict[str, Any]
) -> tuple[dict[str, dict[str, bool]], dict[str, Any]]:
    """
    Remaps split structure capabilities into active bundle structures for api_version
    and removes redundant split structures covered by active bundle structures.
    Preserves original member key insertion order from input JSON files.
    Ensures version bundle structures are ordered strictly by Vulkan version.
    """
    new_features: dict[str, dict[str, bool]] = {}
    new_properties: dict[str, Any] = {}

    active_feature_bundles = get_active_feature_bundles(api_version)
    active_property_bundles = get_active_property_bundles(api_version)

    orig_feature_orders = {s: list(m.keys()) for s, m in features_dict.items() if isinstance(m, dict)}
    orig_property_orders = {s: list(p.keys()) for s, p in properties_dict.items() if isinstance(p, dict)}

    # 1. Features remapping and deduplication
    sorted_feature_structs = sorted(
        features_dict.keys(),
        key=lambda s: (0 if is_bundle_structure(s) else 1)
    )

    for struct_name in sorted_feature_structs:
        members = features_dict[struct_name]
        if not isinstance(members, dict):
            continue

        is_covered = False
        if active_feature_bundles and not is_bundle_structure(struct_name):
            for bundle in active_feature_bundles:
                if is_struct_covered_by_bundle(vk, bundle, struct_name):
                    is_covered = True
                    for member_name, val in members.items():
                        if val and member_name not in new_features.setdefault(bundle, {}):
                            new_features[bundle][member_name] = True
                    break

        if not is_covered:
            for member_name, val in members.items():
                new_features.setdefault(struct_name, {})[member_name] = val

    # Order bundle feature structures strictly by Vulkan version (1.0 -> 1.4)
    ordered_new_features = {}
    for b_name in BUNDLE_FEATURE_ORDER:
        if b_name in new_features:
            ordered_new_features[b_name] = new_features[b_name]
    for s_name, members in new_features.items():
        if s_name not in ordered_new_features:
            ordered_new_features[s_name] = members
    new_features = ordered_new_features

    _restore_member_orders(new_features, orig_feature_orders)

    # 2. Properties remapping and deduplication
    sorted_property_structs = sorted(
        properties_dict.keys(),
        key=lambda s: (0 if is_bundle_structure(s) else 1)
    )

    for struct_name in sorted_property_structs:
        prop_data = properties_dict[struct_name]
        is_covered = False
        if active_property_bundles and not is_bundle_structure(struct_name):
            for bundle in active_property_bundles:
                if is_property_struct_covered_by_bundle(vk, bundle, struct_name):
                    is_covered = True
                    break

        if not is_covered:
            new_properties[struct_name] = prop_data

    # Order bundle property structures strictly by Vulkan version (1.0 -> 1.4)
    ordered_new_properties = {}
    for b_name in BUNDLE_PROPERTY_ORDER:
        if b_name in new_properties:
            ordered_new_properties[b_name] = new_properties[b_name]
    for s_name, prop_data in new_properties.items():
        if s_name not in ordered_new_properties:
            ordered_new_properties[s_name] = prop_data
    new_properties = ordered_new_properties

    _restore_member_orders(new_properties, orig_property_orders)

    return new_features, new_properties


def get_parent_property_value(parent_props_dict: dict, struct_name: str, prop_name: str, vk: VulkanObject) -> tuple[bool, Any]:
    """
    Looks up parent_props_dict for struct_name::prop_name or any of its capability aliases.
    Returns (True, parent_val) if found, or (False, None) if not found in parent.
    """
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


def isStructExtensionEnabled(vk: VulkanObject, struct_name: str, version: VK_VERSION, enabled_exts: set[str]) -> bool:
    """
    Returns True if struct_name is enabled for the given version and enabled extensions.
    - Core structures (no extension suffix) are enabled if core version >= struct core version.
    - Extension structures (with KHR/EXT/vendor suffix) require the defining extension to be in enabled_exts.
    """
    is_ext_struct = is_extension_struct_name(vk, struct_name)

    if not is_ext_struct:
        core_ver = getStructCoreVersion(vk, struct_name)
        if core_ver != VK_VERSION.NONE and version != VK_VERSION.NONE and version >= core_ver:
            return True

    req_keys = set()
    struct_obj = vk.structs.get(struct_name) or getStructByName(vk.structs, struct_name)
    if struct_obj:
        if hasattr(struct_obj, 'definingRequirements') and struct_obj.definingRequirements:
            req_keys.update(struct_obj.definingRequirements.keys())
        elif hasattr(struct_obj, 'extensions') and struct_obj.extensions:
            req_keys.update(struct_obj.extensions)

    if hasattr(vk, 'aliasTypeRequirements') and struct_name in vk.aliasTypeRequirements:
        req_keys.update(vk.aliasTypeRequirements[struct_name].keys())

    if struct_obj and hasattr(struct_obj, 'aliases'):
        for alias in struct_obj.aliases:
            if hasattr(vk, 'aliasTypeRequirements') and alias in vk.aliasTypeRequirements:
                req_keys.update(vk.aliasTypeRequirements[alias].keys())

    if not req_keys:
        return not is_ext_struct

    for req in req_keys:
        if req in enabled_exts:
            return True

        if not is_ext_struct and (req.startswith("VK_VERSION_") or req.startswith("VK_API_VERSION_")):
            ver = VK_VERSION.from_string(req)
            if ver != VK_VERSION.NONE and version != VK_VERSION.NONE and version >= ver:
                return True

    return False


def filter_features_against_context(
    vk: VulkanObject,
    features_dict: dict[str, dict[str, bool]],
    context_features: set[tuple[str, str]]
) -> dict[str, dict[str, bool]]:
    """Filters candidate feature requirements against accumulated context features (including structural aliases)."""
    filtered_features = {}
    for s_name, members in features_dict.items():
        if not isinstance(members, dict):
            continue
        new_members = {}
        for m_name, val in members.items():
            if not val:
                continue

            query_id = StructCapabilityAlias(s_name, m_name)
            aliases = [query_id] + gatherCapabilityAliases(vk, query_id)

            is_in_context = False
            for alias in aliases:
                if isinstance(alias, StructCapabilityAlias):
                    if (alias.struct, alias.member) in context_features:
                        is_in_context = True
                        break

            if not is_in_context:
                new_members[m_name] = val

        if new_members:
            filtered_features[s_name] = new_members
    return filtered_features


def filter_properties_against_context(
    vk: VulkanObject,
    properties_dict: dict[str, Any],
    context_properties: dict[str, Any]
) -> dict[str, Any]:
    """Filters candidate property requirements against accumulated context properties."""
    filtered_properties = {}
    for s_name, p_data in properties_dict.items():
        if not isinstance(p_data, dict):
            continue

        if s_name == "VkPhysicalDeviceProperties":
            new_s_data = {}
            for sub_group_name, sub_dict in p_data.items():
                if not isinstance(sub_dict, dict):
                    continue
                new_sub_dict = {}
                for prop_name, prop_val in sub_dict.items():
                    found_in_ctx, ctx_val = get_parent_property_value(
                        context_properties, s_name, prop_name, vk
                    )
                    if not found_in_ctx or not is_property_satisfied(ctx_val, prop_val, prop_name):
                        new_sub_dict[prop_name] = prop_val
                if new_sub_dict:
                    new_s_data[sub_group_name] = new_sub_dict
            if new_s_data:
                filtered_properties[s_name] = new_s_data
        else:
            new_p_data = {}
            for prop_name, prop_val in p_data.items():
                found_in_ctx, ctx_val = get_parent_property_value(
                    context_properties, s_name, prop_name, vk
                )
                if not found_in_ctx or not is_property_satisfied(ctx_val, prop_val, prop_name):
                    new_p_data[prop_name] = prop_val
            if new_p_data:
                filtered_properties[s_name] = new_p_data
    return filtered_properties


def update_context_from_capabilities(
    caps_dict: dict,
    context_features: set[tuple[str, str]],
    context_properties: dict[str, Any],
    context_extensions: set[str]
):
    """Updates accumulated capabilities context with entries from caps_dict."""
    if "extensions" in caps_dict and isinstance(caps_dict["extensions"], dict):
        context_extensions.update(caps_dict["extensions"].keys())

    if "features" in caps_dict and isinstance(caps_dict["features"], dict):
        for s_name, members in caps_dict["features"].items():
            if isinstance(members, dict):
                for m_name, val in members.items():
                    if val:
                        context_features.add((s_name, m_name))

    if "properties" in caps_dict and isinstance(caps_dict["properties"], dict):
        deep_merge_dict(context_properties, caps_dict["properties"])
        