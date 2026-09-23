#!/usr/bin/python3
#
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

import glob
import json
import os
import sys

def main():
    if len(sys.argv) < 3:
        sys.exit(1)

    instance_path = sys.argv[1]
    schema_path = sys.argv[2]
    build_dir = sys.argv[3] if len(sys.argv) > 3 else ""

    try:
        import jsonschema
    except ImportError:
        # Check python-venv created by CMake build
        for p in glob.glob(os.path.join(build_dir, "python-venv", "*", "*", "site-packages")) + \
                 glob.glob(os.path.join(build_dir, "python-venv", "Lib", "site-packages")):
            sys.path.insert(0, p)
        try:
            import jsonschema
        except ImportError:
            print("WARNING: jsonschema module not found; skipping manifest validation.")
            sys.exit(0)

    try:
        with open(instance_path, "r", encoding="utf-8") as f_inst, \
             open(schema_path, "r", encoding="utf-8") as f_sch:
            jsonschema.validate(instance=json.load(f_inst), schema=json.load(f_sch))
    except Exception as e:
        print(f"ERROR: Manifest validation failed for '{instance_path}':\n{e}")
        sys.exit(1)

if __name__ == "__main__":
    main()