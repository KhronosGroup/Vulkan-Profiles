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
from source.transform_pull_required_capabilities import (
    pull_required_capabilities_profiles_files,
    pull_extension_dependencies_profiles_files
)


class TestConvertPullCapsExactNoauto(unittest.TestCase):
    registry_path = None

    def setUp(self):
        self.vk: VulkanObject = initVulkanObject('vulkan', self.registry_path)

    def test_pull_exact_noauto_vulkan12(self):
        """
        Verifies that pulling the Vulkan 1.2 core requirements does not pull 'exact' structure
        members (device specific values, e.g. deviceUUID, driverID, pointClippingBehavior)
        while unlisted comparable members still get their unsupported default value.
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
                "baseline": {}
            }
        }"""

        json_files_dict = {"test_profile.json": json.loads(original_json_text)}
        pull_extension_dependencies_profiles_files(self.vk, [PullBits.REQUIRED_CAPABILITIES], json_files_dict)
        pull_required_capabilities_profiles_files(self.vk, [PullBits.REQUIRED_CAPABILITIES], json_files_dict)

        baseline_props = json_files_dict["test_profile.json"]["capabilities"]["baseline"]["properties"]

        # 1. Unlisted comparable member MUST be present with its unsupported default value
        v11_props = baseline_props.get("VkPhysicalDeviceVulkan11Properties", {})
        self.assertIn("subgroupQuadOperationsInAllStages", v11_props)
        self.assertFalse(v11_props["subgroupQuadOperationsInAllStages"])

        # 2. 'exact' members of VkPhysicalDeviceVulkan11Properties MUST NOT be pulled
        self.assertNotIn("deviceUUID", v11_props)
        self.assertNotIn("driverUUID", v11_props)
        self.assertNotIn("deviceLUID", v11_props)
        self.assertNotIn("deviceNodeMask", v11_props)
        self.assertNotIn("deviceLUIDValid", v11_props)
        self.assertNotIn("pointClippingBehavior", v11_props)
        self.assertNotIn("protectedNoFault", v11_props)

        # 3. 'exact' members of VkPhysicalDeviceVulkan12Properties MUST NOT be pulled
        v12_props = baseline_props.get("VkPhysicalDeviceVulkan12Properties", {})
        self.assertNotIn("driverID", v12_props)
        self.assertNotIn("driverName", v12_props)
        self.assertNotIn("driverInfo", v12_props)
        self.assertNotIn("conformanceVersion", v12_props)
        self.assertNotIn("denormBehaviorIndependence", v12_props)
        self.assertNotIn("roundingModeIndependence", v12_props)

    def test_pull_exact_noauto_vulkan14(self):
        """
        Verifies that pulling the Vulkan 1.4 core requirements does not pull 'exact' structure
        members nor 'noauto' members (limit types too complex for a simple resolution, not handled
        yet, e.g. pCopySrcLayouts) nor implementation-dependent members (no value required by the
        Vulkan specification, e.g. timestampPeriod) while comparable members are still pulled.
        """
        original_json_text = """{
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8.0-304.json#",
            "profiles": {
                "VP_TEST_profile": {
                    "version": 1,
                    "api-version": "1.4.304",
                    "capabilities": ["baseline"]
                }
            },
            "capabilities": {
                "baseline": {}
            }
        }"""

        json_files_dict = {"test_profile.json": json.loads(original_json_text)}
        pull_extension_dependencies_profiles_files(self.vk, [PullBits.REQUIRED_CAPABILITIES], json_files_dict)
        pull_required_capabilities_profiles_files(self.vk, [PullBits.REQUIRED_CAPABILITIES], json_files_dict)

        baseline_props = json_files_dict["test_profile.json"]["capabilities"]["baseline"]["properties"]
        v14_props = baseline_props.get("VkPhysicalDeviceVulkan14Properties", {})

        # 1. Comparable required member MUST be present
        self.assertIn("lineSubPixelPrecisionBits", v14_props)
        self.assertEqual(v14_props["lineSubPixelPrecisionBits"], 4)

        # 2. 'exact' members of VkPhysicalDeviceVulkan14Properties MUST NOT be pulled
        self.assertNotIn("polygonModePointSize", v14_props)
        self.assertNotIn("maxCombinedImageSamplerDescriptorCount", v14_props)
        self.assertNotIn("defaultRobustnessStorageBuffers", v14_props)
        self.assertNotIn("defaultRobustnessUniformBuffers", v14_props)
        self.assertNotIn("defaultRobustnessVertexInputs", v14_props)
        self.assertNotIn("defaultRobustnessImages", v14_props)

        # 3. 'noauto' members of VkPhysicalDeviceVulkan14Properties MUST NOT be pulled
        self.assertNotIn("copySrcLayoutCount", v14_props)
        self.assertNotIn("pCopySrcLayouts", v14_props)
        self.assertNotIn("copyDstLayoutCount", v14_props)
        self.assertNotIn("pCopyDstLayouts", v14_props)
        self.assertNotIn("optimalTilingLayoutUUID", v14_props)

        # 4. Unsupported default values MUST be pulled
        baseline_features = json_files_dict["test_profile.json"]["capabilities"]["baseline"].get("features", {})
        v14_features = baseline_features.get("VkPhysicalDeviceVulkan14Features", {})
        self.assertIn("rectangularLines", v14_features)
        self.assertFalse(v14_features["rectangularLines"])
        self.assertIn("hostImageCopy", v14_features)
        self.assertFalse(v14_features["hostImageCopy"])
        v10_features = baseline_features.get("VkPhysicalDeviceFeatures", {})
        self.assertIn("geometryShader", v10_features)
        self.assertFalse(v10_features["geometryShader"])
        v12_props = baseline_props.get("VkPhysicalDeviceVulkan12Properties", {})
        self.assertIn("maxUpdateAfterBindDescriptorsInAllPools", v12_props)
        self.assertEqual(v12_props["maxUpdateAfterBindDescriptorsInAllPools"], 0)

        # 5. Implementation-dependent members MUST NOT be pulled
        limits = baseline_props.get("VkPhysicalDeviceProperties", {}).get("limits", {})
        self.assertIn("nonCoherentAtomSize", limits)
        self.assertNotIn("timestampPeriod", limits)
        self.assertNotIn("optimalBufferCopyOffsetAlignment", limits)
        self.assertNotIn("optimalBufferCopyRowPitchAlignment", limits)
        v13_props = baseline_props.get("VkPhysicalDeviceVulkan13Properties", {})
        self.assertNotIn("minSubgroupSize", v13_props)
        self.assertNotIn("maxSubgroupSize", v13_props)
        self.assertNotIn("storageTexelBufferOffsetAlignmentBytes", v13_props)
        self.assertNotIn("uniformTexelBufferOffsetAlignmentBytes", v13_props)

        # 6. Members without Vulkan core requirement but with a valid unsupported default value MUST get the default value
        self.assertEqual(v13_props["maxComputeWorkgroupSubgroups"], 0)
        self.assertEqual(v13_props["requiredSubgroupSizeStages"], [])

    def test_pull_exact_noauto_ignore_unsupported_vulkan14(self):
        """
        Verifies that pulling the Vulkan 1.4 core requirements with PullBits.IGNORE_UNSUPPORTED
        does not pull 'exact' nor 'noauto' structure members while required members are still pulled.
        """
        original_json_text = """{
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8.0-304.json#",
            "profiles": {
                "VP_TEST_profile": {
                    "version": 1,
                    "api-version": "1.4.304",
                    "capabilities": ["baseline"]
                }
            },
            "capabilities": {
                "baseline": {}
            }
        }"""

        json_files_dict = {"test_profile.json": json.loads(original_json_text)}
        pull_extension_dependencies_profiles_files(self.vk, [PullBits.REQUIRED_CAPABILITIES, PullBits.IGNORE_UNSUPPORTED], json_files_dict)
        pull_required_capabilities_profiles_files(self.vk, [PullBits.REQUIRED_CAPABILITIES, PullBits.IGNORE_UNSUPPORTED], json_files_dict)

        baseline_props = json_files_dict["test_profile.json"]["capabilities"]["baseline"]["properties"]
        v11_props = baseline_props.get("VkPhysicalDeviceVulkan11Properties", {})
        v12_props = baseline_props.get("VkPhysicalDeviceVulkan12Properties", {})
        v14_props = baseline_props.get("VkPhysicalDeviceVulkan14Properties", {})

        # 1. Comparable required members MUST be present
        self.assertIn("maxMultiviewViewCount", v11_props)
        self.assertIn("maxTimelineSemaphoreValueDifference", v12_props)
        self.assertIn("lineSubPixelPrecisionBits", v14_props)
        self.assertEqual(v14_props["lineSubPixelPrecisionBits"], 4)

        # 2. 'exact' members MUST NOT be pulled
        self.assertNotIn("deviceUUID", v11_props)
        self.assertNotIn("deviceLUIDValid", v11_props)
        self.assertNotIn("pointClippingBehavior", v11_props)
        self.assertNotIn("protectedNoFault", v11_props)
        self.assertNotIn("driverID", v12_props)
        self.assertNotIn("conformanceVersion", v12_props)
        self.assertNotIn("denormBehaviorIndependence", v12_props)
        self.assertNotIn("roundingModeIndependence", v12_props)
        self.assertNotIn("polygonModePointSize", v14_props)
        self.assertNotIn("maxCombinedImageSamplerDescriptorCount", v14_props)
        self.assertNotIn("defaultRobustnessStorageBuffers", v14_props)
        self.assertNotIn("defaultRobustnessImages", v14_props)

        # 3. 'noauto' members MUST NOT be pulled
        self.assertNotIn("copySrcLayoutCount", v14_props)
        self.assertNotIn("pCopySrcLayouts", v14_props)
        self.assertNotIn("copyDstLayoutCount", v14_props)
        self.assertNotIn("pCopyDstLayouts", v14_props)
        self.assertNotIn("optimalTilingLayoutUUID", v14_props)

        # 4. Unsupported default values MUST NOT be pulled
        baseline_features = json_files_dict["test_profile.json"]["capabilities"]["baseline"].get("features", {})
        self.assertNotIn("rectangularLines", baseline_features.get("VkPhysicalDeviceVulkan14Features", {}))
        self.assertNotIn("hostImageCopy", baseline_features.get("VkPhysicalDeviceVulkan14Features", {}))
        self.assertNotIn("geometryShader", baseline_features.get("VkPhysicalDeviceFeatures", {}))
        self.assertNotIn("maxUpdateAfterBindDescriptorsInAllPools", v12_props)


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument(
        '--registry', '-r', action='store', required=False,
        help='Use specified registry file instead of vk.xml.'
    )

    args, unparsed = parser.parse_known_args()
    TestConvertPullCapsExactNoauto.registry_path = args.registry

    unittest.main(argv=[sys.argv[0]] + unparsed)
