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

import logging
import os
import sys
import json
import re
from pathlib import Path
from enum import Enum
from typing import Any


def is_min_limit_property(prop_name: str) -> bool:
    """Returns True if smaller property value is a stricter requirement (min-type limit)."""
    if prop_name.startswith("min"):
        return True
    if prop_name in ("bufferImageGranularity", "pointSizeGranularity", "lineWidthGranularity", "nonCoherentAtomSize"):
        return True
    return False


def is_range_limit_property(prop_name: str) -> bool:
    """Returns True if property value is a min/max range tuple/list."""
    if prop_name.endswith("Range") or prop_name in ("pointSizeRange", "lineWidthRange", "viewportBoundsRange"):
        return True
    return False


def get_flag_sort_key(flag_str: str) -> tuple:
    """Sorts flag strings numerically by bit value (e.g., VK_SAMPLE_COUNT_1_BIT -> 4_BIT -> 8_BIT)."""
    if not isinstance(flag_str, str):
        return (2, 0, str(flag_str))
    match = re.search(r'_(\d+)_BIT', flag_str)
    if match:
        return (0, int(match.group(1)), flag_str)
    return (1, 0, flag_str)


def merge_capability_value(prop_name: str, val1: Any, val2: Any) -> Any:
    """Merges two capability values (e.g. profile requirement and core requirement), keeping the stricter requirement."""
    import copy
    if val1 is None:
        return copy.deepcopy(val2)
    if val2 is None:
        return copy.deepcopy(val1)

    # 1. Booleans
    if isinstance(val1, bool) and isinstance(val2, bool):
        return val1 or val2

    # 2. Integers and Floats
    if isinstance(val1, (int, float)) and isinstance(val2, (int, float)):
        if type(val1) is bool or type(val2) is bool:
            return bool(val1) or bool(val2)
        if is_min_limit_property(prop_name):
            return min(val1, val2)
        else:
            return max(val1, val2)

    # 3. Lists / Arrays
    if isinstance(val1, list) and isinstance(val2, list):
        if not val1:
            return copy.deepcopy(val2)
        if not val2:
            return copy.deepcopy(val1)

        # 3a. Range lists [min, max]
        if is_range_limit_property(prop_name) and len(val1) == 2 and len(val2) == 2:
            try:
                return [min(val1[0], val2[0]), max(val1[1], val2[1])]
            except TypeError:
                pass

        # 3b. Numeric arrays
        if all(isinstance(x, (int, float)) and not isinstance(x, bool) for x in val1) and \
           all(isinstance(x, (int, float)) and not isinstance(x, bool) for x in val2) and \
           len(val1) == len(val2):
            if is_min_limit_property(prop_name):
                return [min(a, b) for a, b in zip(val1, val2)]
            else:
                return [max(a, b) for a, b in zip(val1, val2)]

        # 3c. Flag / Enum string lists
        merged_list = list(val1)
        for item in val2:
            if item not in merged_list:
                merged_list.append(item)
        merged_list.sort(key=get_flag_sort_key)
        return merged_list

    # 4. Dicts
    if isinstance(val1, dict) and isinstance(val2, dict):
        merged_dict = copy.deepcopy(val1)
        for k, v in val2.items():
            if k in merged_dict:
                merged_dict[k] = merge_capability_value(k, merged_dict[k], v)
            else:
                merged_dict[k] = copy.deepcopy(v)
        return merged_dict

    return copy.deepcopy(val1)


