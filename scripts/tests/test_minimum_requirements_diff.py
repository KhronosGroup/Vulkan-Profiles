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
import tempfile
import unittest

scripts_dir = Path(__file__).resolve().parent.parent
if str(scripts_dir) not in sys.path:
    sys.path.insert(0, str(scripts_dir))

import vkprofiles

profiles_dir = scripts_dir.parent / "profiles"
minimum_requirements_file = profiles_dir / "LunarG" / "VP_LUNARG_minimum_requirements.json"
generated_requirements_file = profiles_dir / "test" / "data" / "VP_LUNARG_generated_requirements" / "VP_LUNARG_generated_requirements.json"

def collect_capability_values(data, path=()):
    """Returns the (path, value) of every capability value, ignoring empty dictionaries and lists."""
    if isinstance(data, dict):
        values = []
        for key, value in data.items():
            values.extend(collect_capability_values(value, path + (key,)))
        return values
    if isinstance(data, list) and not data:
        return []
    return [("/".join(path), data)]


class TestMinimumRequirementsDiff(unittest.TestCase):
    """
    Verifies that pulling the Vulkan core requirements into profiles declaring only an api-version
    (VP_LUNARG_generated_requirements.json) produces the same capabilities as the hand written
    VP_LUNARG_minimum_requirements.json profiles: the 'combine --mode difference' of each Vulkan version
    pair of profiles, extracted with their required profiles and consolidated, must be empty.
    This mirrors the VpTestGeneratedRequirements CMake target.
    """
    registry_path = None

    @classmethod
    def run_vkprofiles(cls, args: list[str]):
        vkprofiles.main(args[:1] + ["--quiet", "--registry", cls.registry_path] + args[1:])

    @classmethod
    def setUpClass(cls):
        cls.temp_dir = tempfile.TemporaryDirectory()
        cls.temp_path = Path(cls.temp_dir.name)

        # 1. Pull the requirements of both profiles files, shared by all the Vulkan versions tests
        cls.transform_dir = cls.temp_path / "1_transform"
        cls.transform_dir.mkdir()
        for input_file, output_path in (
            (minimum_requirements_file, cls.transform_dir),
            (generated_requirements_file, cls.transform_dir / generated_requirements_file.name)
        ):
            cls.run_vkprofiles([
                "transform", "--input", str(input_file), "--output", str(output_path),
                "--pull", "requirements", "aliases", "ignore-extension-versions", "--sort", "--format", "flatten"
            ])

    @classmethod
    def tearDownClass(cls):
        cls.temp_dir.cleanup()

    def assertVulkanVersionDiffEmpty(self, minor: int):
        version_dir = f"vulkan1{minor}"
        profile_names = [f"VP_LUNARG_gen_min_requirements_1_{minor}", f"VP_LUNARG_minimum_requirements_1_{minor}"]

        # 2. Extract each profile with its required profiles
        extract_dir = self.temp_path / "2_extract" / version_dir
        for profile_name in profile_names:
            self.run_vkprofiles([
                "extract", "--input", str(self.transform_dir), "--output", str(extract_dir / f"{profile_name}.json"),
                "--input-profiles", profile_name, "--mode", "pull-required-profiles", "--format", "flatten"
            ])

        # 3. Consolidate the capabilities of each profile
        consolidate_dir = self.temp_path / "3_consolidate" / version_dir
        self.run_vkprofiles([
            "transform", "--input", str(extract_dir), "--output", str(consolidate_dir),
            "--consolidate", "--sort", "--format", "flatten"
        ])

        # 4. Compute the difference of the profiles
        diff_dir = self.temp_path / "4_diff" / version_dir
        self.run_vkprofiles([
            "combine", "--input", str(consolidate_dir), "--output", str(diff_dir),
            "--mode", "difference", "--format", "flatten"
        ])

        # 5. The difference of each profile MUST be empty
        for profile_name in profile_names:
            with open(diff_dir / f"{profile_name}.json", "r", encoding="utf-8") as f:
                diff_data = json.load(f)
            values = collect_capability_values(diff_data.get("capabilities", {}))
            self.assertEqual(values, [], f"Capabilities of '{profile_name}' not found in the other Vulkan 1.{minor} profile")

    def test_minimum_requirements_diff_vulkan10(self):
        """Verifies that the generated and the minimum requirements Vulkan 1.0 profiles are identical."""
        self.assertVulkanVersionDiffEmpty(0)

    def test_minimum_requirements_diff_vulkan11(self):
        """Verifies that the generated and the minimum requirements Vulkan 1.1 profiles are identical."""
        self.assertVulkanVersionDiffEmpty(1)

    def test_minimum_requirements_diff_vulkan12(self):
        """Verifies that the generated and the minimum requirements Vulkan 1.2 profiles are identical."""
        self.assertVulkanVersionDiffEmpty(2)

    def test_minimum_requirements_diff_vulkan13(self):
        """Verifies that the generated and the minimum requirements Vulkan 1.3 profiles are identical."""
        self.assertVulkanVersionDiffEmpty(3)

    def test_minimum_requirements_diff_vulkan14(self):
        """Verifies that the generated and the minimum requirements Vulkan 1.4 profiles are identical."""
        self.assertVulkanVersionDiffEmpty(4)

if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument(
        '--registry', '-r', action='store', required=False,
        help='Use specified registry file instead of vk.xml.'
    )

    args, unparsed = parser.parse_known_args()
    TestMinimumRequirementsDiff.registry_path = args.registry

    unittest.main(argv=[sys.argv[0]] + unparsed)
