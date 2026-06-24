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

from vulkan_object import VulkanObject
from .expression_parsing import collect_extensions
from .expression_parsing import VK_VERSION

def find_extension_version(vk: VulkanObject, extension_name: str) -> int:
    if extension_name in vk.extensions:
        return vk.extensions[extension_name].specVersionValue
    else:
        return 0 # extension not found


def find_dependent_extensions(vk: VulkanObject, version: VK_VERSION, ignore_extension_versions: bool, extensions: dict[str, int]) -> dict[str, int]:
    result = {}
    
    for extension in extensions:
        if extension not in vk.extensions:
            print(f'ERROR: {extension} is part of vk.xml, discarding')
            continue
        
        extension_data = vk.extensions[extension]
       
        depend_extensions = collect_extensions(version, extension_data.depends)
    
        # First insert the dependent extensions
        for depend_extension in depend_extensions:
            if depend_extension not in result:
                if ignore_extension_versions:
                    result[depend_extension] = 1
                else:
                    result[depend_extension] = find_extension_version(vk, depend_extension)
            
        # Then insert the source extension
        if extension not in result:
            if ignore_extension_versions:
                result[extension] = 1
            else:
                result[extension] = extension_data.specVersionValue
    
    return result
    

def find_feature_alias(vk: VulkanObject, version: VK_VERSION, struct: str, feature: str):
    return 