def is_property_satisfied(actual_val: Any, required_val: Any, prop_name: str) -> bool:
    """Returns True if actual_val satisfies required_val according to Vulkan limit comparison rules."""
    if actual_val == required_val:
        return True
    if actual_val is None:
        return False
    if required_val is None:
        return True

    # 1. Booleans
    if isinstance(actual_val, bool) and isinstance(required_val, bool):
        return actual_val if required_val else True

    # 2. Integers and Floats
    if isinstance(actual_val, (int, float)) and isinstance(required_val, (int, float)):
        if type(actual_val) is bool or type(required_val) is bool:
            return bool(actual_val) if bool(required_val) else True
        if is_min_limit_property(prop_name):
            return actual_val <= required_val
        else:
            return actual_val >= required_val

    # 3. Lists / Arrays
    if isinstance(actual_val, list) and isinstance(required_val, list):
        if not required_val:
            return True
        if not actual_val:
            return False

        # 3a. Range lists [min, max]
        if is_range_limit_property(prop_name) and len(actual_val) == 2 and len(required_val) == 2:
            try:
                return actual_val[0] <= required_val[0] and actual_val[1] >= required_val[1]
            except TypeError:
                pass

        # 3b. Numeric arrays
        if all(isinstance(x, (int, float)) and not isinstance(x, bool) for x in actual_val) and \
           all(isinstance(x, (int, float)) and not isinstance(x, bool) for x in required_val) and \
           len(actual_val) == len(required_val):
            if is_min_limit_property(prop_name):
                return all(a <= r for a, r in zip(actual_val, required_val))
            else:
                return all(a >= r for a, r in zip(actual_val, required_val))

        # 3c. Flag / Enum string lists
        return set(required_val).issubset(set(actual_val))

    # 4. Dicts
    if isinstance(actual_val, dict) and isinstance(required_val, dict):
        for k, req_v in required_val.items():
            act_v = actual_val.get(k)
            if not is_property_satisfied(act_v, req_v, k):
                return False
        return True

    return False


def collect_block_names(json_capabilities) -> list[str]:
    """Flattens capability block strings and nested lists into a 1D list of block names."""
    block_names = []
    if not isinstance(json_capabilities, list):
        return block_names
    for value in json_capabilities:
        if isinstance(value, str):
            block_names.append(value)
        elif isinstance(value, list):
            for val in value:
                if isinstance(val, str):
                    block_names.append(val)
    return block_names


def parse_profile_capabilities(json_capabilities: list) -> list:
    """Parses profile capabilities, preserving nested capability sets while filtering string entries."""
    parsed = []
    if not isinstance(json_capabilities, list):
        return parsed
    for entry in json_capabilities:
        if isinstance(entry, str):
            parsed.append(entry)
        elif isinstance(entry, list):
            parsed.append([item for item in entry if isinstance(item, str)])
    return parsed


def deep_merge_dict(target: dict, source: dict):
    """Recursively merges source dict into target dict using capability limit comparison rules."""
    import copy
    for key, value in source.items():
        if key in target:
            if isinstance(target[key], dict) and isinstance(value, dict):
                deep_merge_dict(target[key], value)
            else:
                target[key] = merge_capability_value(key, target[key], value)
        else:
            target[key] = copy.deepcopy(value)


def strip_dict_duplication(target: dict, reference: dict):
    """Recursively removes key-value pairs from target dict that match reference dict."""
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


def get_profile_and_file_data(json_files_dict: dict, profile_name: str):
    """Searches loaded profile files for a given profile name and returns (profile_obj, json_file_data)."""
    for file_path, json_file_data in json_files_dict.items():
        if isinstance(json_file_data, dict) and "profiles" in json_file_data:
            if profile_name in json_file_data["profiles"]:
                return json_file_data["profiles"][profile_name], json_file_data
    return None, None


def get_primary_capability_block(profile_obj: dict, capabilities_dict: dict) -> dict | None:
    """Returns the primary (first mandatory) capability block dictionary for a profile."""
    caps = profile_obj.get("capabilities", [])
    for cap_item in caps:
        if isinstance(cap_item, str) and cap_item in capabilities_dict:
            return capabilities_dict[cap_item]
        elif isinstance(cap_item, list) and cap_item:
            if cap_item[0] in capabilities_dict:
                return capabilities_dict[cap_item[0]]
    return None


def get_topologically_sorted_file_keys(json_files_dict: dict) -> list:
    """Sorts file keys topologically based on profile inheritance graph ("profiles": [...])."""
    profile_to_file = {}
    for file_key, file_data in json_files_dict.items():
        if isinstance(file_data, dict) and "profiles" in file_data:
            for profile_name in file_data["profiles"].keys():
                profile_to_file[profile_name] = file_key

    adj = {fk: set() for fk in json_files_dict.keys()}
    in_degree = {fk: 0 for fk in json_files_dict.keys()}

    for file_key, file_data in json_files_dict.items():
        if not isinstance(file_data, dict) or "profiles" not in file_data:
            continue
        for profile_obj in file_data["profiles"].values():
            req_profiles = profile_obj.get("profiles", [])
            for parent_pname in req_profiles:
                parent_fk = profile_to_file.get(parent_pname)
                if parent_fk and parent_fk != file_key and file_key not in adj[parent_fk]:
                    adj[parent_fk].add(file_key)
                    in_degree[file_key] += 1

    queue = [fk for fk, deg in in_degree.items() if deg == 0]
    sorted_keys = []

    while queue:
        curr = queue.pop(0)
        sorted_keys.append(curr)
        for neighbor in adj[curr]:
            in_degree[neighbor] -= 1
            if in_degree[neighbor] == 0:
                queue.append(neighbor)

    for fk in json_files_dict.keys():
        if fk not in sorted_keys:
            sorted_keys.append(fk)

    return sorted_keys


