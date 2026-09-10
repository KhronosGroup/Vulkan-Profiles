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

from source.vulkan_object_version import (
    is_bundle_structure, 
    get_active_feature_bundles,
    get_active_property_bundles
)
from source.vulkan_object_utils import (
    VulkanObject, 
    VK_VERSION, 
    are_structs_aliases_for_version,
    get_required_extensions_for_struct,
    is_struct_covered_by_bundle,
    is_property_struct_covered_by_bundle,
    should_remove_struct_a_in_favor_of_b
)
from source.profiles_json_utils import (
    parse_profile_capabilities,
    get_topologically_sorted_file_keys,
    collect_required_profiles_capabilities_recursive,
    strip_dict_duplication,
    deep_merge_dict
)


def strip_intra_block_feature_duplication(vk: VulkanObject, version: VK_VERSION, json_block: dict, context_features: dict):
    if "features" not in json_block or not isinstance(json_block["features"], dict):
        return

    block_features = json_block["features"]

    all_features = {}
    deep_merge_dict(all_features, context_features)
    deep_merge_dict(all_features, block_features)

    structs_to_remove = set()

    active_bundles = [
        b for b in get_active_feature_bundles(version)
        if b in all_features
    ]

    for struct_name in list(block_features.keys()):
        if is_bundle_structure(struct_name):
            continue

        if active_bundles and any(is_struct_covered_by_bundle(vk, bundle, struct_name) for bundle in active_bundles):
            structs_to_remove.add(struct_name)
            continue

        for other_struct in all_features.keys():
            if other_struct != struct_name and are_structs_aliases_for_version(vk, version, struct_name, other_struct):
                if other_struct in context_features:
                    structs_to_remove.add(struct_name)
                    break
                elif should_remove_struct_a_in_favor_of_b(vk, version, struct_name, other_struct):
                    structs_to_remove.add(struct_name)
                    break

    for s in structs_to_remove:
        if s in block_features:
            del block_features[s]

    if not block_features:
        del json_block["features"]


def strip_intra_block_property_duplication(vk: VulkanObject, version: VK_VERSION, json_block: dict, context_properties: dict):
    if "properties" not in json_block or not isinstance(json_block["properties"], dict):
        return

    block_properties = json_block["properties"]

    all_properties = {}
    deep_merge_dict(all_properties, context_properties)
    deep_merge_dict(all_properties, block_properties)

    structs_to_remove = set()

    active_bundles = [
        b for b in get_active_property_bundles(version)
        if b in all_properties
    ]

    for struct_name in list(block_properties.keys()):
        if is_bundle_structure(struct_name):
            continue

        if active_bundles and any(is_property_struct_covered_by_bundle(vk, bundle, struct_name) for bundle in active_bundles):
            structs_to_remove.add(struct_name)
            continue

        for other_struct in all_properties.keys():
            if other_struct != struct_name and are_structs_aliases_for_version(vk, version, struct_name, other_struct):
                if other_struct in context_properties:
                    structs_to_remove.add(struct_name)
                    break
                elif should_remove_struct_a_in_favor_of_b(vk, version, struct_name, other_struct):
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

    for fmt_name, structs_dict in list(block_formats.items()):
        if not isinstance(structs_dict, dict):
            continue

        ctx_structs = context_formats.get(fmt_name, {}) if isinstance(context_formats, dict) else {}
        all_structs = {}
        if isinstance(ctx_structs, dict):
            deep_merge_dict(all_structs, ctx_structs)
        deep_merge_dict(all_structs, structs_dict)

        structs_to_remove = set()
        for struct_name in list(structs_dict.keys()):
            for other_struct in all_structs.keys():
                if other_struct != struct_name and are_structs_aliases_for_version(vk, version, struct_name, other_struct):
                    if isinstance(ctx_structs, dict) and other_struct in ctx_structs:
                        structs_to_remove.add(struct_name)
                        break
                    elif should_remove_struct_a_in_favor_of_b(vk, version, struct_name, other_struct):
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
        collected_capabilities: dict = collect_required_profiles_capabilities_recursive(
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
        