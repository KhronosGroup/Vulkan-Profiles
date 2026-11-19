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

from source.vulkan_object_utils import (
    VulkanObject,
    VK_VERSION,
    evaluateFeatureDepends,
    gatherDependentExtensions,
    gatherSatisfiedCoreRequiredFeaturesForVersion,
    gatherSatisfiedCoreRequiredPropertiesForVersion,
    gatherSatisfiedExtensionRequiredFeatures,
    gatherSatisfiedExtensionRequiredProperties,
    get_member_unsupported_default,
    getMemberByName,
    getStructByName,
    parse_property_value
)
from source.vulkan_object_expression_parsing import tokenizeDependsExpression
from source.profiles_json_utils import (
    collect_block_names,
    get_block_depends,
    get_depends_implied_capabilities,
    parse_profile_capabilities,
    collect_profile_capabilities,
    get_primary_capability_block,
    get_topologically_sorted_file_keys,
    collect_required_profiles_capabilities_recursive,
    deep_merge_dict
)
from source.transform_utils import (
    PullBits,
    canonicalize_capabilities_for_version,
    filter_features_against_context,
    filter_properties_against_context,
    get_parent_feature_value,
    get_parent_property_value,
    update_context_from_capabilities
)


def find_profile_in_files_dict(json_files_dict: dict, profile_name: str) -> tuple[dict | None, dict | None]:
    """Helper to locate profile_obj and its file_data across all loaded file keys."""
    if not json_files_dict:
        return None, None
    for file_key, file_data in json_files_dict.items():
        if isinstance(file_data, dict) and "profiles" in file_data:
            if profile_name in file_data["profiles"]:
                return file_data["profiles"][profile_name], file_data
    return None, None


def get_implementation_dependent_members(vk: VulkanObject) -> set[tuple[str, str]]:
    """
    Returns the (struct, member) of the property requirements defined with a None value in
    vulkan_object_data.py, across all Vulkan versions and extensions. A None value means the
    Vulkan specification defines no required value: the member is implementation-dependent.
    Struct names are canonicalized so that alias structures match their requirements.
    """
    if not hasattr(vk, '_implementation_dependent_members'):
        members = set()
        for objs in (vk.versions, vk.extensions):
            for obj in objs.values():
                for req in getattr(obj, 'propertyRequirement', None) or []:
                    if req.value is None:
                        struct_obj = vk.structs.get(req.struct) or getStructByName(vk.structs, req.struct)
                        members.add((struct_obj.name if struct_obj else req.struct, req.name))
        vk._implementation_dependent_members = members
    return vk._implementation_dependent_members


def is_member_default_applicable(vk: VulkanObject, struct_obj, member) -> bool:
    """
    Returns False for members that cannot receive a default value: 'exact' members must match
    a device specific value (e.g. deviceUUID, driverID, pointClippingBehavior), 'noauto'
    members are too complex for a simple resolution and are not handled yet, and
    implementation-dependent members (e.g. timestampPeriod) have no required value to default to.
    """
    tokens = [t.strip() for t in (getattr(member, 'limitType', None) or '').split(',')]
    if 'exact' in tokens or 'noauto' in tokens:
        return False
    return (struct_obj.name, member.name) not in get_implementation_dependent_members(vk)


def populate_missing_structure_defaults(
    vk: VulkanObject,
    features_dict: dict[str, dict[str, bool]],
    properties_dict: dict[str, Any],
    ignore_unsupported: bool,
    context_features: dict[str, dict[str, bool]] | None = None,
    context_properties: dict[str, Any] | None = None
):
    """
    When ignore_unsupported=False, populates all missing/unlisted members of present
    feature and property structures with spec-accurate default fallback values.
    A member is not populated when it is already defined, directly or through a capability
    alias (e.g. VkPhysicalDeviceMultiviewFeatures::multiview for
    VkPhysicalDeviceVulkan11Features::multiview), in the context capabilities or in the other
    structures of features_dict and properties_dict.
    """
    if ignore_unsupported:
        return

    # Snapshot the defined capabilities before populating so that populated defaults are not used as context
    defined_features: dict = {}
    deep_merge_dict(defined_features, context_features or {})
    deep_merge_dict(defined_features, features_dict)
    defined_properties: dict = {}
    deep_merge_dict(defined_properties, context_properties or {})
    deep_merge_dict(defined_properties, properties_dict)

    def is_feature_defined(struct_name: str, member_name: str) -> bool:
        return get_parent_feature_value(defined_features, struct_name, member_name, vk)[0]

    def is_property_defined(struct_name: str, member_name: str) -> bool:
        return get_parent_property_value(defined_properties, struct_name, member_name, vk)[0]

    # 1. Populate missing feature structure members
    for struct_name, members in features_dict.items():
        if not isinstance(members, dict):
            continue
        struct_obj = vk.structs.get(struct_name) or getStructByName(vk.structs, struct_name)
        if not struct_obj:
            continue
        for m in getattr(struct_obj, 'members', []):
            if m.name in ('sType', 'pNext') or m.name in members:
                continue
            if is_member_default_applicable(vk, struct_obj, m) and not is_feature_defined(struct_name, m.name):
                members[m.name] = get_member_unsupported_default(vk, struct_name, m.name)

    # 2. Populate missing property structure members
    for struct_name, prop_data in properties_dict.items():
        if not isinstance(prop_data, dict):
            continue
        if struct_name == "VkPhysicalDeviceProperties":
            limits = prop_data.setdefault("limits", {})
            sparse = prop_data.setdefault("sparseProperties", {})

            limits_obj = vk.structs.get("VkPhysicalDeviceLimits") or getStructByName(vk.structs, "VkPhysicalDeviceLimits")
            if limits_obj:
                for m in getattr(limits_obj, 'members', []):
                    if m.name in ('sType', 'pNext') or m.name in limits:
                        continue
                    if is_member_default_applicable(vk, limits_obj, m) and not is_property_defined(struct_name, m.name):
                        limits[m.name] = get_member_unsupported_default(vk, "VkPhysicalDeviceLimits", m.name)

            sparse_obj = vk.structs.get("VkPhysicalDeviceSparseProperties") or getStructByName(vk.structs, "VkPhysicalDeviceSparseProperties")
            if sparse_obj:
                for m in getattr(sparse_obj, 'members', []):
                    if m.name in ('sType', 'pNext') or m.name in sparse:
                        continue
                    if is_member_default_applicable(vk, sparse_obj, m) and not is_property_defined(struct_name, m.name):
                        sparse[m.name] = get_member_unsupported_default(vk, "VkPhysicalDeviceSparseProperties", m.name)
        else:
            struct_obj = vk.structs.get(struct_name) or getStructByName(vk.structs, struct_name)
            if not struct_obj:
                continue
            for m in getattr(struct_obj, 'members', []):
                if m.name in ('sType', 'pNext') or m.name in prop_data:
                    continue
                if is_member_default_applicable(vk, struct_obj, m) and not is_property_defined(struct_name, m.name):
                    prop_data[m.name] = get_member_unsupported_default(vk, struct_name, m.name)