def collect_required_profiles_capabilities_recursive(json_files_dict: dict, profile_names: list, visited: set = None) -> dict:
    """Recursively aggregates capability blocks across parent profile inheritance hierarchies."""
    if visited is None:
        visited = set()

    aggregated_caps = {}
    for pname in profile_names:
        if pname in visited:
            continue
        visited.add(pname)

        p_obj, p_file_data = get_profile_and_file_data(json_files_dict, pname)
        if not p_obj or not p_file_data:
            continue

        parent_profiles = p_obj.get("profiles", [])
        if parent_profiles:
            parent_caps = collect_required_profiles_capabilities_recursive(json_files_dict, parent_profiles, visited)
            deep_merge_dict(aggregated_caps, parent_caps)

        direct_caps = collect_profile_capabilities(json_files_dict, p_file_data, p_obj)
        deep_merge_dict(aggregated_caps, direct_caps)

    return aggregated_caps


def collect_required_profiles_capabilities(json_files_dict: dict, required_profile_names: list[str], visited_profiles: set[str] = None) -> dict:
    """Traverses required parent profile hierarchies and aggregates their capabilities."""
    return collect_required_profiles_capabilities_recursive(json_files_dict, required_profile_names, visited_profiles)


def collect_profile_capabilities(json_files_dict: dict, json_file_data: dict, profile_obj: dict) -> dict:
    """Aggregates all capabilities (parent profile requirements + local blocks) for a given profile."""
    required_profile_names = profile_obj.get("profiles", [])
    combined_caps = collect_required_profiles_capabilities(json_files_dict, required_profile_names)

    capabilities_dict = json_file_data.get("capabilities", {})
    parsed_caps = parse_profile_capabilities(profile_obj.get("capabilities", []))

    for item in parsed_caps:
        if isinstance(item, str) and item in capabilities_dict:
            deep_merge_dict(combined_caps, capabilities_dict[item])

    return combined_caps


def _validate_profiles_json_data(json_data, schema_data) -> bool:
    try:
        import jsonschema
        jsonschema.validate(json_data, schema_data)
        return True
    except jsonschema.exceptions.ValidationError as e:
        logging.info(f"Message: {e.message}")
        logging.info(f"Location in JSON: {' -> '.join([str(p) for p in e.path])}")
        logging.info(f"Invalid value/context: {e.instance}")
        return False
    except jsonschema.exceptions.SchemaError as e:
        logging.error(f"The provided schema itself is invalid: {e.message}")
        return False
    except ModuleNotFoundError:
        logging.warning("`jsonschema` module is not installed, schema validation skip")
        return False


def validate_profiles_json(json_data_path: Path, json_schema_path: Path) -> bool:
    schema_data = load_schema_json(json_schema_path)
    if schema_data is None:
        logging.error(f"[ERROR] Invalid profile file: {json_schema_path}")
        return False

    json_data = load_profiles_json(json_data_path)
    if json_data is None:
        logging.debug(f"[DEBUG] Invalid profile file: {json_data_path}")
        return False

    logging.info(f"Validating profile file: {json_data_path} with {json_schema_path}")

    return _validate_profiles_json_data(json_data, schema_data)


def validate_profiles_jsons_data(json_data_dir: Path, json_schema_data) -> int:
    profiles_files_paths = []
    if json_data_dir.is_file():
        profiles_files_paths.append(json_data_dir)
    elif json_data_dir.is_dir():
        for pos_json in os.listdir(json_data_dir):
            if pos_json.endswith('.json'):
                full_path = json_data_dir / pos_json
                profiles_files_paths.append(full_path)
            
    result = 0
    for i in range(len(profiles_files_paths)):
        json_data = load_profiles_json(profiles_files_paths[i])
        if json_data is None:
            logging.debug(f"Invalid profile file: {profiles_files_paths[i]}")
            continue
        
        if _validate_profiles_json_data(json_data, json_schema_data):
            result += 1
            
    return result


