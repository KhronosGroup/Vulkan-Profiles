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

"""
Workarounds for the issues of the device reports exported from GPUInfo.org, applied by 'validate --apply-workarounds'
in place to the input profiles files, or to copies written to the 'validate --output' directory.
"""

import copy
import json
import logging
from pathlib import Path
import shutil

from vulkan_object import VulkanObject
from source.transform_utils import WorkaroundBits
from source.profiles_json_utils import load_profiles_json, collect_block_names
from source.vulkan_object_utils import getUndeclaredStructExtension, gatherCapabilityAliases, StructCapabilityAlias
from source.vulkan_object_version import VK_VERSION


# The driverID of the drivers unknown to the device report exporter, by driverName
DRIVER_IDS_BY_DRIVER_NAME = {
    "KosmicKrisp": "VK_DRIVER_ID_MESA_KOSMICKRISP",
}


def apply_driver_id_workaround(vk: VulkanObject, json_data: dict) -> list[str]:
    """
    Fixes the driverID of the property structures written by a device report exporter not knowing the driver ID:
    - Sets the driverID from the driverName, e.g. 'VK_DRIVER_ID_MESA_KOSMICKRISP' for 'KosmicKrisp', replacing the null
      and '_KHR' driverID of a driver ID unknown to the exporter.
    - Removes the '_KHR' suffix the exporter added to the driver IDs without a '_KHR' alias, the driver IDs added after
      the promotion of VK_KHR_driver_properties to Vulkan 1.2, e.g. 'VK_DRIVER_ID_MOLTENVK_KHR' to 'VK_DRIVER_ID_MOLTENVK'.
    Returns the description of each change.
    """
    driver_id_enum = vk.enums.get("VkDriverId")
    valid_driver_ids = {name for field in driver_id_enum.fields for name in [field.name] + field.aliases} if driver_id_enum else set()

    changes = []
    for block_name, block in json_data.get("capabilities", {}).items():
        if not isinstance(block, dict):
            continue
        for struct_name, members in block.get("properties", {}).items():
            if not isinstance(members, dict) or "driverID" not in members:
                continue
            driver_id = DRIVER_IDS_BY_DRIVER_NAME.get(members.get("driverName"))
            reported_driver_id = members["driverID"]
            if driver_id is None and isinstance(reported_driver_id, str) and reported_driver_id not in valid_driver_ids and reported_driver_id.endswith("_KHR"):
                driver_id = reported_driver_id.removesuffix("_KHR")
            if driver_id is None or reported_driver_id == driver_id:
                continue
            if driver_id not in valid_driver_ids:
                logging.warning(f"Skipping the '{WorkaroundBits.DRIVER_ID.value}' workaround of '{struct_name}::driverID': '{driver_id}' is not a VkDriverId value of the Vulkan registry")
                continue
            changes.append(f"capabilities -> {block_name} -> properties -> {struct_name} -> driverID: {json.dumps(members['driverID'])} -> \"{driver_id}\"")
            members["driverID"] = driver_id
    return changes


# The 64-bit integer types, which the device report exporter may write as strings
INTEGER_64_BIT_TYPES = {"uint64_t", "int64_t", "VkDeviceSize", "VkDeviceAddress"}


def apply_member_workaround(vk: VulkanObject, json_data: dict, convert_member) -> list[str]:
    """
    Calls convert_member(member, value) on each member of the property structures, recursively in the nested
    structures, and replaces the value by the returned value when it differs. Returns the description of each change.
    """
    struct_names = {name: struct for struct in vk.structs.values() for name in [struct.name] + struct.aliases}

    def convert_struct(struct_name: str, members: dict, path: str) -> list[str]:
        struct = struct_names.get(struct_name)
        if struct is None:
            return []
        changes = []
        for member in struct.members:
            if member.name not in members:
                continue
            value = members[member.name]
            if isinstance(value, dict) and member.type in struct_names:
                changes.extend(convert_struct(member.type, value, f"{path} -> {member.name}"))
                continue
            converted = convert_member(member, value)
            if converted != value:
                changes.append(f"{path} -> {member.name}: {json.dumps(value)} -> {json.dumps(converted)}")
                members[member.name] = converted
        return changes

    changes = []
    for block_name, block in json_data.get("capabilities", {}).items():
        if not isinstance(block, dict):
            continue
        for struct_name, members in block.get("properties", {}).items():
            if isinstance(members, dict):
                changes.extend(convert_struct(struct_name, members, f"capabilities -> {block_name} -> properties -> {struct_name}"))
    return changes


