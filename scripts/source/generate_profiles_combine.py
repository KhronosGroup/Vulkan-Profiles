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
from source.profiles_json_utils import collect_block_names, deep_merge_dict


class VulkanProfilesCombineGenerator:
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

    def sync_array_count_members(self, combined, struct_obj):
        if not struct_obj or not isinstance(combined, dict):
            return
        count_to_array = self.get_count_to_array_map(struct_obj)
        for count_member, array_name in count_to_array.items():
            if array_name in combined and isinstance(combined[array_name], list):
                combined[count_member] = len(combined[array_name])
            elif count_member in combined or (array_name in combined and not combined[array_name]):
                combined[count_member] = 0

    def combine(self, profile_config, profile_file, mode='intersection'):
        self.mode = mode
        logging.info('Building a Vulkan ' + '.'.join(profile_config.api_version) + ' profile')

        capabilities_key = profile_config.name + "_block"

        combined_caps = self.combine_capabilities(
            profile_config.input_jsons,
            profile_config.input_profile_names,
            profile_config.api_version
        )

        profile_dict = self.get_profile(profile_config, capabilities_key)

        if isinstance(profile_file, dict):
            profile_file.setdefault("capabilities", {})[capabilities_key] = combined_caps
            profile_file.setdefault("profiles", {})[profile_config.name] = profile_dict
        else:
            profile_file.add_capabilities(capabilities_key, combined_caps)
            profile_file.add_profile(profile_config.name, profile_dict)

    def combine_capabilities(self, jsons, profile_names, api_version):
        combined_extensions = dict()
        combined_features = dict()
        combined_properties = dict()
        combined_formats = dict()
        combined_qfp = list()
        combined_video_profiles = list()

        for i in range(len(jsons)):
            self.first = (i == 0)

            # 1. Aggregate all capability blocks for profile i first
            profile_cap = {
                'extensions': {},
                'features': {},
                'properties': {},
                'formats': {},
                'queueFamiliesProperties': [],
                'videoProfiles': []
            }

            block_names = collect_block_names(jsons[i]['profiles'][profile_names[i]].get('capabilities', []))
            for capability_name in block_names:
                if capability_name not in jsons[i].get('capabilities', {}):
                    continue
                cap = jsons[i]['capabilities'][capability_name]

                if 'extensions' in cap:
                    for extension, spec_ver in cap['extensions'].items():
                        profile_cap['extensions'][extension] = spec_ver

                if 'features' in cap:
                    for feature_name, feat_members in cap['features'].items():
                        if feature_name not in profile_cap['features']:
                            profile_cap['features'][feature_name] = {}
                        profile_cap['features'][feature_name].update(feat_members)

                if 'properties' in cap:
                    for property_name, prop_members in cap['properties'].items():
                        if property_name not in profile_cap['properties']:
                            profile_cap['properties'][property_name] = {}
                        deep_merge_dict(profile_cap['properties'][property_name], prop_members)

                if 'formats' in cap:
                    for fmt_name, fmt_data in cap['formats'].items():
                        if fmt_name not in profile_cap['formats']:
                            profile_cap['formats'][fmt_name] = {}
                        deep_merge_dict(profile_cap['formats'][fmt_name], fmt_data)

                if 'queueFamiliesProperties' in cap:
                    for qfp in cap['queueFamiliesProperties']:
                        profile_cap['queueFamiliesProperties'].append(qfp)

                if 'videoProfiles' in cap:
                    for vp in cap['videoProfiles']:
                        profile_cap['videoProfiles'].append(vp)

            # 2. Perform intersection pruning against accumulated profiles
            if self.mode == 'intersection' and not self.first:
                if profile_cap['features']:
                    for feature in dict(combined_features):
                        if feature not in profile_cap['features']:
                            del combined_features[feature]
                else:
                    combined_features.clear()

                if profile_cap['properties']:
                    for prop in dict(combined_properties):
                        if prop not in profile_cap['properties']:
                            del combined_properties[prop]
                else:
                    combined_properties.clear()

                if profile_cap['formats']:
                    for fmt_name in dict(combined_formats):
                        if fmt_name not in profile_cap['formats']:
                            del combined_formats[fmt_name]
                else:
                    combined_formats.clear()

            # Extensions
            if profile_cap['extensions']:
                if self.mode == 'union' or self.first:
                    for extension, spec_ver in profile_cap['extensions'].items():
                        combined_extensions[extension] = spec_ver
                elif self.mode == 'intersection':
                    for extension in list(combined_extensions):
                        if extension not in profile_cap['extensions']:
                            del combined_extensions[extension]

            # Features
            if profile_cap['features']:
                for feature_name, feat_members in profile_cap['features'].items():
                    self.add_struct(feature_name, feat_members, combined_features)

            # Properties
            if profile_cap['properties']:
                for property_name, prop_members in profile_cap['properties'].items():
                    if property_name in combined_properties:
                        self.add_members(combined_properties[property_name], prop_members, property_name)
                    else:
                        if self.mode == 'union' or self.first:
                            combined_properties[property_name] = dict()
                            self.add_members(combined_properties[property_name], prop_members, property_name)

            # Formats
            if profile_cap['formats']:
                for fmt_name in profile_cap['formats']:
                    if (fmt_name not in combined_formats) and (self.mode == 'union' or self.first):
                        combined_formats[fmt_name] = {
                            'VkFormatProperties': {},
                            'VkFormatProperties3': {},
                            'VkFormatProperties3KHR': {}
                        }

                    if fmt_name in combined_formats:
                        for prop_struct_name in ['VkFormatProperties', 'VkFormatProperties3', 'VkFormatProperties3KHR']:
                            for feat_cat in ['linearTilingFeatures', 'optimalTilingFeatures', 'bufferFeatures']:
                                self.combine_format_features(combined_formats, fmt_name, profile_cap, prop_struct_name, feat_cat)

            # Queue Families
            if profile_cap['queueFamiliesProperties']:
                if self.mode == 'intersection':
                    if self.first:
                        for qfp in profile_cap['queueFamiliesProperties']:
                            combined_qfp.append(qfp)
                    else:
                        for mqfp in list(combined_qfp):
                            found = False
                            for qfp in profile_cap['queueFamiliesProperties']:
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
                                combined_qfp.remove(mqfp)

                elif self.mode == 'union':
                    for qfp in profile_cap['queueFamiliesProperties']:
                        if not combined_qfp:
                            combined_qfp.append(qfp)
                        else:
                            for mqfp in combined_qfp:
                                if not self.compareList(mqfp['VkQueueFamilyProperties']['queueFlags'], qfp['VkQueueFamilyProperties']['queueFlags']):
                                    combined_qfp.append(qfp)
                                elif qfp['VkQueueFamilyProperties']['queueCount'] != mqfp['VkQueueFamilyProperties']['queueCount']:
                                    combined_qfp.append(qfp)
                                elif qfp['VkQueueFamilyProperties']['timestampValidBits'] != mqfp['VkQueueFamilyProperties']['timestampValidBits']:
                                    combined_qfp.append(qfp)
                                elif qfp['VkQueueFamilyProperties']['minImageTransferGranularity']['width'] != mqfp['VkQueueFamilyProperties']['minImageTransferGranularity']['width']:
                                    combined_qfp.append(qfp)
                                elif qfp['VkQueueFamilyProperties']['minImageTransferGranularity']['height'] != mqfp['VkQueueFamilyProperties']['minImageTransferGranularity']['height']:
                                    combined_qfp.append(qfp)
                                elif qfp['VkQueueFamilyProperties']['minImageTransferGranularity']['depth'] != mqfp['VkQueueFamilyProperties']['minImageTransferGranularity']['depth']:
                                    combined_qfp.append(qfp)

            # Video Profiles
            if profile_cap['videoProfiles']:
                if self.mode == 'intersection':
                    if self.first:
                        for video_profile in profile_cap['videoProfiles']:
                            combined_video_profiles.append(video_profile)
                    else:
                        def deep_compare(a, b):
                            if isinstance(a, list):
                                if isinstance(b, list) and len(a) == len(b):
                                    return all(deep_compare(a[idx], b[idx]) for idx in range(len(a)))
                                return False
                            elif isinstance(a, dict):
                                if isinstance(b, dict) and len(a.keys()) == len(b.keys()):
                                    return all(k in b and deep_compare(a[k], b[k]) for k in a.keys())
                                return False
                            else:
                                return a == b

                        for combined_video_profile in list(combined_video_profiles):
                            found = any(deep_compare(combined_video_profile, vp) for vp in profile_cap['videoProfiles'])
                            if not found:
                                combined_video_profiles.remove(combined_video_profile)

                elif self.mode == 'union':
                    for video_profile in profile_cap['videoProfiles']:
                        combined_video_profiles.append(video_profile)

        capabilities = dict()
        if combined_extensions:
            sorted_extensions = collections.OrderedDict(sorted(combined_extensions.items()))
            capabilities['extensions'] = dict(sorted_extensions)

        if combined_features:
            for feature in dict(combined_features):
                if not combined_features[feature]:
                    del combined_features[feature]

            sorted_features = collections.OrderedDict(sorted(combined_features.items()))
            capabilities['features'] = dict(sorted_features)

        if combined_properties:
            for prop in dict(combined_properties):
                if not combined_properties[prop]:
                    del combined_properties[prop]

            sorted_properties = collections.OrderedDict(sorted(combined_properties.items()))
            capabilities['properties'] = dict(sorted_properties)

        if combined_formats:
            sorted_formats = collections.OrderedDict(sorted(combined_formats.items()))
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

        if combined_qfp:
            capabilities['queueFamiliesProperties'] = combined_qfp

        if combined_video_profiles:
            capabilities['videoProfiles'] = combined_video_profiles

        return capabilities

    def compareList(self, l1, l2):
        return collections.Counter(l1) == collections.Counter(l2)

    def combine_format_features(self, combined_formats, fmt_name, capability, prop_name, features):
        if fmt_name in capability['formats'] and prop_name in capability['formats'][fmt_name]:
            if features in capability['formats'][fmt_name][prop_name]:
                if features not in combined_formats[fmt_name][prop_name]:
                    if self.mode == 'union' or self.first:
                        combined_formats[fmt_name][prop_name][features] = list(capability['formats'][fmt_name][prop_name][features])
                else:
                    if self.mode == 'union':
                        for feat in capability['formats'][fmt_name][prop_name][features]:
                            if feat not in combined_formats[fmt_name][prop_name][features]:
                                combined_formats[fmt_name][prop_name][features].append(feat)
                    else:
                        for feat in list(combined_formats[fmt_name][prop_name][features]):
                            if feat not in capability['formats'][fmt_name][prop_name][features]:
                                combined_formats[fmt_name][prop_name][features].remove(feat)

    def add_struct(self, struct_name, struct_data, combined):
        if struct_name in combined:
            if self.mode == 'union':
                for member, val in struct_data.items():
                    if member in combined[struct_name]:
                        combined[struct_name][member] = combined[struct_name][member] or val
                    else:
                        combined[struct_name][member] = val
            elif self.mode == 'intersection':
                if self.first:
                    for member, val in struct_data.items():
                        combined[struct_name][member] = val
                for member in list(combined[struct_name]):
                    if member not in struct_data:
                        del combined[struct_name][member]
                    elif struct_data[member] != combined[struct_name][member]:
                        del combined[struct_name][member]
        else:
            if self.mode == 'union' or self.first:
                combined[struct_name] = dict(struct_data)

    def add_members(self, combined, entry, property_name=None):
        struct_obj = getStructByName(self.vk.structs, property_name) if property_name else None
        count_to_array = self.get_count_to_array_map(struct_obj)

        if property_name and struct_obj:
            for member in list(combined):
                if property_name == 'VkPhysicalDeviceProperties' and member in ('limits', 'sparseProperties'):
                    continue
                mem_obj = getMemberByName(struct_obj, member)
                if mem_obj:
                    tokens = [t.strip() for t in mem_obj.limitType.split(',')] if mem_obj.limitType else []
                    is_ptr_array = self.is_pointer_array_member(mem_obj, combined[member])
                    is_count = member in count_to_array
                    if 'noauto' in tokens and not is_ptr_array and not is_count:
                        del combined[member]

        if self.mode == 'intersection' and not self.first:
            for member in list(combined):
                if property_name == 'VkPhysicalDeviceProperties' and member in ('limits', 'sparseProperties'):
                    continue
                if member not in entry:
                    del combined[member]

        for member, val in entry.items():
            if property_name == 'VkPhysicalDeviceProperties' and member == 'limits':
                if 'limits' not in combined:
                    if self.mode == 'union' or self.first:
                        combined['limits'] = dict()
                    else:
                        continue
                limits_struct_obj = self.vk.structs.get('VkPhysicalDeviceLimits')
                self.add_members_nested(combined['limits'], val, limits_struct_obj, 'VkPhysicalDeviceLimits')
                continue

            if property_name == 'VkPhysicalDeviceProperties' and member == 'sparseProperties':
                if 'sparseProperties' not in combined:
                    if self.mode == 'union' or self.first:
                        combined['sparseProperties'] = dict()
                    else:
                        continue
                sparse_struct_obj = self.vk.structs.get('VkPhysicalDeviceSparseProperties')
                self.add_members_nested(combined['sparseProperties'], val, sparse_struct_obj, 'VkPhysicalDeviceSparseProperties')
                continue

            xmlmember = getMemberByName(struct_obj, member) if struct_obj else None

            if member not in combined:
                if xmlmember:
                    tokens = [t.strip() for t in xmlmember.limitType.split(',')] if xmlmember.limitType else []
                    is_ptr_array = self.is_pointer_array_member(xmlmember, val)
                    is_count = member in count_to_array
                    if 'noauto' in tokens and not is_ptr_array and not is_count:
                        continue
                if self.mode == 'union' or self.first:
                    if xmlmember and xmlmember.type in ('uint64_t', 'VkDeviceSize'):
                        combined[member] = int(val)
                    else:
                        combined[member] = val
            else:
                limittype = xmlmember.limitType if xmlmember else None
                tokens = [t.strip() for t in limittype.split(',')] if limittype else []
                if 'struct' in tokens and xmlmember:
                    sub_struct_obj = getStructByName(self.vk.structs, xmlmember.type)
                    if sub_struct_obj:
                        for smember in sub_struct_obj.members:
                            sm_name = smember.name
                            if sm_name in combined[member]:
                                if sm_name in val:
                                    self.combine_members(combined[member], sm_name, val, smember, xmlmember.type)
                            elif (self.mode == 'union' or self.first) and (sm_name in val):
                                if smember.type in ('uint64_t', 'VkDeviceSize'):
                                    combined[member][sm_name] = int(val[sm_name])
                                else:
                                    combined[member][sm_name] = val[sm_name]
                        self.sync_array_count_members(combined[member], sub_struct_obj)
                elif xmlmember:
                    self.combine_members(combined, member, entry, xmlmember, property_name)

        if struct_obj:
            self.sync_array_count_members(combined, struct_obj)

    def add_members_nested(self, combined, entry, struct_obj, struct_name):
        count_to_array = self.get_count_to_array_map(struct_obj)

        if struct_obj:
            for member in list(combined):
                xmlmember = getMemberByName(struct_obj, member)
                if xmlmember:
                    tokens = [t.strip() for t in xmlmember.limitType.split(',')] if xmlmember.limitType else []
                    is_ptr_array = self.is_pointer_array_member(xmlmember, combined[member])
                    is_count = member in count_to_array
                    if 'noauto' in tokens and not is_ptr_array and not is_count:
                        del combined[member]

        if self.mode == 'intersection' and not self.first:
            for member in list(combined):
                if member not in entry:
                    del combined[member]

        for member, val in entry.items():
            xmlmember = getMemberByName(struct_obj, member) if struct_obj else None
            if member not in combined:
                if xmlmember:
                    tokens = [t.strip() for t in xmlmember.limitType.split(',')] if xmlmember.limitType else []
                    is_ptr_array = self.is_pointer_array_member(xmlmember, val)
                    is_count = member in count_to_array
                    if 'noauto' in tokens and not is_ptr_array and not is_count:
                        continue
                if self.mode == 'union' or self.first:
                    if xmlmember and xmlmember.type in ('uint64_t', 'VkDeviceSize'):
                        combined[member] = int(val)
                    else:
                        combined[member] = val
            else:
                if xmlmember:
                    self.combine_members(combined, member, entry, xmlmember, struct_name)

        if struct_obj:
            self.sync_array_count_members(combined, struct_obj)

    def _combine_min_pot_intersection(self, val1, val2):
        try:
            i1 = int(val1)
            i2 = int(val2)
            if i1 > 0 and (i1 & (i1 - 1)) == 0 and i2 > 0 and (i2 & (i2 - 1)) == 0:
                return max(i1, i2)
        except (ValueError, TypeError):
            pass
        return None

    def _combine_min_mul_intersection(self, val1, val2, is_float=False):
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

    def _combine_min_pot_union(self, val1, val2):
        try:
            i1 = int(val1)
            i2 = int(val2)
            if i1 > 0 and (i1 & (i1 - 1)) == 0 and i2 > 0 and (i2 & (i2 - 1)) == 0:
                return min(i1, i2)
        except (ValueError, TypeError):
            pass
        return None

    def _combine_min_mul_union(self, val1, val2, is_float=False):
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

    def combine_members(self, combined, member, entry, xmlmember, property_name=None):
        limittype = xmlmember.limitType if xmlmember else None
        tokens = [t.strip() for t in limittype.split(',')] if limittype else []

        is_list = isinstance(combined[member], list) or (member in entry and isinstance(entry[member], list))
        is_ptr_array = self.is_pointer_array_member(xmlmember, combined[member] if member in combined else entry.get(member))

        if 'noauto' in tokens and not is_ptr_array:
            del combined[member]
            return

        if 'exact' in tokens and not is_ptr_array:
            if xmlmember and xmlmember.type in ('uint64_t', 'VkDeviceSize'):
                if int(combined[member]) != int(entry[member]):
                    del combined[member]
            elif xmlmember and xmlmember.type == 'float':
                if not math.isclose(float(combined[member]), float(entry[member]), abs_tol=1e-5):
                    del combined[member]
            else:
                if combined[member] != entry[member]:
                    del combined[member]
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
                    combined[member]['width'] = max(combined[member]['width'], entry[member]['width'])
                    combined[member]['height'] = max(combined[member]['height'], entry[member]['height'])
                elif xmlmember.type == 'VkExtent3D':
                    combined[member]['width'] = max(combined[member]['width'], entry[member]['width'])
                    combined[member]['height'] = max(combined[member]['height'], entry[member]['height'])
                    combined[member]['depth'] = max(combined[member]['depth'], entry[member]['depth'])
                elif xmlmember.type == 'VkBool32':
                    combined[member] = combined[member] or entry[member]
                elif isinstance(combined[member], list) and isinstance(entry[member], list):
                    length = min(len(combined[member]), len(entry[member]))
                    for idx in range(length):
                        combined[member][idx] = max(combined[member][idx], entry[member][idx])
                else:
                    combined[member] = max(combined[member], entry[member])
            elif is_min:
                if xmlmember.type == 'VkExtent2D':
                    if is_pot:
                        w = self._combine_min_pot_union(combined[member]['width'], entry[member]['width'])
                        h = self._combine_min_pot_union(combined[member]['height'], entry[member]['height'])
                        if w is not None and h is not None:
                            combined[member]['width'], combined[member]['height'] = w, h
                        else:
                            del combined[member]
                    elif is_mul:
                        w = self._combine_min_mul_union(combined[member]['width'], entry[member]['width'])
                        h = self._combine_min_mul_union(combined[member]['height'], entry[member]['height'])
                        if w is not None and h is not None:
                            combined[member]['width'], combined[member]['height'] = w, h
                        else:
                            del combined[member]
                    else:
                        combined[member]['width'] = min(combined[member]['width'], entry[member]['width'])
                        combined[member]['height'] = min(combined[member]['height'], entry[member]['height'])
                elif xmlmember.type == 'VkExtent3D':
                    if is_pot:
                        w = self._combine_min_pot_union(combined[member]['width'], entry[member]['width'])
                        h = self._combine_min_pot_union(combined[member]['height'], entry[member]['height'])
                        d = self._combine_min_pot_union(combined[member]['depth'], entry[member]['depth'])
                        if w is not None and h is not None and d is not None:
                            combined[member]['width'], combined[member]['height'], combined[member]['depth'] = w, h, d
                        else:
                            del combined[member]
                    elif is_mul:
                        w = self._combine_min_mul_union(combined[member]['width'], entry[member]['width'])
                        h = self._combine_min_mul_union(combined[member]['height'], entry[member]['height'])
                        d = self._combine_min_mul_union(combined[member]['depth'], entry[member]['depth'])
                        if w is not None and h is not None and d is not None:
                            combined[member]['width'], combined[member]['height'], combined[member]['depth'] = w, h, d
                        else:
                            del combined[member]
                    else:
                        combined[member]['width'] = min(combined[member]['width'], entry[member]['width'])
                        combined[member]['height'] = min(combined[member]['height'], entry[member]['height'])
                        combined[member]['depth'] = min(combined[member]['depth'], entry[member]['depth'])
                elif xmlmember.type == 'VkBool32':
                    combined[member] = combined[member] and entry[member]
                elif isinstance(combined[member], list) and isinstance(entry[member], list):
                    res_arr = []
                    valid = True
                    for idx in range(min(len(combined[member]), len(entry[member]))):
                        if is_pot:
                            res = self._combine_min_pot_union(combined[member][idx], entry[member][idx])
                        elif is_mul:
                            is_float = (xmlmember.type == 'float') or isinstance(combined[member][idx], float)
                            res = self._combine_min_mul_union(combined[member][idx], entry[member][idx], is_float)
                        else:
                            res = min(combined[member][idx], entry[member][idx])
                        if res is None:
                            valid = False
                            break
                        res_arr.append(res)
                    if valid:
                        combined[member] = res_arr
                    else:
                        del combined[member]
                else:
                    if is_pot:
                        res = self._combine_min_pot_union(combined[member], entry[member])
                        if res is not None:
                            combined[member] = res
                        else:
                            del combined[member]
                    elif is_mul:
                        is_float = (xmlmember.type == 'float') or isinstance(combined[member], float) or isinstance(entry[member], float)
                        res = self._combine_min_mul_union(combined[member], entry[member], is_float)
                        if res is not None:
                            combined[member] = res
                        else:
                            del combined[member]
                    else:
                        combined[member] = min(combined[member], entry[member])
            elif is_bitmask:
                if isinstance(combined[member], list) and isinstance(entry[member], list):
                    for smember in entry[member]:
                        if smember not in combined[member]:
                            combined[member].append(smember)
                elif not isinstance(combined[member], list) and not isinstance(entry[member], list):
                    combined[member] = int(combined[member]) | int(entry[member])
            elif is_range:
                combined[member][0] = min(combined[member][0], entry[member][0])
                combined[member][1] = max(combined[member][1], entry[member][1])

        elif self.mode == 'intersection':
            if is_max:
                if xmlmember.type == 'VkExtent2D':
                    combined[member]['width'] = min(combined[member]['width'], entry[member]['width'])
                    combined[member]['height'] = min(combined[member]['height'], entry[member]['height'])
                elif xmlmember.type == 'VkExtent3D':
                    combined[member]['width'] = min(combined[member]['width'], entry[member]['width'])
                    combined[member]['height'] = min(combined[member]['height'], entry[member]['height'])
                    combined[member]['depth'] = min(combined[member]['depth'], entry[member]['depth'])
                elif xmlmember.type == 'VkBool32':
                    combined[member] = combined[member] and entry[member]
                elif isinstance(combined[member], list) and isinstance(entry[member], list):
                    length = min(len(combined[member]), len(entry[member]))
                    for idx in range(length):
                        combined[member][idx] = min(combined[member][idx], entry[member][idx])
                elif xmlmember.type in ('uint64_t', 'VkDeviceSize'):
                    combined[member] = min(int(entry[member]), int(combined[member]))
                elif xmlmember.type == 'float':
                    combined[member] = min(float(entry[member]), float(combined[member]))
                else:
                    combined[member] = min(combined[member], entry[member])
            elif is_min:
                if xmlmember.type == 'VkExtent2D':
                    if is_pot:
                        w = self._combine_min_pot_intersection(combined[member]['width'], entry[member]['width'])
                        h = self._combine_min_pot_intersection(combined[member]['height'], entry[member]['height'])
                        if w is not None and h is not None:
                            combined[member]['width'], combined[member]['height'] = w, h
                        else:
                            del combined[member]
                    elif is_mul:
                        w = self._combine_min_mul_intersection(combined[member]['width'], entry[member]['width'])
                        h = self._combine_min_mul_intersection(combined[member]['height'], entry[member]['height'])
                        if w is not None and h is not None:
                            combined[member]['width'], combined[member]['height'] = w, h
                        else:
                            del combined[member]
                    else:
                        combined[member]['width'] = max(combined[member]['width'], entry[member]['width'])
                        combined[member]['height'] = max(combined[member]['height'], entry[member]['height'])
                elif xmlmember.type == 'VkExtent3D':
                    if is_pot:
                        w = self._combine_min_pot_intersection(combined[member]['width'], entry[member]['width'])
                        h = self._combine_min_pot_intersection(combined[member]['height'], entry[member]['height'])
                        d = self._combine_min_pot_intersection(combined[member]['depth'], entry[member]['depth'])
                        if w is not None and h is not None and d is not None:
                            combined[member]['width'], combined[member]['height'], combined[member]['depth'] = w, h, d
                        else:
                            del combined[member]
                    elif is_mul:
                        w = self._combine_min_mul_intersection(combined[member]['width'], entry[member]['width'])
                        h = self._combine_min_mul_intersection(combined[member]['height'], entry[member]['height'])
                        d = self._combine_min_mul_intersection(combined[member]['depth'], entry[member]['depth'])
                        if w is not None and h is not None and d is not None:
                            combined[member]['width'], combined[member]['height'], combined[member]['depth'] = w, h, d
                        else:
                            del combined[member]
                    else:
                        combined[member]['width'] = max(combined[member]['width'], entry[member]['width'])
                        combined[member]['height'] = max(combined[member]['height'], entry[member]['height'])
                        combined[member]['depth'] = max(combined[member]['depth'], entry[member]['depth'])
                elif xmlmember.type == 'VkBool32':
                    combined[member] = combined[member] or entry[member]
                elif isinstance(combined[member], list) and isinstance(entry[member], list):
                    res_arr = []
                    valid = True
                    for idx in range(min(len(combined[member]), len(entry[member]))):
                        if is_pot:
                            res = self._combine_min_pot_intersection(combined[member][idx], entry[member][idx])
                        elif is_mul:
                            is_float = (xmlmember.type == 'float') or isinstance(combined[member][idx], float)
                            res = self._combine_min_mul_intersection(combined[member][idx], entry[member][idx], is_float)
                        else:
                            res = max(combined[member][idx], entry[member][idx])
                        if res is None:
                            valid = False
                            break
                        res_arr.append(res)
                    if valid:
                        combined[member] = res_arr
                    else:
                        del combined[member]
                else:
                    if is_pot:
                        res = self._combine_min_pot_intersection(combined[member], entry[member])
                        if res is not None:
                            combined[member] = res
                        else:
                            del combined[member]
                    elif is_mul:
                        is_float = (xmlmember.type == 'float') or isinstance(combined[member], float) or isinstance(entry[member], float)
                        res = self._combine_min_mul_intersection(combined[member], entry[member], is_float)
                        if res is not None:
                            combined[member] = res
                        else:
                            del combined[member]
                    else:
                        if xmlmember.type in ('uint64_t', 'VkDeviceSize'):
                            combined[member] = max(int(entry[member]), int(combined[member]))
                        elif xmlmember.type == 'float':
                            combined[member] = max(float(entry[member]), float(combined[member]))
                        else:
                            combined[member] = max(combined[member], entry[member])
            elif is_bitmask:
                if isinstance(combined[member], list) and isinstance(entry[member], list):
                    remove_list = []
                    for value in combined[member]:
                        if value not in entry[member]:
                            remove_list.append(value)
                    for value in remove_list:
                        combined[member].remove(value)
                elif not isinstance(combined[member], list) and not isinstance(entry[member], list):
                    combined[member] = int(combined[member]) & int(entry[member])
            elif is_range:
                combined[member][0] = max(combined[member][0], entry[member][0])
                combined[member][1] = min(combined[member][1], entry[member][1])

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
    