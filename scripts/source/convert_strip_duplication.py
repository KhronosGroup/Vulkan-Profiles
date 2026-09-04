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
    are_structs_aliases_for_version,
    get_required_extensions_for_struct,
    StructCapabilityAlias
)
from source.profiles_json_utils import (
    parse_profile_capabilities,
    collect_required_profiles_capabilities,
    get_topologically_sorted_file_keys,
    deep_merge_dict
)
from source.convert_utils import (
    should_remove_struct_a_in_favor_of_b,
    get_parent_property_value
)


def strip_dict_duplication(target: dict, reference: dict):
    keys_to_delete = []

    for key, value in list(target.items()):
        if key in reference:
            ref_value = reference[key]

            if isinstance(value, dict) and isinstance(ref_value, dict):
                strip_dict_duplication(value, ref_value)
                if not value:
                    keys_to_delete.append(key)

            elif value == ref_value:
                keys_to_delete.append(key)

            elif isinstance(value, list) and isinstance(ref_value, list):
                try:
                    if sorted(value) == sorted(ref_value):
                        keys_to_delete.append(key)
                except TypeError:
                    if value == ref_value:
                        keys_to_delete.append(key)

    for key in keys_to_delete:
        del target[key]


def is_feature_member_covered(
    vk: VulkanObject,
    version: VK_VERSION,
    struct_name: str,
    m_name: str,
    val: bool,
    ref_features: dict
) -> bool:
    if not ref_features or not val:
        return False

    query_id = StructCapabilityAlias(struct_name, m_name)
    aliases = [query_id] + gatherCapabilityAliases(vk, query_id)

    for alias in aliases:
        if not isinstance(alias, StructCapabilityAlias):
            continue
        a_struct = alias.struct
        a_member = alias.member

        if a_struct in ref_features and isinstance(ref_features[a_struct], dict):
            if ref_features[a_struct].get(a_member) == val:
                return True

    return False


def is_feature_struct_members_covered_by_bundle(vk: VulkanObject, bundle_name: str, struct_name: str, members: dict, all_features: dict) -> bool:
    if bundle_name not in all_features or not isinstance(all_features[bundle_name], dict):
        return False

    bundle_members = all_features[bundle_name]
    for m_name, val in members.items():
        if not val:
            continue
        query_id = StructCapabilityAlias(struct_name, m_name)
        aliases = [query_id] + gatherCapabilityAliases(vk, query_id)
        found = False
        for alias in aliases:
            if isinstance(alias, StructCapabilityAlias) and alias.struct == bundle_name:
                if bundle_members.get(alias.member) == val:
                    found = True
                    break
        if not found:
            return False
    return True


def is_feature_struct_covered_by_alias(vk: VulkanObject, struct_a: str, members_a: dict, struct_b: str, all_features: dict) -> bool:
    if struct_b not in all_features or not isinstance(all_features[struct_b], dict):
        return False
    members_b = all_features[struct_b]
    for m_name, val in members_a.items():
        query_id = StructCapabilityAlias(struct_a, m_name)
        aliases = [query_id] + gatherCapabilityAliases(vk, query_id)
        found = False
        for alias in aliases:
            if isinstance(alias, StructCapabilityAlias) and alias.struct == struct_b:
                if members_b.get(alias.member) == val:
                    found = True
                    break
        if not found:
            return False
    return True


