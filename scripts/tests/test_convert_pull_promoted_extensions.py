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
from source.main_convert import pull_promoted_extensions_profiles_files


class TestConvertPullPromotedExtensions(unittest.TestCase):
    registry_path = None

    def setUp(self):
        self.vk: VulkanObject = initVulkanObject('vulkan', self.registry_path)

    def test_pull_promoted_extensions_ignore_extension_versions_true(self):
        """
        Verifies that when ignore_extension_versions is True, all extensions promoted into 
        Vulkan 1.1 are pulled into vulkan11pulledrequirements with their versions forced to 1.
        """
        original_json_text = """{
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8.0-106.json#",
            "profiles": {
                "VP_TEST_profile_v11": {
                    "version": 1,
                    "api-version": "1.1.0",
                    "capabilities": ["baseline"]
                }
            },
            "capabilities": {
                "baseline": {
                    "features": {
                        "VkPhysicalDeviceFeatures": {
                            "robustBufferAccess": true
                        }
                    }
                }
            }
        }"""

        expected_json_text = """{
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8.0-106.json#",
            "profiles": {
                "VP_TEST_profile_v11": {
                    "version": 1,
                    "api-version": "1.1.0",
                    "capabilities": [
                        "baseline",
                        "vulkan11pulledrequirements"
                    ]
                }
            },
            "capabilities": {
                "baseline": {
                    "features": {
                        "VkPhysicalDeviceFeatures": {
                            "robustBufferAccess": true
                        }
                    }
                },
                "vulkan11pulledrequirements": {
                    "extensions": {
                        "VK_KHR_multiview": 1,
                        "VK_KHR_get_physical_device_properties2": 1,
                        "VK_KHR_device_group_creation": 1,
                        "VK_KHR_device_group": 1,
                        "VK_KHR_shader_draw_parameters": 1,
                        "VK_KHR_maintenance1": 1,
                        "VK_KHR_external_memory_capabilities": 1,
                        "VK_KHR_external_memory": 1,
                        "VK_KHR_external_semaphore_capabilities": 1,
                        "VK_KHR_external_semaphore": 1,
                        "VK_KHR_16bit_storage": 1,
                        "VK_KHR_descriptor_update_template": 1,
                        "VK_KHR_external_fence_capabilities": 1,
                        "VK_KHR_external_fence": 1,                                                                        
                        "VK_KHR_maintenance2": 1,
                        "VK_KHR_variable_pointers": 1,
                        "VK_KHR_dedicated_allocation": 1,
                        "VK_KHR_storage_buffer_storage_class": 1,                        
                        "VK_KHR_relaxed_block_layout": 1,
                        "VK_KHR_get_memory_requirements2": 1,
                        "VK_KHR_sampler_ycbcr_conversion": 1,
                        "VK_KHR_bind_memory2": 1,
                        "VK_KHR_maintenance3": 1
                    }
                }
            }
        }"""

        json_files_dict = {"test_profile.json": json.loads(original_json_text)}
        pull_promoted_extensions_profiles_files(self.vk, True, json_files_dict)

        self.assertEqual(json_files_dict["test_profile.json"], json.loads(expected_json_text))

    def test_pull_promoted_extensions_ignore_extension_versions_false(self):
        """
        Verifies that when ignore_extension_versions is False, extensions promoted into 
        Vulkan 1.1 retain their exact spec versions from vk.xml.
        """
        original_json_text = """{
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8.0-106.json#",
            "profiles": {
                "VP_TEST_profile_v11": {
                    "version": 1,
                    "api-version": "1.1.0",
                    "capabilities": ["baseline"]
                }
            },
            "capabilities": {
                "baseline": {
                    "features": {
                        "VkPhysicalDeviceFeatures": {
                            "robustBufferAccess": true
                        }
                    }
                }
            }
        }"""

        expected_json_text = """{
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8.0-106.json#",
            "profiles": {
                "VP_TEST_profile_v11": {
                    "version": 1,
                    "api-version": "1.1.0",
                    "capabilities": [
                        "baseline",
                        "vulkan11pulledrequirements"
                    ]
                }
            },
            "capabilities": {
                "baseline": {
                    "features": {
                        "VkPhysicalDeviceFeatures": {
                            "robustBufferAccess": true
                        }
                    }
                },
                "vulkan11pulledrequirements": {
                    "extensions": {
                        "VK_KHR_multiview": 1,
                        "VK_KHR_get_physical_device_properties2": 2,
                        "VK_KHR_device_group_creation": 1,
                        "VK_KHR_device_group": 4,
                        "VK_KHR_shader_draw_parameters": 1,
                        "VK_KHR_maintenance1": 2,
                        "VK_KHR_external_memory_capabilities": 1,
                        "VK_KHR_external_memory": 1,
                        "VK_KHR_external_semaphore_capabilities": 1,
                        "VK_KHR_external_semaphore": 1,
                        "VK_KHR_16bit_storage": 1,
                        "VK_KHR_descriptor_update_template": 1,
                        "VK_KHR_external_fence_capabilities": 1,
                        "VK_KHR_external_fence": 1,                                                                        
                        "VK_KHR_maintenance2": 1,
                        "VK_KHR_variable_pointers": 1,
                        "VK_KHR_dedicated_allocation": 3,
                        "VK_KHR_storage_buffer_storage_class": 1,                        
                        "VK_KHR_relaxed_block_layout": 1,
                        "VK_KHR_get_memory_requirements2": 1,
                        "VK_KHR_sampler_ycbcr_conversion": 14,
                        "VK_KHR_bind_memory2": 1,
                        "VK_KHR_maintenance3": 1
                    }
                }
            }
        }"""

        json_files_dict = {"test_profile.json": json.loads(original_json_text)}
        pull_promoted_extensions_profiles_files(self.vk, False, json_files_dict)

        self.assertEqual(json_files_dict["test_profile.json"], json.loads(expected_json_text))


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument(
        '--registry', '-r', action='store', required=False,
        help='Use specified registry file instead of vk.xml.'
    )

    args, unparsed = parser.parse_known_args()
    TestConvertPullPromotedExtensions.registry_path = args.registry

    unittest.main(argv=[sys.argv[0]] + unparsed)
    