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

import os
import sys
import logging
import collections
import math

from source.vulkan_object_utils import (
    VulkanObject,
    getStructByName,
    getMemberByName,
    getStructCoreVersion,
    getStructDefiningExtensions,
    VK_VERSION
)
from source.vulkan_object_version import (
    is_bundle_structure,
    get_bundle_structure_core_version
)


class VulkanProfilesMergeGenerator:
    def __init__(self, vk: VulkanObject):
        self.vk = vk
        self.mode = 'intersection'
        self.first = True

    def is_enum_type(self, type_name: str) -> bool:
        if not type_name:
            return False
        if hasattr(self.vk, 'enums') and type_name in self.vk.enums:
            return True
        if hasattr(self.vk, 'flags') and type_name in self.vk.flags:
            return True
        flag_bits_name = type_name.replace('Flags', 'FlagBits')
        if hasattr(self.vk, 'enums') and (flag_bits_name in self.vk.enums or (flag_bits_name + 'KHR') in self.vk.enums):
            return True
        return False

    def is_pointer_array_member(self, xmlmember, val) -> bool:
        if not xmlmember or not isinstance(val, list):
            return False
        is_ptr = getattr(xmlmember, 'pointer', False) or xmlmember.name.startswith('p')
        is_fixed = bool(getattr(xmlmember, 'fixedSizeArray', None) or getattr(xmlmember, 'array', None))
        return is_ptr and not is_fixed

    def get_count_to_array_map(self, struct_obj):
        count_to_array = {}
        if not struct_obj or not hasattr(struct_obj, 'members'):
            return count_to_array
        for xmlmember in struct_obj.members:
            array_name = xmlmember.name
            count_member_name = None
            length_attr = getattr(xmlmember, 'len', None)
            if length_attr and isinstance(length_attr, str):
                count_member_name = length_attr.split(',')[0].strip()
            else:
                if array_name.startswith('p') and len(array_name) > 2:
                    base = array_name[1:]
                    if base.endswith('s'):
                        cand = base[:-1] + 'Count'
                        cand = cand[0].lower() + cand[1:]
                        if getMemberByName(struct_obj, cand):
                            count_member_name = cand
                    if not count_member_name:
                        cand = base + 'Count'
                        cand = cand[0].lower() + cand[1:]
                        if getMemberByName(struct_obj, cand):
                            count_member_name = cand

            if count_member_name and getMemberByName(struct_obj, count_member_name):
                count_to_array[count_member_name] = array_name
        return count_to_array

    def sync_array_count_members(self, merged, struct_obj):
        if not struct_obj or not isinstance(merged, dict):
            return
        count_to_array = self.get_count_to_array_map(struct_obj)
        for count_member, array_name in count_to_array.items():
            if array_name in merged and isinstance(merged[array_name], list):
                merged[count_member] = len(merged[array_name])
            elif count_member in merged or (array_name in merged and not merged[array_name]):
                merged[count_member] = 0

    def merge(self, profile_config, profile_file, mode='intersection'):
        self.mode = mode
        logging.info('Building a Vulkan ' + '.'.join(profile_config.api_version) + ' profile')

        capabilities_key = profile_config.name + "_block"

        merged_caps = self.merge_capabilities(
            profile_config.input_jsons,
            profile_config.input_profile_names,
            profile_config.api_version
        )

        profile_dict = self.get_profile(profile_config, capabilities_key)

        if isinstance(profile_file, dict):
            profile_file.setdefault("capabilities", {})[capabilities_key] = merged_caps
            profile_file.setdefault("profiles", {})[profile_config.name] = profile_dict
        else:
            profile_file.add_capabilities(capabilities_key, merged_caps)
            profile_file.add_profile(profile_config.name, profile_dict)

    def merge_capabilities(self, jsons, profile_names, api_version):
        merged_extensions = dict()
        merged_features = dict()
        merged_properties = dict()
        merged_formats = dict()
        merged_qfp = list()
        merged_video_profiles = list()

        for i in range(len(jsons)):
            self.first = (i == 0)
            for capability_name in jsons[i]['profiles'][profile_names[i]]['capabilities']:
                capability = jsons[i]['capabilities'][capability_name]

                # Prune structures/formats not present in subsequent JSONs during intersection mode
                if self.mode == 'intersection' and not self.first:
                    if 'features' in capability:
                        for feature in dict(merged_features):
                            if feature not in capability['features']:
                                del merged_features[feature]
                    else:
                        merged_features.clear()

                    if 'properties' in capability:
                        for prop in dict(merged_properties):
                            if prop not in capability['properties']:
                                del merged_properties[prop]
                    else:
                        merged_properties.clear()

                    if 'formats' in capability:
                        for fmt_name in dict(merged_formats):
                            if fmt_name not in capability['formats']:
                                del merged_formats[fmt_name]
                    else:
                        merged_formats.clear()

                # Extensions
                if 'extensions' in capability:
                    if self.mode == 'union' or self.first:
                        for extension, spec_ver in capability['extensions'].items():
                            merged_extensions[extension] = spec_ver
                    elif self.mode == 'intersection':
                        for extension in list(merged_extensions):
                            if extension not in capability['extensions']:
                                del merged_extensions[extension]

                # Features
                if 'features' in capability:
                    for feature_name, feat_members in capability['features'].items():
                        self.add_struct(feature_name, feat_members, merged_features)

                # Properties
                if 'properties' in capability:
                    for property_name, prop_members in capability['properties'].items():
                        if property_name in merged_properties:
                            self.add_members(merged_properties[property_name], prop_members, property_name)
                        else:
                            if self.mode == 'union' or self.first:
                                merged_properties[property_name] = dict()
                                self.add_members(merged_properties[property_name], prop_members, property_name)

                # Formats
                if 'formats' in capability:
                    for fmt_name in capability['formats']:
                        if (fmt_name not in merged_formats) and (self.mode == 'union' or self.first):
                            merged_formats[fmt_name] = {
                                'VkFormatProperties': {},
                                'VkFormatProperties3': {},
                                'VkFormatProperties3KHR': {}
                            }

                        if fmt_name in merged_formats:
                            for prop_struct_name in ['VkFormatProperties', 'VkFormatProperties3', 'VkFormatProperties3KHR']:
                                for feat_cat in ['linearTilingFeatures', 'optimalTilingFeatures', 'bufferFeatures']:
                                    self.merge_format_features(merged_formats, fmt_name, capability, prop_struct_name, feat_cat)

                # Queue Families
                if 'queueFamiliesProperties' in capability:
                    if self.mode == 'intersection':
                        if self.first:
                            for qfp in capability['queueFamiliesProperties']:
                                merged_qfp.append(qfp)
                        else:
                            for mqfp in list(merged_qfp):
                                found = False
                                for qfp in capability['queueFamiliesProperties']:
                                    if mqfp['VkQueueFamilyProperties']['queueFlags'] != qfp['VkQueueFamilyProperties']['queueFlags']:
                                        continue
                                    if mqfp['VkQueueFamilyProperties']['queueCount'] != qfp['VkQueueFamilyProperties']['queueCount']:
                                        continue
                                    if mqfp['VkQueueFamilyProperties']['timestampValidBits'] != qfp['VkQueueFamilyProperties']['timestampValidBits']:
                                        continue
                                    if mqfp['VkQueueFamilyProperties']['minImageTransferGranularity']['width'] != qfp['VkQueueFamilyProperties']['minImageTransferGranularity']['width']:
                                        continue
                                    if mqfp['VkQueueFamilyProperties']['minImageTransferGranularity']['height'] != qfp['VkQueueFamilyProperties']['minImageTransferGranularity']['height']:
                                        continue
                                    if mqfp['VkQueueFamilyProperties']['minImageTransferGranularity']['depth'] != qfp['VkQueueFamilyProperties']['minImageTransferGranularity']['depth']:
                                        continue
                                    found = True
                                    break
                                if not found:
                                    merged_qfp.remove(mqfp)

                    elif self.mode == 'union':
                        for qfp in capability['queueFamiliesProperties']:
                            if not merged_qfp:
                                merged_qfp.append(qfp)
                            else:
                                for mqfp in merged_qfp:
                                    if not self.compareList(mqfp['VkQueueFamilyProperties']['queueFlags'], qfp['VkQueueFamilyProperties']['queueFlags']):
                                        merged_qfp.append(qfp)
                                    elif qfp['VkQueueFamilyProperties']['queueCount'] != mqfp['VkQueueFamilyProperties']['queueCount']:
                                        merged_qfp.append(qfp)
                                    elif qfp['VkQueueFamilyProperties']['timestampValidBits'] != mqfp['VkQueueFamilyProperties']['timestampValidBits']:
                                        merged_qfp.append(qfp)
                                    elif qfp['VkQueueFamilyProperties']['minImageTransferGranularity']['width'] != mqfp['VkQueueFamilyProperties']['minImageTransferGranularity']['width']:
                                        merged_qfp.append(qfp)
                                    elif qfp['VkQueueFamilyProperties']['minImageTransferGranularity']['height'] != mqfp['VkQueueFamilyProperties']['minImageTransferGranularity']['height']:
                                        merged_qfp.append(qfp)
                                    elif qfp['VkQueueFamilyProperties']['minImageTransferGranularity']['depth'] != mqfp['VkQueueFamilyProperties']['minImageTransferGranularity']['depth']:
                                        merged_qfp.append(qfp)

                # Video Profiles
                if 'videoProfiles' in capability:
                    if self.mode == 'intersection':
                        if self.first:
                            for video_profile in capability['videoProfiles']:
                                merged_video_profiles.append(video_profile)
                        else:
                            def deep_compare(a, b):
                                if isinstance(a, list):
                                    if isinstance(b, list) and len(a) == len(b):
                                        return all(deep_compare(a[i], b[i]) for i in range(len(a)))
                                    return False
                                elif isinstance(a, dict):
                                    if isinstance(b, dict) and len(a.keys()) == len(b.keys()):
                                        return all(k in b and deep_compare(a[k], b[k]) for k in a.keys())
                                    return False
                                else:
                                    return a == b

                            for merged_video_profile in list(merged_video_profiles):
                                found = any(deep_compare(merged_video_profile, vp) for vp in capability['videoProfiles'])
                                if not found:
                                    merged_video_profiles.remove(merged_video_profile)

                    elif self.mode == 'union':
                        for video_profile in capability['videoProfiles']:
                            merged_video_profiles.append(video_profile)

        capabilities = dict()
        if merged_extensions:
            sorted_extensions = collections.OrderedDict(sorted(merged_extensions.items()))
            capabilities['extensions'] = dict(sorted_extensions)

        if merged_features:
            for feature in dict(merged_features):
                if not merged_features[feature]:
                    del merged_features[feature]

            sorted_features = collections.OrderedDict(sorted(merged_features.items()))
            capabilities['features'] = dict(sorted_features)

        if merged_properties:
            for prop in dict(merged_properties):
                if not merged_properties[prop]:
                    del merged_properties[prop]

            sorted_properties = collections.OrderedDict(sorted(merged_properties.items()))
            capabilities['properties'] = dict(sorted_properties)

        if merged_formats:
            sorted_formats = collections.OrderedDict(sorted(merged_formats.items()))
            capabilities['formats'] = dict(sorted_formats)

            formats_to_remove = []
            for fmt_name in capabilities['formats']:
                for prop_name in ['VkFormatProperties', 'VkFormatProperties3', 'VkFormatProperties3KHR']:
                    for feat_cat in ['linearTilingFeatures', 'optimalTilingFeatures', 'bufferFeatures']:
                        if feat_cat in capabilities['formats'][fmt_name][prop_name]:
                            if not capabilities['formats'][fmt_name][prop_name][feat_cat]:
                                del capabilities['formats'][fmt_name][prop_name][feat_cat]
                    if prop_name in capabilities['formats'][fmt_name]:
                        if not capabilities['formats'][fmt_name][prop_name]:
                            del capabilities['formats'][fmt_name][prop_name]
            if not capabilities['formats'][fmt_name]:
                formats_to_remove.append(fmt_name)

            for fmt_name in formats_to_remove:
                del capabilities['formats'][fmt_name]

        if merged_qfp:
            capabilities['queueFamiliesProperties'] = merged_qfp

        if merged_video_profiles:
            capabilities['videoProfiles'] = merged_video_profiles

        return capabilities

    def compareList(self, l1, l2):
        return collections.Counter(l1) == collections.Counter(l2)

    def merge_format_features(self, merged_formats, fmt_name, capability, prop_name, features):
        if fmt_name in capability['formats'] and prop_name in capability['formats'][fmt_name]:
            if features in capability['formats'][fmt_name][prop_name]:
                if features not in merged_formats[fmt_name][prop_name]:
                    if self.mode == 'union' or self.first:
                        merged_formats[fmt_name][prop_name][features] = list(capability['formats'][fmt_name][prop_name][features])
                else:
                    if self.mode == 'union':
                        for feat in capability['formats'][fmt_name][prop_name][features]:
                            if feat not in merged_formats[fmt_name][prop_name][features]:
                                merged_formats[fmt_name][prop_name][features].append(feat)
                    else:
                        for feat in list(merged_formats[fmt_name][prop_name][features]):
                            if feat not in capability['formats'][fmt_name][prop_name][features]:
                                merged_formats[fmt_name][prop_name][features].remove(feat)

    def add_struct(self, struct_name, struct_data, merged):
        if struct_name in merged:
            if self.mode == 'union':
                for member, val in struct_data.items():
                    if member in merged[struct_name]:
                        merged[struct_name][member] = merged[struct_name][member] or val
                    else:
                        merged[struct_name][member] = val
            elif self.mode == 'intersection':
                if self.first:
                    for member, val in struct_data.items():
                        merged[struct_name][member] = val
                for member in list(merged[struct_name]):
                    if member not in struct_data:
                        del merged[struct_name][member]
                    elif struct_data[member] != merged[struct_name][member]:
                        del merged[struct_name][member]
        else:
            if self.mode == 'union' or self.first:
                merged[struct_name] = dict(struct_data)

    def add_members(self, merged, entry, property_name=None):
        struct_obj = getStructByName(self.vk.structs, property_name) if property_name else None
        count_to_array = self.get_count_to_array_map(struct_obj)

        if property_name and struct_obj:
            for member in list(merged):
                if property_name == 'VkPhysicalDeviceProperties' and member in ('limits', 'sparseProperties'):
                    continue
                mem_obj = getMemberByName(struct_obj, member)
                if mem_obj:
                    tokens = [t.strip() for t in mem_obj.limitType.split(',')] if mem_obj.limitType else []
                    is_ptr_array = self.is_pointer_array_member(mem_obj, merged[member])
                    is_count = member in count_to_array
                    if 'noauto' in tokens and not is_ptr_array and not is_count:
                        del merged[member]

        if self.mode == 'intersection' and not self.first:
            for member in list(merged):
                if property_name == 'VkPhysicalDeviceProperties' and member in ('limits', 'sparseProperties'):
                    continue
                if member not in entry:
                    del merged[member]

        for member, val in entry.items():
            if property_name == 'VkPhysicalDeviceProperties' and member == 'limits':
                if 'limits' not in merged:
                    if self.mode == 'union' or self.first:
                        merged['limits'] = dict()
                    else:
                        continue
                limits_struct_obj = self.vk.structs.get('VkPhysicalDeviceLimits')
                self.add_members_nested(merged['limits'], val, limits_struct_obj, 'VkPhysicalDeviceLimits')
                continue

            if property_name == 'VkPhysicalDeviceProperties' and member == 'sparseProperties':
                if 'sparseProperties' not in merged:
                    if self.mode == 'union' or self.first:
                        merged['sparseProperties'] = dict()
                    else:
                        continue
                sparse_struct_obj = self.vk.structs.get('VkPhysicalDeviceSparseProperties')
                self.add_members_nested(merged['sparseProperties'], val, sparse_struct_obj, 'VkPhysicalDeviceSparseProperties')
                continue

            xmlmember = getMemberByName(struct_obj, member) if struct_obj else None

            if member not in merged:
                if xmlmember:
                    tokens = [t.strip() for t in xmlmember.limitType.split(',')] if xmlmember.limitType else []
                    is_ptr_array = self.is_pointer_array_member(xmlmember, val)
                    is_count = member in count_to_array
                    if 'noauto' in tokens and not is_ptr_array and not is_count:
                        continue
                if self.mode == 'union' or self.first:
                    if xmlmember and xmlmember.type in ('uint64_t', 'VkDeviceSize'):
                        merged[member] = int(val)
                    else:
                        merged[member] = val
            else:
                limittype = xmlmember.limitType if xmlmember else None
                tokens = [t.strip() for t in limittype.split(',')] if limittype else []
                if 'struct' in tokens and xmlmember:
                    sub_struct_obj = getStructByName(self.vk.structs, xmlmember.type)
                    if sub_struct_obj:
                        for smember in sub_struct_obj.members:
                            sm_name = smember.name
                            if sm_name in merged[member]:
                                if sm_name in val:
                                    self.merge_members(merged[member], sm_name, val, smember, xmlmember.type)
                            elif (self.mode == 'union' or self.first) and (sm_name in val):
                                if smember.type in ('uint64_t', 'VkDeviceSize'):
                                    merged[member][sm_name] = int(val[sm_name])
                                else:
                                    merged[member][sm_name] = val[sm_name]
                        self.sync_array_count_members(merged[member], sub_struct_obj)
                elif xmlmember:
                    self.merge_members(merged, member, entry, xmlmember, property_name)

        if struct_obj:
            self.sync_array_count_members(merged, struct_obj)

    def add_members_nested(self, merged, entry, struct_obj, struct_name):
        count_to_array = self.get_count_to_array_map(struct_obj)

        if struct_obj:
            for member in list(merged):
                xmlmember = getMemberByName(struct_obj, member)
                if xmlmember:
                    tokens = [t.strip() for t in xmlmember.limitType.split(',')] if xmlmember.limitType else []
                    is_ptr_array = self.is_pointer_array_member(xmlmember, merged[member])
                    is_count = member in count_to_array
                    if 'noauto' in tokens and not is_ptr_array and not is_count:
                        del merged[member]

        if self.mode == 'intersection' and not self.first:
            for member in list(merged):
                if member not in entry:
                    del merged[member]

        for member, val in entry.items():
            xmlmember = getMemberByName(struct_obj, member) if struct_obj else None
            if member not in merged:
                if xmlmember:
                    tokens = [t.strip() for t in xmlmember.limitType.split(',')] if xmlmember.limitType else []
                    is_ptr_array = self.is_pointer_array_member(xmlmember, val)
                    is_count = member in count_to_array
                    if 'noauto' in tokens and not is_ptr_array and not is_count:
                        continue
                if self.mode == 'union' or self.first:
                    if xmlmember and xmlmember.type in ('uint64_t', 'VkDeviceSize'):
                        merged[member] = int(val)
                    else:
                        merged[member] = val
            else:
                if xmlmember:
                    self.merge_members(merged, member, entry, xmlmember, struct_name)

        if struct_obj:
            self.sync_array_count_members(merged, struct_obj)

    def _merge_min_pot_intersection(self, val1, val2):
        try:
            i1 = int(val1)
            i2 = int(val2)
            if i1 > 0 and (i1 & (i1 - 1)) == 0 and i2 > 0 and (i2 & (i2 - 1)) == 0:
                return max(i1, i2)
        except (ValueError, TypeError):
            pass
        return None

    def _merge_min_mul_intersection(self, val1, val2, is_float=False):
        if not is_float and isinstance(val1, int) and isinstance(val2, int) and not isinstance(val1, bool) and not isinstance(val2, bool):
            return math.lcm(val1, val2)
        try:
            f1 = float(val1)
            f2 = float(val2)
            a = min(f1, f2)
            b = max(f1, f2)
            if a <= 0:
                return None
            ratio = b / a
            if math.isclose(ratio % 1, 0, abs_tol=1e-4) or math.isclose(ratio % 1, 1, abs_tol=1e-4):
                return b if isinstance(val1, float) or isinstance(val2, float) or is_float else int(b)
            for k in range(2, 11):
                candidate = a * k
                c_ratio = candidate / b
                if math.isclose(c_ratio % 1, 0, abs_tol=1e-4) or math.isclose(c_ratio % 1, 1, abs_tol=1e-4):
                    return candidate if isinstance(val1, float) or isinstance(val2, float) or is_float else int(candidate)
        except (ValueError, TypeError):
            pass
        return None

    def _merge_min_pot_union(self, val1, val2):
        try:
            i1 = int(val1)
            i2 = int(val2)
            if i1 > 0 and (i1 & (i1 - 1)) == 0 and i2 > 0 and (i2 & (i2 - 1)) == 0:
                return min(i1, i2)
        except (ValueError, TypeError):
            pass
        return None

    def _merge_min_mul_union(self, val1, val2, is_float=False):
        if not is_float and isinstance(val1, int) and isinstance(val2, int) and not isinstance(val1, bool) and not isinstance(val2, bool):
            res = math.gcd(val1, val2)
            return res if res > 0 else None
        try:
            f1 = float(val1)
            f2 = float(val2)
            a = min(f1, f2)
            b = max(f1, f2)
            if a <= 0:
                return None
            ratio = b / a
            if math.isclose(ratio % 1, 0, abs_tol=1e-4) or math.isclose(ratio % 1, 1, abs_tol=1e-4):
                return a if isinstance(val1, float) or isinstance(val2, float) or is_float else int(a)
        except (ValueError, TypeError):
            pass
        return None

    def merge_members(self, merged, member, entry, xmlmember, property_name=None):
        limittype = xmlmember.limitType if xmlmember else None
        tokens = [t.strip() for t in limittype.split(',')] if limittype else []

        is_list = isinstance(merged[member], list) or (member in entry and isinstance(entry[member], list))
        is_ptr_array = self.is_pointer_array_member(xmlmember, merged[member] if member in merged else entry.get(member))

        if 'noauto' in tokens and not is_ptr_array:
            del merged[member]
            return

        if 'exact' in tokens and not is_ptr_array:
            if xmlmember and xmlmember.type in ('uint64_t', 'VkDeviceSize'):
                if int(merged[member]) != int(entry[member]):
                    del merged[member]
            elif xmlmember and xmlmember.type == 'float':
                if not math.isclose(float(merged[member]), float(entry[member]), abs_tol=1e-5):
                    del merged[member]
            else:
                if merged[member] != entry[member]:
                    del merged[member]
            return

        is_max = 'max' in tokens or 'bits' in tokens
        is_min = 'min' in tokens
        is_pot = 'pot' in tokens
        is_mul = 'mul' in tokens
        is_range = 'range' in tokens
        is_bitmask = not (is_max or is_min or is_range) and (
            'bitmask' in tokens or 
            is_ptr_array or
            (xmlmember and self.is_enum_type(xmlmember.type))
        )

        if self.mode == 'union':
            if is_max:
                if xmlmember.type == 'VkExtent2D':
                    merged[member]['width'] = max(merged[member]['width'], entry[member]['width'])
                    merged[member]['height'] = max(merged[member]['height'], entry[member]['height'])
                elif xmlmember.type == 'VkExtent3D':
                    merged[member]['width'] = max(merged[member]['width'], entry[member]['width'])
                    merged[member]['height'] = max(merged[member]['height'], entry[member]['height'])
                    merged[member]['depth'] = max(merged[member]['depth'], entry[member]['depth'])
                elif xmlmember.type == 'VkBool32':
                    merged[member] = merged[member] or entry[member]
                elif isinstance(merged[member], list) and isinstance(entry[member], list):
                    length = min(len(merged[member]), len(entry[member]))
                    for idx in range(length):
                        merged[member][idx] = max(merged[member][idx], entry[member][idx])
                else:
                    merged[member] = max(merged[member], entry[member])
            elif is_min:
                if xmlmember.type == 'VkExtent2D':
                    if is_pot:
                        w = self._merge_min_pot_union(merged[member]['width'], entry[member]['width'])
                        h = self._merge_min_pot_union(merged[member]['height'], entry[member]['height'])
                        if w is not None and h is not None:
                            merged[member]['width'], merged[member]['height'] = w, h
                        else:
                            del merged[member]
                    elif is_mul:
                        w = self._merge_min_mul_union(merged[member]['width'], entry[member]['width'])
                        h = self._merge_min_mul_union(merged[member]['height'], entry[member]['height'])
                        if w is not None and h is not None:
                            merged[member]['width'], merged[member]['height'] = w, h
                        else:
                            del merged[member]
                    else:
                        merged[member]['width'] = min(merged[member]['width'], entry[member]['width'])
                        merged[member]['height'] = min(merged[member]['height'], entry[member]['height'])
                elif xmlmember.type == 'VkExtent3D':
                    if is_pot:
                        w = self._merge_min_pot_union(merged[member]['width'], entry[member]['width'])
                        h = self._merge_min_pot_union(merged[member]['height'], entry[member]['height'])
                        d = self._merge_min_pot_union(merged[member]['depth'], entry[member]['depth'])
                        if w is not None and h is not None and d is not None:
                            merged[member]['width'], merged[member]['height'], merged[member]['depth'] = w, h, d
                        else:
                            del merged[member]
                    elif is_mul:
                        w = self._merge_min_mul_union(merged[member]['width'], entry[member]['width'])
                        h = self._merge_min_mul_union(merged[member]['height'], entry[member]['height'])
                        d = self._merge_min_mul_union(merged[member]['depth'], entry[member]['depth'])
                        if w is not None and h is not None and d is not None:
                            merged[member]['width'], merged[member]['height'], merged[member]['depth'] = w, h, d
                        else:
                            del merged[member]
                    else:
                        merged[member]['width'] = min(merged[member]['width'], entry[member]['width'])
                        merged[member]['height'] = min(merged[member]['height'], entry[member]['height'])
                        merged[member]['depth'] = min(merged[member]['depth'], entry[member]['depth'])
                elif xmlmember.type == 'VkBool32':
                    merged[member] = merged[member] and entry[member]
                elif isinstance(merged[member], list) and isinstance(entry[member], list):
                    res_arr = []
                    valid = True
                    for idx in range(min(len(merged[member]), len(entry[member]))):
                        if is_pot:
                            res = self._merge_min_pot_union(merged[member][idx], entry[member][idx])
                        elif is_mul:
                            is_float = (xmlmember.type == 'float') or isinstance(merged[member][idx], float)
                            res = self._merge_min_mul_union(merged[member][idx], entry[member][idx], is_float)
                        else:
                            res = min(merged[member][idx], entry[member][idx])
                        if res is None:
                            valid = False
                            break
                        res_arr.append(res)
                    if valid:
                        merged[member] = res_arr
                    else:
                        del merged[member]
                else:
                    if is_pot:
                        res = self._merge_min_pot_union(merged[member], entry[member])
                        if res is not None:
                            merged[member] = res
                        else:
                            del merged[member]
                    elif is_mul:
                        is_float = (xmlmember.type == 'float') or isinstance(merged[member], float) or isinstance(entry[member], float)
                        res = self._merge_min_mul_union(merged[member], entry[member], is_float)
                        if res is not None:
                            merged[member] = res
                        else:
                            del merged[member]
                    else:
                        merged[member] = min(merged[member], entry[member])
            elif is_bitmask:
                if isinstance(merged[member], list) and isinstance(entry[member], list):
                    for smember in entry[member]:
                        if smember not in merged[member]:
                            merged[member].append(smember)
                elif not isinstance(merged[member], list) and not isinstance(entry[member], list):
                    merged[member] = int(merged[member]) | int(entry[member])
            elif is_range:
                merged[member][0] = min(merged[member][0], entry[member][0])
                merged[member][1] = max(merged[member][1], entry[member][1])

        elif self.mode == 'intersection':
            if is_max:
                if xmlmember.type == 'VkExtent2D':
                    merged[member]['width'] = min(merged[member]['width'], entry[member]['width'])
                    merged[member]['height'] = min(merged[member]['height'], entry[member]['height'])
                elif xmlmember.type == 'VkExtent3D':
                    merged[member]['width'] = min(merged[member]['width'], entry[member]['width'])
                    merged[member]['height'] = min(merged[member]['height'], entry[member]['height'])
                    merged[member]['depth'] = min(merged[member]['depth'], entry[member]['depth'])
                elif xmlmember.type == 'VkBool32':
                    merged[member] = merged[member] and entry[member]
                elif isinstance(merged[member], list) and isinstance(entry[member], list):
                    length = min(len(merged[member]), len(entry[member]))
                    for idx in range(length):
                        merged[member][idx] = min(merged[member][idx], entry[member][idx])
                elif xmlmember.type in ('uint64_t', 'VkDeviceSize'):
                    merged[member] = min(int(entry[member]), int(merged[member]))
                elif xmlmember.type == 'float':
                    merged[member] = min(float(entry[member]), float(merged[member]))
                else:
                    merged[member] = min(merged[member], entry[member])
            elif is_min:
                if xmlmember.type == 'VkExtent2D':
                    if is_pot:
                        w = self._merge_min_pot_intersection(merged[member]['width'], entry[member]['width'])
                        h = self._merge_min_pot_intersection(merged[member]['height'], entry[member]['height'])
                        if w is not None and h is not None:
                            merged[member]['width'], merged[member]['height'] = w, h
                        else:
                            del merged[member]
                    elif is_mul:
                        w = self._merge_min_mul_intersection(merged[member]['width'], entry[member]['width'])
                        h = self._merge_min_mul_intersection(merged[member]['height'], entry[member]['height'])
                        if w is not None and h is not None:
                            merged[member]['width'], merged[member]['height'] = w, h
                        else:
                            del merged[member]
                    else:
                        merged[member]['width'] = max(merged[member]['width'], entry[member]['width'])
                        merged[member]['height'] = max(merged[member]['height'], entry[member]['height'])
                elif xmlmember.type == 'VkExtent3D':
                    if is_pot:
                        w = self._merge_min_pot_intersection(merged[member]['width'], entry[member]['width'])
                        h = self._merge_min_pot_intersection(merged[member]['height'], entry[member]['height'])
                        d = self._merge_min_pot_intersection(merged[member]['depth'], entry[member]['depth'])
                        if w is not None and h is not None and d is not None:
                            merged[member]['width'], merged[member]['height'], merged[member]['depth'] = w, h, d
                        else:
                            del merged[member]
                    elif is_mul:
                        w = self._merge_min_mul_intersection(merged[member]['width'], entry[member]['width'])
                        h = self._merge_min_mul_intersection(merged[member]['height'], entry[member]['height'])
                        d = self._merge_min_mul_intersection(merged[member]['depth'], entry[member]['depth'])
                        if w is not None and h is not None and d is not None:
                            merged[member]['width'], merged[member]['height'], merged[member]['depth'] = w, h, d
                        else:
                            del merged[member]
                    else:
                        merged[member]['width'] = max(merged[member]['width'], entry[member]['width'])
                        merged[member]['height'] = max(merged[member]['height'], entry[member]['height'])
                        merged[member]['depth'] = max(merged[member]['depth'], entry[member]['depth'])
                elif xmlmember.type == 'VkBool32':
                    merged[member] = merged[member] or entry[member]
                elif isinstance(merged[member], list) and isinstance(entry[member], list):
                    res_arr = []
                    valid = True
                    for idx in range(min(len(merged[member]), len(entry[member]))):
                        if is_pot:
                            res = self._merge_min_pot_intersection(merged[member][idx], entry[member][idx])
                        elif is_mul:
                            is_float = (xmlmember.type == 'float') or isinstance(merged[member][idx], float)
                            res = self._merge_min_mul_intersection(merged[member][idx], entry[member][idx], is_float)
                        else:
                            res = max(merged[member][idx], entry[member][idx])
                        if res is None:
                            valid = False
                            break
                        res_arr.append(res)
                    if valid:
                        merged[member] = res_arr
                    else:
                        del merged[member]
                else:
                    if is_pot:
                        res = self._merge_min_pot_intersection(merged[member], entry[member])
                        if res is not None:
                            merged[member] = res
                        else:
                            del merged[member]
                    elif is_mul:
                        is_float = (xmlmember.type == 'float') or isinstance(merged[member], float) or isinstance(entry[member], float)
                        res = self._merge_min_mul_intersection(merged[member], entry[member], is_float)
                        if res is not None:
                            merged[member] = res
                        else:
                            del merged[member]
                    else:
                        if xmlmember.type in ('uint64_t', 'VkDeviceSize'):
                            merged[member] = max(int(entry[member]), int(merged[member]))
                        elif xmlmember.type == 'float':
                            merged[member] = max(float(entry[member]), float(merged[member]))
                        else:
                            merged[member] = max(merged[member], entry[member])
            elif is_bitmask:
                if isinstance(merged[member], list) and isinstance(entry[member], list):
                    remove_list = []
                    for value in merged[member]:
                        if value not in entry[member]:
                            remove_list.append(value)
                    for value in remove_list:
                        merged[member].remove(value)
                elif not isinstance(merged[member], list) and not isinstance(entry[member], list):
                    merged[member] = int(merged[member]) & int(entry[member])
            elif is_range:
                merged[member][0] = max(merged[member][0], entry[member][0])
                merged[member][1] = min(merged[member][1], entry[member][1])

    def get_profile(self, profile_config, capabilities_key):
        profile = dict()
        profile['version'] = profile_config.version
        if profile_config.stage != 'STABLE':
            profile['status'] = profile_config.stage
        profile['api-version'] = '.'.join(profile_config.api_version)
        profile['label'] = profile_config.label
        profile['description'] = profile_config.description

        if len(profile_config.required_profiles) > 0:
            profile['profiles'] = profile_config.required_profiles
        profile['capabilities'] = list()
        profile['capabilities'].append(capabilities_key)
        return profile
    