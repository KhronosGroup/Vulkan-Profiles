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
import re
from xml.etree import ElementTree
from typing import Any

from vulkan_object import VulkanObject, CapabilityAlias, StructCapabilityAlias, ExtensionCapabilityAlias
from reg import Registry
from base_generator import BaseGenerator, BaseGeneratorOptions, SetOutputDirectory, SetOutputFileName, SetTargetApiName, SetMergedApiNames
from source.vulkan_object_version import VK_VERSION
from source.vulkan_object_expression_parsing import collectExtensions, evalExpression

# Define the public API for your package
__all__ = [
    'getVulkanObject',
    'VulkanObject'  # Exposing the class is good for type-hinting
]

# Create the simplified, cached public function
@functools.lru_cache(maxsize=1)
def initVulkanObject(target_api: str = 'vulkan', alternative_xml: str = None, video: bool = False) -> VulkanObject:
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
        SetTargetApiName(target_api)
        SetMergedApiNames(None)

        xml_path = None

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
                if member.name == alias_id.member and isinstance(member.capabilityAlias, StructCapabilityAlias):
                    target_struct_obj = getStructByName(vk.structs, member.capabilityAlias.struct)
                    canonical_key = (target_struct_obj.name if target_struct_obj else member.capabilityAlias.struct, member.capabilityAlias.member)
                    break
    
    elif isinstance(alias_id, ExtensionCapabilityAlias):
        # Extensions lack structural layout, so locate their defining struct member
        for struct_name, struct_obj in vk.structs.items():
            for member in struct_obj.members:
                if isinstance(member.capabilityAlias, ExtensionCapabilityAlias) and member.capabilityAlias.name == alias_id.name:
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
            if isinstance(member.capabilityAlias, StructCapabilityAlias):
                target_struct_obj = getStructByName(vk.structs, member.capabilityAlias.struct)
                current_key = (target_struct_obj.name if target_struct_obj else member.capabilityAlias.struct, member.capabilityAlias.member)
            else:
                current_key = (struct_name, member.name)

            # If it shares the same canonical root, gather it and its structural/extension variants
            if current_key == canonical_key:
                aliases.append(StructCapabilityAlias(struct_name, member.name))
                for alias_struct in struct_obj.aliases:
                    aliases.append(StructCapabilityAlias(alias_struct, member.name))

                if isinstance(member.capabilityAlias, ExtensionCapabilityAlias) and member.capabilityAlias.name in vk.extensions:
                    aliases.append(ExtensionCapabilityAlias(member.capabilityAlias.name))

    # Step 3: Streamlined filtering to remove the original query item
    return [item for item in aliases if item != alias_id]

def _parse_version_tuple(v) -> tuple[int, int]:
    """Extracts a (major, minor) version tuple from a VK_VERSION enum, Version object, or string."""
    if v is None:
        return (0, 0)
    s = str(getattr(v, 'name', v))
    nums = re.findall(r'\d+', s)
    if len(nums) >= 2:
        return (int(nums[0]), int(nums[1]))
    elif len(nums) == 1:
        return (int(nums[0]), 0)
    return (0, 0)

def _get_promoted_core_version(vk: VulkanObject, promoted_to_str: str | None) -> tuple[int, int] | None:
    """Returns the core version tuple if `promoted_to_str` represents a Vulkan Core version (e.g., 'VK_VERSION_1_2')."""
    if not promoted_to_str:
        return None
    if hasattr(vk, 'versions') and promoted_to_str in vk.versions:
        return _parse_version_tuple(promoted_to_str)
    if promoted_to_str.startswith("VK_VERSION_") or promoted_to_str.startswith("VK_API_VERSION_"):
        return _parse_version_tuple(promoted_to_str)
    return None

