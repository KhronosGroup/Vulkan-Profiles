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
from source.transform_utils import PullBits
from source.transform_pull_required_capabilities import pull_required_capabilities_profiles_files


class TestConvertPullRequiredCapabilitiesProf(unittest.TestCase):
    registry_path = None

    def setUp(self):
        self.vk: VulkanObject = initVulkanObject('vulkan', self.registry_path)

    def test_pull_required_capabilities_already_present_vulkan10(self):
        """
        Verifies that pulling core requirements with PullBits.OVERRIDE_CORE_CAPABILITIES onto an existing profile
        preserves profile capability requirements higher than Vulkan core requirements (e.g. maxImageDimension2D = 8192,
        framebufferColorSampleCounts with 8_BIT) while enforcing core baseline minimums (maxImageDimension1D = 4096).
        """
        original_json_text = """{
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8.0-106.json#",
            "profiles": {
                "VP_TEST_profile_v10": {
                    "version": 1,
                    "api-version": "1.0.68",
                    "capabilities": ["baseline"]
                }
            },
            "capabilities": {
                "baseline": {
                    "extensions": {
                        "VK_KHR_shader_draw_parameters": 1,
                        "VK_KHR_16bit_storage": 1
                    },
                    "features": {
                        "VkPhysicalDeviceFeatures": {
                            "robustBufferAccess": true,
                            "fullDrawIndexUint32": true,
                            "imageCubeArray": false
                        },
                        "VkPhysicalDevice16BitStorageFeaturesKHR": {
                            "uniformAndStorageBuffer16BitAccess": true
                        }
                    },
                    "properties": {
                        "VkPhysicalDeviceProperties": {
                            "limits": {
                                "maxImageDimension1D": 2048,
                                "maxImageDimension2D": 8192,
                                "maxImageDimension3D": 256,
                                "framebufferColorSampleCounts": [
                                    "VK_SAMPLE_COUNT_4_BIT",
                                    "VK_SAMPLE_COUNT_8_BIT"
                                ]
                            }
                        }
                    }
                }
            }
        }"""

        json_files_dict = {"test_profile.json": json.loads(original_json_text)}
        pull_required_capabilities_profiles_files(
            self.vk, 
            [PullBits.REQUIRED_CAPABILITIES, PullBits.OVERRIDE_CORE_CAPABILITIES, PullBits.IGNORE_UNSUPPORTED], 
            json_files_dict
        )

        baseline_caps = json_files_dict["test_profile.json"]["capabilities"]["baseline"]
        limits = baseline_caps["properties"]["VkPhysicalDeviceProperties"]["limits"]
        features16 = baseline_caps["features"]["VkPhysicalDevice16BitStorageFeaturesKHR"]

        # 1. Core baseline minimum is enforced over below-spec value (2048 -> 4096)
        self.assertEqual(limits.get("maxImageDimension1D"), 4096)

        # 2. Above-spec profile value is preserved (8192)
        self.assertEqual(limits.get("maxImageDimension2D"), 8192)

        # 3. Sample counts combine core requirements (1_BIT, 4_BIT) with profile value (8_BIT)
        color_samples = limits.get("framebufferColorSampleCounts", [])
        self.assertIn("VK_SAMPLE_COUNT_1_BIT", color_samples)
        self.assertIn("VK_SAMPLE_COUNT_4_BIT", color_samples)
        self.assertIn("VK_SAMPLE_COUNT_8_BIT", color_samples)

        # 4. Extension required features are pulled
        self.assertTrue(features16.get("storageBuffer16BitAccess"))
        self.assertTrue(features16.get("uniformAndStorageBuffer16BitAccess"))

    def test_pull_required_capabilities_not_ext_vulkan10(self):
        """
        Verifies that when VK_KHR_portability_subset is set, robustBufferAccess is not added.
        """
        original_json_text = """{
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8.0-106.json#",
            "profiles": {
                "VP_TEST_profile_v10": {
                    "version": 1,
                    "api-version": "1.0.68",
                    "capabilities": ["baseline"]
                }
            },
            "capabilities": {
                "baseline": {
                    "extensions": {
                        "VK_KHR_portability_subset": 1
                    }
                }
            }
        }"""

        json_files_dict = {"test_profile.json": json.loads(original_json_text)}
        pull_required_capabilities_profiles_files(
            self.vk, 
            [PullBits.REQUIRED_CAPABILITIES, PullBits.OVERRIDE_CORE_CAPABILITIES, PullBits.IGNORE_UNSUPPORTED], 
            json_files_dict
        )

        baseline_caps = json_files_dict["test_profile.json"]["capabilities"]["baseline"]
        vk_features = baseline_caps.get("features", {}).get("VkPhysicalDeviceFeatures", {})

        # robustBufferAccess must not be pulled when VK_KHR_portability_subset is present
        self.assertFalse(vk_features.get("robustBufferAccess", False))


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument(
        '--registry', '-r', action='store', required=False,
        help='Use specified registry file instead of vk.xml.'
    )

    args, unparsed = parser.parse_known_args()
    TestConvertPullRequiredCapabilitiesProf.registry_path = args.registry

    unittest.main(argv=[sys.argv[0]] + unparsed)