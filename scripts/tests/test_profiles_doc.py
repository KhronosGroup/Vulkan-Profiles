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

import argparse
from pathlib import Path
import sys
import tempfile
import unittest

scripts_dir = Path(__file__).resolve().parent.parent
if str(scripts_dir) not in sys.path:
    sys.path.insert(0, str(scripts_dir))

from vulkan_object import VulkanObject
from source.vulkan_object_utils import initVulkanObject
from source.generate_profiles_doc import generate_profiles_doc


def make_profiles_file() -> dict:
    return {
        "$schema": "https://schema.khronos.org/vulkan/profiles-0.8.3-304.json#",
        "capabilities": {
            "baseline": {
                "features": {
                    "VkPhysicalDeviceVulkan14Features": {
                        "pushDescriptor": True
                    }
                }
            },
            "rectangular_lines": {
                "extensions": {
                    "VK_KHR_line_rasterization": 1
                },
                "features": {
                    "VkPhysicalDeviceLineRasterizationFeaturesKHR": {
                        "rectangularLines": True
                    }
                }
            },
            "bresenham_lines": {
                "extensions": {
                    "VK_KHR_line_rasterization": 1
                },
                "features": {
                    "VkPhysicalDeviceLineRasterizationFeaturesKHR": {
                        "bresenhamLines": True
                    }
                }
            }
        },
        "profiles": {
            "VP_LUNARG_test_alternatives": {
                "version": 1,
                "api-version": "1.4.304",
                "label": "Test alternatives",
                "description": "Test of the alternative capability blocks",
                "capabilities": [
                    "baseline",
                    ["rectangular_lines", "bresenham_lines"]
                ]
            },
            "VP_LUNARG_test_alternatives_required": {
                "version": 1,
                "api-version": "1.4.304",
                "label": "Test required alternatives",
                "description": "Test of the alternative capability blocks of a required profile",
                "profiles": ["VP_LUNARG_test_alternatives"],
                "capabilities": []
            }
        }
    }


class TestProfilesDoc(unittest.TestCase):
    registry_path = None

    def setUp(self):
        self.vk: VulkanObject = initVulkanObject('vulkan', self.registry_path)

    def generate_doc(self, json_files: list[dict]) -> str:
        with tempfile.TemporaryDirectory() as temp_dir:
            output = Path(temp_dir) / "PROFILES.md"
            generate_profiles_doc(self.vk, json_files, None, str(output))
            return output.read_text()

    def test_doc_alternatives(self):
        doc = self.generate_doc([make_profiles_file()])

        # The profiles with alternative capability blocks are documented
        self.assertIn("| Profiles | VP_LUNARG_test_alternatives | VP_LUNARG_test_alternatives_required |", doc)
        self.assertIn("| Alternative capability blocks | rectangular_lines **or** bresenham_lines | rectangular_lines **or** bresenham_lines |", doc)

        # The group of alternative capability blocks is listed once, with the profiles requiring it and the requirements of each alternative
        alternatives = doc.split("## Vulkan Profiles Alternative Requirements", 1)[1].split("## Vulkan Profiles Extensions", 1)[0]
        self.assertEqual(alternatives.count("### VP_LUNARG_test_alternatives alternatives"), 1)
        self.assertIn("* Profiles: VP_LUNARG_test_alternatives, VP_LUNARG_test_alternatives_required", alternatives)
        self.assertIn("* Alternatives: `rectangular_lines` **or** `bresenham_lines`", alternatives)
        self.assertIn("#### rectangular_lines", alternatives)
        self.assertIn("::rectangularLines | VK_TRUE |", alternatives)
        self.assertIn("#### bresenham_lines", alternatives)
        self.assertIn("::bresenhamLines | VK_TRUE |", alternatives)

        # The alternative requirements are not requirements of every device supporting the profile
        other_sections = doc.split("## Vulkan Profiles Extensions", 1)[1]
        self.assertNotIn("rectangularLines", other_sections)
        self.assertNotIn("bresenhamLines", other_sections)
        self.assertNotIn("VK_KHR_line_rasterization.html) | :heavy_check_mark:", other_sections)
        self.assertIn("pushDescriptor", other_sections)

    def test_doc_without_alternatives(self):
        profiles_file = make_profiles_file()
        profiles_file["profiles"]["VP_LUNARG_test_alternatives"]["capabilities"] = ["baseline", "rectangular_lines"]
        doc = self.generate_doc([profiles_file])

        # Without alternative capability blocks, the documentation doesn't list alternatives
        self.assertNotIn("Alternative", doc)
        self.assertIn("rectangularLines", doc)


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument(
        '--registry', '-r', action='store', required=False,
        help='Use specified registry file instead of vk.xml.'
    )

    args, unparsed = parser.parse_known_args()
    TestProfilesDoc.registry_path = args.registry

    unittest.main(argv=[sys.argv[0]] + unparsed)
