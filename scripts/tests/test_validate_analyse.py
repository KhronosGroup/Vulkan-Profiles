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
from source.transform_utils import AnalyseBits
from source.main_validate import VulkanProfilesDataValidation


class TestAnalyseBits(unittest.TestCase):
    registry_path = None

    def setUp(self):
        self.vk: VulkanObject = initVulkanObject('vulkan', self.registry_path)
        self.validator = VulkanProfilesDataValidation(self.vk)

    def test_analyse_api_version(self):
        """
        AnalyseBits.API_VERSION:
        Verifies detection when a child profile targets an older Vulkan version (1.0)
        than an inherited parent required profile (1.1).
        """
        json_text = """{
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8.0-106.json#",
            "profiles": {
                "VP_TEST_parent": {
                    "version": 1,
                    "api-version": "1.1.0",
                    "capabilities": ["caps_parent"]
                },
                "VP_TEST_child": {
                    "version": 1,
                    "api-version": "1.0.0",
                    "profiles": ["VP_TEST_parent"],
                    "capabilities": ["caps_child"]
                }
            },
            "capabilities": {
                "caps_parent": {},
                "caps_child": {}
            }
        }"""

        json_files_dict = {"test_profile.json": json.loads(json_text)}
        issues = self.validator.validate_data(json_files_dict, [AnalyseBits.API_VERSION])

        self.assertTrue(any("inherits profile 'VP_TEST_parent' which targets newer Vulkan" in issue for issue in issues))

    def test_analyse_extensions(self):
        """
        AnalyseBits.EXTENSIONS:
        Verifies detection when an extension structure (VkPhysicalDeviceImagelessFramebufferFeaturesKHR)
        is included without declaring its defining extension (VK_KHR_imageless_framebuffer).
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
                        }
                    }
                }
            }
        }"""

        json_files_dict = {"test_profile.json": json.loads(json_text)}
        issues = self.validator.validate_data(json_files_dict, [AnalyseBits.EXTENSIONS])

        self.assertTrue(any("belongs to extension 'VK_KHR_imageless_framebuffer'" in issue for issue in issues))

    def test_analyse_aliases(self):
        """
        AnalyseBits.ALIASES:
        Verifies detection of mismatching member values across aliased capability structures
        (e.g., VkPhysicalDeviceImagelessFramebufferFeaturesKHR vs VkPhysicalDeviceVulkan12Features).
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
        issues = self.validator.validate_data(json_files_dict, [AnalyseBits.ALIASES])

        self.assertTrue(any("has mismatching value" in issue and "aliased structure" in issue for issue in issues))

    def test_analyse_unratified(self):
        """
        AnalyseBits.UNRATIFIED:
        Verifies detection when an extension declared in the profile is unratified in vk.xml.
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
                    "extensions": {
                        "VK_ANDROID_external_memory_android_hardware_buffer": 1
                    }
                }
            }
        }"""

        json_files_dict = {"test_profile.json": json.loads(json_text)}
        issues = self.validator.validate_data(json_files_dict, [AnalyseBits.UNRATIFIED])

        self.assertTrue(any("is not ratified for Vulkan" in issue for issue in issues))

    def test_analyse_subcore_capabilities(self):
        """
        AnalyseBits.SUBCORE_CAPABILITIES:
        Verifies detection when a capability limit (maxImageDimension1D) is set below
        the Vulkan core specification requirement (4096).
        """
        json_text = """{
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
                    "properties": {
                        "VkPhysicalDeviceProperties": {
                            "limits": {
                                "maxImageDimension1D": 2048
                            }
                        }
                    }
                }
            }
        }"""

        json_files_dict = {"test_profile.json": json.loads(json_text)}
        issues = self.validator.validate_data(json_files_dict, [AnalyseBits.SUBCORE_CAPABILITIES])

        self.assertTrue(any("below Vulkan core requirement" in issue and "maxImageDimension1D" in issue for issue in issues))


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument(
        '--registry', '-r', action='store', required=False,
        help='Use specified registry file instead of vk.xml.'
    )

    args, unparsed = parser.parse_known_args()
    TestAnalyseBits.registry_path = args.registry

    unittest.main(argv=[sys.argv[0]] + unparsed)
    