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
from source.vulkan_object_version import VK_VERSION, get_bundle_structure_core_version
from source.vulkan_object_expression_parsing import collectExtensions, evalExpression

__all__ = [
    'getVulkanObject',
    'VulkanObject'
]

@functools.lru_cache(maxsize=1)
def initVulkanObject(target_api: str = 'vulkan', alternative_xml: str = None, video: bool = False) -> VulkanObject:
    class _InternalGenerator(BaseGenerator):
        def __init__(self, *args, **kwargs):
            super().__init__(*args, **kwargs)

        def generate(self):
            pass

    with tempfile.TemporaryDirectory() as output_dir:
        SetOutputDirectory(output_dir)
        SetOutputFileName("unused.txt")
        SetTargetApiName(target_api)
        SetMergedApiNames(None)

        xml_path = None

        if alternative_xml:
            if not os.path.isfile(alternative_xml):
                raise FileNotFoundError(f"The provided alternative XML file does not exist or is not a file: {alternative_xml}")
            xml_path = alternative_xml
        else:
            try:
                resource_path = importlib.resources.files('vulkan_object').joinpath('vk.xml')
                if resource_path.is_file():
                    xml_path = str(resource_path)
            except (ImportError, ModuleNotFoundError, TypeError):
                xml_path = None

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

        generator = _InternalGenerator()
        base_options = BaseGeneratorOptions(videoXmlPath=video_xml_path)
        reg = Registry(generator, base_options)
        tree = ElementTree.parse(xml_path)
        reg.loadElementTree(tree)

        reg.apiGen()

        return generator.vk

def getStructByName(structs_dict, struct_name):
    if struct_name in structs_dict:
        return structs_dict[struct_name]
    return next((struct_obj for struct_obj in structs_dict.values() if struct_name in struct_obj.aliases), None)

def getMemberByName(search_struct, member_name):
    return next((member for member in search_struct.members if member.name == member_name), None)

def getExtensionPromotedTo(vk: VulkanObject, ext_name: str) -> list[str]:
    """Retrieves the list of 'promotedto' targets for an extension from vk.xml."""
    if hasattr(vk, 'extensions') and ext_name in vk.extensions:
        ext_obj = vk.extensions[ext_name]
        promoted = getattr(ext_obj, 'promotedTo', None)
        if promoted is None:
            promoted = getattr(ext_obj, 'promotedto', None)
        if isinstance(promoted, list):
            return [p for p in promoted if p]
        elif isinstance(promoted, str) and promoted:
            return [p.strip() for p in promoted.split(',') if p.strip()]
    return []

def getStructDefiningExtensions(vk: VulkanObject, struct_name: str) -> list[str]:
    """Retrieves extension names defining a structure from vk.xml / vulkan_object."""
    exts = set()

    struct_obj = vk.structs.get(struct_name) or getStructByName(vk.structs, struct_name)
    if struct_obj:
        if hasattr(struct_obj, 'definingRequirements') and struct_obj.definingRequirements:
            exts.update(struct_obj.definingRequirements.keys())
        if hasattr(struct_obj, 'extensions') and struct_obj.extensions:
            exts.update(struct_obj.extensions)

    if hasattr(vk, 'aliasTypeRequirements') and struct_name in vk.aliasTypeRequirements:
        exts.update(vk.aliasTypeRequirements[struct_name].keys())

    if hasattr(vk, 'extensions'):
        for e_name, e_obj in vk.extensions.items():
            e_structs = getattr(e_obj, 'structs', {})
            if isinstance(e_structs, (dict, list, set)) and struct_name in e_structs:
                exts.add(e_name)
            e_features = getattr(e_obj, 'features', {})
            if isinstance(e_features, (dict, list, set)) and struct_name in e_features:
                exts.add(e_name)

    return [e for e in exts if hasattr(vk, 'extensions') and e in vk.extensions]


def is_extension_struct_name(vk: VulkanObject, struct_name: str) -> bool:
    """Returns True if struct_name has a vendor or extension suffix (e.g. KHR, EXT, NV, AMD)."""
    if struct_name.endswith("KHR") or struct_name.endswith("EXT"):
        return True
    if hasattr(vk, 'extensions'):
        for ext_name in vk.extensions.keys():
            parts = ext_name.split('_')
            if len(parts) >= 2:
                tag = parts[1]
                if tag and struct_name.endswith(tag):
                    return True
    return False


