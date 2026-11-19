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

import argparse
from pathlib import Path
import sys
import unittest

scripts_dir = Path(__file__).resolve().parent.parent
if str(scripts_dir) not in sys.path:
    sys.path.insert(0, str(scripts_dir))

from vulkan_object import VulkanObject
from source.vulkan_object_utils import initVulkanObject
from source.generate_profiles_combine import VulkanProfilesCombineGenerator, CombineConfig

TESSELLATION = "VkPhysicalDeviceFeatures::tessellationShader"
GEOMETRY = "VkPhysicalDeviceFeatures::geometryShader"


def make_profile(profile_name: str, max_image_dimension_2d: int, tessellation_level: int | None, features: dict | None = None) -> dict:
    """A profile with an unconditional block and, with tessellation_level, a block with a tessellationShader condition."""
    unconditional = {"properties": {"VkPhysicalDeviceProperties": {"limits": {"maxImageDimension2D": max_image_dimension_2d}}}}
    if features:
        unconditional["features"] = {"VkPhysicalDeviceFeatures": features}
    capabilities = {"baseline": unconditional}
    if tessellation_level is not None:
        capabilities["tessellation"] = {
            "depends": TESSELLATION,
            "properties": {"VkPhysicalDeviceProperties": {"limits": {"maxTessellationGenerationLevel": tessellation_level}}}
        }
    return {
        "$schema": "https://schema.khronos.org/vulkan/profiles-0.8-latest.json#",
        "profiles": {profile_name: {"version": 1, "api-version": "1.2.0", "capabilities": list(capabilities.keys())}},
        "capabilities": capabilities
    }


class TestCombineDepends(unittest.TestCase):
    registry_path = None

    @classmethod
    def setUpClass(cls):
        cls.vk: VulkanObject = initVulkanObject('vulkan', cls.registry_path)

    def combine(self, json_a: dict, json_b: dict, mode: str) -> dict:
        p_config = CombineConfig.__new__(CombineConfig)
        p_config.name = "VP_TEST_combined"
        p_config.version = 1
        p_config.label = "Test combined profile"
        p_config.description = "Test profile"
        p_config.stage = "STABLE"
        p_config.api_version = ["1", "2", "0"]
        p_config.required_profiles = []
        p_config.input_jsons = [json_a, json_b]
        p_config.input_profile_names = ["VP_TEST_a", "VP_TEST_b"]
        p_config.input_profile_values = [json_a["profiles"]["VP_TEST_a"], json_b["profiles"]["VP_TEST_b"]]
        p_config.combine_mode = mode

        profile_file = {"capabilities": {}, "profiles": {}}
        VulkanProfilesCombineGenerator(self.vk).combine(p_config, profile_file, mode=mode)
        return profile_file

    def get_limits(self, profile_file: dict, block_name: str) -> dict:
        return profile_file["capabilities"][block_name]["properties"]["VkPhysicalDeviceProperties"]["limits"]

    def test_combine_depends_same_condition(self):
        """The blocks with the same condition are combined into a block with that condition, following the combine mode."""
        json_a = make_profile("VP_TEST_a", 16384, 64)
        json_b = make_profile("VP_TEST_b", 8192, 32)

        for mode, expected_dimension, expected_level in (("intersection", 8192, 32), ("union", 16384, 64)):
            with self.subTest(mode=mode):
                result = self.combine(json_a, json_b, mode)
                self.assertEqual(result["profiles"]["VP_TEST_combined"]["capabilities"],
                                 ["VP_TEST_combined_block", "VP_TEST_combined_block_tessellationShader"])
                self.assertEqual(self.get_limits(result, "VP_TEST_combined_block"), {"maxImageDimension2D": expected_dimension})
                block = result["capabilities"]["VP_TEST_combined_block_tessellationShader"]
                self.assertEqual(block["depends"], TESSELLATION)
                self.assertEqual(self.get_limits(result, "VP_TEST_combined_block_tessellationShader"), {"maxTessellationGenerationLevel": expected_level})

    def test_combine_depends_missing_condition(self):
        """A profile without a block with the condition contributes its unconditional requirements to the devices satisfying it."""
        json_a = make_profile("VP_TEST_a", 16384, 64)
        json_b = make_profile("VP_TEST_b", 8192, None)

        # Profile B doesn't require any tessellation level from the devices supporting tessellationShader
        result = self.combine(json_a, json_b, "intersection")
        self.assertEqual(result["profiles"]["VP_TEST_combined"]["capabilities"], ["VP_TEST_combined_block"])

        result = self.combine(json_a, json_b, "union")
        self.assertEqual(self.get_limits(result, "VP_TEST_combined_block_tessellationShader"), {"maxTessellationGenerationLevel": 64})

    def test_combine_depends_different_conditions(self):
        """
        Different conditions are combined with their conjunction for an intersection, while a union keeps a block per
        condition: a device satisfying only one condition doesn't satisfy the requirements of the other.
        """
        json_a = make_profile("VP_TEST_a", 16384, 64)
        json_b = make_profile("VP_TEST_b", 8192, None)
        json_b["capabilities"]["geometry"] = {
            "depends": GEOMETRY,
            "properties": {"VkPhysicalDeviceProperties": {"limits": {"maxTessellationGenerationLevel": 32}}}
        }
        json_b["profiles"]["VP_TEST_b"]["capabilities"].append("geometry")

        result = self.combine(json_a, json_b, "intersection")
        self.assertEqual(result["profiles"]["VP_TEST_combined"]["capabilities"],
                         ["VP_TEST_combined_block", "VP_TEST_combined_block_tessellationShader_and_geometryShader"])
        self.assertEqual(result["capabilities"]["VP_TEST_combined_block_tessellationShader_and_geometryShader"]["depends"], f"{TESSELLATION}+{GEOMETRY}")
        self.assertEqual(self.get_limits(result, "VP_TEST_combined_block_tessellationShader_and_geometryShader"), {"maxTessellationGenerationLevel": 32})

        result = self.combine(json_a, json_b, "union")
        self.assertEqual(result["profiles"]["VP_TEST_combined"]["capabilities"],
                         ["VP_TEST_combined_block", "VP_TEST_combined_block_tessellationShader", "VP_TEST_combined_block_geometryShader"])
        self.assertEqual(self.get_limits(result, "VP_TEST_combined_block_tessellationShader"), {"maxTessellationGenerationLevel": 64})
        self.assertEqual(self.get_limits(result, "VP_TEST_combined_block_geometryShader"), {"maxTessellationGenerationLevel": 32})

    def test_combine_depends_satisfied_condition(self):
        """A condition satisfied by the combined unconditional features is merged into the unconditional block."""
        json_a = make_profile("VP_TEST_a", 16384, 64)
        json_b = make_profile("VP_TEST_b", 8192, 32, features={"tessellationShader": True})

        result = self.combine(json_a, json_b, "union")
        self.assertEqual(result["profiles"]["VP_TEST_combined"]["capabilities"], ["VP_TEST_combined_block"])
        self.assertEqual(self.get_limits(result, "VP_TEST_combined_block"), {"maxImageDimension2D": 16384, "maxTessellationGenerationLevel": 64})


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument(
        '--registry', '-r', action='store', required=False,
        help='Use specified registry file instead of vk.xml.'
    )

    args, unparsed = parser.parse_known_args()
    TestCombineDepends.registry_path = args.registry

    unittest.main(argv=[sys.argv[0]] + unparsed)
