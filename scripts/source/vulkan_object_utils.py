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

import functools
import importlib.resources
import tempfile
import os
from xml.etree import ElementTree
from typing import Any

from vulkan_object import (VulkanObject, CapabilityAlias, StructCapabilityAlias, ExtensionCapabilityAlias)
from reg import Registry
from base_generator import BaseGenerator, BaseGeneratorOptions, SetOutputDirectory, SetOutputFileName, SetTargetApiName, SetMergedApiNames

# Define the public API for your package
__all__ = [
    'getVulkanObject',
    'VulkanObject'  # Exposing the class is good for type-hinting
]

# Create the simplified, cached public function
@functools.lru_cache(maxsize=1)
def getVulkanObject(alternative_xml: str = None, video: bool = False) -> VulkanObject:
    """
    Parses the bundled Vulkan registry (vk.xml) and returns the populated
    VulkanObject.

    This function encapsulates all the setup logic. The result is cached,
    so subsequent calls are instantaneous.

    Args:
        api_name: The API name to parse from the registry, defaults to 'vulkan'.
        alternative_xml: Supply a full path to a different vk.xml (used for testing future extensions)

    Returns:
        An initialized VulkanObject instance providing access to the
        Vulkan API registry data.
    """
    # This dummy generator class is required by the reg.py interface.
    # We don't need it to do anything, as we just want the parsed data object.
    class _InternalGenerator(BaseGenerator):
        def __init__(self, *args, **kwargs):
            super().__init__(*args, **kwargs)

        def generate(self):
            # This method is called by reg.apiGen() but we don't need to
            # generate any files, so we just pass. The real goal is to
            # populate self.vk (the VulkanObject).
            pass

    # The original script required setting an output directory, even if
    # it's not used. We'll use a temporary one that cleans itself up.
    with tempfile.TemporaryDirectory() as output_dir:
        SetOutputDirectory(output_dir)
        SetOutputFileName("unused.txt")
        # TODO - Make a get_vulkan_sc_object() or pass this in as a parameter
        SetTargetApiName('vulkan')
        SetMergedApiNames(None)

        xml_path = None

        alternative_xml

        if alternative_xml:
            if not os.path.isfile(alternative_xml):
                raise FileNotFoundError(f"The provided alternative XML file does not exist or is not a file: {alternative_xml}")
            xml_path = alternative_xml
        else:
            # Try the installed package resource first
            try:
                resource_path = importlib.resources.files('vulkan_object').joinpath('vk.xml')
                if resource_path.is_file():
                    xml_path = str(resource_path)
            except (ImportError, ModuleNotFoundError, TypeError):
                xml_path = None

            # Fallback: Check local development path 'src/vulkan_object/vk.xml'
            if xml_path is None:
                base_dir = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
                fallback_path = os.path.join(base_dir, 'vulkan_object', 'vk.xml')
                if os.path.exists(fallback_path):
                    xml_path = fallback_path

            if xml_path is None:
                raise RuntimeError("Could not find the bundled vk.xml - something has gone wrong with packaging.")

        video_xml_path = None
        if video:
            video_xml_path = xml_path[:-6] + 'video.xml'

        # Initialize the generator and the registry machinery
        generator = _InternalGenerator()
        base_options = BaseGeneratorOptions(videoXmlPath=video_xml_path)
        reg = Registry(generator, base_options)
        tree = ElementTree.parse(xml_path)
        reg.loadElementTree(tree)

        # This invokes reg.py and will populate _InternalGenerator
        reg.apiGen()

        return generator.vk

def getStructByName(structs_dict, struct_name):
    """
    Retrieves a structure object from the registry dictionary using either
    its canonical structure name or any of its structure alias names.
    Returns None if the structure name or alias could not be resolved.
    """
    # 1. Direct lookup
    if struct_name in structs_dict:
        return structs_dict[struct_name]
    
    # 2. Alias lookup
    return next((struct_obj for struct_obj in structs_dict.values() if struct_name in struct_obj.aliases), None)

def getMemberByName(search_struct, member_name):
    """
    Retrieves a member object from the specified structure.
    Returns None if the structure or the member name is not found.
    """
    return next((member for member in search_struct.members if member.name == member_name), None)

def gatherCapabilityAliases(vk: VulkanObject, alias_id: CapabilityAlias) -> list[CapabilityAlias]:
    """Retrieves all alternative capability aliases for a given CapabilityAlias."""
    
    canonical_key = None

    # Step 1: Resolve the canonical tracking key based on the input type
    if isinstance(alias_id, StructCapabilityAlias):
        struct_obj = getStructByName(vk.structs, alias_id.struct)
        canonical_struct = struct_obj.name if struct_obj else alias_id.struct
        canonical_key = (canonical_struct, alias_id.member)
        
        # Follow the chain if the canonical member itself points to another structure feature
        if canonical_struct in vk.structs:
            for member in vk.structs[canonical_struct].members:
                if member.name == alias_id.member and isinstance(member.alias, StructCapabilityAlias):
                    target_struct_obj = getStructByName(vk.structs, member.alias.struct)
                    canonical_key = (target_struct_obj.name if target_struct_obj else member.alias.struct, member.alias.member)
                    break
    
    elif isinstance(alias_id, ExtensionCapabilityAlias):
        # Extensions lack structural layout, so locate their defining struct member
        for struct_name, struct_obj in vk.structs.items():
            for member in struct_obj.members:
                if isinstance(member.alias, ExtensionCapabilityAlias) and member.alias.name == alias_id.name:
                    canonical_key = (struct_name, member.name)
                    break
            if canonical_key:
                break
        
        if not canonical_key:
            return []

    # Step 2: Unified Scan and Collection
    aliases = []
    for struct_name, struct_obj in vk.structs.items():
        for member in struct_obj.members:
            # Determine where the current member resolves to
            if isinstance(member.alias, StructCapabilityAlias):
                target_struct_obj = getStructByName(vk.structs, member.alias.struct)
                current_key = (target_struct_obj.name if target_struct_obj else member.alias.struct, member.alias.member)
            else:
                current_key = (struct_name, member.name)

            # If it shares the same canonical root, gather it and its structural/extension variants
            if current_key == canonical_key:
                aliases.append(StructCapabilityAlias(struct_name, member.name))
                for alias_struct in struct_obj.aliases:
                    aliases.append(StructCapabilityAlias(alias_struct, member.name))

                if isinstance(member.alias, ExtensionCapabilityAlias) and member.alias.name in vk.extensions:
                    aliases.append(ExtensionCapabilityAlias(member.alias.name))

    # Step 3: Streamlined filtering to remove the original query item
    return [item for item in aliases if item != alias_id]
