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

from source.main_extract import extract_profile, ExtractMode


class TestExtract(unittest.TestCase):
    registry_path = None

    def test_extract_profile_reference_mode(self):
        """
        Verifies that extracting a profile in 'reference' mode retains external
        parent profile references and extracts only the specified target profile.
        """
        original_json_text = """{
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8.2-latest.json#",
            "profiles": {
                "VP_TEST_parent": {
                    "version": 1,
                    "api-version": "1.0.68",
                    "capabilities": ["parent_block"]
                },
                "VP_TEST_target": {
                    "version": 1,
                    "api-version": "1.1.0",
                    "profiles": ["VP_TEST_parent"],
                    "capabilities": ["target_block"]
                }
            },
            "capabilities": {
                "parent_block": {
                    "features": {
                        "VkPhysicalDeviceFeatures": {
                            "robustBufferAccess": true
                        }
                    }
                },
                "target_block": {
                    "features": {
                        "VkPhysicalDeviceFeatures": {
                            "fullDrawIndexUint32": true
                        }
                    }
                }
            }
        }"""

        expected_json_text = """{
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8.2-latest.json#",
            "profiles": {
                "VP_TEST_target": {
                    "version": 1,
                    "api-version": "1.1.0",
                    "profiles": ["VP_TEST_parent"],
                    "capabilities": ["target_block"]
                }
            },
            "capabilities": {
                "target_block": {
                    "features": {
                        "VkPhysicalDeviceFeatures": {
                            "fullDrawIndexUint32": true
                        }
                    }
                }
            }
        }"""

        input_data = json.loads(original_json_text)
        json_files_dict = {"input.json": input_data}
        result_data = extract_profile(json_files_dict, "VP_TEST_target", ExtractMode.REFERENCE)

        self.assertEqual(result_data, json.loads(expected_json_text))

    def test_extract_profile_pull_mode(self):
        """
        Verifies that extracting a profile in 'pull' mode incorporates required
        parent profiles and capability blocks directly into the target output.
        """
        original_json_text = """{
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8.2-latest.json#",
            "profiles": {
                "VP_TEST_parent": {
                    "version": 1,
                    "api-version": "1.0.68",
                    "capabilities": ["parent_block"]
                },
                "VP_TEST_target": {
                    "version": 1,
                    "api-version": "1.1.0",
                    "profiles": ["VP_TEST_parent"],
                    "capabilities": ["target_block"]
                }
            },
            "capabilities": {
                "parent_block": {
                    "features": {
                        "VkPhysicalDeviceFeatures": {
                            "robustBufferAccess": true
                        }
                    }
                },
                "target_block": {
                    "features": {
                        "VkPhysicalDeviceFeatures": {
                            "fullDrawIndexUint32": true
                        }
                    }
                }
            }
        }"""

        expected_json_text = """{
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8.2-latest.json#",
            "profiles": {
                "VP_TEST_target": {
                    "version": 1,
                    "api-version": "1.1.0",
                    "profiles": [],
                    "capabilities": ["VP_TEST_target_block"]
                }
            },
            "capabilities": {
                "VP_TEST_target_block": {
                    "features": {
                        "VkPhysicalDeviceFeatures": {
                            "robustBufferAccess": true,
                            "fullDrawIndexUint32": true
                        }
                    }
                }
            }
        }"""

        input_data = json.loads(original_json_text)
        json_files_dict = {"input.json": input_data}
        result_data = extract_profile(json_files_dict, "VP_TEST_target", ExtractMode.PULL)

        self.assertEqual(result_data, json.loads(expected_json_text))


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument(
        '--registry', '-r', action='store', required=False,
        help='Use specified registry file instead of vk.xml.'
    )

    args, unparsed = parser.parse_known_args()
    TestExtract.registry_path = args.registry

    unittest.main(argv=[sys.argv[0]] + unparsed)