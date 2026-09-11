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

from source.main_transform import main_transform
from source.transform_utils import TransformBits


REGISTRY_PATH = None


class TestTransformStripHelperValues(unittest.TestCase):

    def setUp(self):
        if not REGISTRY_PATH or not os.path.exists(REGISTRY_PATH):
            self.skipTest(f"Vulkan registry XML (vk.xml) not found at: {REGISTRY_PATH}")

    def create_test_profile(self, input_dir: Path):
        """Creates a profile containing bitmask flag lists with helper values."""
        profile_data = {
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8-latest.json#",
            "capabilities": {
                "cap_test": {
                    "properties": {
                        "VkPhysicalDeviceProperties": {
                            "limits": {
                                "maxImageDimension2D": 4096
                            }
                        }
                    },
                    "flags_test": {
                        "stages": [
                            "VK_SHADER_STAGE_VERTEX_BIT",
                            "VK_SHADER_STAGE_FRAGMENT_BIT",
                            "VK_SHADER_STAGE_ALL_GRAPHICS",
                            "VK_SHADER_STAGE_ALL"
                        ],
                        "cullMode": [
                            "VK_CULL_MODE_NONE",
                            "VK_CULL_MODE_FRONT_BIT",
                            "VK_CULL_MODE_FRONT_AND_BACK"
                        ]
                    }
                }
            },
            "profiles": {
                "VP_TEST_HELPER_STRIP": {
                    "version": 1,
                    "api-version": "1.0.0",
                    "label": "Test Helper Strip Profile",
                    "description": "Test helper strip profile",
                    "capabilities": ["cap_test"]
                }
            }
        }

        with open(input_dir / "VP_TEST_HELPER_STRIP.json", "w", encoding="utf-8") as f:
            json.dump(profile_data, f, indent=4)

    def test_strip_helper_values(self):
        """Tests that STRIP_HELPER_VALUES removes composite and non-bitpos zero/all constants."""
        with tempfile.TemporaryDirectory() as temp_dir:
            temp_path = Path(temp_dir)
            input_dir = temp_path / "input"
            input_dir.mkdir()
            output_dir = temp_path / "output"

            self.create_test_profile(input_dir)

            args = argparse.Namespace(
                registry=REGISTRY_PATH,
                input=str(input_dir),
                output=str(output_dir),
                mode=[TransformBits.STRIP_HELPER_VALUES],
                api="vulkan",
                format="pretty",
                validate=None
            )
            main_transform(args)

            out_file = output_dir / "VP_TEST_HELPER_STRIP.json"
            self.assertTrue(out_file.exists())

            with open(out_file, "r", encoding="utf-8") as f:
                res_data = json.load(f)

            cap = res_data.get("capabilities", {}).get("cap_test", {})
            flags_test = cap.get("flags_test", {})

            stages = flags_test.get("stages", [])
            cull_mode = flags_test.get("cullMode", [])

            # Standard bitpos flags must be retained
            self.assertIn("VK_SHADER_STAGE_VERTEX_BIT", stages)
            self.assertIn("VK_SHADER_STAGE_FRAGMENT_BIT", stages)
            self.assertIn("VK_CULL_MODE_FRONT_BIT", cull_mode)

            # Helper values (composites / non-bitpos constants) must be stripped
            self.assertNotIn("VK_SHADER_STAGE_ALL_GRAPHICS", stages)
            self.assertNotIn("VK_SHADER_STAGE_ALL", stages)
            self.assertNotIn("VK_CULL_MODE_NONE", cull_mode)
            self.assertNotIn("VK_CULL_MODE_FRONT_AND_BACK", cull_mode)


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("--registry", "-r", action="store", help="Path to vk.xml")
    args, unittest_args = parser.parse_known_args()

    if args.registry:
        REGISTRY_PATH = args.registry

    sys.argv = [sys.argv[0]] + unittest_args
    unittest.main()
    