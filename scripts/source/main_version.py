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


def get_version_string() -> str:
    """
    Returns the vkprofiles version string based on the baked build version.
    """
    try:
        from source.baked_version import BAKED_VERSION
        if BAKED_VERSION:
            return f"vkprofiles version {BAKED_VERSION}"
    except ImportError:
        pass

    return "vkprofiles version unknown"


def main_version(args=None) -> str:
    """
    Handler for the 'version' subcommand and top-level --version option.
    """
    version_str = get_version_string()

    if args is not None:
        print(version_str)

    return version_str