def pull_extension_dependencies_capabilities_block(
    vk: VulkanObject, 
    version: VK_VERSION, 
    pull_bits: list[PullBits], 
    json_profiles_capabilities_block: dict,
    context_extensions: set[str] = None,
    context_features: dict[str, dict[str, bool]] | set[tuple[str, str]] = None,
    context_properties: dict[str, Any] = None,
    profile_name: str = ""
):
    if "extensions" not in json_profiles_capabilities_block:
        return

    ignore_extension_versions = PullBits.IGNORE_EXTENSION_VERSIONS in pull_bits
    ignore_subcore = PullBits.IGNORE_SUBCORE in pull_bits
    ignore_unsupported = PullBits.IGNORE_UNSUPPORTED in pull_bits
    ignore_unratified = PullBits.IGNORE_UNRATIFIED in pull_bits

    context_extensions = context_extensions if context_extensions is not None else set()
    context_properties = context_properties if context_properties is not None else {}

    curr_exts = dict(json_profiles_capabilities_block["extensions"])
    if ignore_unratified:
        curr_exts = {ext: ver for ext, ver in curr_exts.items() if ext in vk.extensions and vk.extensions[ext].ratified}

    while True:
        raw_deps = gatherDependentExtensions(
            vk, version, ignore_extension_versions, curr_exts, profile_name
        )
        if ignore_unratified:
            raw_deps = {ext: ver for ext, ver in raw_deps.items() if ext in vk.extensions and vk.extensions[ext].ratified}
        if len(raw_deps) == len(curr_exts):
            break
        curr_exts = raw_deps

    block_exts = json_profiles_capabilities_block["extensions"]
    original_extensions = set(block_exts.keys()) if isinstance(block_exts, dict) else set(block_exts)
    original_order = list(block_exts.keys()) if isinstance(block_exts, dict) else list(block_exts)

    filtered_deps = {}

    for ext_name in original_order:
        if ext_name in raw_deps:
            if ext_name in original_extensions or ext_name not in context_extensions:
                filtered_deps[ext_name] = 1 if ignore_extension_versions else raw_deps[ext_name]

    for ext_name, ext_ver in raw_deps.items():
        if ext_name not in filtered_deps:
            if ext_name in original_extensions or ext_name not in context_extensions:
                filtered_deps[ext_name] = 1 if ignore_extension_versions else ext_ver

    json_profiles_capabilities_block["extensions"] = filtered_deps

    profile_enabled_exts = context_extensions | set(filtered_deps.keys())
    if ignore_unratified:
        profile_enabled_exts = {e for e in profile_enabled_exts if e in vk.extensions and vk.extensions[e].ratified}

    # Normalize context_features safely regardless of whether caller passed a dict or a set
    block_features = json_profiles_capabilities_block.get("features", {})
    combined_context_features = {}
    if isinstance(context_features, dict):
        deep_merge_dict(combined_context_features, context_features)
    elif isinstance(context_features, set):
        for s_name, m_name in context_features:
            combined_context_features.setdefault(s_name, {})[m_name] = True

    if isinstance(block_features, dict):
        deep_merge_dict(combined_context_features, block_features)

    enabled_features = set()
    for s_name, members in combined_context_features.items():
        if isinstance(members, dict):
            for m_name, val in members.items():
                if val:
                    enabled_features.add((s_name, m_name))

    for ext_name in filtered_deps.keys():
        if ignore_unratified and ext_name in vk.extensions and not vk.extensions[ext_name].ratified:
            continue
        ext_satisfied, _ = gatherSatisfiedExtensionRequiredFeatures(
            vk, ext_name, version, profile_enabled_exts, enabled_features
        )
        if ext_satisfied:
            filtered_ext_satisfied = filter_features_against_context(
                vk, ext_satisfied, combined_context_features, ignore_subcore
            )
            if filtered_ext_satisfied:
                features_dict = json_profiles_capabilities_block.setdefault("features", {})
                deep_merge_dict(features_dict, filtered_ext_satisfied)
                deep_merge_dict(combined_context_features, filtered_ext_satisfied)
                for s_name, members in filtered_ext_satisfied.items():
                    if isinstance(members, dict):
                        for m_name, val in members.items():
                            if val:
                                enabled_features.add((s_name, m_name))

        ext_satisfied_props = gatherSatisfiedExtensionRequiredProperties(
            vk, ext_name, version, profile_enabled_exts, enabled_features, ignore_unsupported=ignore_unsupported
        )
        if ext_satisfied_props:
            filtered_ext_satisfied_props = filter_properties_against_context(
                vk, ext_satisfied_props, context_properties, ignore_subcore
            )
            if filtered_ext_satisfied_props:
                properties_dict = json_profiles_capabilities_block.setdefault("properties", {})
                deep_merge_dict(properties_dict, filtered_ext_satisfied_props)