def gatherDependentCapabilityAliases2(vk: VulkanObject, version: VK_VERSION, alias_id: CapabilityAlias) -> list[CapabilityAlias]:
    """
    Retrieves capability aliases for a given `alias_id`, filtering out aliases that belong to
    extensions promoted to a Vulkan core version less than or equal to `version`.
    """
    aliases = gatherCapabilityAliases(vk, alias_id)
    target_ver_tuple = _parse_version_tuple(version)

    filtered_aliases = []
    for alias in aliases:
        ext_names = set()

        if isinstance(alias, ExtensionCapabilityAlias):
            ext_names.add(alias.name)

        elif isinstance(alias, StructCapabilityAlias):
            struct_name = alias.struct

            # 1. Check explicit alias requirement mapping
            if hasattr(vk, 'aliasTypeRequirements') and struct_name in vk.aliasTypeRequirements:
                ext_names.update(vk.aliasTypeRequirements[struct_name].keys())
            # 2. Check canonical structure defining requirements
            elif struct_name in vk.structs:
                struct_obj = vk.structs[struct_name]
                if getattr(struct_obj, 'definingRequirements', None):
                    ext_names.update(struct_obj.definingRequirements.keys())
                elif getattr(struct_obj, 'extensions', None):
                    ext_names.update(struct_obj.extensions)
            # 3. Fallback resolution via getStructByName
            else:
                struct_obj = getStructByName(vk.structs, struct_name)
                if struct_obj:
                    if getattr(struct_obj, 'definingRequirements', None):
                        ext_names.update(struct_obj.definingRequirements.keys())
                    elif getattr(struct_obj, 'extensions', None):
                        ext_names.update(struct_obj.extensions)

        # Only filter out aliases whose defining extension was promoted to Vulkan CORE <= version
        is_promoted_to_core = False
        for ext_name in ext_names:
            if ext_name in vk.extensions:
                ext = vk.extensions[ext_name]
                promoted_core_ver = _get_promoted_core_version(vk, ext.promotedTo)
                if promoted_core_ver is not None and promoted_core_ver <= target_ver_tuple:
                    is_promoted_to_core = True
                    break

        if not is_promoted_to_core:
            filtered_aliases.append(alias)

    return filtered_aliases

def _get_struct_core_version(vk: VulkanObject, struct_name: str) -> tuple[int, int] | None:
    """
    Returns the Vulkan Core version tuple in which a core structure was introduced,
    or None if the structure belongs to an extension.
    """
    if struct_name in vk.structs:
        struct_obj = vk.structs[struct_name]
        if getattr(struct_obj, 'version', None):
            ver = _parse_version_tuple(struct_obj.version)
            if ver > (0, 0):
                return ver
        if getattr(struct_obj, 'definingRequirements', None):
            for req in struct_obj.definingRequirements:
                ver = _get_promoted_core_version(vk, req)
                if ver:
                    return ver

    if hasattr(vk, 'aliasTypeRequirements') and struct_name in vk.aliasTypeRequirements:
        for req in vk.aliasTypeRequirements[struct_name]:
            ver = _get_promoted_core_version(vk, req)
            if ver:
                return ver

    return None

def gatherDependentCapabilityAliases(vk: VulkanObject, version: VK_VERSION, alias_id: CapabilityAlias) -> list[CapabilityAlias]:
    """
    Retrieves capability aliases for `alias_id`, filtered according to the target `version`:
    - If `version` is `VK_VERSION.NONE` (0, 0), no version filtering is applied.
    - Core structures introduced in a version newer than `version` are filtered out.
    - Extension capability aliases promoted to a core version <= `version` are filtered out.
    """
    aliases = gatherCapabilityAliases(vk, alias_id)
    target_ver_tuple = _parse_version_tuple(version)

    # Return all aliases without filtering if target version is NONE
    if target_ver_tuple == (0, 0):
        return aliases

    filtered_aliases = []
    for alias in aliases:
        # Rule 1: Filter out Core structures introduced in a version newer than target version
        if isinstance(alias, StructCapabilityAlias):
            struct_core_ver = _get_struct_core_version(vk, alias.struct)
            if struct_core_ver is not None and target_ver_tuple < struct_core_ver:
                continue

        # Rule 2: Filter out Extension Capability Aliases promoted to Core <= target version
        if isinstance(alias, ExtensionCapabilityAlias):
            if alias.name in vk.extensions:
                ext = vk.extensions[alias.name]
                promoted_core_ver = _get_promoted_core_version(vk, ext.promotedTo)
                if promoted_core_ver is not None and target_ver_tuple >= promoted_core_ver:
                    continue

        filtered_aliases.append(alias)

    return filtered_aliases

def findExtensionVersion(vk: VulkanObject, extension_name: str) -> int:
    if extension_name in vk.extensions:
        return vk.extensions[extension_name].specVersionValue
    else:
        return 0 # extension not found
    
def gatherDependentExtensions(vk: VulkanObject, version: VK_VERSION, ignore_extension_versions: bool, extensions: dict[str, int]) -> dict[str, int]:
    result = {}
    
    for extension in extensions:
        if extension not in vk.extensions:
            print(f'ERROR: {extension} is part of vk.xml, discarding')
            continue
        
        extension_data = vk.extensions[extension]
       
        depend_extensions = collectExtensions(version, extension_data.depends)
    
        # First insert the dependent extensions
        for depend_extension in depend_extensions:
            if depend_extension not in result:
                if ignore_extension_versions:
                    result[depend_extension] = 1
                else:
                    result[depend_extension] = findExtensionVersion(vk, depend_extension)
            
        # Then insert the source extension
        if extension not in result:
            if ignore_extension_versions:
                result[extension] = 1
            else:
                result[extension] = extension_data.specVersionValue
    
    return result


