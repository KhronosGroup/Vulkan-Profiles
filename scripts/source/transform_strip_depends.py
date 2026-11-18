#!/usr/bin/python3
#
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

from typing import Any

from source.vulkan_object_utils import (
    VulkanObject,
    VK_VERSION,
    evaluateFeatureDepends,
    getMemberByName,
    getStructByName,
    get_member_unsupported_default,
    parse_property_value
)
from source.vulkan_object_expression_parsing import tokenizeDependsExpression
from source.profiles_json_utils import (
    collect_block_names,
    collect_profile_capabilities,
    get_block_depends,
    get_topologically_sorted_file_keys
)
from source.transform_pull_required_capabilities import get_capability_key, is_member_default_applicable


def get_unsatisfied_requirements(
    vk: VulkanObject,
    api_version: VK_VERSION,
    enabled_exts: set[str],
    enabled_features: set[tuple[str, str]]
) -> tuple[set[tuple[str, tuple[str, str]]], dict[tuple[str, str], Any]]:
    """
    Returns the capabilities whose Vulkan core requirement depends on a condition the profile doesn't satisfy, as
    (category, capability), and the property values of the devices not satisfying such a condition, by capability.
    """
    dependent_capabilities: set[tuple[str, tuple[str, str]]] = set()
    unsatisfied_values: dict[tuple[str, str], Any] = {}

    def is_satisfied(depends: str) -> bool:
        return evaluateFeatureDepends(vk, depends, api_version, enabled_exts, enabled_features)

    for ver in VK_VERSION.versions():
        ver_obj = vk.versions.get(ver.value)
        if ver > api_version or not ver_obj:
            continue

        for req in getattr(ver_obj, 'featureRequirement', []) or []:
            fields = [field.strip() for field in req.field.split(',')] if req.field else []
            if len(fields) == 1 and req.depends and '!' not in req.depends and not is_satisfied(req.depends):
                dependent_capabilities.add(('features', get_capability_key(vk, api_version, req.struct, fields[0], 'features')))

        for req in getattr(ver_obj, 'propertyRequirement', []) or []:
            if req.value is None or req.value == '???' or not req.depends:
                continue
            key = get_capability_key(vk, api_version, req.struct, req.name, 'properties')
            tokens = tokenizeDependsExpression(req.depends)
            if len(tokens) == 1 and tokens[0].startswith('!'):
                if not is_satisfied(tokens[0][1:].strip()):
                    value = parse_property_value(vk, req.struct, req.name, req.value)
                    if value is not None:
                        unsatisfied_values[key] = value
            elif '!' not in req.depends and not is_satisfied(req.depends):
                dependent_capabilities.add(('properties', key))

    return dependent_capabilities, unsatisfied_values


def strip_dependent_capabilities_block(
    vk: VulkanObject,
    api_version: VK_VERSION,
    block: dict,
    dependent_capabilities: set[tuple[str, tuple[str, str]]],
    unsatisfied_values: dict[tuple[str, str], Any]
):
    """
    Sets the features and properties of a capability block whose condition isn't satisfied to the value of the
    devices not satisfying their condition: the Vulkan core value for these devices (e.g. a maxGeometryShaderInvocations
    of 0 without geometryShader) or the value of an unsupported capability (e.g. false for a feature, false for a
    'max' VkBool32 property and true for a 'min' one). The members without such a value (e.g. 'exact' or 'noauto'
    members) are removed.
    """
    def strip_members(members: dict, category: str, struct_name: str, get_key):
        struct_obj = vk.structs.get(struct_name) or getStructByName(vk.structs, struct_name)
        for member_name in list(members.keys()):
            key = get_key(member_name)
            if category == 'properties' and key in unsatisfied_values:
                members[member_name] = unsatisfied_values[key]
            elif (category, key) in dependent_capabilities:
                member_obj = getMemberByName(struct_obj, member_name) if struct_obj else None
                if member_obj is not None and is_member_default_applicable(vk, struct_obj, member_obj):
                    members[member_name] = get_member_unsupported_default(vk, struct_name, member_name)
                else:
                    del members[member_name]

    features = block.get("features", {})
    for struct_name in list(features.keys()):
        members = features[struct_name]
        if isinstance(members, dict):
            strip_members(members, 'features', struct_name, lambda member_name: get_capability_key(vk, api_version, struct_name, member_name, 'features'))
            if not members:
                del features[struct_name]
    if "features" in block and not features:
        del block["features"]

    properties = block.get("properties", {})
    for struct_name in list(properties.keys()):
        members = properties[struct_name]
        if not isinstance(members, dict):
            continue
        if struct_name == 'VkPhysicalDeviceProperties':
            for sub_name, sub_struct_name in (('limits', 'VkPhysicalDeviceLimits'), ('sparseProperties', 'VkPhysicalDeviceSparseProperties')):
                sub_members = members.get(sub_name)
                if isinstance(sub_members, dict):
                    strip_members(sub_members, 'properties', sub_struct_name, lambda member_name: (sub_struct_name, member_name))
                    if not sub_members:
                        del members[sub_name]
        else:
            strip_members(members, 'properties', struct_name, lambda member_name: get_capability_key(vk, api_version, struct_name, member_name, 'properties'))
        if not members:
            del properties[struct_name]
    if "properties" in block and not properties:
        del block["properties"]


