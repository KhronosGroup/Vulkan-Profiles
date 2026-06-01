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

import unittest

from pathlib import Path

from ..profiles import load_profiles_jsons
from ..profiles import collect_extensions
from ..profiles import VK_VERSION

class TestJsonMethods(unittest.TestCase):
    def test_load_profiles_jsons(self):
        repository_path = Path(__file__).resolve().parent.parent.parent
        profiles_files_dir = repository_path / "profiles"
       
        jsons = load_profiles_jsons(profiles_files_dir)
        
        self.assertEqual(len(jsons), 8)
       
        
    def test_collect_extensions(self):
        A = collect_extensions(VK_VERSION.V1_1, "(VK_VERSION_1_1+VK_KHR_synchronization2),VK_VERSION_1_3")
        self.assertEqual(A[0], "VK_KHR_synchronization2")
        
        B = collect_extensions(VK_VERSION.V1_3, "(VK_VERSION_1_1+VK_KHR_synchronization2),VK_VERSION_1_3")
        self.assertEqual(len(B), 0)
        
        C = collect_extensions(VK_VERSION.V1_0, "(VK_VERSION_1_1+VK_KHR_synchronization2),VK_VERSION_1_3")
        self.assertEqual(len(C), 0)


if __name__ == '__main__':
    unittest.main()