def gatherDynamicStructs(vk: VulkanObject):
    """
    Global discovery function that automatically identifies all Vulkan structures 
    containing variable-length pointer arrays by scanning metadata within a VulkanObject.
    """
    discovered = set()
    for struct_name, struct_def in vk.structs.items():
        for member in struct_def.members:
            if member.pointer and member.length is not None:
                discovered.add(struct_name)
    return sorted(list(discovered))


def gatherPromotedExtensionsForVersion(vk: VulkanObject, target_version: VK_VERSION) -> dict[str, int]:
    """
    Collects all extensions in vk.xml promoted to the target core API version or an earlier core version.
    """
    promoted_exts = {}
    if target_version == VK_VERSION.NONE:
        return promoted_exts

    for ext_name, ext_obj in vk.extensions.items():
        promoted_to = getattr(ext_obj, 'promotedTo', None) or getattr(ext_obj, 'promoted_to', None)
        if promoted_to:
            promoted_ver = VK_VERSION.from_string(promoted_to)
            if promoted_ver != VK_VERSION.NONE and target_version >= promoted_ver:
                spec_version = getattr(ext_obj, 'specVersionValue', None) or findExtensionVersion(vk, ext_name) or 1
                promoted_exts[ext_name] = spec_version

    return promoted_exts


def gatherRequiredFeaturesForVersion(vk: VulkanObject, target_version: VK_VERSION) -> dict[str, dict[str, bool]]:
    """
    Collects all feature requirements defined by the target core API version or earlier core versions in vk.xml.
    Returns a dictionary structure: { "VkPhysicalDeviceVulkan12Features": { "timelineSemaphore": True, ... } }
    """
    required_features: dict[str, dict[str, bool]] = {}
    if target_version == VK_VERSION.NONE:
        return required_features

    for ver_name, ver_obj in vk.versions.items():
        ver = VK_VERSION.from_string(ver_name)
        if ver != VK_VERSION.NONE and target_version >= ver:
            # 1. Direct requiredFeatures attribute
            req_features = getattr(ver_obj, 'requiredFeatures', None) or getattr(ver_obj, 'required_features', None)
            if req_features:
                if isinstance(req_features, dict):
                    for struct_name, feat_dict in req_features.items():
                        required_features.setdefault(struct_name, {}).update(feat_dict)
                elif isinstance(req_features, list):
                    for feat in req_features:
                        struct_name = getattr(feat, 'struct', None) or getattr(feat, 'struct_name', None)
                        feat_name = getattr(feat, 'name', None) or getattr(feat, 'feature_name', None)
                        if struct_name and feat_name:
                            required_features.setdefault(struct_name, {})[feat_name] = True

            # 2. Requirements list attribute
            reqs = getattr(ver_obj, 'requirements', None) or []
            for req in reqs:
                features = getattr(req, 'features', None) or getattr(req, 'requiredFeatures', None) or []
                for feat in features:
                    struct_name = getattr(feat, 'struct', None) or getattr(feat, 'struct_name', None)
                    feat_name = getattr(feat, 'name', None) or getattr(feat, 'feature_name', None)
                    if struct_name and feat_name:
                        required_features.setdefault(struct_name, {})[feat_name] = True

    return required_features


def isStructExtensionEnabled(vk: VulkanObject, struct_name: str, version: VK_VERSION, enabled_exts: set[str]) -> bool:
    """
    Checks whether a structure's defining requirements (extensions or core versions) are satisfied.
    """
    req_keys = set()

    # 1. Direct struct definingRequirements
    struct_obj = vk.structs.get(struct_name) or getStructByName(vk.structs, struct_name)
    if struct_obj:
        if hasattr(struct_obj, 'definingRequirements') and struct_obj.definingRequirements:
            req_keys.update(struct_obj.definingRequirements.keys())
        elif hasattr(struct_obj, 'extensions') and struct_obj.extensions:
            req_keys.update(struct_obj.extensions)

    # 2. Alias definingRequirements
    if hasattr(vk, 'aliasTypeRequirements') and struct_name in vk.aliasTypeRequirements:
        req_keys.update(vk.aliasTypeRequirements[struct_name].keys())

    if struct_obj and hasattr(struct_obj, 'aliases'):
        for alias in struct_obj.aliases:
            if hasattr(vk, 'aliasTypeRequirements') and alias in vk.aliasTypeRequirements:
                req_keys.update(vk.aliasTypeRequirements[alias].keys())

    if not req_keys:
        return True

    # Check if ANY requirement key is satisfied
    for req in req_keys:
        # Check if req is a core API version (e.g., "VK_VERSION_1_3")
        if req.startswith("VK_VERSION_") or req.startswith("VK_API_VERSION_"):
            ver = VK_VERSION.from_string(req)
            if ver != VK_VERSION.NONE and version != VK_VERSION.NONE and version >= ver:
                return True

        # Check if req is an enabled extension
        if req in enabled_exts:
            return True

        # Check if req is an extension promoted to 'version' or earlier
        ext_obj = vk.extensions.get(req)
        if ext_obj and getattr(ext_obj, 'promotedTo', None):
            promoted_ver = VK_VERSION.from_string(ext_obj.promotedTo)
            if promoted_ver != VK_VERSION.NONE and version != VK_VERSION.NONE and version >= promoted_ver:
                return True

    return False


