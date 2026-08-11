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

from vulkan_object import VulkanObject
from source.vulkan_object_utils import initVulkanObject
from source.main_convert import FormatFeatureFlagConverter


class TestFormatFeatureFlagConverter(unittest.TestCase):
    registry_path = None

    @classmethod
    def setUpClass(cls):
        cls.vk: VulkanObject = initVulkanObject('vulkan', cls.registry_path)
        cls.converter = FormatFeatureFlagConverter(cls.vk)

    def testFormatFeature32To64Conversion(self):
        flags32 = [
            "VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT",
            "VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT",
            "VK_FORMAT_FEATURE_BLIT_SRC_BIT",
            "VK_FORMAT_FEATURE_TRANSFER_DST_BIT",
        ]
        flags64 = self.converter.to_flag64_list(flags32)

        self.assertEqual(len(flags64), 4)
        self.assertIn("VK_FORMAT_FEATURE_2_SAMPLED_IMAGE_BIT", flags64)
        self.assertIn("VK_FORMAT_FEATURE_2_COLOR_ATTACHMENT_BIT", flags64)
        self.assertIn("VK_FORMAT_FEATURE_2_BLIT_SRC_BIT", flags64)
        self.assertIn("VK_FORMAT_FEATURE_2_TRANSFER_DST_BIT", flags64)

    def testFormatFeature64To32Conversion(self):
        flags64 = [
            "VK_FORMAT_FEATURE_2_SAMPLED_IMAGE_BIT",
            "VK_FORMAT_FEATURE_2_COLOR_ATTACHMENT_BIT",
            "VK_FORMAT_FEATURE_2_BLIT_SRC_BIT",
            "VK_FORMAT_FEATURE_2_TRANSFER_DST_BIT",
        ]
        flags32 = self.converter.to_flag32_list(flags64)

        self.assertEqual(len(flags32), 4)
        self.assertIn("VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT", flags32)
        self.assertIn("VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT", flags32)
        self.assertIn("VK_FORMAT_FEATURE_BLIT_SRC_BIT", flags32)
        self.assertIn("VK_FORMAT_FEATURE_TRANSFER_DST_BIT", flags32)

    def testExtendedFlags32To4KHRConversion(self):
        """
        Verifies 32-bit flags conversion to VK_KHR_extended_flags (VkFormatFeatureFlagBits4KHR).
        """
        flags32 = [
            "VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT",
            "VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT",
            "VK_FORMAT_FEATURE_BLIT_SRC_BIT",
            "VK_FORMAT_FEATURE_TRANSFER_DST_BIT",
        ]
        flags4khr = self.converter.to_flag4khr_list(flags32)

        self.assertEqual(len(flags4khr), 4)
        for flag in flags4khr:
            self.assertTrue(
                flag.startswith("VK_FORMAT_FEATURE_4_") or flag.startswith("VK_FORMAT_FEATURE_2_"),
                f"Flag '{flag}' should start with a valid 64-bit format feature flag prefix"
            )

    def testExtendedFlags4KHRTo32Conversion(self):
        """
        Verifies VkFormatFeatureFlagBits4KHR flags convert back to 32-bit VkFormatFeatureFlagBits.
        """
        # Retrieve mapped 4KHR flags for canonical 32-bit flags
        flags32_expected = [
            "VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT",
            "VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT",
        ]
        flags4khr = self.converter.to_flag4khr_list(flags32_expected)

        # Convert back from 4KHR -> 32-bit
        flags32_actual = self.converter.to_flag32_list(flags4khr)

        self.assertEqual(len(flags32_actual), 2)
        for expected in flags32_expected:
            self.assertIn(expected, flags32_actual)

    def testExtendedFlagsPassthrough(self):
        """
        Ensures already-converted VkFormatFeatureFlagBits4KHR / extended flags pass through unchanged.
        """
        # Mapped 4KHR flag should pass through to_flag4khr_list
        flags32 = ["VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT"]
        flags4khr = self.converter.to_flag4khr_list(flags32)
        self.assertGreater(len(flags4khr), 0)

        passthrough_result = self.converter.to_flag4khr_list(flags4khr)
        self.assertEqual(flags4khr, passthrough_result)

    def testPassthroughAlreadyConvertedFlags(self):
        # 64-bit flag passed into to_flag64_list should pass through unchanged
        flags64_input = ["VK_FORMAT_FEATURE_2_SAMPLED_IMAGE_BIT"]
        flags64_output = self.converter.to_flag64_list(flags64_input)
        self.assertEqual(flags64_output, ["VK_FORMAT_FEATURE_2_SAMPLED_IMAGE_BIT"])

        # 32-bit flag passed into to_flag32_list should pass through unchanged
        flags32_input = ["VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT"]
        flags32_output = self.converter.to_flag32_list(flags32_input)
        self.assertEqual(flags32_output, ["VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT"])

    def testEmptyList(self):
        self.assertEqual(self.converter.to_flag64_list([]), [])
        self.assertEqual(self.converter.to_flag32_list([]), [])
        self.assertEqual(self.converter.to_flag4khr_list([]), [])


if __name__ == '__main__':
    parser = argparse.ArgumentParser()

    parser.add_argument(
        '--registry', '-r', action='store', required=True,
        help='Use specified registry file instead of vk.xml.'
    )

    args, unparsed = parser.parse_known_args()
    TestFormatFeatureFlagConverter.registry_path = args.registry

    unittest.main(argv=[sys.argv[0]] + unparsed)

    