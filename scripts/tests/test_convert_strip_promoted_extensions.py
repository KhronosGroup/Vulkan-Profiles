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
    strip_promoted_extensions_profiles_files
)


class TestConvertStripPromotedExtensions(unittest.TestCase):
    registry_path = None

    def setUp(self):
        self.vk: VulkanObject = initVulkanObject('vulkan', self.registry_path)

    # -------------------------------------------------------------------------
    # Vulkan 1.0 Tests (VK_KHR_variable_pointers)
    # -------------------------------------------------------------------------

    def test_strip_promoted_extensions_block_unique_vulkan10(self):
        """
        Verifies that for Vulkan 1.0 profiles, extensions promoted to Vulkan 1.1+
        (such as VK_KHR_variable_pointers) are not stripped.
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
                    }
                }
            }
        }"""

        try:
            expected_data = json.loads(expected_json_text)
        except json.JSONDecodeError as e:
            print(f"JSON syntax is incorrect: {e.msg} at line {e.lineno}, column {e.colno}")

        json_files_dict = {"test_profile.json": original_data}
        strip_promoted_extensions_profiles_files(self.vk, json_files_dict)

        self.assertEqual(json_files_dict["test_profile.json"], expected_data)


    def test_strip_promoted_extensions_block_inheritance_vulkan10(self):
        """
        Verifies that for Vulkan 1.0 profiles with block inheritance, extensions promoted
        to Vulkan 1.1+ (such as VK_KHR_get_physical_device_properties2 and VK_KHR_variable_pointers) are retained.
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
                        "VK_KHR_get_physical_device_properties2": 2
                    }
                },
                "block_b": {
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
                    "capabilities": [ "block_a", "block_b" ]
                }
            },
            "capabilities": {
                "block_a": {
                    "extensions": {
                        "VK_KHR_get_physical_device_properties2": 2
                    }
                },
                "block_b": {
                    "extensions": {
                        "VK_KHR_variable_pointers": 1
                    }
                }
            }
        }"""

        try:
            expected_data = json.loads(expected_json_text)
        except json.JSONDecodeError as e:
            print(f"JSON syntax is incorrect: {e.msg} at line {e.lineno}, column {e.colno}")

        json_files_dict = {"test_profile.json": original_data}
        strip_promoted_extensions_profiles_files(self.vk, json_files_dict)

        self.assertEqual(json_files_dict["test_profile.json"], expected_data)


    def test_strip_promoted_extensions_profile_inheritance_vulkan10(self):
        """
        Verifies that for Vulkan 1.0 profiles with profile inheritance, non-promoted extensions are retained across all profiles.
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
                    "capabilities": [ "block_b" ],
                    "profiles": [ "VP_TEST_profile_a" ]
                }
            },
            "capabilities": {
                "block_a": {
                    "extensions": {
                        "VK_KHR_get_physical_device_properties2": 1
                    }
                },
                "block_b": {
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
                "VP_TEST_profile_a": {
                    "version": 1,
                    "api-version": "1.0.68",
                    "capabilities": [ "block_a" ]
                },
                "VP_TEST_profile_b": {
                    "version": 1,
                    "api-version": "1.0.68",
                    "capabilities": [ "block_b" ],
                    "profiles": [ "VP_TEST_profile_a" ]
                }
            },
            "capabilities": {
                "block_a": {
                    "extensions": {
                        "VK_KHR_get_physical_device_properties2": 1
                    }
                },
                "block_b": {
                    "extensions": {
                        "VK_KHR_variable_pointers": 1
                    }
                }
            }
        }"""

        try:
            expected_data = json.loads(expected_json_text)
        except json.JSONDecodeError as e:
            print(f"JSON syntax is incorrect: {e.msg} at line {e.lineno}, column {e.colno}")

        json_files_dict = {"test_profile.json": original_data}
        strip_promoted_extensions_profiles_files(self.vk, json_files_dict)

        self.assertEqual(json_files_dict["test_profile.json"], expected_data)

    # -------------------------------------------------------------------------
    # Vulkan 1.1 Tests (VK_KHR_variable_pointers)
    # -------------------------------------------------------------------------

    def test_strip_promoted_extensions_block_unique_vulkan11(self):
        """
        Verifies that for Vulkan 1.1 profiles, extensions promoted to Vulkan 1.1 core
        (such as VK_KHR_variable_pointers) are stripped from capability blocks.
        """
        original_json_text = """{
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8.0-106.json#",
            "profiles": {
                "VP_TEST_profile": {
                    "version": 1,
                    "api-version": "1.1.106",
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
                    "api-version": "1.1.106",
                    "capabilities": ["baseline"]
                }
            },
            "capabilities": {
                "baseline": {}
            }
        }"""

        try:
            expected_data = json.loads(expected_json_text)
        except json.JSONDecodeError as e:
            print(f"JSON syntax is incorrect: {e.msg} at line {e.lineno}, column {e.colno}")

        json_files_dict = {"test_profile.json": original_data}
        strip_promoted_extensions_profiles_files(self.vk, json_files_dict)

        self.assertEqual(json_files_dict["test_profile.json"], expected_data)


    def test_strip_promoted_extensions_block_inheritance_vulkan11(self):
        """
        Verifies that for Vulkan 1.1 profiles with block inheritance, extensions promoted
        to Vulkan 1.1 core are stripped across all capability blocks.
        """
        original_json_text = """{
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8.0-106.json#",
            "profiles": {
                "VP_TEST_profile": {
                    "version": 1,
                    "api-version": "1.1.106",
                    "capabilities": [ "block_a", "block_b" ]
                }
            },
            "capabilities": {
                "block_a": {
                    "extensions": {
                        "VK_KHR_get_physical_device_properties2": 2
                    }
                },
                "block_b": {
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
                    "api-version": "1.1.106",
                    "capabilities": [ "block_a", "block_b" ]
                }
            },
            "capabilities": {
                "block_a": {},
                "block_b": {}
            }
        }"""

        try:
            expected_data = json.loads(expected_json_text)
        except json.JSONDecodeError as e:
            print(f"JSON syntax is incorrect: {e.msg} at line {e.lineno}, column {e.colno}")

        json_files_dict = {"test_profile.json": original_data}
        strip_promoted_extensions_profiles_files(self.vk, json_files_dict)

        self.assertEqual(json_files_dict["test_profile.json"], expected_data)


    def test_strip_promoted_extensions_profile_inheritance_vulkan11(self):
        """
        Verifies that for Vulkan 1.1 profiles with profile inheritance, extensions promoted
        to Vulkan 1.1 core are stripped from all profile blocks.
        """
        original_json_text = """{
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8.0-106.json#",
            "profiles": {
                "VP_TEST_profile_a": {
                    "version": 1,
                    "api-version": "1.1.106",
                    "capabilities": [ "block_a" ]
                },
                "VP_TEST_profile_b": {
                    "version": 1,
                    "api-version": "1.1.106",
                    "capabilities": [ "block_b" ],
                    "profiles": [ "VP_TEST_profile_a" ]
                }
            },
            "capabilities": {
                "block_a": {
                    "extensions": {
                        "VK_KHR_get_physical_device_properties2": 2
                    }
                },
                "block_b": {
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
                "VP_TEST_profile_a": {
                    "version": 1,
                    "api-version": "1.1.106",
                    "capabilities": [ "block_a" ]
                },
                "VP_TEST_profile_b": {
                    "version": 1,
                    "api-version": "1.1.106",
                    "capabilities": [ "block_b" ],
                    "profiles": [ "VP_TEST_profile_a" ]
                }
            },
            "capabilities": {
                "block_a": {},
                "block_b": {}
            }
        }"""

        try:
            expected_data = json.loads(expected_json_text)
        except json.JSONDecodeError as e:
            print(f"JSON syntax is incorrect: {e.msg} at line {e.lineno}, column {e.colno}")

        json_files_dict = {"test_profile.json": original_data}
        strip_promoted_extensions_profiles_files(self.vk, json_files_dict)

        self.assertEqual(json_files_dict["test_profile.json"], expected_data)

    # -------------------------------------------------------------------------
    # Vulkan 1.1 Advanced Tests (VK_KHR_swapchain_mutable_format)
    # -------------------------------------------------------------------------

    def test_strip_promoted_extensions_block_unique_vulkan11_advanced(self):
        """
        Verifies that non-promoted extensions (such as VK_KHR_swapchain_mutable_format under Vulkan 1.1)
        are not stripped from capability blocks.
        """
        original_json_text = """{
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8.0-106.json#",
            "profiles": {
                "VP_TEST_profile": {
                    "version": 1,
                    "api-version": "1.1.106",
                    "capabilities": ["baseline"]
                }
            },
            "capabilities": {
                "baseline": {
                    "extensions": {
                        "VK_KHR_swapchain_mutable_format": 1
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
                    "api-version": "1.1.106",
                    "capabilities": ["baseline"]
                }
            },
            "capabilities": {
                "baseline": {
                    "extensions": {
                        "VK_KHR_swapchain_mutable_format": 1
                    }
                }
            }
        }"""

        try:
            expected_data = json.loads(expected_json_text)
        except json.JSONDecodeError as e:
            print(f"JSON syntax is incorrect: {e.msg} at line {e.lineno}, column {e.colno}")

        json_files_dict = {"test_profile.json": original_data}
        strip_promoted_extensions_profiles_files(self.vk, json_files_dict)

        self.assertEqual(json_files_dict["test_profile.json"], expected_data)


    def test_strip_promoted_extensions_block_inheritance_vulkan11_advanced(self):
        """
        Verifies that non-promoted extensions (such as VK_KHR_swapchain and VK_KHR_swapchain_mutable_format)
        are retained in their respective capability blocks under Vulkan 1.1.
        """
        original_json_text = """{
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8.0-106.json#",
            "profiles": {
                "VP_TEST_profile": {
                    "version": 1,
                    "api-version": "1.1.106",
                    "capabilities": [ "block_a", "block_b" ]
                }
            },
            "capabilities": {
                "block_a": {
                    "extensions": {
                        "VK_KHR_swapchain": 70
                    }
                },
                "block_b": {
                    "extensions": {
                        "VK_KHR_swapchain_mutable_format": 1
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
                    "api-version": "1.1.106",
                    "capabilities": [ "block_a", "block_b" ]
                }
            },
            "capabilities": {
                "block_a": {
                    "extensions": {
                        "VK_KHR_swapchain": 70
                    }
                },
                "block_b": {
                    "extensions": {
                        "VK_KHR_swapchain_mutable_format": 1
                    }
                }
            }
        }"""

        try:
            expected_data = json.loads(expected_json_text)
        except json.JSONDecodeError as e:
            print(f"JSON syntax is incorrect: {e.msg} at line {e.lineno}, column {e.colno}")

        json_files_dict = {"test_profile.json": original_data}
        strip_promoted_extensions_profiles_files(self.vk, json_files_dict)

        self.assertEqual(json_files_dict["test_profile.json"], expected_data)


    def test_strip_promoted_extensions_profile_inheritance_vulkan11_advanced(self):
        """
        Verifies that non-promoted extensions are retained across parent and child profile capability blocks.
        """
        original_json_text = """{
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8.0-106.json#",
            "profiles": {
                "VP_TEST_profile_a": {
                    "version": 1,
                    "api-version": "1.1.106",
                    "capabilities": [ "block_a" ]
                },
                "VP_TEST_profile_b": {
                    "version": 1,
                    "api-version": "1.1.106",
                    "capabilities": [ "block_b" ],
                    "profiles": [ "VP_TEST_profile_a" ]
                }
            },
            "capabilities": {
                "block_a": {
                    "extensions": {
                        "VK_KHR_swapchain": 70
                    }
                },
                "block_b": {
                    "extensions": {
                        "VK_KHR_swapchain_mutable_format": 1
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
                    "api-version": "1.1.106",
                    "capabilities": [ "block_a" ]
                },
                "VP_TEST_profile_b": {
                    "version": 1,
                    "api-version": "1.1.106",
                    "capabilities": [ "block_b" ],
                    "profiles": [ "VP_TEST_profile_a" ]
                }
            },
            "capabilities": {
                "block_a": {
                    "extensions": {
                        "VK_KHR_swapchain": 70
                    }
                },
                "block_b": {
                    "extensions": {
                        "VK_KHR_swapchain_mutable_format": 1
                    }
                }
            }
        }"""

        try:
            expected_data = json.loads(expected_json_text)
        except json.JSONDecodeError as e:
            print(f"JSON syntax is incorrect: {e.msg} at line {e.lineno}, column {e.colno}")

        json_files_dict = {"test_profile.json": original_data}
        strip_promoted_extensions_profiles_files(self.vk, json_files_dict)

        self.assertEqual(json_files_dict["test_profile.json"], expected_data)


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument(
        '--registry', '-r', action='store', required=False,
        help='Use specified registry file instead of vk.xml.'
    )

    args, unparsed = parser.parse_known_args()
    TestConvertStripPromotedExtensions.registry_path = args.registry

    unittest.main(argv=[sys.argv[0]] + unparsed)
    