def pull_extension_dependencies_profiles_file(
    vk: VulkanObject, 
    pull_bits: list[PullBits], 
    json_file_data: dict, 
    json_files_dict: dict = None
):
    profiles_data = json_file_data.get("profiles", {})
    json_profiles_capabilities = json_file_data.get("capabilities", {})

    for profile_key, profile_obj in profiles_data.items():
        api_version = VK_VERSION.from_string(profile_obj.get("api-version", "1.0.0"))

        context_extensions = set()
        context_features = {}
        context_properties = {}
        parent_profiles = profile_obj.get("profiles", [])
        if json_files_dict and parent_profiles:
            parent_caps = collect_required_profiles_capabilities_recursive(json_files_dict, parent_profiles)
            update_context_from_capabilities(parent_caps, context_features, context_properties, context_extensions)

        block_names = collect_block_names(profile_obj.get("capabilities", []))
        for block_name in block_names:
            if block_name in json_profiles_capabilities:
                block = json_profiles_capabilities[block_name]
                block_context_extensions = context_extensions
                block_context_features = context_features
                block_depends = get_block_depends(block)
                if block_depends is not None:
                    # The extensions and features required by the 'depends' condition are enabled when the
                    # capability block requirements apply
                    implied_exts, implied_features = get_depends_implied_capabilities(block_depends)
                    block_context_extensions = context_extensions | implied_exts
                    block_context_features = {}
                    deep_merge_dict(block_context_features, context_features)
                    for s_name, m_name in implied_features:
                        block_context_features.setdefault(s_name, {})[m_name] = True
                pull_extension_dependencies_capabilities_block(
                    vk, api_version, pull_bits, block, block_context_extensions, block_context_features, context_properties, profile_key
                )
                update_context_from_capabilities(block, context_features, context_properties, context_extensions)


def pull_extension_dependencies_profiles_files(
    vk: VulkanObject, 
    pull_bits: list[PullBits], 
    json_files_dict: dict
):
    if not isinstance(json_files_dict, dict):
        return

    if "profiles" in json_files_dict or "capabilities" in json_files_dict:
        pull_extension_dependencies_profiles_file(
            vk, pull_bits, json_files_dict, None
        )
        return

    sorted_file_keys = get_topologically_sorted_file_keys(json_files_dict)
    for file_key in sorted_file_keys:
        json_file_data = json_files_dict[file_key]
        if isinstance(json_file_data, dict):
            pull_extension_dependencies_profiles_file(
                vk, pull_bits, json_file_data, json_files_dict
            )


def process_or_disjunction(
    vk: VulkanObject,
    profile_obj: dict,
    capabilities_dict: dict,
    block_name: str,
    disjunction: dict
):
    struct_name = disjunction["struct"]
    fields = disjunction["fields"]
    ext_name = disjunction.get("ext")

    parent_block = capabilities_dict.get(block_name, {})
    ext_ver_val = 1

    if ext_name and "extensions" in parent_block and isinstance(parent_block["extensions"], dict):
        if ext_name in parent_block["extensions"]:
            ext_ver_val = parent_block["extensions"].pop(ext_name)
            if not parent_block["extensions"]:
                del parent_block["extensions"]

    parent_depends = get_block_depends(parent_block)

    variant_block_names = []
    for field_name in fields:
        var_block_name = f"{block_name}_{field_name}"
        variant_block_names.append(var_block_name)

        var_block = capabilities_dict.setdefault(var_block_name, {})

        # The variants of a capability block with a 'depends' condition have the same condition
        if parent_depends is not None:
            var_block["depends"] = parent_depends

        if ext_name:
            var_exts = var_block.setdefault("extensions", {})
            var_exts[ext_name] = ext_ver_val

        var_feats = var_block.setdefault("features", {})
        var_struct = var_feats.setdefault(struct_name, {})
        var_struct[field_name] = True

    profile_caps = profile_obj.setdefault("capabilities", [])
    if variant_block_names not in profile_caps:
        if block_name in profile_caps:
            idx = profile_caps.index(block_name)
            profile_caps.insert(idx + 1, variant_block_names)
        else:
            profile_caps.append(variant_block_names)