def apply_64_bit_strings_workaround(vk: VulkanObject, json_data: dict) -> list[str]:
    """
    Converts to numbers the 64-bit integer values of the property structures written as strings, e.g. the "4096"
    VkPhysicalDeviceExternalMemoryHostPropertiesEXT::minImportedHostPointerAlignment of some device reports.
    Returns the description of each change.
    """
    def convert(value):
        if isinstance(value, str) and value.lstrip("-").isdigit():
            return int(value)
        if isinstance(value, list):
            return [convert(item) for item in value]
        return value

    return apply_member_workaround(vk, json_data, lambda member, value: convert(value) if member.type in INTEGER_64_BIT_TYPES else value)


def apply_flags_as_numbers_workaround(vk: VulkanObject, json_data: dict) -> list[str]:
    """
    Converts to lists of flag bit names the flags values of the property structures written as numbers or numeric
    strings, e.g. the "6" VkPhysicalDeviceExtendedSparseAddressSpacePropertiesNV::extendedSparseImageUsageFlags of some
    device reports, converted to ["VK_IMAGE_USAGE_TRANSFER_DST_BIT", "VK_IMAGE_USAGE_SAMPLED_BIT"].
    Returns the description of each change.
    """
    flags_types = {name: flags for flags in vk.flags.values() for name in [flags.name] + flags.aliases}

    def convert(member, value):
        flags = flags_types.get(member.type)
        if flags is None or isinstance(value, bool) or not isinstance(value, (int, str)):
            return value
        if isinstance(value, str) and not value.isdigit():
            return value
        number = int(value)
        bitmask = vk.bitmasks.get(flags.bitmaskName) if flags.bitmaskName else None
        single_bits = [flag for flag in bitmask.flags if not flag.multiBit and not flag.zero] if bitmask else []
        names = [flag.name for flag in sorted(single_bits, key=lambda flag: flag.value) if number & flag.value]
        unknown_bits = number & ~sum(flag.value for flag in single_bits)
        if unknown_bits:
            logging.warning(f"Skipping the '{WorkaroundBits.FLAGS_AS_NUMBERS.value}' workaround of '{member.name}': {number:#x} has bits unknown to '{member.type}' in the Vulkan registry: {unknown_bits:#x}")
            return value
        return names

    return apply_member_workaround(vk, json_data, convert)


def apply_enums_as_numbers_workaround(vk: VulkanObject, json_data: dict) -> list[str]:
    """
    Converts to enum value names the enum values of the property structures written as numbers or numeric strings,
    e.g. the 1000117000 of VkPhysicalDeviceHostImageCopyPropertiesEXT::pCopyDstLayouts, written by a device report
    exporter not knowing the names of the extension enum values, converted to
    'VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_STENCIL_ATTACHMENT_OPTIMAL'. Returns the description of each change.
    """
    enum_types = {name: enum for enum in vk.enums.values() for name in [enum.name] + enum.aliases}

    def convert(member, value):
        enum = enum_types.get(member.type)
        if enum is None:
            return value
        if isinstance(value, list):
            return [convert(member, item) for item in value]
        if isinstance(value, bool) or not isinstance(value, (int, str)):
            return value
        if isinstance(value, str) and not value.lstrip("-").isdigit():
            return value
        number = int(value)
        field = next((field for field in enum.fields if field.value == number), None)
        if field is None:
            logging.warning(f"Skipping the '{WorkaroundBits.ENUMS_AS_NUMBERS.value}' workaround of '{member.name}': {number} is not a '{member.type}' value of the Vulkan registry")
            return value
        return field.name

    return apply_member_workaround(vk, json_data, convert)


