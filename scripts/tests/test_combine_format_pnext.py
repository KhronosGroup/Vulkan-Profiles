#!/usr/bin/python3
#
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

import os
import sys
import json
import argparse
import unittest
import tempfile
from pathlib import Path

scripts_dir = Path(__file__).resolve().parent.parent
if str(scripts_dir) not in sys.path:
    sys.path.insert(0, str(scripts_dir))

from source.main_combine import main_combine, CombineMode

REGISTRY_PATH = None


def make_profile(profile_name, formats):
    return {
        "$schema": "https://schema.khronos.org/vulkan/profiles-0.8-latest.json#",
        "capabilities": {
            "block": {
                "extensions": {
                    "VK_EXT_image_drm_format_modifier": 1,
                    "VK_EXT_multisampled_render_to_single_sampled": 1,
                    "VK_ARM_tensors": 1,
                    "VK_ARM_tensor_controls": 1
                },
                "formats": {
                    "VK_FORMAT_R8G8B8A8_UNORM": formats
                }
            }
        },
        "profiles": {
            profile_name: {
                "version": 1,
                "api-version": "1.3.204",
                "label": profile_name,
                "description": profile_name,
                "capabilities": ["block"]
            }
        }
    }


def drm_modifier(modifier, plane_count, features):
    return {"drmFormatModifier": modifier, "drmFormatModifierPlaneCount": plane_count, "drmFormatModifierTilingFeatures": features}


PROFILE_A_FORMATS = {
    "VkSubpassResolvePerformanceQueryEXT": {"optimal": True},
    "VkTensorFormatPropertiesARM": {
        "optimalTilingTensorFeatures": ["VK_FORMAT_FEATURE_2_TENSOR_SHADER_BIT_ARM"]
    },
    "VkTensorExplicitTilingFormatPropertiesARM": {
        "brick16TilingTensorFeatures": ["VK_FORMAT_FEATURE_2_TENSOR_SHADER_BIT_ARM"]
    },
    "VkDrmFormatModifierPropertiesListEXT": {
        "drmFormatModifierCount": 1,
        "pDrmFormatModifierProperties": [drm_modifier(0, 1, ["VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT"])]
    }
}

PROFILE_B_FORMATS = {
    "VkSubpassResolvePerformanceQueryEXT": {"optimal": False},
    "VkTensorFormatPropertiesARM": {
        "optimalTilingTensorFeatures": ["VK_FORMAT_FEATURE_2_TENSOR_SHADER_BIT_ARM", "VK_FORMAT_FEATURE_2_TENSOR_IMAGE_ALIASING_BIT_ARM"],
        "linearTilingTensorFeatures": ["VK_FORMAT_FEATURE_2_TENSOR_SHADER_BIT_ARM"]
    },
    "VkDrmFormatModifierPropertiesListEXT": {
        "drmFormatModifierCount": 2,
        "pDrmFormatModifierProperties": [
            drm_modifier(0, 1, ["VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT", "VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT"]),
            drm_modifier(7, 2, ["VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT"])
        ]
    }
}


