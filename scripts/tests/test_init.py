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

class TestVulkanObjectInit(unittest.TestCase):
    registry_path = None

    def test_load_vulkan_object(self):
        vk: VulkanObject = initVulkanObject(self.registry_path)

if __name__ == '__main__':
    parser = argparse.ArgumentParser()

    parser.add_argument('--registry', '-r', action='store', required=True,
                        help='Use specified registry file instead of vk.xml (video.xml must be present in the same directory for video support).')

    args, unparsed = parser.parse_known_args()
    TestVulkanObjectInit.registry_path = args.registry

    unittest.main(argv=[sys.argv[0]] + unparsed)