def apply_undeclared_extension_structures_workaround(vk: VulkanObject, json_data: dict) -> list[str]:
    """
    Removes the feature and property extension structures whose extension the capability block doesn't declare, and
    which aren't promoted to the Vulkan version of the profiles using the block, e.g. the
    VkPhysicalDeviceMaintenance7PropertiesKHR of the device reports not listing VK_KHR_maintenance7, written by a
    device report exporter not knowing the extension. Returns the description of each change.
    """
    profiles = json_data.get("profiles", {})
    profile_versions = [VK_VERSION.from_string(profile.get("api-version", "1.0.0")) for profile in profiles.values() if isinstance(profile, dict)]

    changes = []
    for block_name, block in json_data.get("capabilities", {}).items():
        if not isinstance(block, dict):
            continue
        # The structures promoted to the Vulkan version of every profile using the block are kept
        block_versions = [VK_VERSION.from_string(profile.get("api-version", "1.0.0")) for profile in profiles.values()
                          if isinstance(profile, dict) and block_name in collect_block_names(profile.get("capabilities"))]
        versions = block_versions or profile_versions
        api_version = min(versions) if versions else VK_VERSION.from_string("1.0.0")
        enabled_exts = set(block.get("extensions", {}).keys())
        for category in ("features", "properties"):
            structs = block.get(category)
            if not isinstance(structs, dict):
                continue
            for struct_name in list(structs.keys()):
                undeclared_ext = getUndeclaredStructExtension(vk, struct_name, api_version, enabled_exts)
                if undeclared_ext is None:
                    continue
                changes.append(f"capabilities -> {block_name} -> {category} -> {struct_name}: removed, '{undeclared_ext}' is not declared in extensions")
                del structs[struct_name]
    return changes


