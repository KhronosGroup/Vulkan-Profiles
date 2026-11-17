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

import argparse
import copy
import json
import logging
from pathlib import Path
import sys
import tempfile
import unittest

scripts_dir = Path(__file__).resolve().parent.parent
if str(scripts_dir) not in sys.path:
    sys.path.insert(0, str(scripts_dir))

import vkprofiles
from source.vulkan_object_utils import initVulkanObject
from source.generate_profiles_schema import VulkanProfilesSchemaGenerator2
from source.profiles_json_utils import validate_profiles_jsons_data

valid_profiles_file = scripts_dir.parent / "profiles" / "test" / "data" / "VP_LUNARG_test_api_1_0.json"


class TestValidateSchema(unittest.TestCase):
    """Verifies that 'vkprofiles validate' fails when a profiles file doesn't pass the schema validation."""
    registry_path = None

    @classmethod
    def setUpClass(cls):
        cls.temp_dir = tempfile.TemporaryDirectory()
        cls.temp_path = Path(cls.temp_dir.name)

        cls.schema_file = cls.temp_path / "profiles-0.8-latest.json"
        schema = VulkanProfilesSchemaGenerator2(initVulkanObject('vulkan', cls.registry_path)).schema
        with open(cls.schema_file, "w", encoding="utf-8") as f:
            json.dump(schema, f, indent=4)

        with open(valid_profiles_file, encoding="utf-8") as f:
            valid_data = json.load(f)

        cls.valid_dir = cls.temp_path / "valid"
        cls.valid_dir.mkdir()
        cls.valid_file = cls.valid_dir / "VP_LUNARG_test_valid.json"
        with open(cls.valid_file, "w", encoding="utf-8") as f:
            json.dump(valid_data, f, indent=4)
        # A JSON file which isn't a profiles file is skipped
        with open(cls.valid_dir / "not_a_profiles_file.json", "w", encoding="utf-8") as f:
            json.dump({"nodes": []}, f)

        # A feature value must be a boolean
        invalid_data = copy.deepcopy(valid_data)
        invalid_data["capabilities"]["invalid_block"] = {
            "features": {"VkPhysicalDeviceFeatures": {"depthClamp": "yes"}}
        }
        cls.mixed_dir = cls.temp_path / "mixed"
        cls.mixed_dir.mkdir()
        with open(cls.mixed_dir / "VP_LUNARG_test_valid.json", "w", encoding="utf-8") as f:
            json.dump(valid_data, f, indent=4)
        cls.invalid_file = cls.mixed_dir / "VP_LUNARG_test_invalid.json"
        with open(cls.invalid_file, "w", encoding="utf-8") as f:
            json.dump(invalid_data, f, indent=4)

    @classmethod
    def tearDownClass(cls):
        cls.temp_dir.cleanup()

    def run_validate(self, input_path: Path, schema: bool):
        args = ["validate", "--quiet", "--registry", self.registry_path, "--input", str(input_path)]
        if schema:
            args += ["--schema", str(self.schema_file)]
        vkprofiles.main(args)

    def assertValidateFails(self, input_path: Path, schema: bool):
        # vkprofiles.main() configures the logging, the logged errors are checked by test_invalid_files_list
        with self.assertRaises(SystemExit) as cm:
            self.run_validate(input_path, schema)
        self.assertEqual(cm.exception.code, 1)

    def test_valid_file(self):
        for schema in (False, True):
            with self.subTest(schema=schema):
                self.run_validate(self.valid_file, schema)

    def test_valid_dir(self):
        for schema in (False, True):
            with self.subTest(schema=schema):
                self.run_validate(self.valid_dir, schema)

    def test_invalid_file(self):
        for schema in (False, True):
            with self.subTest(schema=schema):
                self.assertValidateFails(self.invalid_file, schema)

    def test_invalid_file_in_dir(self):
        for schema in (False, True):
            with self.subTest(schema=schema):
                self.assertValidateFails(self.mixed_dir, schema)

    def test_missing_input(self):
        for schema in (False, True):
            with self.subTest(schema=schema):
                with self.assertRaises(SystemExit) as cm:
                    self.run_validate(self.temp_path / "missing.json", schema)
                self.assertEqual(cm.exception.code, 1)

    def test_invalid_schema_file(self):
        with self.assertRaises(SystemExit) as cm:
            vkprofiles.main(["validate", "--quiet", "--registry", self.registry_path,
                             "--input", str(self.valid_file), "--schema", str(self.valid_file)])
        self.assertEqual(cm.exception.code, 1)

    def test_invalid_files_list(self):
        with open(self.schema_file, encoding="utf-8") as f:
            schema = json.load(f)
        invalid_files = []
        with self.assertLogs(level=logging.ERROR) as logs:
            count = validate_profiles_jsons_data(self.mixed_dir, schema, logging.ERROR, invalid_files)
        self.assertTrue(any("Schema validation failed" in message for message in logs.output), logs.output)
        self.assertEqual(count, 1)
        self.assertEqual(invalid_files, [self.invalid_file])

        # Expected failures, e.g. when checking the schemas supported by a profile, are not reported as errors
        with self.assertNoLogs(level=logging.WARNING):
            count = validate_profiles_jsons_data(self.invalid_file, schema)
        self.assertEqual(count, 0)


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument(
        '--registry', '-r', action='store', required=False,
        help='Use specified registry file instead of vk.xml.'
    )

    args, unparsed = parser.parse_known_args()
    TestValidateSchema.registry_path = args.registry

    unittest.main(argv=[sys.argv[0]] + unparsed)
