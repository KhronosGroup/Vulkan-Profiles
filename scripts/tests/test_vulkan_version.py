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
from pathlib import Path
import sys
import unittest

scripts_dir = Path(__file__).resolve().parent.parent
if str(scripts_dir) not in sys.path:
    sys.path.insert(0, str(scripts_dir))

from source.vulkan_object_version import (
    VK_VERSION,
    BUNDLE_STRUCT_VERSIONS,
    is_bundle_structure,
    get_bundle_structure_core_version
)


class TestVulkanObjectVersion(unittest.TestCase):

    def tearDown(self):
        """Cleans up non-standard dynamic version instances to prevent test pollution."""
        standard_keys = {(-1, -1), (1, 0), (1, 1), (1, 2), (1, 3), (1, 4)}
        for key in list(VK_VERSION._instances.keys()):
            if key not in standard_keys:
                del VK_VERSION._instances[key]

    def testFromString(self):
        """Tests VK_VERSION.from_string across various string formats."""
        self.assertEqual(VK_VERSION.from_string("1.3.276"), VK_VERSION.V1_3)
        self.assertEqual(VK_VERSION.from_string("1.4.335"), VK_VERSION.V1_4)
        self.assertEqual(VK_VERSION.from_string("1.1"), VK_VERSION.V1_1)
        self.assertEqual(VK_VERSION.from_string("VK_VERSION_1_2"), VK_VERSION.V1_2)
        self.assertEqual(VK_VERSION.from_string("VK_NONE"), VK_VERSION.NONE)
        self.assertEqual(VK_VERSION.from_string("NONE"), VK_VERSION.NONE)
        self.assertEqual(VK_VERSION.from_string(""), VK_VERSION.NONE)
        self.assertEqual(VK_VERSION.from_string(None), VK_VERSION.NONE)

    def testAsTuple(self):
        """Tests numeric tuple extraction from VK_VERSION enums."""
        self.assertEqual(VK_VERSION.V1_0.as_tuple(), (1, 0))
        self.assertEqual(VK_VERSION.V1_1.as_tuple(), (1, 1))
        self.assertEqual(VK_VERSION.V1_3.as_tuple(), (1, 3))
        self.assertEqual(VK_VERSION.V1_4.as_tuple(), (1, 4))
        self.assertEqual(VK_VERSION.NONE.as_tuple(), (-1, -1))

    def testComparisons(self):
        """Tests rich comparisons (<, <=, >, >=, ==) between VK_VERSION enums and string representations."""
        # Enum vs Enum comparisons
        self.assertTrue(VK_VERSION.V1_4 > VK_VERSION.V1_3)
        self.assertTrue(VK_VERSION.V1_3 >= VK_VERSION.V1_3)
        self.assertTrue(VK_VERSION.V1_1 < VK_VERSION.V1_2)
        self.assertTrue(VK_VERSION.V1_0 <= VK_VERSION.V1_0)
        self.assertTrue(VK_VERSION.NONE < VK_VERSION.V1_0)

        # Enum vs String comparisons
        self.assertTrue(VK_VERSION.V1_4 >= "1.3.273")
        self.assertTrue(VK_VERSION.V1_1 < "1.2")
        self.assertTrue(VK_VERSION.V1_3 == "1.3.276")

    def testVersionLists(self):
        """Tests VK_VERSION.versions() helper method."""
        expected_versions = [
            VK_VERSION.V1_0,
            VK_VERSION.V1_1,
            VK_VERSION.V1_2,
            VK_VERSION.V1_3,
            VK_VERSION.V1_4,
        ]

        self.assertEqual(VK_VERSION.versions(), expected_versions)

    def testMetaclassIterationAndLen(self):
        """Tests metaclass iteration list(VK_VERSION) and len(VK_VERSION)."""
        expected_versions = [
            VK_VERSION.V1_0,
            VK_VERSION.V1_1,
            VK_VERSION.V1_2,
            VK_VERSION.V1_3,
            VK_VERSION.V1_4,
        ]

        self.assertEqual(len(VK_VERSION), len(expected_versions))
        self.assertEqual(list(VK_VERSION), expected_versions)
        self.assertIn(VK_VERSION.V1_0, VK_VERSION)
        self.assertIn(VK_VERSION.V1_4, VK_VERSION)

    def testMetaclassSubscript(self):
        """Tests item indexing VK_VERSION[key] via attribute, version string, or version enum string."""
        self.assertEqual(VK_VERSION["V1_1"], VK_VERSION.V1_1)
        self.assertEqual(VK_VERSION["VK_VERSION_1_2"], VK_VERSION.V1_2)
        self.assertEqual(VK_VERSION["1.3"], VK_VERSION.V1_3)
        self.assertEqual(VK_VERSION["VK_NONE"], VK_VERSION.NONE)

        with self.assertRaises(KeyError):
            _ = VK_VERSION["INVALID_VERSION_NAME"]

    def testInstantiation(self):
        """Tests instantiating VK_VERSION via single string token, major/minor arguments, or tuple."""
        self.assertEqual(VK_VERSION("VK_VERSION_1_1"), VK_VERSION.V1_1)
        self.assertEqual(VK_VERSION("1.2"), VK_VERSION.V1_2)
        self.assertEqual(VK_VERSION(1, 3), VK_VERSION.V1_3)
        self.assertEqual(VK_VERSION(VK_VERSION.V1_4), VK_VERSION.V1_4)

        with self.assertRaises(ValueError):
            _ = VK_VERSION("INVALID_VERSION_NAME")

    def testDynamicVersions(self):
        """Tests dynamic resolution and comparison of future Vulkan core versions (e.g. 1.5, 2.0)."""
        v1_5 = VK_VERSION.from_string("1.5.100")
        v2_0 = VK_VERSION.from_string("VK_VERSION_2_0")

        self.assertEqual(v1_5.as_tuple(), (1, 5))
        self.assertEqual(v2_0.as_tuple(), (2, 0))

        self.assertTrue(v1_5 > VK_VERSION.V1_4)
        self.assertTrue(v2_0 > v1_5)
        self.assertTrue(v1_5 >= "1.5.0")

    def testBundleStructureDetection(self):
        """Tests dynamic bundle structure detection and version mapping for current and future versions."""
        self.assertTrue(is_bundle_structure("VkPhysicalDeviceFeatures"))
        self.assertTrue(is_bundle_structure("VkPhysicalDeviceVulkan11Features"))
        self.assertTrue(is_bundle_structure("VkPhysicalDeviceVulkan15Features"))
        self.assertTrue(is_bundle_structure("VkPhysicalDeviceVulkan20Properties"))
        self.assertFalse(is_bundle_structure("VkPhysicalDeviceCustomBorderColorFeaturesEXT"))

        self.assertEqual(get_bundle_structure_core_version("VkPhysicalDeviceFeatures"), VK_VERSION.V1_0)
        self.assertEqual(get_bundle_structure_core_version("VkPhysicalDeviceVulkan11Features"), VK_VERSION.V1_2)
        self.assertEqual(get_bundle_structure_core_version("VkPhysicalDeviceVulkan14Properties"), VK_VERSION.V1_4)
        self.assertEqual(get_bundle_structure_core_version("VkPhysicalDeviceVulkan15Features"), VK_VERSION.from_string("1.5"))

        self.assertEqual(BUNDLE_STRUCT_VERSIONS["VkPhysicalDeviceVulkan11Features"], (1, 2))
        self.assertEqual(BUNDLE_STRUCT_VERSIONS["VkPhysicalDeviceVulkan15Features"], (1, 5))


if __name__ == '__main__':
    parser = argparse.ArgumentParser()

    parser.add_argument(
        '--registry', '-r', action='store', required=False,
        help='Optional registry file path for compatibility with test runners.'
    )

    args, unparsed = parser.parse_known_args()

    unittest.main(argv=[sys.argv[0]] + unparsed)
    