def apply_mismatching_aliases_workaround(vk: VulkanObject, json_data: dict, limit_members: bool = True, exact_booleans: bool = False) -> list[str]:
    """
    Sets the aliased property members with mismatching values to their least capable value, considering the capability
    not reliable enough to be supported:
    - With limit_members, the members with a 'max', 'min', 'bitmask' or 'range' limittype in vk.xml, e.g. false for the
      VkPhysicalDeviceVulkan13Properties::integerDotProduct8BitUnsignedAccelerated false and
      VkPhysicalDeviceShaderIntegerDotProductPropertiesKHR::integerDotProduct8BitUnsignedAccelerated true of some
      device reports.
    - With exact_booleans, the VkBool32 members with an 'exact' limittype, set to false, e.g. the
      uniformTexelBufferOffsetSingleTexelAlignment of VkPhysicalDeviceVulkan13Properties and
      VkPhysicalDeviceTexelBufferAlignmentPropertiesEXT.
    The other members with an 'exact' or 'noauto' limittype, or without limittype, are ignored.
    Returns the description of each change.
    """
    workaround_bit = WorkaroundBits.MISMATCHING_ALIASES if limit_members else WorkaroundBits.MISMATCHING_ALIASES_EXTRA
    structs_by_name = {name: struct for struct in vk.structs.values() for name in [struct.name] + struct.aliases}
    flags_by_name = {name: flags for flags in vk.flags.values() for name in [flags.name] + flags.aliases}

    def get_member(struct_name: str, member_name: str):
        struct = structs_by_name.get(struct_name)
        return next((member for member in struct.members if member.name == member_name), None) if struct else None

    def get_flag_bits(member) -> dict[str, int] | None:
        flags = flags_by_name.get(member.type)
        bitmask = vk.bitmasks.get(flags.bitmaskName) if flags is not None and flags.bitmaskName else None
        if bitmask is None:
            return None
        return {name: flag.value for flag in bitmask.flags for name in [flag.name] + flag.aliases}

    def get_least_capable(member, limittypes: list[str], val1, val2):
        """Returns the least capable of two values, or None when it can't be determined."""
        if "max" in limittypes or "min" in limittypes:
            pick = min if "max" in limittypes else max
            if member.type == "VkBool32" and isinstance(val1, bool) and isinstance(val2, bool):
                return (val1 and val2) if "max" in limittypes else (val1 or val2)
            if isinstance(val1, dict) and isinstance(val2, dict) and val1.keys() == val2.keys():
                return {key: pick(val1[key], val2[key]) for key in val1}
            if isinstance(val1, list) and isinstance(val2, list) and len(val1) == len(val2):
                return [pick(v1, v2) for v1, v2 in zip(val1, val2)]
            if isinstance(val1, (int, float)) and isinstance(val2, (int, float)) and not isinstance(val1, bool) and not isinstance(val2, bool):
                value = pick(val1, val2)
                # A 'mul' limit must be a multiple of both values
                if "mul" in limittypes and "min" in limittypes and value % min(val1, val2) != 0:
                    return None
                return value
            return None
        if "bitmask" in limittypes:
            flag_bits = get_flag_bits(member)
            if flag_bits is None or not isinstance(val1, list) or not isinstance(val2, list) or any(name not in flag_bits for name in val1 + val2):
                return None
            bits2 = 0
            for name in val2:
                bits2 |= flag_bits[name]
            # The flag bits of val1, including the multi-bit flags, which are all set in val2
            return [name for name in val1 if flag_bits[name] & ~bits2 == 0]
        if "range" in limittypes:
            if isinstance(val1, list) and isinstance(val2, list) and len(val1) == 2 and len(val2) == 2:
                return [max(val1[0], val2[0]), min(val1[1], val2[1])]
            return None
        return None

    def are_equal(member, limittypes: list[str], val1, val2) -> bool:
        if "bitmask" in limittypes:
            flag_bits = get_flag_bits(member)
            if flag_bits is not None and isinstance(val1, list) and isinstance(val2, list) and all(name in flag_bits for name in val1 + val2):
                bits1 = 0
                for name in val1:
                    bits1 |= flag_bits[name]
                bits2 = 0
                for name in val2:
                    bits2 |= flag_bits[name]
                return bits1 == bits2
        return val1 == val2

    changes = []
    for block_name, block in json_data.get("capabilities", {}).items():
        if not isinstance(block, dict):
            continue
        properties = block.get("properties")
        if not isinstance(properties, dict):
            continue
        checked = set()
        for struct_name, members in properties.items():
            if not isinstance(members, dict):
                continue
            for member_name in members:
                if (struct_name, member_name) in checked:
                    continue
                checked.add((struct_name, member_name))
                member = get_member(struct_name, member_name)
                if member is None or not member.limitType:
                    continue
                limittypes = [token.strip() for token in member.limitType.split(",")]
                is_exact_boolean = "exact" in limittypes and member.type == "VkBool32"
                if is_exact_boolean:
                    if not exact_booleans:
                        continue
                    # An exact boolean is least capable when false, like a max boolean
                    limittypes = ["max"]
                elif "exact" in limittypes or "noauto" in limittypes or not limit_members:
                    continue

                # The aliased members of the block, including this member
                aliased = [(struct_name, member_name)]
                for alias in gatherCapabilityAliases(vk, StructCapabilityAlias(struct_name, member_name)):
                    if not isinstance(alias, StructCapabilityAlias):
                        continue
                    checked.add((alias.struct, alias.member))
                    alias_members = properties.get(alias.struct)
                    if (alias.struct, alias.member) not in aliased and isinstance(alias_members, dict) and alias.member in alias_members:
                        aliased.append((alias.struct, alias.member))
                if len(aliased) < 2:
                    continue

                values = [properties[s][m] for s, m in aliased]
                if all(are_equal(member, limittypes, values[0], value) for value in values[1:]):
                    continue
                least_capable = values[0]
                for value in values[1:]:
                    least_capable = get_least_capable(member, limittypes, least_capable, value)
                    if least_capable is None:
                        break
                if least_capable is None:
                    logging.warning(f"Skipping the '{workaround_bit.value}' workaround of '{struct_name}::{member_name}': no least capable value for the '{member.limitType}' limittype between {', '.join(json.dumps(value) for value in values)}")
                    continue
                for (s, m), value in zip(aliased, values):
                    if value != least_capable:
                        changes.append(f"capabilities -> {block_name} -> properties -> {s} -> {m}: {json.dumps(value)} -> {json.dumps(least_capable)}")
                        properties[s][m] = copy.deepcopy(least_capable)
    return changes


