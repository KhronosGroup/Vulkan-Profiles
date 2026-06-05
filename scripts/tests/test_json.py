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

from ..source.profiles_parsing import load_profiles_jsons

class TestJsonMethods(unittest.TestCase):
    def test_load_profiles_jsons(self):
        repository_path = Path(__file__).resolve().parent.parent.parent
        profiles_files_dir = repository_path / "profiles"

        jsons = load_profiles_jsons(profiles_files_dir)

        self.assertEqual(len(jsons), 8)


if __name__ == '__main__':
    unittest.main()