def validate_profiles_jsons(json_data_dir: Path, json_schema_path: Path) -> int:
    schema_data = load_schema_json(json_schema_path)
    if schema_data is None:
        logging.error(f"Invalid profile file: {json_schema_path}")
        return 0
            
    return validate_profiles_jsons_data(json_data_dir, schema_data)


def load_schema_json(input_file):
    with open(input_file, "r", encoding="utf-8") as file:
        schema_file_data = json.load(file)

        if isinstance(schema_file_data, dict) and "$schema" in schema_file_data:
            schema_url = schema_file_data["$schema"]
            id_url = schema_file_data["$id"]

            if isinstance(schema_url, str) and schema_url.startswith("http://json-schema.org/draft-07/schema#"):
                if isinstance(id_url, str) and id_url.startswith("https://schema.khronos.org/vulkan/profiles-0."):
                    logging.debug(f"Loading schema: {input_file}")
                    return schema_file_data


def load_profiles_json(input_file):
    with open(input_file, "r", encoding="utf-8") as file:
        json_file_data = json.load(file)

        if isinstance(json_file_data, dict) and "$schema" in json_file_data:
            schema_url = json_file_data["$schema"]

            if isinstance(schema_url, str) and schema_url.startswith("https://schema.khronos.org/vulkan/profiles-0."):
                logging.debug(f"Loading: {input_file}")
                return json_file_data


def load_profiles_jsons(input_dir):
    if isinstance(input_dir, str):
        input_dir = Path(input_dir)
    if input_dir is None:
        logging.error('No input directory or file set, use --input')
        sys.exit(1)
    if not isinstance(input_dir, Path):
        logging.error('`input_dir` is not a Path type')
        sys.exit(1)

    profiles_files_paths = []
    if input_dir.is_file():
        profiles_files_paths.append(input_dir)
    elif input_dir.is_dir():
        for pos_json in os.listdir(input_dir):
            if pos_json.endswith('.json'):
                full_path = input_dir / pos_json
                profiles_files_paths.append(full_path)
    else:
        logging.error(f"Input path '{input_dir}' does not exist")
        sys.exit(1)

    json_files_dict = {}
    for i in range(len(profiles_files_paths)):
        json_file_data = load_profiles_json(profiles_files_paths[i])
        if json_file_data:
            json_files_dict[profiles_files_paths[i]] = json_file_data

    return json_files_dict


class OutputFormatType(str, Enum):
    PRETTY = 'pretty'
    FLATTEN = 'flatten'


def save_profiles_jsons(json_files_dict, output_path, format: OutputFormatType):
    if isinstance(output_path, str):
        output_path = Path(output_path)
    if output_path is None:
        logging.error('No output path set, use --output')
        sys.exit(1)
    if not isinstance(output_path, Path):
        logging.error('`output_path` is not a Path type')
        sys.exit(1)

    def flatten_array(match):
        content = re.sub(r'\s+', ' ', match.group(1)).strip()
        return f"[ {content} ]" if content else "[]"

    if output_path.suffix == '.json' or (len(json_files_dict) == 1 and not output_path.is_dir()):
        output_path.parent.mkdir(parents=True, exist_ok=True)
        for value in json_files_dict.values():
            with open(output_path, "w", encoding="utf-8") as file:
                if format == OutputFormatType.FLATTEN:
                    pretty_json = json.dumps(value, indent=4)
                    flat_json = re.sub(r'\[([^\[\]{}]*?)\]', flatten_array, pretty_json)
                    file.write(flat_json)
                else:
                    json.dump(value, file, indent=4)
            break
    else:
        output_path.mkdir(parents=True, exist_ok=True)
        for key, value in json_files_dict.items():
            file_name = key.name if isinstance(key, Path) else Path(key).name
            out_file = output_path / file_name
            with open(out_file, "w", encoding="utf-8") as file:
                if format == OutputFormatType.FLATTEN:
                    pretty_json = json.dumps(value, indent=4)
                    flat_json = re.sub(r'\[([^\[\]{}]*?)\]', flatten_array, pretty_json)
                    file.write(flat_json)
                else:
                    json.dump(value, file, indent=4)
                    