def strip_dependent_capabilities_single_profile(vk: VulkanObject, json_files_dict: dict, json_file_data: dict, profile_obj: dict) -> set[str]:
    """
    Strips the capabilities of a profile whose condition the profile doesn't satisfy: the capability blocks with a
    'depends' condition the profile doesn't satisfy are removed from the profile, the properties with a value for the
    devices not satisfying their condition are set to that value, and the other capabilities whose Vulkan core
    requirement depends on a condition the profile doesn't satisfy are removed. Returns the names of the capability
    blocks removed from the profile.
    """
    api_version = VK_VERSION.from_string(profile_obj.get("api-version", "1.0.0"))
    capabilities_dict = json_file_data.get("capabilities", {})

    profile_caps = collect_profile_capabilities(json_files_dict, json_file_data, profile_obj)
    enabled_exts = set(profile_caps.get("extensions", {}).keys())
    enabled_features: set[tuple[str, str]] = set()
    for struct_name, members in profile_caps.get("features", {}).items():
        if isinstance(members, dict):
            enabled_features.update((struct_name, member_name) for member_name, value in members.items() if value)

    # 1. Remove the capability blocks with a 'depends' condition the profile doesn't satisfy
    removed_blocks: set[str] = set()

    def is_unsatisfied_block(block_name) -> bool:
        depends = get_block_depends(capabilities_dict.get(block_name)) if isinstance(block_name, str) else None
        if depends is not None and not evaluateFeatureDepends(vk, depends, api_version, enabled_exts, enabled_features):
            removed_blocks.add(block_name)
            return True
        return False

    new_caps_list = []
    for entry in profile_obj.get("capabilities", []):
        if isinstance(entry, list):
            entry = [block_name for block_name in entry if not is_unsatisfied_block(block_name)]
            if entry:
                new_caps_list.append(entry)
        elif not is_unsatisfied_block(entry):
            new_caps_list.append(entry)
    profile_obj["capabilities"] = new_caps_list

    # 2. Strip the capabilities of the unconditional capability blocks whose condition the profile doesn't satisfy
    dependent_capabilities, unsatisfied_values = get_unsatisfied_requirements(vk, api_version, enabled_exts, enabled_features)
    for block_name in collect_block_names(new_caps_list):
        block = capabilities_dict.get(block_name)
        if isinstance(block, dict) and get_block_depends(block) is None:
            strip_dependent_capabilities_block(vk, api_version, block, dependent_capabilities, unsatisfied_values)

    return removed_blocks


def strip_dependent_capabilities_profiles_files(vk: VulkanObject, json_files_dict: dict):
    if not isinstance(json_files_dict, dict):
        return

    if "profiles" in json_files_dict or "capabilities" in json_files_dict:
        json_files_dict = {"": json_files_dict}

    for file_key in get_topologically_sorted_file_keys(json_files_dict):
        json_file_data = json_files_dict[file_key]
        removed_blocks: set[str] = set()
        for profile_obj in json_file_data.get("profiles", {}).values():
            removed_blocks |= strip_dependent_capabilities_single_profile(vk, json_files_dict, json_file_data, profile_obj)

        # The removed capability blocks no profile of the file references anymore are deleted
        referenced_blocks = set()
        for profile_obj in json_file_data.get("profiles", {}).values():
            referenced_blocks.update(collect_block_names(profile_obj.get("capabilities", [])))
        for block_name in removed_blocks - referenced_blocks:
            json_file_data.get("capabilities", {}).pop(block_name, None)
