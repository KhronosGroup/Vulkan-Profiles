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
from source.main_validate import VulkanProfilesDataValidation


class TestValidate(unittest.TestCase):
    registry_path = None

    def setUp(self):
        self.vk: VulkanObject = initVulkanObject('vulkan', self.registry_path)
        self.validator = VulkanProfilesDataValidation(self.vk)

    def test_undeclared_extension_structure(self):
        """
        Verifies that static analysis detects when a capability block lists an extension structure
        (VkPhysicalDeviceImagelessFramebufferFeaturesKHR) without declaring its extension (VK_KHR_imageless_framebuffer).
        """
        json_text = """{
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8.0-106.json#",
            "profiles": {
                "VP_TEST_profile": {
                    "version": 1,
                    "api-version": "1.1.0",
                    "capabilities": ["baseline"]
                }
            },
            "capabilities": {
                "baseline": {
                    "features": {
                        "VkPhysicalDeviceImagelessFramebufferFeaturesKHR": {
                            "imagelessFramebuffer": true
                        }, 
                        "VkPhysicalDeviceImagelessFramebufferFeatures": {
                            "imagelessFramebuffer": true
                        }, 
                        "VkPhysicalDeviceVulkan12Features": {
                            "imagelessFramebuffer": true
                        }
                    }
                }
            }
        }"""

        json_files_dict = {"test_profile.json": json.loads(json_text)}
        issues = self.validator.validate_data(json_files_dict)

        self.assertTrue(any("belongs to extension 'VK_KHR_imageless_framebuffer'" in issue for issue in issues))

    def test_core_structure_unsupported_api_version(self):
        """
        Verifies that static analysis detects when a Vulkan core structure (VkPhysicalDeviceVulkan12Features)
        is used in a profile targeting an older Vulkan version (Vulkan 1.1).
        """
        json_text = """{
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8.0-106.json#",
            "profiles": {
                "VP_TEST_profile": {
                    "version": 1,
                    "api-version": "1.1.0",
                    "capabilities": ["baseline"]
                }
            },
            "capabilities": {
                "baseline": {
                    "features": {
                        "VkPhysicalDeviceVulkan12Features": {
                            "drawIndirectCount": true
                        }
                    }
                }
            }
        }"""

        json_files_dict = {"test_profile.json": json.loads(json_text)}
        issues = self.validator.validate_data(json_files_dict)

        self.assertTrue(any("Core structure 'VkPhysicalDeviceVulkan12Features' requires Vulkan 1.2" in issue for issue in issues))

    def test_aliased_capability_value_mismatch(self):
        """
        Verifies that static analysis uses gatherCapabilityAliases to detect value mismatches
        across aliased capability structures (e.g., KHR extension vs Vulkan 1.2 core structure).
        """
        json_text = """{
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8.0-106.json#",
            "profiles": {
                "VP_TEST_profile": {
                    "version": 1,
                    "api-version": "1.2.0",
                    "capabilities": ["baseline"]
                }
            },
            "capabilities": {
                "baseline": {
                    "features": {
                        "VkPhysicalDeviceImagelessFramebufferFeaturesKHR": {
                            "imagelessFramebuffer": true
                        },
                        "VkPhysicalDeviceVulkan12Features": {
                            "imagelessFramebuffer": false
                        }
                    }
                }
            }
        }"""

        json_files_dict = {"test_profile.json": json.loads(json_text)}
        issues = self.validator.validate_data(json_files_dict)

        self.assertTrue(any("has mismatching values across aliased structures" in issue for issue in issues))

if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument(
        '--registry', '-r', action='store', required=False,
        help='Use specified registry file instead of vk.xml.'
    )

    args, unparsed = parser.parse_known_args()
    TestValidate.registry_path = args.registry

    unittest.main(argv=[sys.argv[0]] + unparsed)
