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

from vulkan_object import VulkanObject
from source.vulkan_object_utils import initVulkanObject
from source.transform_strip_unratified import strip_unratified_extensions_profiles_files


class TestConvertStripUnratifiedExtensions(unittest.TestCase):
    registry_path = None

    def setUp(self):
        self.vk: VulkanObject = initVulkanObject('vulkan', self.registry_path)

    def test_strip_unratified_extensions_ratified_retained(self):
        """
        Verifies that ratified extensions (e.g., VK_KHR_variable_pointers) and their
        structures (e.g., VkPhysicalDeviceVariablePointersFeatures) are retained.
        """
        original_json_text = """{
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8.0-106.json#",
            "profiles": {
                "VP_TEST_profile": {
                    "version": 1,
                    "api-version": "1.0.68",
                    "capabilities": ["baseline"]
                }
            },
            "capabilities": {
                "baseline": {
                    "extensions": {
                        "VK_KHR_variable_pointers": 1
                    },
                    "features": {
                        "VkPhysicalDeviceVariablePointersFeatures": {
                            "variablePointers": true
                        }
                    }
                }
            }
        }"""

        expected_json_text = original_json_text

        original_data = json.loads(original_json_text)
        expected_data = json.loads(expected_json_text)

        json_files_dict = {"test_profile.json": original_data}
        strip_unratified_extensions_profiles_files(self.vk, json_files_dict)

        self.assertEqual(json_files_dict["test_profile.json"], expected_data)

    def test_strip_unratified_extensions_unratified_removed(self):
        """
        Verifies that unratified extensions (e.g., VK_AMD_device_coherent_memory) and their
        defining structures (e.g., VkPhysicalDeviceCoherentMemoryFeaturesAMD) are removed.
        """
        original_json_text = """{
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8.0-106.json#",
            "profiles": {
                "VP_TEST_profile": {
                    "version": 1,
                    "api-version": "1.0.68",
                    "capabilities": ["baseline"]
                }
            },
            "capabilities": {
                "baseline": {
                    "extensions": {
                        "VK_AMD_device_coherent_memory": 1
                    },
                    "features": {
                        "VkPhysicalDeviceCoherentMemoryFeaturesAMD": {
                            "deviceCoherentMemory": true
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
                    "api-version": "1.0.68",
                    "capabilities": ["baseline"]
                }
            },
            "capabilities": {
                "baseline": {}
            }
        }"""

        original_data = json.loads(original_json_text)
        expected_data = json.loads(expected_json_text)

        json_files_dict = {"test_profile.json": original_data}
        strip_unratified_extensions_profiles_files(self.vk, json_files_dict)

        self.assertEqual(json_files_dict["test_profile.json"], expected_data)

    def test_strip_unratified_extensions_mixed(self):
        """
        Verifies that in a block with mixed extensions, unratified ones and their structures
        are stripped while ratified extensions and their structures remain intact.
        """
        original_json_text = """{
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8.0-106.json#",
            "profiles": {
                "VP_TEST_profile": {
                    "version": 1,
                    "api-version": "1.0.68",
                    "capabilities": ["baseline"]
                }
            },
            "capabilities": {
                "baseline": {
                    "extensions": {
                        "VK_KHR_variable_pointers": 1,
                        "VK_AMD_device_coherent_memory": 1
                    },
                    "features": {
                        "VkPhysicalDeviceVariablePointersFeatures": {
                            "variablePointers": true
                        },
                        "VkPhysicalDeviceCoherentMemoryFeaturesAMD": {
                            "deviceCoherentMemory": true
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
                    "api-version": "1.0.68",
                    "capabilities": ["baseline"]
                }
            },
            "capabilities": {
                "baseline": {
                    "extensions": {
                        "VK_KHR_variable_pointers": 1
                    },
                    "features": {
                        "VkPhysicalDeviceVariablePointersFeatures": {
                            "variablePointers": true
                        }
                    }
                }
            }
        }"""

        original_data = json.loads(original_json_text)
        expected_data = json.loads(expected_json_text)

        json_files_dict = {"test_profile.json": original_data}
        strip_unratified_extensions_profiles_files(self.vk, json_files_dict)

        self.assertEqual(json_files_dict["test_profile.json"], expected_data)


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument(
        '--registry', '-r', action='store', required=False,
        help='Use specified registry file instead of vk.xml.'
    )

    args, unparsed = parser.parse_known_args()
    TestConvertStripUnratifiedExtensions.registry_path = args.registry

    unittest.main(argv=[sys.argv[0]] + unparsed)