def strip_intra_block_feature_duplication(vk: VulkanObject, version: VK_VERSION, json_block: dict, context_features: dict):
    if "features" not in json_block or not isinstance(json_block["features"], dict):
        return

    block_features = json_block["features"]

    if context_features:
        for struct_name in list(block_features.keys()):
            members = block_features[struct_name]
            if not isinstance(members, dict):
                continue

            for m_name in list(members.keys()):
                val = members[m_name]
                if is_feature_member_covered(vk, version, struct_name, m_name, val, context_features):
                    del members[m_name]

            if not members:
                del block_features[struct_name]

    if not block_features:
        del json_block["features"]
        return

    active_bundles = [
        b for b in get_active_feature_bundles(version)
        if b in block_features
    ]

    structs_to_remove = set()

    for struct_name, members in list(block_features.items()):
        if is_bundle_structure(struct_name) or not isinstance(members, dict):
            continue

        if active_bundles:
            covered_by_bundle = False
            for bundle in active_bundles:
                if is_feature_struct_members_covered_by_bundle(vk, bundle, struct_name, members, block_features):
                    covered_by_bundle = True
                    break
            if covered_by_bundle:
                structs_to_remove.add(struct_name)
                continue

        for other_struct in block_features.keys():
            if other_struct != struct_name and are_structs_aliases_for_version(vk, version, struct_name, other_struct):
                if should_remove_struct_a_in_favor_of_b(vk, version, struct_name, other_struct):
                    if is_feature_struct_covered_by_alias(vk, struct_name, members, other_struct, block_features):
                        structs_to_remove.add(struct_name)
                        break

    for s in structs_to_remove:
        if s in block_features:
            del block_features[s]

    if not block_features:
        del json_block["features"]


def is_property_member_covered(
    vk: VulkanObject,
    version: VK_VERSION,
    struct_name: str,
    prop_name: str,
    prop_val: Any,
    ref_properties: dict
) -> bool:
    if not ref_properties:
        return False

    found, parent_val = get_parent_property_value(ref_properties, struct_name, prop_name, vk)
    if found and parent_val == prop_val:
        return True

    return False


def is_property_struct_members_covered_by_bundle(vk: VulkanObject, bundle_name: str, struct_name: str, prop_data: dict, all_properties: dict) -> bool:
    if bundle_name not in all_properties or not isinstance(all_properties[bundle_name], dict):
        return False

    bundle_data = all_properties[bundle_name]
    if not isinstance(prop_data, dict):
        return False

    for prop_name, prop_val in prop_data.items():
        found = False
        query_id = StructCapabilityAlias(struct_name, prop_name)
        aliases = [query_id] + gatherCapabilityAliases(vk, query_id)

        for alias in aliases:
            if isinstance(alias, StructCapabilityAlias) and alias.struct == bundle_name:
                b_val = None
                if bundle_name == "VkPhysicalDeviceProperties":
                    if "limits" in bundle_data and isinstance(bundle_data["limits"], dict) and alias.member in bundle_data["limits"]:
                        b_val = bundle_data["limits"][alias.member]
                    elif "sparseProperties" in bundle_data and isinstance(bundle_data["sparseProperties"], dict) and alias.member in bundle_data["sparseProperties"]:
                        b_val = bundle_data["sparseProperties"][alias.member]
                else:
                    if alias.member in bundle_data:
                        b_val = bundle_data[alias.member]

                if b_val is not None and b_val == prop_val:
                    found = True
                    break

        if not found:
            return False
    return True


def is_property_struct_covered_by_alias(vk: VulkanObject, struct_a: str, prop_data_a: dict, struct_b: str, all_properties: dict) -> bool:
    if struct_b not in all_properties or not isinstance(all_properties[struct_b], dict):
        return False
    prop_data_b = all_properties[struct_b]
    if not isinstance(prop_data_a, dict):
        return False

    for prop_name, prop_val in prop_data_a.items():
        query_id = StructCapabilityAlias(struct_a, prop_name)
        aliases = [query_id] + gatherCapabilityAliases(vk, query_id)
        found = False
        for alias in aliases:
            if isinstance(alias, StructCapabilityAlias) and alias.struct == struct_b:
                b_val = prop_data_b.get(alias.member)
                if b_val is not None and b_val == prop_val:
                    found = True
                    break
        if not found:
            return False
    return True