def getStructCoreVersion(vk: VulkanObject, struct_name: str) -> VK_VERSION:
    """Returns the VK_VERSION where a structure was introduced into core Vulkan."""
    bundle_ver = get_bundle_structure_core_version(struct_name)
    if bundle_ver != VK_VERSION.NONE:
        return bundle_ver

    # Extension structs/aliases ending in vendor tags (KHR, EXT, etc.) are not core structures
    if is_extension_struct_name(vk, struct_name):
        return VK_VERSION.NONE

    struct_obj = vk.structs.get(struct_name) or getStructByName(vk.structs, struct_name)

    if struct_obj:
        def_ver = getattr(struct_obj, 'definedByVersion', None)
        if def_ver is not None and def_ver != VK_VERSION.NONE:
            return VK_VERSION.from_string(def_ver) if not isinstance(def_ver, VK_VERSION) else def_ver

        obj_ver = getattr(struct_obj, 'version', None)
        if obj_ver:
            ver_enum = VK_VERSION.from_string(obj_ver)
            if ver_enum != VK_VERSION.NONE:
                return ver_enum

    # Check if the structure's defining extension was promoted to a core version
    def_exts = getStructDefiningExtensions(vk, struct_name)
    for ext_name in def_exts:
        promoted_targets = getExtensionPromotedTo(vk, ext_name)
        for target in promoted_targets:
            p_ver = VK_VERSION.from_string(target)
            if p_ver != VK_VERSION.NONE:
                return p_ver

    return VK_VERSION.NONE


def _get_alias_key(alias_id: CapabilityAlias):
    """Converts a CapabilityAlias object into a hashable tuple key for caching."""
    if isinstance(alias_id, StructCapabilityAlias):
        return ('struct', alias_id.struct, alias_id.member)
    elif isinstance(alias_id, ExtensionCapabilityAlias):
        return ('ext', alias_id.name)
    return str(alias_id)


def gatherCapabilityAliases(vk: VulkanObject, alias_id: CapabilityAlias) -> list[CapabilityAlias]:
    alias_key = _get_alias_key(alias_id)
    if not hasattr(vk, '_capability_aliases_cache'):
        vk._capability_aliases_cache = {}
    if alias_key in vk._capability_aliases_cache:
        return vk._capability_aliases_cache[alias_key]

    canonical_key = None

    if isinstance(alias_id, StructCapabilityAlias):
        struct_obj = getStructByName(vk.structs, alias_id.struct)
        canonical_struct = struct_obj.name if struct_obj else alias_id.struct
        canonical_key = (canonical_struct, alias_id.member)
        
        if canonical_struct in vk.structs:
            for member in vk.structs[canonical_struct].members:
                if member.name == alias_id.member and isinstance(member.capabilityAlias, StructCapabilityAlias):
                    target_struct_obj = getStructByName(vk.structs, member.capabilityAlias.struct)
                    canonical_key = (target_struct_obj.name if target_struct_obj else member.capabilityAlias.struct, member.capabilityAlias.member)
                    break
    
    elif isinstance(alias_id, ExtensionCapabilityAlias):
        for struct_name, struct_obj in vk.structs.items():
            for member in struct_obj.members:
                if isinstance(member.capabilityAlias, ExtensionCapabilityAlias) and member.capabilityAlias.name == alias_id.name:
                    canonical_key = (struct_name, member.name)
                    break
            if canonical_key:
                break
        
        if not canonical_key:
            vk._capability_aliases_cache[alias_key] = []
            return []

    aliases = []
    for struct_name, struct_obj in vk.structs.items():
        for member in struct_obj.members:
            if isinstance(member.capabilityAlias, StructCapabilityAlias):
                target_struct_obj = getStructByName(vk.structs, member.capabilityAlias.struct)
                current_key = (target_struct_obj.name if target_struct_obj else member.capabilityAlias.struct, member.capabilityAlias.member)
            else:
                current_key = (struct_name, member.name)

            if current_key == canonical_key:
                aliases.append(StructCapabilityAlias(struct_name, member.name))
                for alias_struct in struct_obj.aliases:
                    aliases.append(StructCapabilityAlias(alias_struct, member.name))

                if isinstance(member.capabilityAlias, ExtensionCapabilityAlias) and member.capabilityAlias.name in vk.extensions:
                    aliases.append(ExtensionCapabilityAlias(member.capabilityAlias.name))

    result = [item for item in aliases if item != alias_id]
    vk._capability_aliases_cache[alias_key] = result
    return result


