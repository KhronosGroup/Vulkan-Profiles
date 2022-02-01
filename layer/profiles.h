/*
 * Copyright (C) 2015-2022 Valve Corporation
 * Copyright (C) 2015-2022 LunarG, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * Author: Mark Lobodzinski <mark@lunarg.com>
 */

enum SimulateCapabilityFlag {
    SIMULATE_API_VERSION_BIT = 1 << 0,
    SIMULATE_FEATURES_BIT = 1 << 1,
    SIMULATE_PROPERTIES_BIT = 1 << 2,
    SIMULATE_EXTENSIONS_BIT = 1 << 3,
    SIMULATE_FORMATS_BIT = 1 << 4,
    SIMULATE_FORMAT_PROPERTIES_BIT = 1 << 5
};
typedef int SimulateCapabilityFlags;