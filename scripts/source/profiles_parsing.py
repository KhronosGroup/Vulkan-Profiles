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

import os
import json
from pathlib import Path

def load_profiles_jsons(input_dir):
    if not isinstance(input_dir, Path):
        print('ERROR: No `input_dir` is not a Path type')
        exit()
    if input_dir is None:
        print('ERROR: No input directory set, use --input')
        exit()

    profiles_files_paths = []
    for pos_json in os.listdir(input_dir):
        if pos_json.endswith('.json'):
            full_path = input_dir / pos_json
            profiles_files_paths.append(full_path)

    json_files_dict = {}

    # Load the json files in the directory
    for i in range(len(profiles_files_paths)):
        with open(profiles_files_paths[i], "r", encoding="utf-8") as file:
            json_file_data = json.load(file)

            # Check the schema start with "https://schema.khronos.org/vulkan/profiles-0.8.", otherwise it's not a profiles file (or a valid one)
            if isinstance(json_file_data, dict) and "$schema" in json_file_data:
                schema_url = json_file_data["$schema"]

                if isinstance(schema_url, str) and schema_url.startswith("https://schema.khronos.org/vulkan/profiles-0.8"):
                    print(f"[DEBUG] Loading: {profiles_files_paths[i]}")
                    json_files_dict[profiles_files_paths[i]] = json_file_data

    return json_files_dict

def save_profiles_jsons(output_dir, json_files_dict):
    if not isinstance(output_dir, Path):
        print('ERROR: `output_dir` is not a Path type')
        exit()
    if output_dir is None:
        print('ERROR: No output directory set, use --output')
        exit()
    if not output_dir.exists():
        print(f"ERROR: {output_dir} doesn't exist")
        exit()
    if not output_dir.is_dir():
        print(f'ERROR: {output_dir} is not a directory')
        exit()
    
    for key, value in json_files_dict.items():
        output_file = output_dir / key.name
        with open(output_file, "w", encoding="utf-8") as file:
            json.dump(value, file, indent=4)
            
            
