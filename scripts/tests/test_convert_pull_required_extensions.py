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
    pull_profiles_files_dependencies
)


class TestConvertPullExtensionsDependencies(unittest.TestCase):
    registry_path = None

    def setUp(self):
        self.vk: VulkanObject = initVulkanObject('vulkan', self.registry_path)

    # -------------------------------------------------------------------------
    # Vulkan 1.0 Tests (VK_KHR_variable_pointers)
    # -------------------------------------------------------------------------

    def test_pull_extension_dependencies_block_unique_vulkan10(self):
        """
        Verifies that dependent extensions (e.g. VK_KHR_get_physical_device_properties2 and
        VK_KHR_storage_buffer_storage_class for VK_KHR_variable_pointers) are automatically pulled in.
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
                        "VK_KHR_get_physical_device_properties2": 2,
                        "VK_KHR_storage_buffer_storage_class": 1,
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
        pull_profiles_files_dependencies(self.vk, False, json_files_dict)

        self.assertEqual(json_files_dict["test_profile.json"], expected_data)


    def test_pull_extension_dependencies_block_inheritance_vulkan10(self):
        """
        Verifies that dependent extensions (e.g. VK_KHR_get_physical_device_properties2 and
        VK_KHR_storage_buffer_storage_class for VK_KHR_variable_pointers) are automatically pulled in across capability blocks.
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
                        "VK_KHR_storage_buffer_storage_class": 1,
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
        pull_profiles_files_dependencies(self.vk, False, json_files_dict)

        self.assertEqual(json_files_dict["test_profile.json"], expected_data)


    def test_pull_extension_dependencies_profile_inheritance_vulkan10(self):
        """
        Verifies that dependent extensions (e.g. VK_KHR_get_physical_device_properties2 and
        VK_KHR_storage_buffer_storage_class for VK_KHR_variable_pointers) are automatically pulled in across parent profiles.
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
                        "VK_KHR_get_physical_device_properties2": 2
                    }
                },
                "block_b": {
                    "extensions": {
                        "VK_KHR_storage_buffer_storage_class": 1,
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
        pull_profiles_files_dependencies(self.vk, False, json_files_dict)

        self.assertEqual(json_files_dict["test_profile.json"], expected_data)

    # -------------------------------------------------------------------------
    # Vulkan 1.1 Tests (VK_KHR_variable_pointers - Core Promotion Suppression)
    # -------------------------------------------------------------------------

    def test_pull_extension_dependencies_block_unique_vulkan11(self):
        """
        Verifies that extensions promoted to Vulkan 1.1 core (e.g. VK_KHR_get_physical_device_properties2
        for VK_KHR_variable_pointers) are not pulled in as extension dependencies under Vulkan 1.1.
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
        pull_profiles_files_dependencies(self.vk, False, json_files_dict)

        self.assertEqual(json_files_dict["test_profile.json"], expected_data)


    def test_pull_extension_dependencies_block_inheritance_vulkan11(self):
        """
        Verifies that extensions promoted to Vulkan 1.1 core are not pulled in as extension dependencies
        across capability blocks for Vulkan 1.1.
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
                "block_a": {
                    "extensions": {
                        "VK_KHR_get_physical_device_properties2": 2
                    }
                },
                "block_b": {
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
        pull_profiles_files_dependencies(self.vk, False, json_files_dict)

        self.assertEqual(json_files_dict["test_profile.json"], expected_data)


    def test_pull_extension_dependencies_profile_inheritance_vulkan11(self):
        """
        Verifies that extensions promoted to Vulkan 1.1 core are not pulled in as extension dependencies
        across parent profiles for Vulkan 1.1.
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
                "block_a": {
                    "extensions": {
                        "VK_KHR_get_physical_device_properties2": 2
                    }
                },
                "block_b": {
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
        pull_profiles_files_dependencies(self.vk, False, json_files_dict)

        self.assertEqual(json_files_dict["test_profile.json"], expected_data)

    # -------------------------------------------------------------------------
    # Vulkan 1.1 Advanced Tests (VK_KHR_swapchain_mutable_format)
    # -------------------------------------------------------------------------

    def test_pull_extension_dependencies_block_unique_vulkan11_advanced(self):
        """
        Verifies that non-promoted dependent extensions (VK_KHR_surface, VK_KHR_swapchain,
        and VK_KHR_image_format_list for VK_KHR_swapchain_mutable_format) are automatically pulled in under Vulkan 1.1.
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
                        "VK_KHR_surface": 25,
                        "VK_KHR_swapchain": 70,
                        "VK_KHR_image_format_list": 1,
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
        pull_profiles_files_dependencies(self.vk, False, json_files_dict)

        self.assertEqual(json_files_dict["test_profile.json"], expected_data)


    def test_pull_extension_dependencies_block_inheritance_vulkan11_advanced(self):
        """
        Verifies that dependent extensions already listed in a preceding capability block (e.g. VK_KHR_swapchain &
        VK_KHR_surface in block_a) are omitted when pulling dependencies for VK_KHR_swapchain_mutable_format in block_b.
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
                        "VK_KHR_surface": 25,
                        "VK_KHR_swapchain": 70
                    }
                },
                "block_b": {
                    "extensions": {
                        "VK_KHR_image_format_list": 1,
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
        pull_profiles_files_dependencies(self.vk, False, json_files_dict)

        self.assertEqual(json_files_dict["test_profile.json"], expected_data)


    def test_pull_extension_dependencies_profile_inheritance_vulkan11_advanced(self):
        """
        Verifies that dependent extensions already inherited from a parent profile (e.g. VK_KHR_swapchain &
        VK_KHR_surface from VP_TEST_profile_a) are omitted when pulling dependencies for VK_KHR_swapchain_mutable_format in VP_TEST_profile_b.
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
                        "VK_KHR_surface": 25,
                        "VK_KHR_swapchain": 70
                    }
                },
                "block_b": {
                    "extensions": {
                        "VK_KHR_image_format_list": 1,
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
        pull_profiles_files_dependencies(self.vk, False, json_files_dict)

        self.assertEqual(json_files_dict["test_profile.json"], expected_data)

    # -------------------------------------------------------------------------
    # Vulkan 1.2 Tests (VK_KHR_timeline_semaphore - Core Promotion Suppression)
    # -------------------------------------------------------------------------

    def test_pull_extension_dependencies_block_unique_vulkan12(self):
        """
        Verifies that extensions promoted to Vulkan 1.2 core (e.g. VK_KHR_get_physical_device_properties2
        for VK_KHR_timeline_semaphore) are not pulled in as extension dependencies under Vulkan 1.2.
        """
        original_json_text = """{
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8.0-131.json#",
            "profiles": {
                "VP_TEST_profile": {
                    "version": 1,
                    "api-version": "1.2.131",
                    "capabilities": ["baseline"]
                }
            },
            "capabilities": {
                "baseline": {
                    "extensions": {
                        "VK_KHR_timeline_semaphore": 1
                    }
                }
            }
        }"""

        try:
            original_data = json.loads(original_json_text)
        except json.JSONDecodeError as e:
            print(f"JSON syntax is incorrect: {e.msg} at line {e.lineno}, column {e.colno}")

        expected_json_text = """{
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8.0-131.json#",
            "profiles": {
                "VP_TEST_profile": {
                    "version": 1,
                    "api-version": "1.2.131",
                    "capabilities": ["baseline"]
                }
            },
            "capabilities": {
                "baseline": {
                    "extensions": {
                        "VK_KHR_timeline_semaphore": 2
                    },
                    "features": {
                        "VkPhysicalDeviceTimelineSemaphoreFeaturesKHR": {
                            "timelineSemaphore": true
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
        pull_profiles_files_dependencies(self.vk, False, json_files_dict)

        self.assertEqual(json_files_dict["test_profile.json"], expected_data)


    def test_pull_extension_dependencies_block_inheritance_vulkan12(self):
        """
        Verifies that extensions promoted to Vulkan 1.2 core are not pulled in as extension dependencies
        across capability blocks for Vulkan 1.2.
        """
        original_json_text = """{
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8.0-131.json#",
            "profiles": {
                "VP_TEST_profile": {
                    "version": 1,
                    "api-version": "1.2.131",
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
                        "VK_KHR_timeline_semaphore": 1
                    }
                }
            }
        }"""

        try:
            original_data = json.loads(original_json_text)
        except json.JSONDecodeError as e:
            print(f"JSON syntax is incorrect: {e.msg} at line {e.lineno}, column {e.colno}")

        expected_json_text = """{
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8.0-131.json#",
            "profiles": {
                "VP_TEST_profile": {
                    "version": 1,
                    "api-version": "1.2.131",
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
                        "VK_KHR_timeline_semaphore": 2
                    },
                    "features": {
                        "VkPhysicalDeviceTimelineSemaphoreFeaturesKHR": {
                            "timelineSemaphore": true
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
        pull_profiles_files_dependencies(self.vk, False, json_files_dict)

        self.assertEqual(json_files_dict["test_profile.json"], expected_data)


    def test_pull_extension_dependencies_profile_inheritance_vulkan12(self):
        """
        Verifies that extensions promoted to Vulkan 1.2 core are not pulled in as extension dependencies
        across parent profiles for Vulkan 1.2.
        """
        original_json_text = """{
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8.0-131.json#",
            "profiles": {
                "VP_TEST_profile_a": {
                    "version": 1,
                    "api-version": "1.2.131",
                    "capabilities": [ "block_a" ]
                },
                "VP_TEST_profile_b": {
                    "version": 1,
                    "api-version": "1.2.131",
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
                        "VK_KHR_timeline_semaphore": 1
                    }
                }
            }
        }"""

        try:
            original_data = json.loads(original_json_text)
        except json.JSONDecodeError as e:
            print(f"JSON syntax is incorrect: {e.msg} at line {e.lineno}, column {e.colno}")

        expected_json_text = """{
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8.0-131.json#",
            "profiles": {
                "VP_TEST_profile_a": {
                    "version": 1,
                    "api-version": "1.2.131",
                    "capabilities": [ "block_a" ]
                },
                "VP_TEST_profile_b": {
                    "version": 1,
                    "api-version": "1.2.131",
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
                        "VK_KHR_timeline_semaphore": 2
                    },
                    "features": {
                        "VkPhysicalDeviceTimelineSemaphoreFeaturesKHR": {
                            "timelineSemaphore": true
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
        pull_profiles_files_dependencies(self.vk, False, json_files_dict)

        self.assertEqual(json_files_dict["test_profile.json"], expected_data)

    # -------------------------------------------------------------------------
    # Vulkan 1.2 Advanced Tests (VK_KHR_present_wait)
    # -------------------------------------------------------------------------

    def test_pull_extension_dependencies_block_unique_vulkan12_advanced(self):
        """
        Verifies that non-promoted dependent extensions (VK_KHR_present_id, VK_KHR_swapchain, VK_KHR_surface)
        and required features (presentWait, presentId) are transitively pulled in under Vulkan 1.2.
        """
        original_json_text = """{
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8.0-131.json#",
            "profiles": {
                "VP_TEST_profile": {
                    "version": 1,
                    "api-version": "1.2.131",
                    "capabilities": ["baseline"]
                }
            },
            "capabilities": {
                "baseline": {
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

        expected_json_text = """{
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8.0-131.json#",
            "profiles": {
                "VP_TEST_profile": {
                    "version": 1,
                    "api-version": "1.2.131",
                    "capabilities": ["baseline"]
                }
            },
            "capabilities": {
                "baseline": {
                    "extensions": {
                        "VK_KHR_present_id": 1,
                        "VK_KHR_present_wait": 1,
                        "VK_KHR_surface": 25,
                        "VK_KHR_swapchain": 70
                    },
                    "features": {
                        "VkPhysicalDevicePresentIdFeaturesKHR": {
                            "presentId": true
                        },
                        "VkPhysicalDevicePresentWaitFeaturesKHR": {
                            "presentWait": true
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
        pull_profiles_files_dependencies(self.vk, False, json_files_dict)

        self.assertEqual(json_files_dict["test_profile.json"], expected_data)


    def test_pull_extension_dependencies_block_inheritance_vulkan12_advanced(self):
        """
        Verifies that dependent extensions already listed in a preceding capability block (e.g. VK_KHR_swapchain &
        VK_KHR_surface in block_a) are omitted when pulling dependencies for VK_KHR_present_wait in block_b.
        """
        original_json_text = """{
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8.0-131.json#",
            "profiles": {
                "VP_TEST_profile": {
                    "version": 1,
                    "api-version": "1.2.131",
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
                        "VK_KHR_present_wait": 1
                    }
                }
            }
        }"""

        try:
            original_data = json.loads(original_json_text)
        except json.JSONDecodeError as e:
            print(f"JSON syntax is incorrect: {e.msg} at line {e.lineno}, column {e.colno}")

        expected_json_text = """{
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8.0-131.json#",
            "profiles": {
                "VP_TEST_profile": {
                    "version": 1,
                    "api-version": "1.2.131",
                    "capabilities": [ "block_a", "block_b" ]
                }
            },
            "capabilities": {
                "block_a": {
                    "extensions": {
                        "VK_KHR_surface": 25,
                        "VK_KHR_swapchain": 70
                    }
                },
                "block_b": {
                    "extensions": {
                        "VK_KHR_present_id": 1,
                        "VK_KHR_present_wait": 1
                    },
                    "features": {
                        "VkPhysicalDevicePresentIdFeaturesKHR": {
                            "presentId": true
                        },
                        "VkPhysicalDevicePresentWaitFeaturesKHR": {
                            "presentWait": true
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
        pull_profiles_files_dependencies(self.vk, False, json_files_dict)

        self.assertEqual(json_files_dict["test_profile.json"], expected_data)


    def test_pull_extension_dependencies_profile_inheritance_vulkan12_advanced(self):
        """
        Verifies that dependent extensions already inherited from a parent profile (e.g. VK_KHR_swapchain &
        VK_KHR_surface from VP_TEST_profile_a) are omitted when pulling dependencies for VK_KHR_present_wait in VP_TEST_profile_b.
        """
        original_json_text = """{
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8.0-131.json#",
            "profiles": {
                "VP_TEST_profile_a": {
                    "version": 1,
                    "api-version": "1.2.131",
                    "capabilities": [ "block_a" ]
                },
                "VP_TEST_profile_b": {
                    "version": 1,
                    "api-version": "1.2.131",
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
                        "VK_KHR_present_wait": 1
                    }
                }
            }
        }"""

        try:
            original_data = json.loads(original_json_text)
        except json.JSONDecodeError as e:
            print(f"JSON syntax is incorrect: {e.msg} at line {e.lineno}, column {e.colno}")

        expected_json_text = """{
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8.0-131.json#",
            "profiles": {
                "VP_TEST_profile_a": {
                    "version": 1,
                    "api-version": "1.2.131",
                    "capabilities": [ "block_a" ]
                },
                "VP_TEST_profile_b": {
                    "version": 1,
                    "api-version": "1.2.131",
                    "capabilities": [ "block_b" ],
                    "profiles": [ "VP_TEST_profile_a" ]
                }
            },
            "capabilities": {
                "block_a": {
                    "extensions": {
                        "VK_KHR_surface": 25,
                        "VK_KHR_swapchain": 70
                    }
                },
                "block_b": {
                    "extensions": {
                        "VK_KHR_present_id": 1,
                        "VK_KHR_present_wait": 1
                    },
                    "features": {
                        "VkPhysicalDevicePresentIdFeaturesKHR": {
                            "presentId": true
                        },
                        "VkPhysicalDevicePresentWaitFeaturesKHR": {
                            "presentWait": true
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
        pull_profiles_files_dependencies(self.vk, False, json_files_dict)

        self.assertEqual(json_files_dict["test_profile.json"], expected_data)

    # -------------------------------------------------------------------------
    # Vulkan 1.3 Advanced Tests (VK_KHR_present_wait in block_a)
    # -------------------------------------------------------------------------

    def test_pull_extension_dependencies_block_unique_vulkan13_advanced(self):
        """
        Verifies that under Vulkan 1.3, non-promoted dependent extensions (VK_KHR_present_id, VK_KHR_swapchain, VK_KHR_surface)
        and required features (presentWait, presentId) are transitively pulled in.
        Note: VK_KHR_image_format_list is suppressed because it was promoted to Vulkan 1.2 core.
        """
        original_json_text = """{
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8.0-204.json#",
            "profiles": {
                "VP_TEST_profile": {
                    "version": 1,
                    "api-version": "1.3.204",
                    "capabilities": ["baseline"]
                }
            },
            "capabilities": {
                "baseline": {
                    "extensions": {
                        "VK_KHR_present_wait": 1,
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
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8.0-204.json#",
            "profiles": {
                "VP_TEST_profile": {
                    "version": 1,
                    "api-version": "1.3.204",
                    "capabilities": ["baseline"]
                }
            },
            "capabilities": {
                "baseline": {
                    "extensions": {
                        "VK_KHR_present_id": 1,
                        "VK_KHR_present_wait": 1,
                        "VK_KHR_surface": 25,
                        "VK_KHR_swapchain": 70,
                        "VK_KHR_swapchain_mutable_format": 1
                    },
                    "features": {
                        "VkPhysicalDevicePresentIdFeaturesKHR": {
                            "presentId": true
                        },
                        "VkPhysicalDevicePresentWaitFeaturesKHR": {
                            "presentWait": true
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
        pull_profiles_files_dependencies(self.vk, False, json_files_dict)

        self.assertEqual(json_files_dict["test_profile.json"], expected_data)


    def test_pull_extension_dependencies_block_inheritance_vulkan13_advanced(self):
        """
        Verifies that under Vulkan 1.3, placing VK_KHR_present_wait in block_a adds its dependent extensions
        and feature structures into block_a, while block_b with VK_KHR_swapchain_mutable_format omits already satisfied dependencies.
        Note: VK_KHR_image_format_list is suppressed because it was promoted to Vulkan 1.2 core.
        """
        original_json_text = """{
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8.0-204.json#",
            "profiles": {
                "VP_TEST_profile": {
                    "version": 1,
                    "api-version": "1.3.204",
                    "capabilities": [ "block_a", "block_b" ]
                }
            },
            "capabilities": {
                "block_a": {
                    "extensions": {
                        "VK_KHR_present_wait": 1
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
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8.0-204.json#",
            "profiles": {
                "VP_TEST_profile": {
                    "version": 1,
                    "api-version": "1.3.204",
                    "capabilities": [ "block_a", "block_b" ]
                }
            },
            "capabilities": {
                "block_a": {
                    "extensions": {
                        "VK_KHR_present_id": 1,
                        "VK_KHR_present_wait": 1,
                        "VK_KHR_surface": 25,
                        "VK_KHR_swapchain": 70
                    },
                    "features": {
                        "VkPhysicalDevicePresentIdFeaturesKHR": {
                            "presentId": true
                        },
                        "VkPhysicalDevicePresentWaitFeaturesKHR": {
                            "presentWait": true
                        }
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
        pull_profiles_files_dependencies(self.vk, False, json_files_dict)

        self.assertEqual(json_files_dict["test_profile.json"], expected_data)


    def test_pull_extension_dependencies_profile_inheritance_vulkan13_advanced(self):
        """
        Verifies that under Vulkan 1.3, inherited extension dependencies and feature structures from parent profile_a
        (containing VK_KHR_present_wait) populate block_a, so child profile_b (containing VK_KHR_swapchain_mutable_format)
        only receives its remaining unique dependencies.
        Note: VK_KHR_image_format_list is suppressed because it was promoted to Vulkan 1.2 core.
        """
        original_json_text = """{
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8.0-204.json#",
            "profiles": {
                "VP_TEST_profile_a": {
                    "version": 1,
                    "api-version": "1.3.204",
                    "capabilities": [ "block_a" ]
                },
                "VP_TEST_profile_b": {
                    "version": 1,
                    "api-version": "1.3.204",
                    "capabilities": [ "block_b" ],
                    "profiles": [ "VP_TEST_profile_a" ]
                }
            },
            "capabilities": {
                "block_a": {
                    "extensions": {
                        "VK_KHR_present_wait": 1
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
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8.0-204.json#",
            "profiles": {
                "VP_TEST_profile_a": {
                    "version": 1,
                    "api-version": "1.3.204",
                    "capabilities": [ "block_a" ]
                },
                "VP_TEST_profile_b": {
                    "version": 1,
                    "api-version": "1.3.204",
                    "capabilities": [ "block_b" ],
                    "profiles": [ "VP_TEST_profile_a" ]
                }
            },
            "capabilities": {
                "block_a": {
                    "extensions": {
                        "VK_KHR_present_id": 1,
                        "VK_KHR_present_wait": 1,
                        "VK_KHR_surface": 25,
                        "VK_KHR_swapchain": 70
                    },
                    "features": {
                        "VkPhysicalDevicePresentIdFeaturesKHR": {
                            "presentId": true
                        },
                        "VkPhysicalDevicePresentWaitFeaturesKHR": {
                            "presentWait": true
                        }
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
        pull_profiles_files_dependencies(self.vk, False, json_files_dict)

        self.assertEqual(json_files_dict["test_profile.json"], expected_data)


    # -------------------------------------------------------------------------
    # Multi-Level Inheritance Tests (Vulkan 1.1 -> Vulkan 1.2 -> Vulkan 1.4)
    # -------------------------------------------------------------------------

    def test_pull_extension_dependencies_profile_inheritance_vulkan11_to_vulkan12_to_vulkan14(self):
        """
        Verifies multi-level profile inheritance across three version tiers:
        Profile A (Vulkan 1.1) -> Profile B (Vulkan 1.2) -> Profile C (Vulkan 1.4).
        Ensures that:
        1. Profile A pulls VK_KHR_surface for VK_KHR_swapchain under Vulkan 1.1.
        2. Profile B inherits Profile A's context and suppresses VK_KHR_image_format_list
           for VK_KHR_swapchain_mutable_format due to Vulkan 1.2 core promotion.
        3. Profile C inherits the combined context of A and B, pulling only unique remaining
           dependencies (VK_KHR_present_id and required feature structs for VK_KHR_present_wait).
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
                        "VK_KHR_swapchain": 70
                    }
                },
                "block_b": {
                    "extensions": {
                        "VK_KHR_swapchain_mutable_format": 1
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

        expected_json_text = """{
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
                        "VK_KHR_surface": 25,
                        "VK_KHR_swapchain": 70
                    }
                },
                "block_b": {
                    "extensions": {
                        "VK_KHR_swapchain_mutable_format": 1
                    }
                },
                "block_c": {
                    "extensions": {
                        "VK_KHR_present_id": 1,
                        "VK_KHR_present_wait": 1
                    },
                    "features": {
                        "VkPhysicalDevicePresentIdFeaturesKHR": {
                            "presentId": true
                        },
                        "VkPhysicalDevicePresentWaitFeaturesKHR": {
                            "presentWait": true
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
        pull_profiles_files_dependencies(self.vk, False, json_files_dict)

        self.assertEqual(json_files_dict["test_profile.json"], expected_data)


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument(
        '--registry', '-r', action='store', required=False,
        help='Use specified registry file instead of vk.xml.'
    )

    args, unparsed = parser.parse_known_args()
    TestConvertPullExtensionsDependencies.registry_path = args.registry

    unittest.main(argv=[sys.argv[0]] + unparsed)
    