def canonicalize_depends(vk: VulkanObject, api_version: VK_VERSION, depends: str) -> str:
    """
    Returns the 'depends' condition with its feature symbols named after the structures used for api_version,
    e.g. 'VkPhysicalDeviceProtectedMemoryFeatures::protectedMemory' is 'VkPhysicalDeviceVulkan11Features::protectedMemory'
    for Vulkan 1.1 and later, so that equivalent conditions are written the same way.
    """
    gen = ''
    for token in tokenizeDependsExpression(depends):
        symbol = token[1:].strip() if token.startswith('!') else token
        if '::' not in symbol:
            gen += token
            continue
        struct_name, member_name = symbol.split('::', 1)
        canonical_features, _ = canonicalize_capabilities_for_version(vk, api_version, {struct_name: {member_name: True}}, {})
        canonical_struct = next((s_name for s_name, members in canonical_features.items() if member_name in members), struct_name)
        gen += ('!' if token.startswith('!') else '') + f"{canonical_struct}::{member_name}"
    return gen


def get_depends_block_suffix(depends: str) -> str:
    """Returns a capability block name suffix describing a 'depends' condition, e.g. 'wideLines' or 'VK_EXT_descriptor_indexing_or_descriptorIndexing'."""
    parts = []
    for token in tokenizeDependsExpression(depends):
        if token == '+':
            parts.append('and')
        elif token == ',':
            parts.append('or')
        elif token not in ('(', ')'):
            parts.append(token.split('::')[-1])
    return re.sub(r'[^A-Za-z0-9_]', '', '_'.join(parts))


def get_capability_key(vk: VulkanObject, api_version: VK_VERSION, struct_name: str, member_name: str, category: str) -> tuple[str, str]:
    """
    Returns the (structure, member) identifying a capability whatever the equivalent structure defining it, e.g.
    VkPhysicalDeviceProtectedMemoryFeatures::protectedMemory is VkPhysicalDeviceVulkan11Features::protectedMemory
    for Vulkan 1.1 and later. The VkPhysicalDeviceProperties limits and sparse properties are identified by
    VkPhysicalDeviceLimits and VkPhysicalDeviceSparseProperties.
    """
    if struct_name in ('VkPhysicalDeviceLimits', 'VkPhysicalDeviceSparseProperties'):
        return (struct_name, member_name)
    if category == 'features':
        canonical, _ = canonicalize_capabilities_for_version(vk, api_version, {struct_name: {member_name: True}}, {})
    else:
        _, canonical = canonicalize_capabilities_for_version(vk, api_version, {}, {struct_name: {member_name: 0}})
    canonical_struct = next((s_name for s_name, members in canonical.items() if isinstance(members, dict) and member_name in members), struct_name)
    return (canonical_struct, member_name)


def get_profile_enabled_capabilities(json_files_dict: dict, json_file_data: dict, profile_obj: dict) -> tuple[set[str], set[tuple[str, str]]]:
    """Returns the extensions and the (structure, member) features required by a profile, including its required profiles."""
    profile_caps = collect_profile_capabilities(json_files_dict, json_file_data, profile_obj)
    enabled_exts = set(profile_caps.get("extensions", {}).keys())
    enabled_features: set[tuple[str, str]] = set()
    for struct_name, members in profile_caps.get("features", {}).items():
        if isinstance(members, dict):
            enabled_features.update((struct_name, member_name) for member_name, value in members.items() if value)
    return enabled_exts, enabled_features


def get_unsatisfied_property_values(
    vk: VulkanObject,
    api_version: VK_VERSION,
    enabled_exts: set[str],
    enabled_features: set[tuple[str, str]],
    include_unsupported_defaults: bool = True
) -> dict[tuple[str, str], tuple[str, Any]]:
    """
    Returns the property values of the devices not supporting a capability the profile doesn't require, by property:
    (the condition of the devices supporting the capability, the value of the devices not supporting it). The value is
    the Vulkan core value of these devices (a '!condition' requirement) or, without it and with
    include_unsupported_defaults, the value of an unsupported property (e.g. false for a 'max' VkBool32 property), when
    the property has such a value.
    """
    unsatisfied_values: dict[tuple[str, str], tuple[str, Any]] = {}
    dependent_properties: dict[tuple[str, str], tuple[str, str, str]] = {}
    for ver in VK_VERSION.versions():
        ver_obj = vk.versions.get(ver.value)
        if ver > api_version or not ver_obj:
            continue

        for req in getattr(ver_obj, 'propertyRequirement', []) or []:
            if req.value is None or req.value == '???' or not req.depends:
                continue
            key = get_capability_key(vk, api_version, req.struct, req.name, 'properties')
            tokens = tokenizeDependsExpression(req.depends)
            if len(tokens) == 1 and tokens[0].startswith('!'):
                condition = tokens[0][1:].strip()
                if evaluateFeatureDepends(vk, condition, api_version, enabled_exts, enabled_features):
                    continue
                value = parse_property_value(vk, req.struct, req.name, req.value)
                if value is not None:
                    unsatisfied_values[key] = (canonicalize_depends(vk, api_version, condition), value)
            elif include_unsupported_defaults and '!' not in req.depends and not evaluateFeatureDepends(vk, req.depends, api_version, enabled_exts, enabled_features):
                dependent_properties.setdefault(key, (canonicalize_depends(vk, api_version, req.depends), req.struct, req.name))

    for key, (depends, struct_name, member_name) in dependent_properties.items():
        if key in unsatisfied_values:
            continue
        struct_obj = vk.structs.get(struct_name) or getStructByName(vk.structs, struct_name)
        member_obj = getMemberByName(struct_obj, member_name) if struct_obj else None
        if member_obj is not None and is_member_default_applicable(vk, struct_obj, member_obj):
            unsatisfied_values[key] = (depends, get_member_unsupported_default(vk, struct_name, member_name))

    return unsatisfied_values


