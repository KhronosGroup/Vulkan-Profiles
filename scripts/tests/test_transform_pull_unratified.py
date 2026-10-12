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
from source.transform_pull_promoted_extensions import pull_promoted_extensions_profiles_files
from source.transform_pull_aliases import pull_aliases_profiles_files


class TestConvertPullUnratifiedExtensions(unittest.TestCase):
    registry_path = None

    def setUp(self):
        self.vk: VulkanObject = initVulkanObject('vulkan', self.registry_path)

    def test_pull_promoted_extensions_ignore_unratified(self):
        """
        Verifies that when IGNORE_UNRATIFIED is passed to pull_promoted_extensions_profiles_files,
        unratified promoted extensions are not pulled.
        """
        original_json_text = """{
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8.0-106.json#",
            "profiles": {
                "VP_TEST_profile": {
                    "version": 1,
                    "api-version": "1.1.106",
                    "capabilities": ["baseline"]
                }
            },
            "capabilities": {
                "baseline": {
                    "extensions": {}
                }
            }
        }"""

        original_data = json.loads(original_json_text)
        json_files_dict = {"test_profile.json": original_data}

        pull_bits = [PullBits.PROMOTED_EXTENSIONS, PullBits.IGNORE_UNRATIFIED]
        pull_promoted_extensions_profiles_files(self.vk, pull_bits, json_files_dict)

        pulled_exts = json_files_dict["test_profile.json"]["capabilities"]["baseline"]["extensions"]
        for ext_name in pulled_exts.keys():
            if ext_name in self.vk.extensions:
                self.assertTrue(
                    self.vk.extensions[ext_name].ratified,
                    f"Unratified extension '{ext_name}' was pulled when IGNORE_UNRATIFIED was specified"
                )

    def test_pull_aliases_ignore_unratified(self):
        """
        Verifies that when IGNORE_UNRATIFIED is passed to pull_aliases_profiles_files,
        aliases defined by unratified extensions are not pulled.
        """
        original_json_text = """{
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8.0-106.json#",
            "profiles": {
                "VP_TEST_profile": {
                    "version": 1,
                    "api-version": "1.1.106",
                    "capabilities": ["baseline"]
                }
            },
            "capabilities": {
                "baseline": {
                    "features": {
                        "VkPhysicalDeviceVulkan11Features": {
                            "multiview": true
                        }
                    }
                }
            }
        }"""

        original_data = json.loads(original_json_text)
        json_files_dict = {"test_profile.json": original_data}

        pull_bits = [PullBits.ALIASES, PullBits.IGNORE_UNRATIFIED]
        pull_aliases_profiles_files(self.vk, pull_bits, json_files_dict)

        pulled_features = json_files_dict["test_profile.json"]["capabilities"]["baseline"]["features"]
        for struct_name in pulled_features.keys():
            if struct_name != "VkPhysicalDeviceVulkan11Features":
                def_exts = [e for e, obj in self.vk.extensions.items() if hasattr(obj, 'structs') and struct_name in getattr(obj, 'structs', [])]
                for ext_name in def_exts:
                    if ext_name in self.vk.extensions:
                        self.assertTrue(
                            self.vk.extensions[ext_name].ratified,
                            f"Alias structure '{struct_name}' defined by unratified extension '{ext_name}' was pulled when IGNORE_UNRATIFIED was specified"
                        )


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument(
        '--registry', '-r', action='store', required=False,
        help='Use specified registry file instead of vk.xml.'
    )

    args, unparsed = parser.parse_known_args()
    TestConvertPullUnratifiedExtensions.registry_path = args.registry

    unittest.main(argv=[sys.argv[0]] + unparsed)