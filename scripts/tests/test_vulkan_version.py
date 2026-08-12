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

from source.vulkan_object_version import VK_VERSION


class TestVulkanObjectVersion(unittest.TestCase):

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


if __name__ == '__main__':
    parser = argparse.ArgumentParser()

    parser.add_argument(
        '--registry', '-r', action='store', required=False,
        help='Optional registry file path for compatibility with test runners.'
    )

    args, unparsed = parser.parse_known_args()

    unittest.main(argv=[sys.argv[0]] + unparsed)
    