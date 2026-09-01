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
from source.main_convert import (
    pull_required_capabilities_profiles_files
)


class TestConvertPullRequiredCapabilities(unittest.TestCase):
    registry_path = None

    def setUp(self):
        self.vk: VulkanObject = initVulkanObject('vulkan', self.registry_path)

    # -------------------------------------------------------------------------
    # Same-Version Extension Feature Pull Tests (Vulkan 1.0)
    # -------------------------------------------------------------------------

    def test_pull_required_capabilities_block_unique_vulkan10(self):
        """
        Verifies that required feature requirements satisfied by an enabled extension
        (e.g., VK_KHR_variable_pointers) are pulled into the single capability block.
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
                    }
                }
            }
        }"""

        try:
            original_data = json.loads(original_json_text)
        except json.JSONDecodeError as e:
            print(f"JSON syntax is incorrect: {e.msg} at line {e.lineno}, column {e.colno}")

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
                        "VkPhysicalDeviceVariablePointerFeaturesKHR": {
                            "variablePointersStorageBuffer": true
                        }
                    }
                }
            }
        }"""

        try:
            expected_data = json.loads(expected_json_text)
        except json.JSONDecodeError as e:
            print(f"JSON syntax is incorrect: {e.msg} at line {e.lineno}, column {e.colno}")

        json_files_dict = {"test_profile.json": original_data}
        pull_required_capabilities_profiles_files(self.vk, json_files_dict)

        self.assertEqual(json_files_dict["test_profile.json"], expected_data)

    def test_pull_required_capabilities_block_inheritance_vulkan10(self):
        """
        Verifies that feature requirements satisfied by extensions are evaluated
        across sequential capability blocks in the same profile.
        """
        original_json_text = """{
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8.0-106.json#",
            "profiles": {
                "VP_TEST_profile": {
                    "version": 1,
                    "api-version": "1.0.68",
                    "capabilities": [ "block_a", "block_b" ]
                }
            },
            "capabilities": {
                "block_a": {
                    "extensions": {
                        "VK_KHR_variable_pointers": 1
                    }
                },
                "block_b": {
                    "extensions": {
                        "VK_KHR_8bit_storage": 1
                    }
                }
            }
        }"""

        try:
            original_data = json.loads(original_json_text)
        except json.JSONDecodeError as e:
            print(f"JSON syntax is incorrect: {e.msg} at line {e.lineno}, column {e.colno}")

        expected_json_text = """{
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8.0-106.json#",
            "profiles": {
                "VP_TEST_profile": {
                    "version": 1,
                    "api-version": "1.0.68",
                    "capabilities": [ "block_a", "block_b" ]
                }
            },
            "capabilities": {
                "block_a": {
                    "extensions": {
                        "VK_KHR_variable_pointers": 1
                    },
                    "features": {
                        "VkPhysicalDeviceVariablePointerFeaturesKHR": {
                            "variablePointersStorageBuffer": true
                        }
                    }
                },
                "block_b": {
                    "extensions": {
                        "VK_KHR_8bit_storage": 1
                    },
                    "features": {
                        "VkPhysicalDevice8BitStorageFeaturesKHR": {
                            "storageBuffer8BitAccess": true
                        }
                    }
                }
            }
        }"""

        try:
            expected_data = json.loads(expected_json_text)
        except json.JSONDecodeError as e:
            print(f"JSON syntax is incorrect: {e.msg} at line {e.lineno}, column {e.colno}")

        json_files_dict = {"test_profile.json": original_data}
        pull_required_capabilities_profiles_files(self.vk, json_files_dict)

        self.assertEqual(json_files_dict["test_profile.json"], expected_data)

    def test_pull_required_capabilities_profile_inheritance_vulkan10(self):
        """
        Verifies feature requirements pulling across inherited parent profiles at the same API version (1.0).
        """
        original_json_text = """{
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8.0-106.json#",
            "profiles": {
                "VP_TEST_profile_a": {
                    "version": 1,
                    "api-version": "1.0.68",
                    "capabilities": [ "block_a" ]
                },
                "VP_TEST_profile_b": {
                    "version": 1,
                    "api-version": "1.0.68",
                    "profiles": [ "VP_TEST_profile_a" ],
                    "capabilities": [ "block_b" ]
                }
            },
            "capabilities": {
                "block_a": {
                    "extensions": {
                        "VK_KHR_variable_pointers": 1
                    }
                },
                "block_b": {
                    "extensions": {
                        "VK_KHR_8bit_storage": 1
                    }
                }
            }
        }"""

        try:
            original_data = json.loads(original_json_text)
        except json.JSONDecodeError as e:
            print(f"JSON syntax is incorrect: {e.msg} at line {e.lineno}, column {e.colno}")

        expected_json_text = """{
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8.0-106.json#",
            "profiles": {
                "VP_TEST_profile_a": {
                    "version": 1,
                    "api-version": "1.0.68",
                    "capabilities": [ "block_a" ]
                },
                "VP_TEST_profile_b": {
                    "version": 1,
                    "api-version": "1.0.68",
                    "profiles": [ "VP_TEST_profile_a" ],
                    "capabilities": [ "block_b" ]
                }
            },
            "capabilities": {
                "block_a": {
                    "extensions": {
                        "VK_KHR_variable_pointers": 1
                    },
                    "features": {
                        "VkPhysicalDeviceVariablePointerFeaturesKHR": {
                            "variablePointersStorageBuffer": true
                        }
                    }
                },
                "block_b": {
                    "extensions": {
                        "VK_KHR_8bit_storage": 1
                    },
                    "features": {
                        "VkPhysicalDevice8BitStorageFeaturesKHR": {
                            "storageBuffer8BitAccess": true
                        }
                    }
                }
            }
        }"""

        try:
            expected_data = json.loads(expected_json_text)
        except json.JSONDecodeError as e:
            print(f"JSON syntax is incorrect: {e.msg} at line {e.lineno}, column {e.colno}")

        json_files_dict = {"test_profile.json": original_data}
        pull_required_capabilities_profiles_files(self.vk, json_files_dict)

        self.assertEqual(json_files_dict["test_profile.json"], expected_data)

    # -------------------------------------------------------------------------
    # Cross-Version Transition Block Tests (API Version Upgrades)
    # -------------------------------------------------------------------------

    def test_pull_required_capabilities_profile_inheritance_vulkan10_to_vulkan11(self):
        """
        Verifies that when a Vulkan 1.1 child profile inherits from a Vulkan 1.0 parent profile,
        a transition capability block ('VP_TEST_profile_a_to_vulkan11') is dynamically generated
        and inserted at the start of the child profile's capability list.
        """
        original_json_text = """{
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8.0-106.json#",
            "profiles": {
                "VP_TEST_profile_a": {
                    "version": 1,
                    "api-version": "1.0.68",
                    "capabilities": [ "block_a" ]
                },
                "VP_TEST_profile_b": {
                    "version": 1,
                    "api-version": "1.1.106",
                    "profiles": [ "VP_TEST_profile_a" ],
                    "capabilities": [ "block_b" ]
                }
            },
            "capabilities": {
                "block_a": {
                    "extensions": {
                        "VK_KHR_variable_pointers": 1
                    }
                },
                "block_b": {
                    "extensions": {
                        "VK_KHR_8bit_storage": 1
                    }
                }
            }
        }"""

        try:
            original_data = json.loads(original_json_text)
        except json.JSONDecodeError as e:
            print(f"JSON syntax is incorrect: {e.msg} at line {e.lineno}, column {e.colno}")

        json_files_dict = {"test_profile.json": original_data}
        pull_required_capabilities_profiles_files(self.vk, json_files_dict)

        result_profile_b = json_files_dict["test_profile.json"]["profiles"]["VP_TEST_profile_b"]
        result_caps = json_files_dict["test_profile.json"]["capabilities"]

        transition_block_name = "VP_TEST_profile_a_to_vulkan11"
        self.assertEqual(result_profile_b["capabilities"][0], transition_block_name)
        self.assertIn(transition_block_name, result_caps)

    def test_pull_required_capabilities_profile_inheritance_vulkan11_to_vulkan12(self):
        """
        Verifies transition block generation when a Vulkan 1.2 child profile inherits
        from a Vulkan 1.1 parent profile containing extensions promoted to Vulkan 1.2 core.
        """
        original_json_text = """{
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8.0-131.json#",
            "profiles": {
                "VP_TEST_profile_a": {
                    "version": 1,
                    "api-version": "1.1.106",
                    "capabilities": [ "block_a" ]
                },
                "VP_TEST_profile_b": {
                    "version": 1,
                    "api-version": "1.2.131",
                    "profiles": [ "VP_TEST_profile_a" ],
                    "capabilities": [ "block_b" ]
                }
            },
            "capabilities": {
                "block_a": {
                    "extensions": {
                        "VK_KHR_draw_indirect_count": 1
                    }
                },
                "block_b": {
                    "extensions": {
                        "VK_KHR_present_wait": 1
                    }
                }
            }
        }"""

        try:
            original_data = json.loads(original_json_text)
        except json.JSONDecodeError as e:
            print(f"JSON syntax is incorrect: {e.msg} at line {e.lineno}, column {e.colno}")

        json_files_dict = {"test_profile.json": original_data}
        pull_required_capabilities_profiles_files(self.vk, json_files_dict)

        result_profile_b = json_files_dict["test_profile.json"]["profiles"]["VP_TEST_profile_b"]
        result_caps = json_files_dict["test_profile.json"]["capabilities"]

        transition_block_name = "VP_TEST_profile_a_to_vulkan12"
        self.assertEqual(result_profile_b["capabilities"][0], transition_block_name)
        self.assertIn(transition_block_name, result_caps)

    def test_pull_required_capabilities_profile_inheritance_vulkan11_to_vulkan13(self):
        """
        Verifies transition block generation for a Vulkan 1.1 to Vulkan 1.3 profile upgrade chain.
        """
        original_json_text = """{
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8.0-204.json#",
            "profiles": {
                "VP_TEST_profile_a": {
                    "version": 1,
                    "api-version": "1.1.106",
                    "capabilities": [ "block_a" ]
                },
                "VP_TEST_profile_b": {
                    "version": 1,
                    "api-version": "1.3.204",
                    "profiles": [ "VP_TEST_profile_a" ],
                    "capabilities": [ "block_b" ]
                }
            },
            "capabilities": {
                "block_a": {
                    "extensions": {
                        "VK_KHR_draw_indirect_count": 1
                    }
                },
                "block_b": {
                    "extensions": {
                        "VK_KHR_present_wait": 1
                    }
                }
            }
        }"""

        try:
            original_data = json.loads(original_json_text)
        except json.JSONDecodeError as e:
            print(f"JSON syntax is incorrect: {e.msg} at line {e.lineno}, column {e.colno}")

        json_files_dict = {"test_profile.json": original_data}
        pull_required_capabilities_profiles_files(self.vk, json_files_dict)

        result_profile_b = json_files_dict["test_profile.json"]["profiles"]["VP_TEST_profile_b"]
        result_caps = json_files_dict["test_profile.json"]["capabilities"]

        transition_block_name = "VP_TEST_profile_a_to_vulkan13"
        self.assertEqual(result_profile_b["capabilities"][0], transition_block_name)
        self.assertIn(transition_block_name, result_caps)

    def test_pull_required_capabilities_profile_inheritance_vulkan11_to_vulkan14(self):
        """
        Verifies transition block generation for a Vulkan 1.1 to Vulkan 1.4 profile upgrade chain.
        """
        original_json_text = """{
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8.0-304.json#",
            "profiles": {
                "VP_TEST_profile_a": {
                    "version": 1,
                    "api-version": "1.1.106",
                    "capabilities": [ "block_a" ]
                },
                "VP_TEST_profile_b": {
                    "version": 1,
                    "api-version": "1.4.304",
                    "profiles": [ "VP_TEST_profile_a" ],
                    "capabilities": [ "block_b" ]
                }
            },
            "capabilities": {
                "block_a": {
                    "extensions": {
                        "VK_KHR_draw_indirect_count": 1
                    }
                },
                "block_b": {
                    "extensions": {
                        "VK_KHR_present_wait": 1
                    }
                }
            }
        }"""

        try:
            original_data = json.loads(original_json_text)
        except json.JSONDecodeError as e:
            print(f"JSON syntax is incorrect: {e.msg} at line {e.lineno}, column {e.colno}")

        json_files_dict = {"test_profile.json": original_data}
        pull_required_capabilities_profiles_files(self.vk, json_files_dict)

        result_profile_b = json_files_dict["test_profile.json"]["profiles"]["VP_TEST_profile_b"]
        result_caps = json_files_dict["test_profile.json"]["capabilities"]

        transition_block_name = "VP_TEST_profile_a_to_vulkan14"
        self.assertEqual(result_profile_b["capabilities"][0], transition_block_name)
        self.assertIn(transition_block_name, result_caps)

    def test_pull_required_capabilities_profile_inheritance_vulkan11_to_vulkan12_to_vulkan14(self):
        """
        Verifies transition block generation across a three-level inheritance chain:
        Profile A (1.1) -> Profile B (1.2) -> Profile C (1.4).
        Ensures transition blocks are prepended for both version transitions:
        - 'VP_TEST_profile_a_to_vulkan12' in Profile B
        - 'VP_TEST_profile_b_to_vulkan14' in Profile C
        """
        original_json_text = """{
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8.0-304.json#",
            "profiles": {
                "VP_TEST_profile_a": {
                    "version": 1,
                    "api-version": "1.1.106",
                    "capabilities": [ "block_a" ]
                },
                "VP_TEST_profile_b": {
                    "version": 1,
                    "api-version": "1.2.131",
                    "profiles": [ "VP_TEST_profile_a" ],
                    "capabilities": [ "block_b" ]
                },
                "VP_TEST_profile_c": {
                    "version": 1,
                    "api-version": "1.4.304",
                    "profiles": [ "VP_TEST_profile_b" ],
                    "capabilities": [ "block_c" ]
                }
            },
            "capabilities": {
                "block_a": {
                    "extensions": {
                        "VK_KHR_draw_indirect_count": 1
                    }
                },
                "block_b": {
                    "extensions": {
                        "VK_KHR_timeline_semaphore": 1
                    }
                },
                "block_c": {
                    "extensions": {
                        "VK_KHR_present_wait": 1
                    }
                }
            }
        }"""

        try:
            original_data = json.loads(original_json_text)
        except json.JSONDecodeError as e:
            print(f"JSON syntax is incorrect: {e.msg} at line {e.lineno}, column {e.colno}")

        json_files_dict = {"test_profile.json": original_data}
        pull_required_capabilities_profiles_files(self.vk, json_files_dict)

        result_profiles = json_files_dict["test_profile.json"]["profiles"]
        result_caps = json_files_dict["test_profile.json"]["capabilities"]

        trans_b = "VP_TEST_profile_a_to_vulkan12"
        trans_c = "VP_TEST_profile_b_to_vulkan14"

        self.assertEqual(result_profiles["VP_TEST_profile_b"]["capabilities"][0], trans_b)
        self.assertEqual(result_profiles["VP_TEST_profile_c"]["capabilities"][0], trans_c)
        self.assertIn(trans_b, result_caps)
        self.assertIn(trans_c, result_caps)


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument(
        '--registry', '-r', action='store', required=False,
        help='Use specified registry file instead of vk.xml.'
    )

    args, unparsed = parser.parse_known_args()
    TestConvertPullRequiredCapabilities.registry_path = args.registry

    unittest.main(argv=[sys.argv[0]] + unparsed)
    