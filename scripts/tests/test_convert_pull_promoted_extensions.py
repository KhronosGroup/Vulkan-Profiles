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
    pull_promoted_extensions_profiles_files
)


class TestConvertPullPromotedExtensions(unittest.TestCase):
    registry_path = None

    def setUp(self):
        self.vk: VulkanObject = initVulkanObject('vulkan', self.registry_path)

    # -------------------------------------------------------------------------
    # Vulkan 1.0 Tests (VK_KHR_variable_pointers)
    # -------------------------------------------------------------------------

    def test_pull_promoted_extensions_block_unique_vulkan10(self):
        """
        Verifies that for Vulkan 1.0 profiles, pull_promoted_extensions_profiles_files pulls no core
        promoted extensions (since core promotions start at Vulkan 1.1+), but still resolves dependencies.
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
                    }
                }
            }
        }"""

        try:
            expected_data = json.loads(expected_json_text)
        except json.JSONDecodeError as e:
            print(f"JSON syntax is incorrect: {e.msg} at line {e.lineno}, column {e.colno}")

        json_files_dict = {"test_profile.json": original_data}
        pull_promoted_extensions_profiles_files(self.vk, False, json_files_dict)

        self.assertEqual(json_files_dict["test_profile.json"], expected_data)


    def test_pull_promoted_extensions_block_inheritance_vulkan10(self):
        """
        Verifies that for Vulkan 1.0 profiles with block inheritance, promoted extension extraction
        only targets the primary capability block and preserves non-primary blocks.
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
        pull_promoted_extensions_profiles_files(self.vk, False, json_files_dict)

        self.assertEqual(json_files_dict["test_profile.json"], expected_data)


    def test_pull_promoted_extensions_profile_inheritance_vulkan10(self):
        """
        Verifies that for Vulkan 1.0 profiles with profile inheritance, dependency pulling
        respects context extensions inherited from parent profiles.
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
                    }
                }
            }
        }"""

        try:
            expected_data = json.loads(expected_json_text)
        except json.JSONDecodeError as e:
            print(f"JSON syntax is incorrect: {e.msg} at line {e.lineno}, column {e.colno}")

        json_files_dict = {"test_profile.json": original_data}
        pull_promoted_extensions_profiles_files(self.vk, False, json_files_dict)

        self.assertEqual(json_files_dict["test_profile.json"], expected_data)

    # -------------------------------------------------------------------------
    # Vulkan 1.1 Tests (VK_KHR_variable_pointers - Core Promotion Injections)
    # -------------------------------------------------------------------------

    def test_pull_promoted_extensions_block_unique_vulkan11(self):
        """
        Verifies that all extensions promoted to Vulkan 1.1 core (e.g. VK_KHR_multiview, VK_KHR_variable_pointers,
        VK_KHR_get_physical_device_properties2, etc.) and their non-core dependencies (e.g. VK_KHR_surface)
        are automatically injected into a Vulkan 1.1 profile's primary capability block.
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
                        "VK_KHR_16bit_storage": 1,
                        "VK_KHR_bind_memory2": 1,
                        "VK_KHR_dedicated_allocation": 1,
                        "VK_KHR_descriptor_update_template": 1,
                        "VK_KHR_device_group": 1,
                        "VK_KHR_device_group_creation": 1,
                        "VK_KHR_external_fence": 1,
                        "VK_KHR_external_fence_capabilities": 1,
                        "VK_KHR_external_memory": 1,
                        "VK_KHR_external_memory_capabilities": 1,
                        "VK_KHR_external_semaphore": 1,
                        "VK_KHR_external_semaphore_capabilities": 1,
                        "VK_KHR_get_memory_requirements2": 1,
                        "VK_KHR_get_physical_device_properties2": 2,
                        "VK_KHR_get_surface_capabilities2": 1,
                        "VK_KHR_maintenance1": 2,
                        "VK_KHR_maintenance2": 1,
                        "VK_KHR_maintenance3": 1,
                        "VK_KHR_multiview": 1,
                        "VK_KHR_relaxed_block_layout": 1,
                        "VK_KHR_sampler_ycbcr_conversion": 1,
                        "VK_KHR_shader_draw_parameters": 1,
                        "VK_KHR_storage_buffer_storage_class": 1,
                        "VK_KHR_surface": 25,
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
        pull_promoted_extensions_profiles_files(self.vk, False, json_files_dict)

        self.assertEqual(json_files_dict["test_profile.json"], expected_data)


    def test_pull_promoted_extensions_block_inheritance_vulkan11(self):
        """
        Verifies that Vulkan 1.1 promoted extensions are injected into the primary block (block_a),
        leaving non-primary blocks untouched.
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
                        "VK_KHR_16bit_storage": 1,
                        "VK_KHR_bind_memory2": 1,
                        "VK_KHR_dedicated_allocation": 1,
                        "VK_KHR_descriptor_update_template": 1,
                        "VK_KHR_device_group": 1,
                        "VK_KHR_device_group_creation": 1,
                        "VK_KHR_external_fence": 1,
                        "VK_KHR_external_fence_capabilities": 1,
                        "VK_KHR_external_memory": 1,
                        "VK_KHR_external_memory_capabilities": 1,
                        "VK_KHR_external_semaphore": 1,
                        "VK_KHR_external_semaphore_capabilities": 1,
                        "VK_KHR_get_memory_requirements2": 1,
                        "VK_KHR_get_physical_device_properties2": 2,
                        "VK_KHR_get_surface_capabilities2": 1,
                        "VK_KHR_maintenance1": 2,
                        "VK_KHR_maintenance2": 1,
                        "VK_KHR_maintenance3": 1,
                        "VK_KHR_multiview": 1,
                        "VK_KHR_relaxed_block_layout": 1,
                        "VK_KHR_sampler_ycbcr_conversion": 1,
                        "VK_KHR_shader_draw_parameters": 1,
                        "VK_KHR_storage_buffer_storage_class": 1,
                        "VK_KHR_surface": 25,
                        "VK_KHR_variable_pointers": 1
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
        pull_promoted_extensions_profiles_files(self.vk, False, json_files_dict)

        self.assertEqual(json_files_dict["test_profile.json"], expected_data)


    def test_pull_promoted_extensions_profile_inheritance_vulkan11(self):
        """
        Verifies that Vulkan 1.1 promoted extensions are injected into each profile's primary block across profile inheritance trees.
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
                        "VK_KHR_16bit_storage": 1,
                        "VK_KHR_bind_memory2": 1,
                        "VK_KHR_dedicated_allocation": 1,
                        "VK_KHR_descriptor_update_template": 1,
                        "VK_KHR_device_group": 1,
                        "VK_KHR_device_group_creation": 1,
                        "VK_KHR_external_fence": 1,
                        "VK_KHR_external_fence_capabilities": 1,
                        "VK_KHR_external_memory": 1,
                        "VK_KHR_external_memory_capabilities": 1,
                        "VK_KHR_external_semaphore": 1,
                        "VK_KHR_external_semaphore_capabilities": 1,
                        "VK_KHR_get_memory_requirements2": 1,
                        "VK_KHR_get_physical_device_properties2": 2,
                        "VK_KHR_get_surface_capabilities2": 1,
                        "VK_KHR_maintenance1": 2,
                        "VK_KHR_maintenance2": 1,
                        "VK_KHR_maintenance3": 1,
                        "VK_KHR_multiview": 1,
                        "VK_KHR_relaxed_block_layout": 1,
                        "VK_KHR_sampler_ycbcr_conversion": 1,
                        "VK_KHR_shader_draw_parameters": 1,
                        "VK_KHR_storage_buffer_storage_class": 1,
                        "VK_KHR_surface": 25,
                        "VK_KHR_variable_pointers": 1
                    }
                },
                "block_b": {
                    "extensions": {
                        "VK_KHR_16bit_storage": 1,
                        "VK_KHR_bind_memory2": 1,
                        "VK_KHR_dedicated_allocation": 1,
                        "VK_KHR_descriptor_update_template": 1,
                        "VK_KHR_device_group": 1,
                        "VK_KHR_device_group_creation": 1,
                        "VK_KHR_external_fence": 1,
                        "VK_KHR_external_fence_capabilities": 1,
                        "VK_KHR_external_memory": 1,
                        "VK_KHR_external_memory_capabilities": 1,
                        "VK_KHR_external_semaphore": 1,
                        "VK_KHR_external_semaphore_capabilities": 1,
                        "VK_KHR_get_memory_requirements2": 1,
                        "VK_KHR_get_physical_device_properties2": 2,
                        "VK_KHR_get_surface_capabilities2": 1,
                        "VK_KHR_maintenance1": 2,
                        "VK_KHR_maintenance2": 1,
                        "VK_KHR_maintenance3": 1,
                        "VK_KHR_multiview": 1,
                        "VK_KHR_relaxed_block_layout": 1,
                        "VK_KHR_sampler_ycbcr_conversion": 1,
                        "VK_KHR_shader_draw_parameters": 1,
                        "VK_KHR_storage_buffer_storage_class": 1,
                        "VK_KHR_surface": 25,
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
        pull_promoted_extensions_profiles_files(self.vk, False, json_files_dict)

        self.assertEqual(json_files_dict["test_profile.json"], expected_data)

    # -------------------------------------------------------------------------
    # Vulkan 1.1 Advanced Tests (VK_KHR_swapchain_mutable_format)
    # -------------------------------------------------------------------------

    def test_pull_promoted_extensions_block_unique_vulkan11_advanced(self):
        """
        Verifies that when pulling promoted extensions for a Vulkan 1.1 profile containing VK_KHR_swapchain_mutable_format,
        all 1.1 promoted extensions plus transitive dependencies (VK_KHR_swapchain, VK_KHR_surface) are present.
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
                        "VK_KHR_16bit_storage": 1,
                        "VK_KHR_bind_memory2": 1,
                        "VK_KHR_dedicated_allocation": 1,
                        "VK_KHR_descriptor_update_template": 1,
                        "VK_KHR_device_group": 1,
                        "VK_KHR_device_group_creation": 1,
                        "VK_KHR_external_fence": 1,
                        "VK_KHR_external_fence_capabilities": 1,
                        "VK_KHR_external_memory": 1,
                        "VK_KHR_external_memory_capabilities": 1,
                        "VK_KHR_external_semaphore": 1,
                        "VK_KHR_external_semaphore_capabilities": 1,
                        "VK_KHR_get_memory_requirements2": 1,
                        "VK_KHR_get_physical_device_properties2": 2,
                        "VK_KHR_get_surface_capabilities2": 1,
                        "VK_KHR_image_format_list": 1,
                        "VK_KHR_maintenance1": 2,
                        "VK_KHR_maintenance2": 1,
                        "VK_KHR_maintenance3": 1,
                        "VK_KHR_multiview": 1,
                        "VK_KHR_relaxed_block_layout": 1,
                        "VK_KHR_sampler_ycbcr_conversion": 1,
                        "VK_KHR_shader_draw_parameters": 1,
                        "VK_KHR_storage_buffer_storage_class": 1,
                        "VK_KHR_surface": 25,
                        "VK_KHR_swapchain": 70,
                        "VK_KHR_swapchain_mutable_format": 1,
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
        pull_promoted_extensions_profiles_files(self.vk, False, json_files_dict)

        self.assertEqual(json_files_dict["test_profile.json"], expected_data)


    def test_pull_promoted_extensions_block_inheritance_vulkan11_advanced(self):
        """
        Verifies that when pulling promoted extensions for a Vulkan 1.1 profile with block inheritance,
        promoted extensions and their dependencies are populated into primary block_a.
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
                        "VK_KHR_16bit_storage": 1,
                        "VK_KHR_bind_memory2": 1,
                        "VK_KHR_dedicated_allocation": 1,
                        "VK_KHR_descriptor_update_template": 1,
                        "VK_KHR_device_group": 1,
                        "VK_KHR_device_group_creation": 1,
                        "VK_KHR_external_fence": 1,
                        "VK_KHR_external_fence_capabilities": 1,
                        "VK_KHR_external_memory": 1,
                        "VK_KHR_external_memory_capabilities": 1,
                        "VK_KHR_external_semaphore": 1,
                        "VK_KHR_external_semaphore_capabilities": 1,
                        "VK_KHR_get_memory_requirements2": 1,
                        "VK_KHR_get_physical_device_properties2": 2,
                        "VK_KHR_get_surface_capabilities2": 1,
                        "VK_KHR_maintenance1": 2,
                        "VK_KHR_maintenance2": 1,
                        "VK_KHR_maintenance3": 1,
                        "VK_KHR_multiview": 1,
                        "VK_KHR_relaxed_block_layout": 1,
                        "VK_KHR_sampler_ycbcr_conversion": 1,
                        "VK_KHR_shader_draw_parameters": 1,
                        "VK_KHR_storage_buffer_storage_class": 1,
                        "VK_KHR_surface": 25,
                        "VK_KHR_swapchain": 70,
                        "VK_KHR_variable_pointers": 1
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
        pull_promoted_extensions_profiles_files(self.vk, False, json_files_dict)

        self.assertEqual(json_files_dict["test_profile.json"], expected_data)


    def test_pull_promoted_extensions_profile_inheritance_vulkan11_advanced(self):
        """
        Verifies that when pulling promoted extensions for Vulkan 1.1 profiles with profile inheritance,
        each profile's primary block receives promoted extensions while filtering dependencies already provided in parent profile_a.
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
                        "VK_KHR_16bit_storage": 1,
                        "VK_KHR_bind_memory2": 1,
                        "VK_KHR_dedicated_allocation": 1,
                        "VK_KHR_descriptor_update_template": 1,
                        "VK_KHR_device_group": 1,
                        "VK_KHR_device_group_creation": 1,
                        "VK_KHR_external_fence": 1,
                        "VK_KHR_external_fence_capabilities": 1,
                        "VK_KHR_external_memory": 1,
                        "VK_KHR_external_memory_capabilities": 1,
                        "VK_KHR_external_semaphore": 1,
                        "VK_KHR_external_semaphore_capabilities": 1,
                        "VK_KHR_get_memory_requirements2": 1,
                        "VK_KHR_get_physical_device_properties2": 2,
                        "VK_KHR_get_surface_capabilities2": 1,
                        "VK_KHR_maintenance1": 2,
                        "VK_KHR_maintenance2": 1,
                        "VK_KHR_maintenance3": 1,
                        "VK_KHR_multiview": 1,
                        "VK_KHR_relaxed_block_layout": 1,
                        "VK_KHR_sampler_ycbcr_conversion": 1,
                        "VK_KHR_shader_draw_parameters": 1,
                        "VK_KHR_storage_buffer_storage_class": 1,
                        "VK_KHR_surface": 25,
                        "VK_KHR_swapchain": 70,
                        "VK_KHR_variable_pointers": 1
                    }
                },
                "block_b": {
                    "extensions": {
                        "VK_KHR_16bit_storage": 1,
                        "VK_KHR_bind_memory2": 1,
                        "VK_KHR_dedicated_allocation": 1,
                        "VK_KHR_descriptor_update_template": 1,
                        "VK_KHR_device_group": 1,
                        "VK_KHR_device_group_creation": 1,
                        "VK_KHR_external_fence": 1,
                        "VK_KHR_external_fence_capabilities": 1,
                        "VK_KHR_external_memory": 1,
                        "VK_KHR_external_memory_capabilities": 1,
                        "VK_KHR_external_semaphore": 1,
                        "VK_KHR_external_semaphore_capabilities": 1,
                        "VK_KHR_get_memory_requirements2": 1,
                        "VK_KHR_get_physical_device_properties2": 2,
                        "VK_KHR_get_surface_capabilities2": 1,
                        "VK_KHR_maintenance1": 2,
                        "VK_KHR_maintenance2": 1,
                        "VK_KHR_maintenance3": 1,
                        "VK_KHR_multiview": 1,
                        "VK_KHR_relaxed_block_layout": 1,
                        "VK_KHR_sampler_ycbcr_conversion": 1,
                        "VK_KHR_shader_draw_parameters": 1,
                        "VK_KHR_storage_buffer_storage_class": 1,
                        "VK_KHR_swapchain_mutable_format": 1,
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
        pull_promoted_extensions_profiles_files(self.vk, False, json_files_dict)

        self.assertEqual(json_files_dict["test_profile.json"], expected_data)


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument(
        '--registry', '-r', action='store', required=False,
        help='Use specified registry file instead of vk.xml.'
    )

    args, unparsed = parser.parse_known_args()
    TestConvertPullPromotedExtensions.registry_path = args.registry

    unittest.main(argv=[sys.argv[0]] + unparsed)
    