def gatherDependentCapabilityAliases(vk: VulkanObject, version: VK_VERSION, alias_id: CapabilityAlias) -> list[CapabilityAlias]:
    cache_key = (version, _get_alias_key(alias_id))
    if not hasattr(vk, '_dependent_capability_aliases_cache'):
        vk._dependent_capability_aliases_cache = {}
    if cache_key in vk._dependent_capability_aliases_cache:
        return vk._dependent_capability_aliases_cache[cache_key]

    aliases = gatherCapabilityAliases(vk, alias_id)
    target_ver_tuple = _parse_version_tuple(version)

    if target_ver_tuple == (0, 0):
        vk._dependent_capability_aliases_cache[cache_key] = aliases
        return aliases

    filtered_aliases = []
    for alias in aliases:
        if isinstance(alias, StructCapabilityAlias):
            struct_core_ver = getStructCoreVersion(vk, alias.struct)
            if struct_core_ver != VK_VERSION.NONE and version < struct_core_ver:
                continue

        if isinstance(alias, ExtensionCapabilityAlias):
            if alias.name in vk.extensions:
                ext = vk.extensions[alias.name]
                promoted_core_ver = _get_promoted_core_version(vk, ext.promotedTo)
                if promoted_core_ver is not None and target_ver_tuple >= promoted_core_ver:
                    continue

        filtered_aliases.append(alias)

    vk._dependent_capability_aliases_cache[cache_key] = filtered_aliases
    return filtered_aliases


def _parse_version_tuple(v) -> tuple[int, int]:
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
    if not promoted_to_str:
        return None
    if hasattr(vk, 'versions') and promoted_to_str in vk.versions:
        return _parse_version_tuple(promoted_to_str)
    if promoted_to_str.startswith("VK_VERSION_") or promoted_to_str.startswith("VK_API_VERSION_"):
        return _parse_version_tuple(promoted_to_str)
    return None

def gatherDependentCapabilityAliases2(vk: VulkanObject, version: VK_VERSION, alias_id: CapabilityAlias) -> list[CapabilityAlias]:
    aliases = gatherCapabilityAliases(vk, alias_id)
    target_ver_tuple = _parse_version_tuple(version)

    filtered_aliases = []
    for alias in aliases:
        ext_names = set()

        if isinstance(alias, ExtensionCapabilityAlias):
            ext_names.add(alias.name)

        elif isinstance(alias, StructCapabilityAlias):
            struct_name = alias.struct

            if hasattr(vk, 'aliasTypeRequirements') and struct_name in vk.aliasTypeRequirements:
                ext_names.update(vk.aliasTypeRequirements[struct_name].keys())
            elif struct_name in vk.structs:
                struct_obj = vk.structs[struct_name]
                if getattr(struct_obj, 'definingRequirements', None):
                    ext_names.update(struct_obj.definingRequirements.keys())
                elif getattr(struct_obj, 'extensions', None):
                    ext_names.update(struct_obj.extensions)
            else:
                struct_obj = getStructByName(vk.structs, struct_name)
                if struct_obj:
                    if getattr(struct_obj, 'definingRequirements', None):
                        ext_names.update(struct_obj.definingRequirements.keys())
                    elif getattr(struct_obj, 'extensions', None):
                        ext_names.update(struct_obj.extensions)

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


def findExtensionVersion(vk: VulkanObject, extension_name: str) -> int:
    if extension_name in vk.extensions:
        return vk.extensions[extension_name].specVersionValue
    else:
        return 0
    
def gatherDependentExtensions(vk: VulkanObject, version: VK_VERSION, ignore_extension_versions: bool, extensions: dict[str, int]) -> dict[str, int]:
    result = {}
    
    for extension in extensions:
        if extension not in vk.extensions:
            print(f'ERROR: {extension} is part of vk.xml, discarding')
            continue
        
        extension_data = vk.extensions[extension]
        depend_extensions = collectExtensions(version, extension_data.depends)
    
        for depend_extension in depend_extensions:
            if depend_extension not in result:
                if ignore_extension_versions:
                    result[depend_extension] = 1
                else:
                    result[depend_extension] = findExtensionVersion(vk, depend_extension)
            
        if extension not in result:
            if ignore_extension_versions:
                result[extension] = 1
            else:
                result[extension] = extension_data.specVersionValue
    
    return result

def gatherDynamicStructs(vk: VulkanObject):
    discovered = set()
    for struct_name, struct_def in vk.structs.items():
        for member in struct_def.members:
            if member.pointer and member.length is not None:
                discovered.add(struct_name)
    return sorted(list(discovered))

def gatherPromotedExtensionsForVersion(vk: VulkanObject, target_version: VK_VERSION) -> dict[str, int]:
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

def gatherPromotedExtensionsForExactVersion(vk: VulkanObject, exact_version: VK_VERSION) -> dict[str, int]:
    promoted_exts = {}
    if exact_version == VK_VERSION.NONE:
        return promoted_exts

    for ext_name, ext_obj in vk.extensions.items():
        promoted_to = getattr(ext_obj, 'promotedTo', None) or getattr(ext_obj, 'promoted_to', None)
        if promoted_to:
            promoted_ver = VK_VERSION.from_string(promoted_to)
            if promoted_ver == exact_version:
                spec_version = getattr(ext_obj, 'specVersionValue', None) or findExtensionVersion(vk, ext_name) or 1
                promoted_exts[ext_name] = spec_version

    return promoted_exts