def evaluateFeatureDepends(
    vk: VulkanObject,
    depends_expr: str | None, 
    api_version: VK_VERSION, 
    enabled_exts: set[str], 
    enabled_features: set[tuple[str, str]]
) -> bool:
    """
    Evaluates a FeatureRequirement 'depends' expression string against the active version,
    enabled extensions, and enabled features.
    """
    if not depends_expr:
        return True

    def is_symbol_enabled(token: str) -> bool:
        # 1. Feature flag reference (e.g. "VkPhysicalDeviceVulkan12Features::descriptorIndexing")
        if "::" in token:
            parts = token.split("::")
            return (parts[0], parts[1]) in enabled_features

        # 2. Core API Version reference (e.g. "VK_VERSION_1_2")
        if token.startswith("VK_VERSION_") or token.startswith("VK_API_VERSION_") or (token and token[0].isdigit()):
            ver = VK_VERSION.from_string(token)
            if ver != VK_VERSION.NONE:
                return api_version != VK_VERSION.NONE and api_version >= ver

        # 3. Extension name reference (e.g. "VK_KHR_sampler_mirror_clamp_to_edge")
        if token in enabled_exts:
            return True

        ext_obj = vk.extensions.get(token)
        if ext_obj and getattr(ext_obj, 'promotedTo', None):
            promoted_ver = VK_VERSION.from_string(ext_obj.promotedTo)
            if promoted_ver != VK_VERSION.NONE and api_version != VK_VERSION.NONE and api_version >= promoted_ver:
                return True

        return False

    return evalExpression(depends_expr, is_symbol_enabled)


def gatherSatisfiedRequiredFeatures(
    vk: VulkanObject, 
    api_version: VK_VERSION, 
    enabled_exts: set[str], 
    json_features_block: dict
) -> dict[str, dict[str, bool]]:
    """
    Scans vk.versions and enabled vk.extensions for FeatureRequirements whose 'depends'
    conditions are satisfied by the current API version, enabled extensions, and active features.
    """
    # Build set of currently enabled (struct, member) feature pairs
    enabled_features: set[tuple[str, str]] = set()
    for struct_name, members in json_features_block.items():
        if isinstance(members, dict):
            for member_name, val in members.items():
                if val:
                    enabled_features.add((struct_name, member_name))

    satisfied_features: dict[str, dict[str, bool]] = {}

    def process_requirements(req_list, ver: VK_VERSION = None):
        for req in req_list:
            if req.depends:
                # If 'depends' condition exists, evaluate whether it's satisfied
                if evaluateFeatureDepends(vk, req.depends, api_version, enabled_exts, enabled_features):
                    fields = [f.strip() for f in req.field.split(',')] if req.field else []
                    for field_name in fields:
                        satisfied_features.setdefault(req.struct, {})[field_name] = True
            else:
                # Unconditional requirement: only active if part of enabled extension (ver is None) 
                # or if core version is <= api_version
                if ver is None or (ver != VK_VERSION.NONE and api_version != VK_VERSION.NONE and ver <= api_version):
                    fields = [f.strip() for f in req.field.split(',')] if req.field else []
                    for field_name in fields:
                        satisfied_features.setdefault(req.struct, {})[field_name] = True

    # 1. Process featureRequirements across ALL core Vulkan versions
    for ver_name, ver_obj in vk.versions.items():
        ver = VK_VERSION.from_string(ver_name)
        process_requirements(getattr(ver_obj, 'featureRequirement', []), ver)

    # 2. Process featureRequirements across ALL enabled extensions
    for ext_name in enabled_exts:
        if ext_name in vk.extensions:
            ext_obj = vk.extensions[ext_name]
            process_requirements(getattr(ext_obj, 'featureRequirement', []), ver=None)

    return satisfied_features