class TestCombineFormatPNext(unittest.TestCase):
    """Tests combining the VkFormatProperties2 pNext chain structures other than VkFormatProperties[3]."""

    def setUp(self):
        if not REGISTRY_PATH or not os.path.exists(REGISTRY_PATH):
            self.skipTest(f"Vulkan registry XML (vk.xml) not found at: {REGISTRY_PATH}")

    def combine(self, mode):
        with tempfile.TemporaryDirectory() as temp_dir:
            temp_path = Path(temp_dir)
            input_dir = temp_path / "input"
            input_dir.mkdir()
            with open(input_dir / "profile_a.json", "w", encoding="utf-8") as f:
                json.dump(make_profile("VP_TEST_profile_a", PROFILE_A_FORMATS), f)
            with open(input_dir / "profile_b.json", "w", encoding="utf-8") as f:
                json.dump(make_profile("VP_TEST_profile_b", PROFILE_B_FORMATS), f)

            output_path = temp_path / ("output" if mode == CombineMode.DIFFERENCE else "combined.json")
            args = argparse.Namespace(
                registry=REGISTRY_PATH,
                input=str(input_dir),
                output=str(output_path),
                mode=mode,
                profile_name="VP_TEST_combined",
                api="vulkan",
                format="pretty",
                transform=None,
                validate=None
            )
            main_combine(args)

            if mode == CombineMode.DIFFERENCE:
                result = {}
                for name in ("profile_a", "profile_b"):
                    with open(output_path / (name + ".json"), "r", encoding="utf-8") as f:
                        result[name] = json.load(f)["capabilities"]["block"].get("formats", {}).get("VK_FORMAT_R8G8B8A8_UNORM", {})
                return result

            with open(output_path, "r", encoding="utf-8") as f:
                data = json.load(f)
            return data["capabilities"]["VP_TEST_combined_block"]["formats"]["VK_FORMAT_R8G8B8A8_UNORM"]

    def test_combine_union_format_pnext(self):
        formats = self.combine(CombineMode.UNION)

        # 1. max limittype: the largest value
        self.assertEqual(formats["VkSubpassResolvePerformanceQueryEXT"], {"optimal": True})

        # 2. bitmask limittype: all the flags
        tensor = formats["VkTensorFormatPropertiesARM"]
        self.assertEqual(sorted(tensor["optimalTilingTensorFeatures"]), ["VK_FORMAT_FEATURE_2_TENSOR_IMAGE_ALIASING_BIT_ARM", "VK_FORMAT_FEATURE_2_TENSOR_SHADER_BIT_ARM"])
        self.assertEqual(tensor["linearTilingTensorFeatures"], ["VK_FORMAT_FEATURE_2_TENSOR_SHADER_BIT_ARM"])

        # 3. A structure defined by a single profile is kept
        self.assertIn("VkTensorExplicitTilingFormatPropertiesARM", formats)

        # 4. Array elements matched by their exact members (drmFormatModifier), their other members combined, the count updated
        drm = formats["VkDrmFormatModifierPropertiesListEXT"]
        self.assertEqual(drm["drmFormatModifierCount"], 2)
        self.assertEqual(drm["pDrmFormatModifierProperties"], [
            drm_modifier(0, 1, ["VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT", "VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT"]),
            drm_modifier(7, 2, ["VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT"])
        ])

    def test_combine_intersection_format_pnext(self):
        formats = self.combine(CombineMode.INTERSECTION)

        # 1. max limittype: the smallest value
        self.assertEqual(formats["VkSubpassResolvePerformanceQueryEXT"], {"optimal": False})

        # 2. bitmask limittype: the common flags, members not defined by all profiles are removed
        self.assertEqual(formats["VkTensorFormatPropertiesARM"], {"optimalTilingTensorFeatures": ["VK_FORMAT_FEATURE_2_TENSOR_SHADER_BIT_ARM"]})

        # 3. A structure not defined by all profiles is removed
        self.assertNotIn("VkTensorExplicitTilingFormatPropertiesARM", formats)

        # 4. Only the array elements of all profiles, their other members combined, the count updated
        drm = formats["VkDrmFormatModifierPropertiesListEXT"]
        self.assertEqual(drm["drmFormatModifierCount"], 1)
        self.assertEqual(drm["pDrmFormatModifierProperties"], [drm_modifier(0, 1, ["VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT"])])

    def test_combine_difference_format_pnext(self):
        result = self.combine(CombineMode.DIFFERENCE)

        # 1. The common format structures are removed, the structures defined by a single profile are kept
        self.assertNotIn("VkTensorExplicitTilingFormatPropertiesARM", result["profile_b"])
        self.assertIn("VkTensorExplicitTilingFormatPropertiesARM", result["profile_a"])

        # 2. The array count matches the remaining array elements
        drm = result["profile_b"]["VkDrmFormatModifierPropertiesListEXT"]
        self.assertEqual(drm["drmFormatModifierCount"], len(drm["pDrmFormatModifierProperties"]))
        self.assertIn(drm_modifier(7, 2, ["VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT"]), drm["pDrmFormatModifierProperties"])


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("--registry", "-r", action="store", help="Path to vk.xml")
    args, unittest_args = parser.parse_known_args()

    if args.registry:
        REGISTRY_PATH = args.registry

    sys.argv = [sys.argv[0]] + unittest_args
    unittest.main()
