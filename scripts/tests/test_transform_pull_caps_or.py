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
    pull_extension_dependencies_profiles_files,
    pull_required_capabilities_profiles_files
)
from source.transform_sort import sort_profiles_files


class TestConvertPullRequiredCapabilitiesOR(unittest.TestCase):
    registry_path = None

    def setUp(self):
        self.vk: VulkanObject = initVulkanObject('vulkan', self.registry_path)

    def test_pull_required_capabilities_or_extension_feature_requirement(self):
        """
        Case 1: An extension (e.g. VK_KHR_shader_float16_int8) specifies an OR feature requirement
        in vk.xml (e.g., field="shaderFloat16, shaderInt8").
        When neither feature option is pre-enabled in context, pull-required-capabilities
        extracts the extension from the source block, creates variant blocks for each feature option,
        and adds an OR variant array [baseline_shaderFloat16, baseline_shaderInt8] into the profile.
        """
        original_json_text = """{
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8.0-106.json#",
            "profiles": {
                "VP_TEST_profile_or_ext": {
                    "version": 1,
                    "api-version": "1.0.68",
                    "capabilities": ["baseline"]
                }
            },
            "capabilities": {
                "baseline": {
                    "extensions": {
                        "VK_KHR_shader_float16_int8": 1
                    }
                }
            }
        }"""

        json_files_dict = {"test_profile.json": json.loads(original_json_text)}
        pull_extension_dependencies_profiles_files(self.vk, [PullBits.REQUIRED_CAPABILITIES, PullBits.IGNORE_EXTENSION_VERSIONS], json_files_dict)
        pull_required_capabilities_profiles_files(self.vk, [PullBits.REQUIRED_CAPABILITIES], json_files_dict)
        sort_profiles_files(self.vk, json_files_dict)

        result_data = json_files_dict["test_profile.json"]
        profile_obj = result_data["profiles"]["VP_TEST_profile_or_ext"]
        capabilities_dict = result_data["capabilities"]

        # 1. Verify profile capabilities array contains baseline and the OR variant array
        self.assertEqual(
            profile_obj.get("capabilities"),
            ["baseline", ["baseline_shaderFloat16", "baseline_shaderInt8"]]
        )

        # 2. Verify VK_KHR_shader_float16_int8 was extracted from baseline extensions
        self.assertNotIn("VK_KHR_shader_float16_int8", capabilities_dict["baseline"].get("extensions", {}))

        # 3. Verify variant block baseline_shaderFloat16
        self.assertIn("baseline_shaderFloat16", capabilities_dict)
        self.assertEqual(
            capabilities_dict["baseline_shaderFloat16"].get("extensions"),
            {"VK_KHR_shader_float16_int8": 1}
        )
        self.assertEqual(
            capabilities_dict["baseline_shaderFloat16"].get("features"),
            {"VkPhysicalDeviceShaderFloat16Int8FeaturesKHR": {"shaderFloat16": True}}
        )

        # 4. Verify variant block baseline_shaderInt8
        self.assertIn("baseline_shaderInt8", capabilities_dict)
        self.assertEqual(
            capabilities_dict["baseline_shaderInt8"].get("extensions"),
            {"VK_KHR_shader_float16_int8": 1}
        )
        self.assertEqual(
            capabilities_dict["baseline_shaderInt8"].get("features"),
            {"VkPhysicalDeviceShaderFloat16Int8FeaturesKHR": {"shaderInt8": True}}
        )

    def test_pull_required_capabilities_or_variant_capability_blocks(self):
        """
        Case 2: Profile includes an array of OR-variant capability blocks:
        "capabilities": [ ["primitivesGeneratedQuery", "pipelineStatisticsQuery"] ]
        Verifies that capabilities conversion handles variant array structures correctly,
        pulling extension dependencies for each block while leaving the variant array intact.
        """
        original_json_text = """{
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8.0-106.json#",
            "profiles": {
                "VP_TEST_profile_variant_blocks": {
                    "version": 1,
                    "api-version": "1.0.68",
                    "capabilities": [
                        ["primitivesGeneratedQuery", "pipelineStatisticsQuery"]
                    ]
                }
            },
            "capabilities": {
                "primitivesGeneratedQuery": {
                    "extensions": {
                        "VK_EXT_primitives_generated_query": 1
                    },
                    "features": {
                        "VkPhysicalDevicePrimitivesGeneratedQueryFeaturesEXT": {
                            "primitivesGeneratedQuery": true
                        }
                    }
                },
                "pipelineStatisticsQuery": {
                    "features": {
                        "VkPhysicalDeviceFeatures": {
                            "pipelineStatisticsQuery": true
                        }
                    }
                }
            }
        }"""

        json_files_dict = {"test_profile.json": json.loads(original_json_text)}
        pull_extension_dependencies_profiles_files(self.vk, [PullBits.REQUIRED_CAPABILITIES, PullBits.IGNORE_EXTENSION_VERSIONS], json_files_dict)
        pull_required_capabilities_profiles_files(self.vk, [PullBits.REQUIRED_CAPABILITIES], json_files_dict)
        sort_profiles_files(self.vk, json_files_dict)

        result_data = json_files_dict["test_profile.json"]
        profile_obj = result_data["profiles"]["VP_TEST_profile_variant_blocks"]
        capabilities_dict = result_data["capabilities"]

        # Capabilities array remains intact with the inner array of variant blocks
        self.assertEqual(
            profile_obj.get("capabilities"),
            [["primitivesGeneratedQuery", "pipelineStatisticsQuery"]]
        )

        # primitivesGeneratedQuery block pulled dependencies correctly
        self.assertIn("VK_EXT_primitives_generated_query", capabilities_dict["primitivesGeneratedQuery"].get("extensions", {}))
        self.assertTrue(
            capabilities_dict["primitivesGeneratedQuery"]["features"]["VkPhysicalDevicePrimitivesGeneratedQueryFeaturesEXT"]["primitivesGeneratedQuery"]
        )

        # pipelineStatisticsQuery block remains valid
        self.assertTrue(
            capabilities_dict["pipelineStatisticsQuery"]["features"]["VkPhysicalDeviceFeatures"]["pipelineStatisticsQuery"]
        )

    def test_pull_required_capabilities_or_inheritance_ext_to_khr(self):
        """
        Case 4: Profile inheritance where parent profile uses VK_EXT_line_rasterization
        and child profile uses VK_KHR_line_rasterization.
        Verifies that OR feature extraction creates variant capability blocks independently
        for parent (VK_EXT_line_rasterization -> VkPhysicalDeviceLineRasterizationFeaturesEXT)
        and child (VK_KHR_line_rasterization -> VkPhysicalDeviceLineRasterizationFeaturesKHR).
        """
        original_json_text = """{
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8.0-106.json#",
            "profiles": {
                "VP_TEST_profile_parent": {
                    "version": 1,
                    "api-version": "1.0.68",
                    "capabilities": ["baseline_parent"]
                },
                "VP_TEST_profile_child": {
                    "version": 1,
                    "api-version": "1.0.68",
                    "profiles": ["VP_TEST_profile_parent"],
                    "capabilities": ["baseline_child"]
                }
            },
            "capabilities": {
                "baseline_parent": {
                    "extensions": {
                        "VK_EXT_line_rasterization": 1
                    }
                },
                "baseline_child": {
                    "extensions": {
                        "VK_KHR_line_rasterization": 1
                    }
                }
            }
        }"""

        json_files_dict = {"test_profile.json": json.loads(original_json_text)}
        pull_extension_dependencies_profiles_files(self.vk, [PullBits.REQUIRED_CAPABILITIES, PullBits.IGNORE_EXTENSION_VERSIONS], json_files_dict)
        pull_required_capabilities_profiles_files(self.vk, [PullBits.REQUIRED_CAPABILITIES], json_files_dict)
        sort_profiles_files(self.vk, json_files_dict)

        result_data = json_files_dict["test_profile.json"]
        parent_profile = result_data["profiles"]["VP_TEST_profile_parent"]
        child_profile = result_data["profiles"]["VP_TEST_profile_child"]
        capabilities_dict = result_data["capabilities"]

        expected_parent_variants = [
            "baseline_parent_rectangularLines",
            "baseline_parent_bresenhamLines",
            "baseline_parent_smoothLines",
            "baseline_parent_stippledRectangularLines",
            "baseline_parent_stippledBresenhamLines",
            "baseline_parent_stippledSmoothLines"
        ]

        # Parent gets variant blocks for line rasterization features
        self.assertEqual(
            parent_profile.get("capabilities"),
            ["baseline_parent", expected_parent_variants]
        )

        # Child capabilities remains ["baseline_child"]
        self.assertEqual(child_profile.get("capabilities"), ["baseline_child"])

        # Check each parent variant block
        for var_name in expected_parent_variants:
            self.assertIn(var_name, capabilities_dict)
            feature_field = var_name.replace("baseline_parent_", "")
            self.assertEqual(
                capabilities_dict[var_name].get("extensions"),
                {"VK_EXT_line_rasterization": 1}
            )
            self.assertTrue(
                capabilities_dict[var_name]["features"]["VkPhysicalDeviceLineRasterizationFeaturesEXT"].get(feature_field)
            )

    def test_pull_required_capabilities_or_line_rasterization_already_satisfied(self):
        """
        Case 5: VK_KHR_line_rasterization specifies an OR feature requirement in vk.xml,
        and two of the feature options (rectangularLines and bresenhamLines) are ALREADY
        pre-enabled in the baseline capability block.
        Verifies that because the OR feature condition is already satisfied in context,
        no OR variant capability blocks are extracted or created, and the profile capabilities
        array remains ["baseline"].
        """
        original_json_text = """{
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8.0-106.json#",
            "profiles": {
                "VP_TEST_profile_or_satisfied": {
                    "version": 1,
                    "api-version": "1.0.68",
                    "capabilities": ["baseline"]
                }
            },
            "capabilities": {
                "baseline": {
                    "extensions": {
                        "VK_KHR_line_rasterization": 1
                    },
                    "features": {
                        "VkPhysicalDeviceLineRasterizationFeaturesKHR": {
                            "rectangularLines": true,
                            "bresenhamLines": true
                        }
                    }
                }
            }
        }"""

        json_files_dict = {"test_profile.json": json.loads(original_json_text)}
        pull_extension_dependencies_profiles_files(self.vk, [PullBits.REQUIRED_CAPABILITIES, PullBits.IGNORE_EXTENSION_VERSIONS], json_files_dict)
        pull_required_capabilities_profiles_files(self.vk, [PullBits.REQUIRED_CAPABILITIES], json_files_dict)
        sort_profiles_files(self.vk, json_files_dict)

        result_data = json_files_dict["test_profile.json"]
        profile_obj = result_data["profiles"]["VP_TEST_profile_or_satisfied"]
        capabilities_dict = result_data["capabilities"]

        # 1. Profile capabilities array remains unchanged
        self.assertEqual(profile_obj.get("capabilities"), ["baseline"])

        # 2. VK_KHR_line_rasterization remains in baseline extensions
        self.assertIn("VK_KHR_line_rasterization", capabilities_dict["baseline"].get("extensions", {}))

        # 3. No variant capability blocks created
        self.assertEqual(list(capabilities_dict.keys()), ["baseline"])

        # 4. Features in baseline contain rectangularLines and bresenhamLines
        line_feats = capabilities_dict["baseline"]["features"]["VkPhysicalDeviceLineRasterizationFeaturesKHR"]
        self.assertTrue(line_feats.get("rectangularLines"))
        self.assertTrue(line_feats.get("bresenhamLines"))


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument(
        '--registry', '-r', action='store', required=False,
        help='Use specified registry file instead of vk.xml.'
    )

    args, unparsed = parser.parse_known_args()
    TestConvertPullRequiredCapabilitiesOR.registry_path = args.registry

    unittest.main(argv=[sys.argv[0]] + unparsed)