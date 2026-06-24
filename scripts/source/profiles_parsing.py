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
import json
import re
from pathlib import Path
from enum import StrEnum

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


def validate_profiles_jsons(json_data_dir: Path, json_schema_path: Path) -> int:
    schema_data = load_schema_json(json_schema_path)
    if schema_data is None:
        logging.error(f"Invalid profile file: {json_schema_path}")
        return 0
    
    profiles_files_paths = []
    for pos_json in os.listdir(json_data_dir):
        if pos_json.endswith('.json'):
            full_path = json_data_dir / pos_json
            profiles_files_paths.append(full_path)
            
    result = 0
    for i in range(len(profiles_files_paths)):
        json_data = load_profiles_json(profiles_files_paths[i])
        if json_data == None:
            logging.debug(f"Invalid profile file: {profiles_files_paths[i]}")
            continue
        
        if _validate_profiles_json_data(json_data, schema_data):
            result += 1
            
    return result


def load_schema_json(input_file):
    with open(input_file, "r", encoding="utf-8") as file:
        schema_file_data = json.load(file)

        # Check the schema start with "http://json-schema.org/draft-07/schema#", otherwise it's not a profiles file (or a valid one)
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

        # Check the schema start with "https://schema.khronos.org/vulkan/profiles-0.", otherwise it's not a profiles file (or a valid one)
        if isinstance(json_file_data, dict) and "$schema" in json_file_data:
            schema_url = json_file_data["$schema"]

            if isinstance(schema_url, str) and schema_url.startswith("https://schema.khronos.org/vulkan/profiles-0."):
                logging.debug(f"Loading: {input_file}")
                return json_file_data


def load_profiles_jsons(input_dir):
    if not isinstance(input_dir, Path):
        logging.error('`input_dir` is not a Path type')
        exit()
    if input_dir is None:
        logging.error('No input directory set, use --input')
        exit()

    profiles_files_paths = []
    for pos_json in os.listdir(input_dir):
        if pos_json.endswith('.json'):
            full_path = input_dir / pos_json
            profiles_files_paths.append(full_path)

    # Load the json files in the directory
    json_files_dict = {}
    for i in range(len(profiles_files_paths)):
        json_file_data = load_profiles_json(profiles_files_paths[i])
        if json_file_data:
            json_files_dict[profiles_files_paths[i]] = json_file_data

    return json_files_dict


class OutputFormatType(StrEnum):
    PRETTY = 'pretty'
    FLATTEN = 'flatten'


def save_profiles_jsons(json_files_dict, output_dir, format: OutputFormatType):
    if not isinstance(output_dir, Path):
        logging.error('`output_dir` is not a Path type')
        exit()
    if output_dir is None:
        logging.error('No output directory set, use --output')
        exit()
    if not output_dir.exists():
        logging.error(f"{output_dir} doesn't exist")
        exit()
    if not output_dir.is_dir():
        logging.error(f'{output_dir} is not a directory')
        exit()
    
    for key, value in json_files_dict.items():
        output_file = output_dir / key.name
        with open(output_file, "w", encoding="utf-8") as file:
            if format == OutputFormatType.FLATTEN:
                pretty_json = json.dumps(value, indent=4)
                # Updated Regex: [^\[\]{}] means "no brackets AND no curly braces"
                flat_json = re.sub(
                    r'\[([^\[\]{}]*?)\]', 
                    lambda m: '[' + re.sub(r'\s+', ' ', m.group(1)).strip() + ']', 
                    pretty_json
                )
                file.write(flat_json)
            else:
                json.dump(value, file, indent=4)
            
            