def find_contradicting_properties(
    vk: VulkanObject,
    api_version: VK_VERSION,
    capabilities_dict: dict,
    block_names: list[str],
    unsatisfied_values: dict[tuple[str, str], tuple[str, Any]]
) -> list[dict]:
    """
    Returns the properties of the unconditional capability blocks which contradict the value of the devices not
    supporting a capability the profile doesn't require, e.g. a maxTessellationGenerationLevel of 64 in a profile not
    requiring tessellationShader: the value only applies to the devices supporting the capability. Each property is
    described by its block name, its members dictionary, its member name, its path in the block, its structure name,
    its value, its condition and the value of the devices not satisfying the condition.
    """
    contradictions = []

    def check(block_name: str, members: dict, member_name: str, key: tuple[str, str], struct_name: str, path: list[str]):
        if key not in unsatisfied_values:
            return
        depends, unsatisfied_value = unsatisfied_values[key]
        if members[member_name] == unsatisfied_value:
            return
        contradictions.append({
            "block": block_name, "members": members, "member": member_name, "path": path, "struct": struct_name,
            "value": members[member_name], "depends": depends, "unsatisfied_value": unsatisfied_value
        })

    for block_name in block_names:
        block = capabilities_dict.get(block_name)
        if not isinstance(block, dict) or get_block_depends(block) is not None:
            continue

        for struct_name, members in block.get("properties", {}).items():
            if not isinstance(members, dict):
                continue
            if struct_name == 'VkPhysicalDeviceProperties':
                for sub_name, sub_struct_name in (('limits', 'VkPhysicalDeviceLimits'), ('sparseProperties', 'VkPhysicalDeviceSparseProperties')):
                    sub_members = members.get(sub_name)
                    if isinstance(sub_members, dict):
                        for member_name in list(sub_members.keys()):
                            check(block_name, sub_members, member_name, (sub_struct_name, member_name), sub_struct_name,
                                  ["properties", struct_name, sub_name])
            else:
                for member_name in list(members.keys()):
                    key = get_capability_key(vk, api_version, struct_name, member_name, 'properties')
                    check(block_name, members, member_name, key, struct_name, ["properties", struct_name])

    return contradictions


def extract_dependent_capabilities_single_profile(
    vk: VulkanObject,
    json_files_dict: dict,
    json_file_data: dict,
    profile_obj: dict,
    profile_name: str
):
    """
    Extracts the properties set by the unconditional capability blocks of the profile which contradict the value of the
    devices not supporting a capability the profile doesn't require, into capability blocks with a 'depends' condition
    on that capability: one capability block per condition. E.g. a device without wideLines has a lineWidthGranularity
    of 1.0 ('!VkPhysicalDeviceFeatures::wideLines' requirement): a lineWidthGranularity of 0.5 of a profile not
    requiring wideLines only applies to the devices supporting wideLines. The unconditional property keeps the value of
    the devices not supporting the capability. The other capabilities are left unchanged: a feature or a property which
    doesn't contradict such a value is an unconditional requirement of the profile.
    """
    api_version = VK_VERSION.from_string(profile_obj["api-version"])
    capabilities_dict = json_file_data.setdefault("capabilities", {})
    profile_caps_list = profile_obj.setdefault("capabilities", [])

    enabled_exts, enabled_features = get_profile_enabled_capabilities(json_files_dict, json_file_data, profile_obj)
    unsatisfied_values = get_unsatisfied_property_values(vk, api_version, enabled_exts, enabled_features)
    if not unsatisfied_values:
        return

    profile_block_names = collect_block_names(profile_caps_list)
    contradictions = find_contradicting_properties(vk, api_version, capabilities_dict, list(profile_block_names), unsatisfied_values)

    def get_dependent_block(depends: str) -> dict:
        # The capability block of the profile with the same condition, if any
        block_name = next((name for name in profile_block_names if name in capabilities_dict and
                           get_block_depends(capabilities_dict[name]) is not None and
                           canonicalize_depends(vk, api_version, get_block_depends(capabilities_dict[name])) == depends), None)
        if block_name is None:
            block_name = f"{profile_name}_depends_{get_depends_block_suffix(depends)}"
            capabilities_dict[block_name] = {"depends": depends}
            profile_caps_list.append(block_name)
            profile_block_names.append(block_name)
        return capabilities_dict[block_name]

    for contradiction in contradictions:
        # The value only applies to the devices satisfying the condition
        target = get_dependent_block(contradiction["depends"])
        for path_key in contradiction["path"]:
            target = target.setdefault(path_key, {})
        target[contradiction["member"]] = contradiction["value"]
        contradiction["members"][contradiction["member"]] = contradiction["unsatisfied_value"]


