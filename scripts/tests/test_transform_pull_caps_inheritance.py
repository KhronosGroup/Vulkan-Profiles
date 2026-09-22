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
from source.transform_sort import sort_profiles_files


class TestConvertPullRequiredCapabilitiesCoreInheriance(unittest.TestCase):
    registry_path = None

    def setUp(self):
        self.vk: VulkanObject = initVulkanObject('vulkan', self.registry_path)

    def test_pull_required_capabilities_profile_inheritance_vulkan10_to_vulkan11(self):
        """
        Verifies core capability pulling when a Vulkan 1.1 child profile inherits from a Vulkan 1.0 parent profile.
        The Vulkan 1.0 parent profile pulls Vulkan 1.0 core capabilities, and a transition block
        ('VP_TEST_profile_v10_to_vulkan11') is dynamically generated for the Vulkan 1.1 child profile.
        """
        original_json_text = """{
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8.0-106.json#",
            "profiles": {
                "VP_TEST_profile_v10": {
                    "version": 1,
                    "api-version": "1.0.68",
                    "capabilities": ["baseline_v10"]
                },
                "VP_TEST_profile_v11": {
                    "version": 1,
                    "api-version": "1.1.108",
                    "profiles": ["VP_TEST_profile_v10"],
                    "capabilities": ["baseline_v11"]
                }
            },
            "capabilities": {
                "baseline_v10": {},
                "baseline_v11": {}
            }
        }"""

        json_files_dict = {"test_profile.json": json.loads(original_json_text)}
        pull_required_capabilities_profiles_files(self.vk, [PullBits.REQUIRED_CAPABILITIES], json_files_dict)
        sort_profiles_files(self.vk, json_files_dict)

        result_data = json_files_dict["test_profile.json"]
        profiles_dict = result_data["profiles"]
        capabilities_dict = result_data["capabilities"]

        # 1. Verify parent profile v10 capabilities list
        self.assertEqual(profiles_dict["VP_TEST_profile_v10"].get("capabilities"), ["baseline_v10"])

        # 2. Verify child profile v11 received generated transition block
        self.assertEqual(
            profiles_dict["VP_TEST_profile_v11"].get("capabilities"),
            ["VP_TEST_profile_v10_to_vulkan11", "baseline_v11"]
        )

        # 3. Verify baseline_v10 contains Vulkan 1.0 core features & properties
        self.assertTrue(capabilities_dict["baseline_v10"]["features"]["VkPhysicalDeviceFeatures"].get("robustBufferAccess"))
        self.assertIn("VkPhysicalDeviceProperties", capabilities_dict["baseline_v10"].get("properties", {}))

        # 4. Verify transition block VP_TEST_profile_v10_to_vulkan11
        trans_v11 = capabilities_dict.get("VP_TEST_profile_v10_to_vulkan11", {})
        self.assertTrue(trans_v11["features"]["VkPhysicalDeviceMultiviewFeatures"].get("multiview"))
        self.assertIn("VkPhysicalDeviceMaintenance3Properties", trans_v11.get("properties", {}))
        self.assertIn("VkPhysicalDeviceMultiviewProperties", trans_v11.get("properties", {}))
        self.assertIn("VkPhysicalDeviceProtectedMemoryProperties", trans_v11.get("properties", {}))
        self.assertIn("VkPhysicalDeviceSubgroupProperties", trans_v11.get("properties", {}))

    def test_pull_required_capabilities_profile_inheritance_vulkan11_to_vulkan12(self):
        """
        Verifies core capability pulling when a Vulkan 1.2 child profile inherits from a Vulkan 1.1 parent profile.
        A transition block ('VP_TEST_profile_v11_to_vulkan12') is dynamically generated for the Vulkan 1.2 child profile,
        pulling Vulkan 1.2 core features and Vulkan 1.2 core properties (VkPhysicalDeviceVulkan11Properties is omitted
        as its values are already specified via split structures in Vulkan 1.1).
        """
        original_json_text = """{
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8.0-131.json#",
            "profiles": {
                "VP_TEST_profile_v11": {
                    "version": 1,
                    "api-version": "1.1.108",
                    "capabilities": ["baseline_v11"]
                },
                "VP_TEST_profile_v12": {
                    "version": 1,
                    "api-version": "1.2.131",
                    "profiles": ["VP_TEST_profile_v11"],
                    "capabilities": ["baseline_v12"]
                }
            },
            "capabilities": {
                "baseline_v11": {},
                "baseline_v12": {}
            }
        }"""

        json_files_dict = {"test_profile.json": json.loads(original_json_text)}
        pull_required_capabilities_profiles_files(self.vk, [PullBits.REQUIRED_CAPABILITIES], json_files_dict)
        sort_profiles_files(self.vk, json_files_dict)

        result_data = json_files_dict["test_profile.json"]
        profiles_dict = result_data["profiles"]
        capabilities_dict = result_data["capabilities"]

        # 1. Verify parent profile v11 capabilities list
        self.assertEqual(profiles_dict["VP_TEST_profile_v11"].get("capabilities"), ["baseline_v11"])

        # 2. Verify child profile v12 received generated transition block
        self.assertEqual(
            profiles_dict["VP_TEST_profile_v12"].get("capabilities"),
            ["VP_TEST_profile_v11_to_vulkan12", "baseline_v12"]
        )

        # 3. Verify parent baseline_v11 features and properties
        v11_features = capabilities_dict["baseline_v11"].get("features", {})
        self.assertTrue(v11_features["VkPhysicalDeviceFeatures"].get("robustBufferAccess"))
        self.assertTrue(v11_features["VkPhysicalDeviceMultiviewFeatures"].get("multiview"))

        v11_properties = capabilities_dict["baseline_v11"].get("properties", {})
        self.assertIn("VkPhysicalDeviceProperties", v11_properties)
        self.assertIn("VkPhysicalDeviceMaintenance3Properties", v11_properties)
        self.assertIn("VkPhysicalDeviceMultiviewProperties", v11_properties)
        self.assertIn("VkPhysicalDeviceProtectedMemoryProperties", v11_properties)
        self.assertIn("VkPhysicalDeviceSubgroupProperties", v11_properties)

        # 4. Verify transition block VP_TEST_profile_v11_to_vulkan12
        trans_block = capabilities_dict.get("VP_TEST_profile_v11_to_vulkan12", {})
        self.assertIn("VkPhysicalDeviceVulkan12Features", trans_block.get("features", {}))
        self.assertTrue(trans_block["features"]["VkPhysicalDeviceVulkan12Features"].get("imagelessFramebuffer"))
        self.assertTrue(trans_block["features"]["VkPhysicalDeviceVulkan12Features"].get("timelineSemaphore"))

        self.assertIn("VkPhysicalDeviceVulkan12Properties", trans_block.get("properties", {}))
        self.assertEqual(
            trans_block["properties"]["VkPhysicalDeviceVulkan12Properties"].get("maxTimelineSemaphoreValueDifference"),
            2147483647
        )

    def test_pull_required_capabilities_profile_inheritance_vulkan10_to_vulkan12(self):
        """
        Verifies core capability pulling across a 3-level profile chain (Vulkan 1.0 -> 1.1 -> 1.2).
        - VP_TEST_profile_v10 (V1.0) pulls Vulkan 1.0 core capabilities into baseline_v10.
        - VP_TEST_profile_v11 (V1.1) inherits VP_TEST_profile_v10 and generates transition block 'VP_TEST_profile_v10_to_vulkan11'.
        - VP_TEST_profile_v12 (V1.2) inherits VP_TEST_profile_v11 and generates transition block 'VP_TEST_profile_v11_to_vulkan12'.
        """
        original_json_text = """{
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8.0-131.json#",
            "profiles": {
                "VP_TEST_profile_v10": {
                    "version": 1,
                    "api-version": "1.0.68",
                    "capabilities": ["baseline_v10"]
                },
                "VP_TEST_profile_v11": {
                    "version": 1,
                    "api-version": "1.1.108",
                    "profiles": ["VP_TEST_profile_v10"],
                    "capabilities": ["baseline_v11"]
                },
                "VP_TEST_profile_v12": {
                    "version": 1,
                    "api-version": "1.2.131",
                    "profiles": ["VP_TEST_profile_v11"],
                    "capabilities": ["baseline_v12"]
                }
            },
            "capabilities": {
                "baseline_v10": {},
                "baseline_v11": {},
                "baseline_v12": {}
            }
        }"""

        json_files_dict = {"test_profile.json": json.loads(original_json_text)}
        pull_required_capabilities_profiles_files(self.vk, [PullBits.REQUIRED_CAPABILITIES], json_files_dict)
        sort_profiles_files(self.vk, json_files_dict)

        result_data = json_files_dict["test_profile.json"]
        profiles_dict = result_data["profiles"]
        capabilities_dict = result_data["capabilities"]

        # 1. Verify profile capabilities references
        self.assertEqual(profiles_dict["VP_TEST_profile_v10"].get("capabilities"), ["baseline_v10"])
        self.assertEqual(
            profiles_dict["VP_TEST_profile_v11"].get("capabilities"),
            ["VP_TEST_profile_v10_to_vulkan11", "baseline_v11"]
        )
        self.assertEqual(
            profiles_dict["VP_TEST_profile_v12"].get("capabilities"),
            ["VP_TEST_profile_v11_to_vulkan12", "baseline_v12"]
        )

        # 2. Verify baseline_v10 contains Vulkan 1.0 core features & properties
        self.assertTrue(capabilities_dict["baseline_v10"]["features"]["VkPhysicalDeviceFeatures"].get("robustBufferAccess"))
        self.assertIn("VkPhysicalDeviceProperties", capabilities_dict["baseline_v10"].get("properties", {}))

        # 3. Verify transition block VP_TEST_profile_v10_to_vulkan11
        trans_v11 = capabilities_dict.get("VP_TEST_profile_v10_to_vulkan11", {})
        self.assertTrue(trans_v11["features"]["VkPhysicalDeviceMultiviewFeatures"].get("multiview"))
        self.assertIn("VkPhysicalDeviceMaintenance3Properties", trans_v11.get("properties", {}))
        self.assertIn("VkPhysicalDeviceMultiviewProperties", trans_v11.get("properties", {}))
        self.assertIn("VkPhysicalDeviceProtectedMemoryProperties", trans_v11.get("properties", {}))
        self.assertIn("VkPhysicalDeviceSubgroupProperties", trans_v11.get("properties", {}))

        # 4. Verify transition block VP_TEST_profile_v11_to_vulkan12
        trans_v12 = capabilities_dict.get("VP_TEST_profile_v11_to_vulkan12", {})
        self.assertTrue(trans_v12["features"]["VkPhysicalDeviceVulkan12Features"].get("imagelessFramebuffer"))
        self.assertIn("VkPhysicalDeviceVulkan12Properties", trans_v12.get("properties", {}))


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument(
        '--registry', '-r', action='store', required=False,
        help='Use specified registry file instead of vk.xml.'
    )

    args, unparsed = parser.parse_known_args()
    TestConvertPullRequiredCapabilitiesCoreInheriance.registry_path = args.registry

    unittest.main(argv=[sys.argv[0]] + unparsed)