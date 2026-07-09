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

import argparse
from pathlib import Path
import sys
import unittest

scripts_dir = Path(__file__).resolve().parent.parent
if str(scripts_dir) not in sys.path:
    sys.path.insert(0, str(scripts_dir))

import logging
import shutil

from source.profiles_parsing import OutputFormatType
from source.profiles_parsing import load_profiles_jsons
from source.profiles_parsing import save_profiles_jsons
from source.profiles_parsing import validate_profiles_json
from source.profiles_parsing import validate_profiles_jsons

class TestJsonMethods(unittest.TestCase):
    registry_path = None

    def test_load_profiles_jsons(self):
        repository_path = Path(__file__).resolve().parent.parent.parent
        profiles_files_dir = repository_path / "profiles/LunarG"

        jsons = load_profiles_jsons(profiles_files_dir)
        self.assertEqual(len(jsons), 2)

    def test_save_profiles_jsons(self):
        repository_path = Path(__file__).resolve().parent.parent.parent
        profiles_files_dir = repository_path / "profiles/LunarG"

        jsons_ref = load_profiles_jsons(profiles_files_dir)
        self.assertEqual(len(jsons_ref), 2)

        tmp_dir = repository_path / "_tmp"
        tmp_dir.mkdir(parents=True, exist_ok=True)

        save_profiles_jsons(jsons_ref, tmp_dir, OutputFormatType.FLATTEN)
        
        jsons_copy = load_profiles_jsons(tmp_dir)
        self.assertEqual(len(jsons_copy), 2)
        
        for ref_key in jsons_ref.keys():
            filename = Path(ref_key).name
            
            copy_key = None
            for search_key in jsons_copy.keys():
                if search_key.name == filename:
                    copy_key = search_key
                    break

            self.assertEqual(jsons_ref[ref_key], jsons_copy[copy_key])

        shutil.rmtree(tmp_dir, ignore_errors=True)

    def test_validate_profiles_json(self):
        repository_path = Path(__file__).resolve().parent.parent.parent
        profiles_files_file = repository_path / "profiles/LunarG/VP_LUNARG_minimum_requirements.json"
        profiles_schema_file = repository_path / "schema/profiles-0.8-latest.json"

        validated_json = validate_profiles_json(profiles_files_file, profiles_schema_file)
        self.assertEqual(validated_json, True)

    def test_validate_profiles_jsons(self):
        repository_path = Path(__file__).resolve().parent.parent.parent
        profiles_files_dir = repository_path / "profiles/LunarG"
        profiles_schema_file = repository_path / "schema/profiles-0.8-latest.json"

        validated_jsons = validate_profiles_jsons(profiles_files_dir, profiles_schema_file)
        self.assertEqual(validated_jsons, 2)


if __name__ == '__main__':
    logging.basicConfig(level=logging.FATAL, format='%(levelname)s: %(message)s')
    
    parser = argparse.ArgumentParser()

    parser.add_argument('--registry', '-r', action='store', required=True,
                        help='Use specified registry file instead of vk.xml (video.xml must be present in the same directory for video support).')

    args, unparsed = parser.parse_known_args()
    TestJsonMethods.registry_path = args.registry

    unittest.main(argv=[sys.argv[0]] + unparsed)

