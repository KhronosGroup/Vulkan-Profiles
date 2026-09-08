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
from pathlib import Path
from source.profiles_json_utils import load_profiles_jsons


class JsonConfig:
    def __init__(self, input_dir=None, input_profile_names=None, api_version=None, merge_mode='intersection'):
        self.name = "VP_merged"
        self.version = 1
        self.label = ""
        self.description = ""
        self.stage = "STABLE"
        self.date = ""
        self.api_version = ["1", "0", "0"]
        self.required_profiles = []
        self.input_jsons = []
        self.input_profile_names = []
        self.input_profile_values = []
        self.merge_mode = merge_mode

        if api_version:
            if isinstance(api_version, str):
                self.api_version = api_version.split('.')
            elif isinstance(api_version, (list, tuple)):
                self.api_version = [str(x) for x in api_version]

        if input_dir:
            self.load_inputs(input_dir, input_profile_names)

    def load_inputs(self, input_dir, input_profile_names=None):
        jsons_dict = load_profiles_jsons(Path(input_dir))
        target_names = [p.strip() for p in input_profile_names if p.strip()] if input_profile_names else []

        for file_path, json_data in jsons_dict.items():
            profiles = json_data.get("profiles", {})
            for p_name, p_val in profiles.items():
                if not target_names or p_name in target_names:
                    self.input_jsons.append(json_data)
                    self.input_profile_names.append(p_name)
                    self.input_profile_values.append(p_val)

        if (not self.api_version or self.api_version == ["1", "0", "0"]) and self.input_profile_values:
            first_api_ver = self.input_profile_values[0].get("api-version")
            if first_api_ver:
                self.api_version = str(first_api_ver).split('.')

    def apply_json_value(self, p_name, p_val):
        self.name = p_name
        if "version" in p_val:
            self.version = int(p_val["version"])
        if "label" in p_val:
            self.label = p_val["label"]
        if "description" in p_val:
            self.description = p_val["description"]
        if "status" in p_val:
            self.stage = p_val["status"]
        elif "stage" in p_val:
            self.stage = p_val["stage"]
        if "date" in p_val:
            self.date = p_val["date"]
        if "api-version" in p_val:
            api_ver = p_val["api-version"]
            if isinstance(api_ver, str):
                self.api_version = api_ver.split('.')
            elif isinstance(api_ver, (list, tuple)):
                self.api_version = [str(x) for x in api_ver]
        if "profiles" in p_val:
            self.required_profiles = list(p_val["profiles"])
            