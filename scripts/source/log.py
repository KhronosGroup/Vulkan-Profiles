#!/usr/bin/python3
#
# Copyright (c) 2021-2026 LunarG, Inc.
# Copyright (c) 2023-2024 RasterGrid Kft.
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
# - Daniel Rakos <daniel.rakos@rastergrid.com>
# - Christophe Riccio <christophe@lunarg.com>

class Log():
    def f(msg):
        print('FATAL: ' + msg)
        raise Exception(msg)

    def e(msg):
        print('ERROR: ' + msg)

    def w(msg):
        print('WARNING: ' + msg)

    def i(msg):
        print(msg)

