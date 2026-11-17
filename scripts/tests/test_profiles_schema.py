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
from pathlib import Path
import sys
import unittest

scripts_dir = Path(__file__).resolve().parent.parent
if str(scripts_dir) not in sys.path:
    sys.path.insert(0, str(scripts_dir))

from vulkan_object import VulkanObject
from source.vulkan_object_utils import initVulkanObject
from source.profiles_json_utils import validate_profiles_json_data, SCHEMA_VERSION
from source.main_schema import upgrade_schema_json_data
from source.generate_profiles_schema import VulkanProfilesSchemaGenerator2


def make_depends_profiles_file() -> dict:
    return {
        "$schema": f"https://schema.khronos.org/vulkan/profiles-{SCHEMA_VERSION}-304.json#",
        "capabilities": {
            "baseline": {
                "features": {
                    "VkPhysicalDeviceVulkan14Features": {
                        "pushDescriptor": True
                    }
                }
            },
            "protected": {
                "depends": "VkPhysicalDeviceVulkan11Features::protectedMemory",
                "features": {
                    "VkPhysicalDeviceVulkan14Features": {
                        "pipelineProtectedAccess": True
                    }
                }
            }
        },
        "profiles": {
            "VP_LUNARG_test_schema": {
                "version": 1,
                "api-version": "1.4.304",
                "label": "Test schema",
                "description": "Test of the profiles schema",
                "capabilities": ["baseline", "protected"]
            }
        }
    }


class TestProfilesSchema(unittest.TestCase):
    registry_path = None

    def setUp(self):
        self.vk: VulkanObject = initVulkanObject('vulkan', self.registry_path)

    def test_schema_version(self):
        schema = VulkanProfilesSchemaGenerator2(self.vk).schema
        self.assertEqual(schema["$id"], f"https://schema.khronos.org/vulkan/profiles-{SCHEMA_VERSION}-{self.vk.headerVersion}.json#")

    def test_schema_depends(self):
        schema = VulkanProfilesSchemaGenerator2(self.vk).schema

        profile = make_depends_profiles_file()
        self.assertTrue(validate_profiles_json_data(profile, schema))

        invalid_profile = copy.deepcopy(profile)
        invalid_profile["capabilities"]["protected"]["depends"] = "VkPhysicalDeviceVulkan11Features::protectedMemory;"
        self.assertFalse(validate_profiles_json_data(invalid_profile, schema))

        invalid_profile["capabilities"]["protected"]["depends"] = 1
        self.assertFalse(validate_profiles_json_data(invalid_profile, schema))

    def test_schema_upgrade(self):
        schema = VulkanProfilesSchemaGenerator2(self.vk).schema
        old_schema = copy.deepcopy(schema)
        old_schema["$id"] = old_schema["$id"].replace(f"profiles-{SCHEMA_VERSION}-", "profiles-0.8.2-")
        del old_schema["properties"]["capabilities"]["additionalProperties"]["properties"]["depends"]
        old_profile_props = next(iter(old_schema["properties"]["profiles"]["patternProperties"].values()))["properties"]
        old_profile_props["optionals"]["description"] = "The list of optional capability sets that can be referenced by a profile."

        upgraded = upgrade_schema_json_data(old_schema)
        self.assertIn(f"profiles-{SCHEMA_VERSION}-", upgraded["$id"])
        self.assertEqual(
            upgraded["properties"]["capabilities"]["additionalProperties"]["properties"]["depends"],
            schema["properties"]["capabilities"]["additionalProperties"]["properties"]["depends"])

        # The upgraded schema is the generated schema
        self.assertEqual(json.dumps(upgraded, indent=4), json.dumps(schema, indent=4))
        self.assertTrue(old_profile_props["optionals"]["description"].startswith("Deprecated:"))


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument(
        '--registry', '-r', action='store', required=False,
        help='Use specified registry file instead of vk.xml.'
    )

    args, unparsed = parser.parse_known_args()
    TestProfilesSchema.registry_path = args.registry

    unittest.main(argv=[sys.argv[0]] + unparsed)
