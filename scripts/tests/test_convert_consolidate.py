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
import json
from pathlib import Path
import sys
import unittest

scripts_dir = Path(__file__).resolve().parent.parent
if str(scripts_dir) not in sys.path:
    sys.path.insert(0, str(scripts_dir))

from source.main_convert import consolidate_profiles_files


class TestConvertConsolidate(unittest.TestCase):

    def test_consolidate_mandatory_capability_blocks(self):
        """
        Verifies that consolidate_profiles_files combines all mandatory capability blocks 
        into a single consolidated block named <profile_name>_requirements.
        """
        original_json_text = """{
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8.0-106.json#",
            "profiles": {
                "VP_TEST_profile": {
                    "version": 1,
                    "api-version": "1.1.0",
                    "capabilities": ["block_a", "block_b"]
                }
            },
            "capabilities": {
                "block_a": {
                    "extensions": {
                        "VK_KHR_16bit_storage": 1
                    }
                },
                "block_b": {
                    "features": {
                        "VkPhysicalDevice16BitStorageFeatures": {
                            "storageBuffer16BitAccess": true
                        }
                    }
                }
            }
        }"""

        expected_json_text = """{
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8.0-106.json#",
            "profiles": {
                "VP_TEST_profile": {
                    "version": 1,
                    "api-version": "1.1.0",
                    "profiles": [],
                    "capabilities": ["VP_TEST_profile_requirements"]
                }
            },
            "capabilities": {
                "VP_TEST_profile_requirements": {
                    "extensions": {
                        "VK_KHR_16bit_storage": 1
                    },
                    "features": {
                        "VkPhysicalDevice16BitStorageFeatures": {
                            "storageBuffer16BitAccess": true
                        }
                    }
                }
            }
        }"""

        json_files_dict = {"test_profile.json": json.loads(original_json_text)}
        consolidate_profiles_files(json_files_dict)

        self.assertEqual(json_files_dict["test_profile.json"], json.loads(expected_json_text))


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument(
        '--registry', '-r', action='store', required=False,
        help='Use specified registry file instead of vk.xml.'
    )

    args, unparsed = parser.parse_known_args()
    TestConvertConsolidate.registry_path = args.registry

    unittest.main(argv=[sys.argv[0]] + unparsed)