def gatherRequiredFeaturesForVersion(vk: VulkanObject, target_version: VK_VERSION) -> dict[str, dict[str, bool]]:
    required_features: dict[str, dict[str, bool]] = {}
    if target_version == VK_VERSION.NONE:
        return required_features

    for ver_name, ver_obj in vk.versions.items():
        ver = VK_VERSION.from_string(ver_name)
        if ver != VK_VERSION.NONE and target_version >= ver:
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
    req_keys = set()

    struct_obj = vk.structs.get(struct_name) or getStructByName(vk.structs, struct_name)
    if struct_obj:
        if hasattr(struct_obj, 'definingRequirements') and struct_obj.definingRequirements:
            req_keys.update(struct_obj.definingRequirements.keys())
        elif hasattr(struct_obj, 'extensions') and struct_obj.extensions:
            req_keys.update(struct_obj.extensions)

    if hasattr(vk, 'aliasTypeRequirements') and struct_name in vk.aliasTypeRequirements:
        req_keys.update(vk.aliasTypeRequirements[struct_name].keys())

    if struct_obj and hasattr(struct_obj, 'aliases'):
        for alias in struct_obj.aliases:
            if hasattr(vk, 'aliasTypeRequirements') and alias in vk.aliasTypeRequirements:
                req_keys.update(vk.aliasTypeRequirements[alias].keys())

    if not req_keys:
        return True

    for req in req_keys:
        if req.startswith("VK_VERSION_") or req.startswith("VK_API_VERSION_"):
            ver = VK_VERSION.from_string(req)
            if ver != VK_VERSION.NONE and version != VK_VERSION.NONE and version >= ver:
                return True

        if req in enabled_exts:
            return True

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
    if not depends_expr:
        return True

    def is_symbol_enabled(token: str) -> bool:
        if "::" in token:
            parts = token.split("::")
            return (parts[0], parts[1]) in enabled_features

        if token.startswith("VK_VERSION_") or token.startswith("VK_API_VERSION_") or (token and token[0].isdigit()):
            ver = VK_VERSION.from_string(token)
            if ver != VK_VERSION.NONE:
                return api_version != VK_VERSION.NONE and api_version >= ver

        if token in enabled_exts:
            return True

        ext_obj = vk.extensions.get(token)
        if ext_obj and getattr(ext_obj, 'promotedTo', None):
            promoted_ver = VK_VERSION.from_string(ext_obj.promotedTo)
            if promoted_ver != VK_VERSION.NONE and api_version != VK_VERSION.NONE and api_version >= promoted_ver:
                return True

        return False

    return evalExpression(depends_expr, is_symbol_enabled)

def gatherSatisfiedCoreRequiredFeaturesForVersion(
    vk: VulkanObject, 
    exact_ver: VK_VERSION,
    api_version: VK_VERSION, 
    enabled_exts: set[str], 
    enabled_features: set[tuple[str, str]]
) -> dict[str, dict[str, bool]]:
    satisfied_features: dict[str, dict[str, bool]] = {}

    if exact_ver == VK_VERSION.NONE or api_version == VK_VERSION.NONE or exact_ver > api_version:
        return satisfied_features

    ver_obj = vk.versions.get(exact_ver.value)
    if not ver_obj:
        return satisfied_features

    for req in getattr(ver_obj, 'featureRequirement', []):
        if req.depends:
            if evaluateFeatureDepends(vk, req.depends, api_version, enabled_exts, enabled_features):
                fields = [f.strip() for f in req.field.split(',')] if req.field else []
                for field_name in fields:
                    satisfied_features.setdefault(req.struct, {})[field_name] = True
        else:
            fields = [f.strip() for f in req.field.split(',')] if req.field else []
            for field_name in fields:
                satisfied_features.setdefault(req.struct, {})[field_name] = True

    return satisfied_features

def gatherSatisfiedExtensionRequiredFeatures(
    vk: VulkanObject, 
    ext_name: str, 
    api_version: VK_VERSION, 
    enabled_exts: set[str], 
    enabled_features: set[tuple[str, str]]
) -> dict[str, dict[str, bool]]:
    satisfied_features: dict[str, dict[str, bool]] = {}

    if ext_name not in vk.extensions:
        return satisfied_features

    ext_obj = vk.extensions[ext_name]
    for req in getattr(ext_obj, 'featureRequirement', []):
        if req.depends:
            if evaluateFeatureDepends(vk, req.depends, api_version, enabled_exts, enabled_features):
                fields = [f.strip() for f in req.field.split(',')] if req.field else []
                for field_name in fields:
                    satisfied_features.setdefault(req.struct, {})[field_name] = True
        else:
            fields = [f.strip() for f in req.field.split(',')] if req.field else []
            for field_name in fields:
                satisfied_features.setdefault(req.struct, {})[field_name] = True

    return satisfied_features