def strip_intra_block_property_duplication(vk: VulkanObject, version: VK_VERSION, json_block: dict, context_properties: dict):
    if "properties" not in json_block or not isinstance(json_block["properties"], dict):
        return

    block_properties = json_block["properties"]

    if context_properties:
        for struct_name in list(block_properties.keys()):
            prop_data = block_properties[struct_name]
            if not isinstance(prop_data, dict):
                continue

            if struct_name == "VkPhysicalDeviceProperties":
                for sub_group in list(prop_data.keys()):
                    sub_dict = prop_data[sub_group]
                    if isinstance(sub_dict, dict):
                        for p_name in list(sub_dict.keys()):
                            p_val = sub_dict[p_name]
                            if is_property_member_covered(vk, version, struct_name, p_name, p_val, context_properties):
                                del sub_dict[p_name]
                        if not sub_dict:
                            del prop_data[sub_group]
            else:
                for p_name in list(prop_data.keys()):
                    p_val = prop_data[p_name]
                    if is_property_member_covered(vk, version, struct_name, p_name, p_val, context_properties):
                        del prop_data[p_name]

            if not prop_data:
                del block_properties[struct_name]

    if not block_properties:
        del json_block["properties"]
        return

    active_bundles = [
        b for b in get_active_property_bundles(version)
        if b in block_properties
    ]

    structs_to_remove = set()

    for struct_name, prop_data in list(block_properties.items()):
        if is_bundle_structure(struct_name) or not isinstance(prop_data, dict):
            continue

        if active_bundles:
            covered_by_bundle = False
            for bundle in active_bundles:
                if is_property_struct_members_covered_by_bundle(vk, bundle, struct_name, prop_data, block_properties):
                    covered_by_bundle = True
                    break
            if covered_by_bundle:
                structs_to_remove.add(struct_name)
                continue

        for other_struct in block_properties.keys():
            if other_struct != struct_name and are_structs_aliases_for_version(vk, version, struct_name, other_struct):
                if should_remove_struct_a_in_favor_of_b(vk, version, struct_name, other_struct):
                    if is_property_struct_covered_by_alias(vk, struct_name, prop_data, other_struct, block_properties):
                        structs_to_remove.add(struct_name)
                        break

    for s in structs_to_remove:
        if s in block_properties:
            del block_properties[s]

    if not block_properties:
        del json_block["properties"]


def strip_intra_block_format_duplication(vk: VulkanObject, version: VK_VERSION, json_block: dict, context_formats: dict):
    if "formats" not in json_block or not isinstance(json_block["formats"], dict):
        return

    block_formats = json_block["formats"]

    if context_formats:
        for fmt_name in list(block_formats.keys()):
            structs_dict = block_formats[fmt_name]
            if not isinstance(structs_dict, dict):
                continue

            ctx_structs = context_formats.get(fmt_name, {}) if isinstance(context_formats, dict) else {}
            if isinstance(ctx_structs, dict) and ctx_structs:
                for struct_name in list(structs_dict.keys()):
                    members_dict = structs_dict[struct_name]
                    if not isinstance(members_dict, dict):
                        continue

                    covered_in_ctx = False
                    for ctx_struct_name, ctx_members in ctx_structs.items():
                        if are_structs_aliases_for_version(vk, version, struct_name, ctx_struct_name):
                            if isinstance(ctx_members, dict):
                                all_covered = True
                                for f_key, f_list in members_dict.items():
                                    if f_key in ctx_members and isinstance(f_list, list) and isinstance(ctx_members[f_key], list):
                                        if not set(f_list).issubset(set(ctx_members[f_key])):
                                            all_covered = False
                                            break
                                    elif f_key not in ctx_members:
                                        all_covered = False
                                        break
                                if all_covered:
                                    covered_in_ctx = True
                                    break
                    if covered_in_ctx:
                        del structs_dict[struct_name]

            if not structs_dict:
                del block_formats[fmt_name]

    if not block_formats:
        del json_block["formats"]
        return

    for fmt_name, structs_dict in list(block_formats.items()):
        if not isinstance(structs_dict, dict):
            continue

        structs_to_remove = set()
        for struct_name, members_dict in list(structs_dict.items()):
            if not isinstance(members_dict, dict):
                continue

            for other_struct in structs_dict.keys():
                if other_struct != struct_name and are_structs_aliases_for_version(vk, version, struct_name, other_struct):
                    if should_remove_struct_a_in_favor_of_b(vk, version, struct_name, other_struct):
                        other_members = structs_dict[other_struct]
                        if isinstance(other_members, dict):
                            all_covered = True
                            for f_key, f_list in members_dict.items():
                                if f_key in other_members and isinstance(f_list, list) and isinstance(other_members[f_key], list):
                                    if not set(f_list).issubset(set(other_members[f_key])):
                                        all_covered = False
                                        break
                                elif f_key not in other_members:
                                    all_covered = False
                                    break
                            if all_covered:
                                structs_to_remove.add(struct_name)
                                break

        for s in structs_to_remove:
            if s in structs_dict:
                del structs_dict[s]

        if not structs_dict:
            del block_formats[fmt_name]

    if not block_formats:
        del json_block["formats"]