def pull_required_capabilities_single_profile(
    vk: VulkanObject,
    pull_bits: list[PullBits],
    json_files_dict: dict,
    profile_name: str,
    processed_profiles: set[str] = None
):
    """
    Recursively processes required profiles first (in topological order) before
    pulling required capabilities for profile_name.
    """
    if processed_profiles is None:
        processed_profiles = set()

    if profile_name in processed_profiles:
        return
    processed_profiles.add(profile_name)

    profile_obj, json_file_data = find_profile_in_files_dict(json_files_dict, profile_name)
    if not profile_obj or not json_file_data:
        return

    # 1. Recursively ensure all parent/required profiles are fully transformed first
    required_parent_profiles = profile_obj.get("profiles", [])
    for parent_pname in required_parent_profiles:
        pull_required_capabilities_single_profile(
            vk, pull_bits, json_files_dict, parent_pname, processed_profiles
        )

    # 2. Process this profile with fully-transformed parent context
    ignore_subcore = PullBits.IGNORE_SUBCORE in pull_bits
    ignore_unsupported = PullBits.IGNORE_UNSUPPORTED in pull_bits

    capabilities_dict = json_file_data.setdefault("capabilities", {})
    api_version = VK_VERSION.from_string(profile_obj["api-version"])
    profile_caps_list = profile_obj.setdefault("capabilities", [])

    context_features: dict[str, dict[str, bool]] = {}
    context_properties: dict[str, Any] = {}
    context_extensions: set[str] = set()

    if required_parent_profiles:
        inherited_caps = collect_required_profiles_capabilities_recursive(json_files_dict, required_parent_profiles)
        update_context_from_capabilities(inherited_caps, context_features, context_properties, context_extensions)

    explicit_profile_caps = collect_profile_capabilities(json_files_dict, json_file_data, profile_obj)
    explicit_context_properties = {}
    deep_merge_dict(explicit_context_properties, context_properties)
    if "properties" in explicit_profile_caps and isinstance(explicit_profile_caps["properties"], dict):
        deep_merge_dict(explicit_context_properties, explicit_profile_caps["properties"])

    explicit_context_features = {}
    deep_merge_dict(explicit_context_features, context_features)
    if "features" in explicit_profile_caps and isinstance(explicit_profile_caps["features"], dict):
        deep_merge_dict(explicit_context_features, explicit_profile_caps["features"])

    if required_parent_profiles:
        for parent_pname in required_parent_profiles:
            parent_obj, _ = find_profile_in_files_dict(json_files_dict, parent_pname)
            if not parent_obj:
                continue

            parent_api_version = VK_VERSION.from_string(parent_obj.get("api-version", "1.0.0"))
            if api_version > parent_api_version:
                current_caps = collect_profile_capabilities(json_files_dict, json_file_data, profile_obj)
                all_exts = context_extensions | set(current_caps.get("extensions", {}).keys())

                all_features_dict = {}
                if inherited_caps:
                    deep_merge_dict(all_features_dict, inherited_caps.get("features", {}))
                deep_merge_dict(all_features_dict, current_caps.get("features", {}))

                all_enabled_features_set: set[tuple[str, str]] = set()
                for struct_name, members in all_features_dict.items():
                    if isinstance(members, dict):
                        for member_name, val in members.items():
                            if val:
                                all_enabled_features_set.add((struct_name, member_name))

                transition_features = {}
                transition_properties = {}

                # The features required by the new Vulkan versions are gathered first, followed by the features of
                # the required profile Vulkan versions which depend on them, so that the requirements of every
                # Vulkan version are evaluated with all the features enabled by the profile.
                new_versions = [ver for ver in VK_VERSION.versions() if parent_api_version < ver <= api_version]
                required_versions = [ver for ver in VK_VERSION.versions() if ver <= parent_api_version]
                for ver in new_versions + required_versions:
                    satisfied_feat, _ = gatherSatisfiedCoreRequiredFeaturesForVersion(
                        vk, ver, api_version, all_exts, all_enabled_features_set
                    )
                    if satisfied_feat:
                        deep_merge_dict(transition_features, satisfied_feat)
                        for s_name, members in satisfied_feat.items():
                            if isinstance(members, dict):
                                for m_name, val in members.items():
                                    if val:
                                        all_enabled_features_set.add((s_name, m_name))

                # The property requirements of the required profile Vulkan versions are gathered too: the ones depending
                # on features newly enabled by the profile (e.g. the Vulkan 1.0 maxSamplerAnisotropy = 16 requirement
                # once Vulkan 1.4 requires samplerAnisotropy) are not satisfied by the required profile capabilities.
                # Default values are not requirements of the Vulkan version: they are not pulled into the transition
                # block but populated afterward in the structures of the profile.
                for ver in required_versions + new_versions:
                    satisfied_prop = gatherSatisfiedCoreRequiredPropertiesForVersion(
                        vk, ver, api_version, all_exts, all_enabled_features_set, ignore_unsupported=ignore_unsupported,
                        skip_unsupported_defaults=True
                    )
                    if satisfied_prop:
                        deep_merge_dict(transition_properties, satisfied_prop)

                # The capabilities defined by the profile itself take precedence over the pulled requirements,
                # unless ignore_subcore is set.
                own_caps = {}
                for block_name in parse_profile_capabilities(profile_caps_list):
                    if isinstance(block_name, str) and block_name in capabilities_dict:
                        if get_block_depends(capabilities_dict[block_name]) is None:
                            deep_merge_dict(own_caps, capabilities_dict[block_name])
                transition_features = filter_features_against_context(
                    vk, transition_features, own_caps.get("features", {}), ignore_subcore
                )
                transition_properties = filter_properties_against_context(
                    vk, transition_properties, own_caps.get("properties", {}), ignore_subcore
                )

                # The required profiles target a lower Vulkan version: their capabilities don't override the
                # requirements of the profile Vulkan version, they only make redundant the requirements they
                # already satisfy. E.g. a VkPhysicalDeviceVulkan12Features::bufferDeviceAddress = false of a
                # Vulkan 1.2 required profile doesn't prevent pulling the Vulkan 1.3 bufferDeviceAddress requirement.
                transition_features = filter_features_against_context(
                    vk, transition_features, context_features, True
                )
                transition_properties = filter_properties_against_context(
                    vk, transition_properties, context_properties, True
                )

                transition_features, transition_properties = canonicalize_capabilities_for_version(
                    vk, api_version, transition_features, transition_properties
                )

                populate_missing_structure_defaults(
                    vk, transition_features, transition_properties, ignore_unsupported,
                    explicit_context_features, explicit_context_properties
                )

                if transition_features or transition_properties:
                    ver_tuple = api_version.as_tuple()
                    transition_block_name = f"{parent_pname}_to_vulkan{ver_tuple[0]}{ver_tuple[1]}"

                    first_direct_cap = None
                    for cap_item in profile_caps_list:
                        if isinstance(cap_item, str) and cap_item != transition_block_name:
                            first_direct_cap = cap_item
                            break
                        elif isinstance(cap_item, list) and cap_item:
                            first_direct_cap = cap_item[0]
                            break

                    trans_block = capabilities_dict.get(transition_block_name)
                    if trans_block is None:
                        trans_block = {}
                        if first_direct_cap and first_direct_cap in capabilities_dict:
                            new_caps_dict = {}
                            for c_key, c_val in capabilities_dict.items():
                                if c_key == first_direct_cap:
                                    new_caps_dict[transition_block_name] = trans_block
                                new_caps_dict[c_key] = c_val
                            capabilities_dict.clear()
                            capabilities_dict.update(new_caps_dict)
                        else:
                            capabilities_dict[transition_block_name] = trans_block

                    if transition_features:
                        trans_features = trans_block.setdefault("features", {})
                        deep_merge_dict(trans_features, transition_features)
                    if transition_properties:
                        trans_properties = trans_block.setdefault("properties", {})
                        deep_merge_dict(trans_properties, transition_properties)

                    if transition_block_name in profile_caps_list:
                        profile_caps_list.remove(transition_block_name)
                    profile_caps_list.insert(0, transition_block_name)

                    update_context_from_capabilities(trans_block, context_features, context_properties, context_extensions)

        # Populate the defaults of the structures defined by the profile capability blocks, as done for the
        # primary capability block of profiles without required profiles. Otherwise a structure gets its
        # defaults when pulled into a transition block but not when already defined by the profile. Members
        # defined by the required profiles or by the transition blocks are not populated.
        own_blocks_context_features = {}
        deep_merge_dict(own_blocks_context_features, explicit_context_features)
        deep_merge_dict(own_blocks_context_features, context_features)
        own_blocks_context_properties = {}
        deep_merge_dict(own_blocks_context_properties, explicit_context_properties)
        deep_merge_dict(own_blocks_context_properties, context_properties)
        for block_name in parse_profile_capabilities(profile_caps_list):
            if isinstance(block_name, str) and block_name in capabilities_dict:
                block = capabilities_dict[block_name]
                # A capability block with a 'depends' condition only defines the conditional requirements
                if get_block_depends(block) is not None:
                    continue
                populate_missing_structure_defaults(
                    vk, block.get("features", {}), block.get("properties", {}), ignore_unsupported,
                    own_blocks_context_features, own_blocks_context_properties
                )

    else:
        primary_block = get_primary_capability_block(profile_obj, capabilities_dict)
        created_block_name = None
        if primary_block is None:
            # The profile defines no capability block yet (e.g. a profile only declaring an
            # api-version): create one to receive the core version requirements.
            created_block_name = f"{profile_name}_requirements"
            primary_block = capabilities_dict.setdefault(created_block_name, {})
            if created_block_name not in collect_block_names(profile_caps_list):
                profile_caps_list.insert(0, created_block_name)
        if primary_block is not None:
            profile_caps = collect_profile_capabilities(json_files_dict, json_file_data, profile_obj)
            profile_enabled_exts = set(profile_caps.get("extensions", {}).keys())

            all_enabled_features_set: set[tuple[str, str]] = set()
            for struct_name, members in explicit_context_features.items():
                if isinstance(members, dict):
                    for member_name, val in members.items():
                        if val:
                            all_enabled_features_set.add((struct_name, member_name))

            core_satisfied_features = {}
            core_satisfied_properties = {}
            for ver in VK_VERSION.versions():
                if ver <= api_version:
                    satisfied_feat, _ = gatherSatisfiedCoreRequiredFeaturesForVersion(
                        vk, ver, api_version, profile_enabled_exts, all_enabled_features_set
                    )
                    if satisfied_feat:
                        deep_merge_dict(core_satisfied_features, satisfied_feat)
                        for s_name, members in satisfied_feat.items():
                            if isinstance(members, dict):
                                for m_name, val in members.items():
                                    if val:
                                        all_enabled_features_set.add((s_name, m_name))

                    satisfied_prop = gatherSatisfiedCoreRequiredPropertiesForVersion(
                        vk, ver, api_version, profile_enabled_exts, all_enabled_features_set, ignore_unsupported=ignore_unsupported
                    )
                    if satisfied_prop:
                        deep_merge_dict(core_satisfied_properties, satisfied_prop)

            core_satisfied_features = filter_features_against_context(
                vk, core_satisfied_features, explicit_context_features, ignore_subcore
            )
            core_satisfied_properties = filter_properties_against_context(
                vk, core_satisfied_properties, explicit_context_properties, ignore_subcore
            )

            core_satisfied_features, core_satisfied_properties = canonicalize_capabilities_for_version(
                vk, api_version, core_satisfied_features, core_satisfied_properties
            )

            if core_satisfied_features:
                block_features = primary_block.setdefault("features", {})
                deep_merge_dict(block_features, core_satisfied_features)

            if core_satisfied_properties:
                block_properties = primary_block.setdefault("properties", {})
                deep_merge_dict(block_properties, core_satisfied_properties)

            if "features" in primary_block or "properties" in primary_block:
                p_feat, p_prop = canonicalize_capabilities_for_version(
                    vk, api_version,
                    primary_block.get("features", {}),
                    primary_block.get("properties", {})
                )
                populate_missing_structure_defaults(
                    vk, p_feat, p_prop, ignore_unsupported, explicit_context_features, explicit_context_properties
                )
                if p_feat:
                    primary_block["features"] = p_feat
                if p_prop:
                    primary_block["properties"] = p_prop

        if created_block_name is not None and not primary_block:
            # Nothing was pulled: do not leave an empty block behind.
            del capabilities_dict[created_block_name]
            profile_caps_list.remove(created_block_name)

    if PullBits.DEPENDS in pull_bits:
        extract_dependent_capabilities_single_profile(vk, json_files_dict, json_file_data, profile_obj, profile_name)

    block_names = collect_block_names(profile_obj.get("capabilities", []))
    for block_name in block_names:
        if block_name in capabilities_dict:
            block = capabilities_dict[block_name]
            ext_dict = block.get("extensions", {})
            ext_list = list(ext_dict.keys()) if isinstance(ext_dict, dict) else (ext_dict if isinstance(ext_dict, list) else [])

            profile_caps = collect_profile_capabilities(json_files_dict, json_file_data, profile_obj)
            profile_enabled_exts = set(profile_caps.get("extensions", {}).keys())

            # The requirements of a capability block with a 'depends' condition apply when the condition is
            # satisfied: the extensions and features the condition requires are enabled.
            block_depends = get_block_depends(block)
            implied_exts, implied_features = get_depends_implied_capabilities(block_depends)
            profile_enabled_exts |= set(ext_list) | implied_exts

            current_enabled_features: set[tuple[str, str]] = set(implied_features)
            for s_name, members in context_features.items():
                if isinstance(members, dict):
                    for m_name, val in members.items():
                        if val:
                            current_enabled_features.add((s_name, m_name))
            if "features" in block and isinstance(block["features"], dict):
                for struct_name, members in block["features"].items():
                    if isinstance(members, dict):
                        for member_name, val in members.items():
                            if val:
                                current_enabled_features.add((struct_name, member_name))

            for ext_name in ext_list:
                ext_satisfied, ext_or_disjunctions = gatherSatisfiedExtensionRequiredFeatures(
                    vk, ext_name, api_version, profile_enabled_exts, current_enabled_features
                )
                if ext_or_disjunctions:
                    for disj in ext_or_disjunctions:
                        process_or_disjunction(
                            vk, profile_obj, capabilities_dict, block_name, disj
                        )

                if ext_satisfied:
                    filtered_ext_satisfied = filter_features_against_context(
                        vk, ext_satisfied, explicit_context_features, ignore_subcore
                    )
                    if filtered_ext_satisfied:
                        block_features = block.setdefault("features", {})
                        deep_merge_dict(block_features, filtered_ext_satisfied)
                        if block_depends is None:
                            deep_merge_dict(explicit_context_features, filtered_ext_satisfied)
                        for s_name, members in filtered_ext_satisfied.items():
                            if isinstance(members, dict):
                                for m_name, val in members.items():
                                    if val:
                                        current_enabled_features.add((s_name, m_name))

                ext_satisfied_props = gatherSatisfiedExtensionRequiredProperties(
                    vk, ext_name, api_version, profile_enabled_exts, current_enabled_features, ignore_unsupported=ignore_unsupported
                )
                if ext_satisfied_props:
                    filtered_ext_satisfied_props = filter_properties_against_context(
                        vk, ext_satisfied_props, explicit_context_properties, ignore_subcore
                    )
                    if filtered_ext_satisfied_props:
                        block_properties = block.setdefault("properties", {})
                        deep_merge_dict(block_properties, filtered_ext_satisfied_props)

            update_context_from_capabilities(block, context_features, context_properties, context_extensions)


def pull_required_capabilities_profiles_files(
    vk: VulkanObject, 
    pull_bits: list[PullBits], 
    json_files_dict: dict
):
    """
    Executes extension dependency pulling and required capability processing in
    topological order across all files and profiles.
    """
    # 1. Pull extension dependencies across all profile files
    pull_extension_dependencies_profiles_files(vk, pull_bits, json_files_dict)

    # 2. Process required capabilities topologically in dependency order
    processed_profiles: set[str] = set()
    sorted_file_keys = get_topologically_sorted_file_keys(json_files_dict)

    for file_key in sorted_file_keys:
        file_data = json_files_dict[file_key]
        profiles_data = file_data.get("profiles", {})
        for profile_name in profiles_data.keys():
            pull_required_capabilities_single_profile(
                vk, pull_bits, json_files_dict, profile_name, processed_profiles
            )