def save_profiles_json_preserving_format(input_path: Path, output_path: Path, json_data: dict):
    """
    Writes a profiles file with an indentation of 4 spaces, preserving the line endings and the final line ending of
    the input profiles file.
    """
    with open(input_path, "r", encoding="utf-8", newline="") as f:
        original = f.read()
    newline = "\r\n" if "\r\n" in original else "\n"
    text = json.dumps(json_data, indent=4, ensure_ascii=False)
    if original.endswith(("\n", "\r")):
        text += "\n"
    with open(output_path, "w", encoding="utf-8", newline=newline) as f:
        f.write(text)


def apply_workarounds(vk: VulkanObject, input_path: Path, workaround_bits: list[WorkaroundBits], output_dir: Path | None = None) -> int:
    """
    Applies the workarounds to the profiles file or the profiles files of a directory. Without output_dir, each changed
    file is rewritten in place. With output_dir, every profiles file is written to output_dir, changed or not, so that
    output_dir replaces the input profiles files, which are left unchanged. Returns the number of changed files.
    """
    if input_path.is_file():
        paths = [input_path]
    else:
        paths = sorted(path for path in input_path.iterdir() if path.suffix == ".json")

    if output_dir is not None:
        output_dir.mkdir(parents=True, exist_ok=True)
        input_names = {path.name for path in paths}
        for stale_path in sorted(output_dir.glob("*.json")):
            if stale_path.name not in input_names:
                logging.warning(f"'{stale_path}' is not in the input '{input_path}': remove it if it's a previous output")

    changed_files = 0
    for path in paths:
        json_data = load_profiles_json(path)
        if json_data is None:
            continue

        changes = []
        # First, so that the other workarounds don't fix the removed structures
        if WorkaroundBits.UNDECLARED_EXTENSION_STRUCTURES in workaround_bits:
            changes.extend(apply_undeclared_extension_structures_workaround(vk, json_data))
        if WorkaroundBits.DRIVER_ID in workaround_bits:
            changes.extend(apply_driver_id_workaround(vk, json_data))
        if WorkaroundBits.STRINGS_64_BIT in workaround_bits:
            changes.extend(apply_64_bit_strings_workaround(vk, json_data))
        if WorkaroundBits.FLAGS_AS_NUMBERS in workaround_bits:
            changes.extend(apply_flags_as_numbers_workaround(vk, json_data))
        if WorkaroundBits.ENUMS_AS_NUMBERS in workaround_bits:
            changes.extend(apply_enums_as_numbers_workaround(vk, json_data))
        # Last, so that the values are compared once written as expected
        if WorkaroundBits.MISMATCHING_ALIASES in workaround_bits or WorkaroundBits.MISMATCHING_ALIASES_EXTRA in workaround_bits:
            changes.extend(apply_mismatching_aliases_workaround(vk, json_data,
                                                                limit_members=WorkaroundBits.MISMATCHING_ALIASES in workaround_bits,
                                                                exact_booleans=WorkaroundBits.MISMATCHING_ALIASES_EXTRA in workaround_bits))

        output_path = path if output_dir is None else output_dir / path.name
        if changes:
            save_profiles_json_preserving_format(path, output_path, json_data)
            changed_files += 1
            for change in changes:
                logging.info(f"'{path}': Applied workaround: {change}")
        elif output_path != path:
            shutil.copyfile(path, output_path)

    return changed_files