def strip_duplication_capabilities_block(vk: VulkanObject, json_files_dict, version: VK_VERSION, json_profiles_capabilities_block: dict, collected_capabilities: dict):
    context_features = collected_capabilities.get("features", {})
    strip_intra_block_feature_duplication(vk, version, json_profiles_capabilities_block, context_features)

    context_properties = collected_capabilities.get("properties", {})
    strip_intra_block_property_duplication(vk, version, json_profiles_capabilities_block, context_properties)

    context_formats = collected_capabilities.get("formats", {})
    strip_intra_block_format_duplication(vk, version, json_profiles_capabilities_block, context_formats)

    for section in ("features", "properties", "formats"):
        if section in json_profiles_capabilities_block and section in collected_capabilities:
            strip_dict_duplication(json_profiles_capabilities_block[section], collected_capabilities[section])
            if not json_profiles_capabilities_block[section]:
                del json_profiles_capabilities_block[section]

    needed_extensions = set()
    for section in ("features", "properties"):
        if section in json_profiles_capabilities_block and isinstance(json_profiles_capabilities_block[section], dict):
            for struct_name in json_profiles_capabilities_block[section].keys():
                req_exts = get_required_extensions_for_struct(vk, struct_name, version)
                needed_extensions.update(req_exts)

    if "extensions" in json_profiles_capabilities_block and "extensions" in collected_capabilities:
        stripped_extensions: dict[str, int] = {}
        ref_extensions = collected_capabilities["extensions"]

        for extension_name, extension_version in json_profiles_capabilities_block["extensions"].items():
            if extension_name in ref_extensions and extension_name not in needed_extensions:
                continue
            stripped_extensions[extension_name] = extension_version

        if stripped_extensions:
            json_profiles_capabilities_block["extensions"] = stripped_extensions
        else:
            del json_profiles_capabilities_block["extensions"]


def strip_duplication_profiles_file(vk: VulkanObject, json_files_dict, json_file_data):
    profiles_data = json_file_data.get("profiles", {})
    json_profiles_capabilities = json_file_data.get("capabilities", {})

    for key, value in profiles_data.items():
        required_profile_names = value.get("profiles", [])
        collected_capabilities: dict = collect_required_profiles_capabilities(
            json_files_dict, required_profile_names
        )

        version = VK_VERSION.from_string(value.get("api-version", "1.0.0"))
        parsed_caps = parse_profile_capabilities(value.get("capabilities", []))

        for item in parsed_caps:
            if isinstance(item, str):
                if item in json_profiles_capabilities:
                    strip_duplication_capabilities_block(
                        vk, json_files_dict, version, json_profiles_capabilities[item], collected_capabilities
                    )
                    deep_merge_dict(collected_capabilities, json_profiles_capabilities[item])

            elif isinstance(item, list):
                for alt_block_name in item:
                    if alt_block_name in json_profiles_capabilities:
                        strip_duplication_capabilities_block(
                            vk, json_files_dict, version, json_profiles_capabilities[alt_block_name], collected_capabilities
                        )


def strip_duplication_profiles_files(vk: VulkanObject, json_files_dict: dict):
    if not isinstance(json_files_dict, dict):
        return

    if "profiles" in json_files_dict or "capabilities" in json_files_dict:
        strip_duplication_profiles_file(vk, json_files_dict, json_files_dict)
        return

    sorted_file_keys = get_topologically_sorted_file_keys(json_files_dict)
    for file_key in sorted_file_keys:
        strip_duplication_profiles_file(vk, json_files_dict, json_files_dict[file_key])