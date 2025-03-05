#!/usr/bin/python3
#
# Copyright (c) 2022-2024 LunarG, Inc.
# Copyright (c) 2024 RasterGrid Kft.
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
# - Ziga Markus <ziga@lunarg.com>
# - Christophe Riccio <christophe@lunarg.com>

from datetime import datetime
import argparse
import json
import gen_profiles_solution
import re
import os
import collections

class ProfileFile():
    def __init__(self):
        self.json_output = dict()
        self.json_output['$schema'] = 'https://schema.khronos.org/vulkan/profiles-0.8-latest.json#'
        self.json_output['capabilities'] = dict()
        self.json_output['profiles'] = dict()
        self.json_output['contributors'] = dict()
        self.json_output['history'] = list()

        # Get current time
        now = datetime.now()

        revision = dict()
        revision['revision'] = 1
        revision['date'] = str(now.year) + '-' + str(now.month).zfill(2) + '-' + str(now.day).zfill(2)
        revision['author'] = 'LunarG Profiles Merge Script'
        revision['comment'] = 'Generated profiles file'
        self.json_output['history'].append(revision)

    def set_schema(self, schema):
        self.json_output['$schema'] = "https://schema.khronos.org/vulkan/{0}.json#".format(schema)

    def set_contributors(self, contributors):
        self.json_output['contributors'] = contributors

    def set_history(self, history):
        self.json_output['history'] = history

    def add_capabilities(self, json_capabilities_key, json_capabilities_value):
        self.json_output['capabilities'][json_capabilities_key] = json_capabilities_value

    def add_profile(self, json_profile_key, json_profile_value):
        self.json_output['profiles'][json_profile_key] = json_profile_value

    def dump(self, path):
        # Wite new merged profile
        with open(path, 'w') as file:
            json.dump(self.json_output, file, indent = 4)

class ProfileConfig():
    def __init__(self, input_dir, input_profile_names, profile_api_version, merge_mode):
        self.merge_mode = merge_mode
        
        self.input_paths = list()
        self.input_jsons = list()
        self.input_profile_values = list()
        self.input_profile_names = input_profile_names

        self.load_jsons(input_dir)

        self.name = "VP_LUNARG_generated_profile"
        self.version = 1
        self.label = 'Generated profile'
        self.description = self.get_profile_description(self.input_profile_names, merge_mode)
        self.stage = "STABLE"

        # Get current time
        now = datetime.now()
        self.date = str(now.year) + '-' + str(now.month).zfill(2) + '-' + str(now.day).zfill(2)
        
        # Find the api version to use
        if profile_api_version is not None:
            self.api_version = self.get_api_version_list(profile_api_version)
        else:
            self.api_version = self.get_api_version(self.input_profile_values, merge_mode)

        self.required_profiles = list()

    def apply_json_value(self, profile_name, json_profile_value):
        self.name = profile_name
        self.version = json_profile_value["version"]
        self.label = json_profile_value["label"]
        self.description = json_profile_value["description"]
        self.date = json_profile_value["date"]
        self.stage = json_profile_value["stage"]
        if json_profile_value["api-version"]:
            self.api_version = self.get_api_version_list(json_profile_value["api-version"])
        else:
            self.api_version = self.get_api_version(self.input_profile_values, self.merge_mode)
        if json_profile_value["required-profiles"]:
            self.required_profiles = json_profile_value["required-profiles"].split(',')

    def load_jsons(self, input_dir):
        if input_dir is not None:
            profiles_not_found = self.input_profile_names.copy()
            # Find all jsons in the folder
            paths = [input_dir + '/' + pos_json for pos_json in os.listdir(input_dir) if pos_json.endswith('.json')]
            json_files = list()
            for i in range(len(paths)):
                print('Opening: ' + paths[i])
                file = open(paths[i], "r")
                json_files.append(json.load(file))
            # We need to iterate through profile names first, so the indices of jsons and profiles lists will match
            if (len(input_profile_names) > 0):
                for profile_name in input_profile_names:
                    for json_file in json_files:
                        if 'profiles' in json_file and profile_name in json_file['profiles']:
                            self.input_jsons.append(json_file)
                            # Select profiles and capabilities
                            self.input_profile_values.append(json_file['profiles'][profile_name])
                            profiles_not_found.remove(profile_name)
                            break
                if profiles_not_found:
                    print('Profiles: ' + ' '.join(profiles_not_found) + ' not found in directory ' + args.input)
                    exit()
            else:
                for json_file in json_files:
                    if 'profiles' in json_file:
                        for profile in json_file['profiles']:
                            self.input_jsons.append(json_file)
                            self.input_profile_values.append(json_file['profiles'][profile])
                            self.input_profile_names.append(profile)
        else:
            print('ERROR: Not input directory set, use --input')
            exit()

    def get_api_version(self, profiles, merge_mode):
        api_version = self.get_api_version_list(profiles[0]['api-version'])
        for profile in profiles:
            current_api_version = self.get_api_version_list(profile['api-version'])
            if merge_mode == 'union':
                for i in range(len(api_version)):
                    if (api_version[i] > current_api_version[i]):
                        break
                    elif (api_version[i] < current_api_version[i]):
                        api_version = current_api_version
                        break
            elif merge_mode == 'intersection':
                for i in range(len(api_version)):
                    if (api_version[i] < current_api_version[i]):
                        break
                    elif (api_version[i] > current_api_version[i]):
                        api_version = current_api_version
                        break
            else:
                print('ERROR: Unknown mode when computing api-version')
        return api_version

    def get_api_version_list(self, ver):
        version = ver.split('.')
        return version

    def get_profile_description(self, profile_names, mode):
        desc = 'Generated profile doing an ' + mode + ' between profiles: '
        count = len(profile_names)
        for i in range(count):
            desc += profile_names[i]
            if i == count - 2:
                desc += ' and '
            elif i < count - 2:
                desc += ', '
        return desc

class ProfileMerger():
    def __init__(self, registry):
        self.registry = registry

    def merge(self, profile_config, profile_file, mode, strip_duplicate_struct):
        self.mode = mode

        print('Building a Vulkan ' + '.'.join(profile_config.api_version) + ' profile')

        capabilities_key = profile_config.name + "_block"

        profile_file.add_capabilities(
            capabilities_key,
            self.merge_capabilities(profile_config.input_jsons, profile_config.input_profile_names, profile_config.api_version, strip_duplicate_struct))

        profile_file.add_profile(
            profile_config.name,
            self.get_profile(profile_config, capabilities_key))

    def merge_capabilities(self, jsons, profile_names, api_version, strip_duplicate_struct):
        merged_extensions = dict()
        merged_features = dict()
        merged_properties = dict()
        merged_formats = dict()
        merged_qfp = list()
        merged_video_profiles = list()

        for i in range(len(jsons)):
            self.first = i == 0
            for capability_name in jsons[i]['profiles'][profile_names[i]]['capabilities']:
                capability = jsons[i]['capabilities'][capability_name]

                # Removed feature and properties not in the current json from already merged dicts
                if self.mode == 'intersection' and self.first is False:
                    if 'features' in capability:
                        for feature in dict(merged_features):
                            if feature not in capability['features']:
                                del merged_features[feature]
                    else:
                        merged_features.clear()

                    if 'properties' in capability:
                        for property in dict(merged_properties):
                            if property not in capability['properties']:
                                del merged_properties[property]
                    else:
                        merged_properties.clear()

                if 'extensions' in capability:
                    if self.mode == 'union' or self.first:
                        for extension in capability['extensions']:
                            # vk_version = self.get_promoted_version(self.registry.extensions[extension].promotedTo)
                            # Check if the extension was not promoted in the version used
                            # if vk_version is None or (vk_version[0] > api_version[0]) or (vk_version[0] == api_version[0] and vk_version[1] > api_version[1]):
                            merged_extensions[extension] = capability['extensions'][extension]
                    elif self.mode == 'intersection':
                        for extension in list(merged_extensions):
                            if not extension in capability['extensions']:
                                del merged_extensions[extension]
                    else:
                        print("ERROR: Unknown combination mode: " + self.mode)
                        
                if 'features' in capability:
                    for feature in capability['features']:
                        # Feature already exists, add or overwrite members
                        if feature in merged_features:
                            self.add_struct(feature, capability['features'][feature], merged_features)
                        else:
                            written = False
                            # Check if the promoted struct of current feature was already added
                            promoted_struct = self.get_promoted_struct_name(feature, True)
                            if promoted_struct and promoted_struct in merged_features:
                                self.add_members(merged_features[promoted_struct], capability['features'][feature])
                                written = True
                            # If this is a promoted struct, check if any structs already exists which are extension struct that are promoted to this struct
                            elif promoted_struct == feature:
                                # Add this structure
                                self.add_struct(feature, capability['features'][feature], merged_features)
                                # Combine all other extension structures (which are promoted to this version) into this structure
                                self.promote_structs(feature, merged_features, True)
                                written = True
                            if not written:
                                aliases = self.registry.structs[feature].aliases
                                for alias in aliases:
                                    if alias in merged_features:
                                        # Alias is already set, find which one to use
                                        struct = self.find_higher_struct(feature, alias)
                                        if struct == feature:
                                            merged_features[feature] = merged_features.pop(alias)
                                            self.add_members(merged_features[feature], capability['features'][feature])
                                        if struct == alias:
                                            self.add_members(merged_features[alias], capability['features'][feature])
                                        written = True
                                        break
                            if not written:
                                self.add_struct(feature, capability['features'][feature], merged_features)

                if 'properties' in capability:
                    for property in capability['properties']:
                        # Property already exists, add or overwrite members
                        if property in merged_properties:
                            self.add_members(merged_properties[property], capability['properties'][property], property)
                        else:
                            # Check if the promoted struct of current property was already added
                            promoted_struct = self.get_promoted_struct_name(property, True)
                            if promoted_struct and promoted_struct in merged_properties:
                                self.add_members(merged_properties[promoted_struct], capability['properties'][property])
                            # If this is a promoted struct, check if any structs already exists which are extension struct that are promoted to this struct
                            elif promoted_struct == property:
                                # Add this structure
                                self.add_struct(property, capability['properties'][property], merged_properties)
                                # Combine all other extension structures (which are promoted to this version) into this structure
                                self.promote_structs(feature, merged_properties, True)
                            else:
                                aliases = self.registry.structs[property].aliases
                                for alias in aliases:
                                    if alias in merged_properties:
                                        # Alias is already set, find which one to use
                                        struct = self.find_higher_struct(property, alias)
                                        if struct == property:
                                            merged_properties[property] = merged_properties.pop(alias)
                                            self.add_members(merged_properties[property], capability['properties'][property])
                                        if struct == alias:
                                            self.add_members(merged_properties[alias], capability['properties'][property])
                                        break
                                self.add_struct(property, capability['properties'][property], merged_properties)

                if 'formats' in capability:
                    for format in capability['formats']:
                        if (format not in merged_formats) and (self.mode == 'union' or self.first):
                            merged_formats[format] = dict()
                            merged_formats[format]['VkFormatProperties'] = dict()
                            merged_formats[format]['VkFormatProperties3'] = dict()
                            merged_formats[format]['VkFormatProperties3KHR'] = dict()

                        if (format in merged_formats):
                            for prop_name in ['VkFormatProperties', 'VkFormatProperties3', 'VkFormatProperties3KHR']:
                                for features in ['linearTilingFeatures', 'optimalTilingFeatures', 'bufferFeatures']:
                                    self.merge_format_features(merged_formats, format, capability, prop_name, features)

                            # Remove empty entries (can occur when using intersect)
                            #if not dict(merged_formats[format]['VkFormatProperties']) and not dict(merged_formats[format]['VkFormatProperties3']) and not dict(merged_formats[format]['VkFormatProperties3KHR']):
                            #    del merged_formats[format]

                if 'queueFamiliesProperties' in capability:
                    if self.mode == 'intersection':
                        # If this is the first json just append all queue family properties
                        if self.first:
                            for qfp in capability['queueFamiliesProperties']:
                                merged_qfp.append(qfp)
                        # Otherwise do an intersect
                        else:
                            for mqfp in list(merged_qfp):
                                found = False
                                #if self.compareList(mqfp['VkQueueFamilyProperties']['queueFlags'], qfp['VkQueueFamilyProperties']['queueFlags']):
                                #    found = True
                                #    if (qfp['VkQueueFamilyProperties']['queueCount'] < mqfp['VkQueueFamilyProperties']['queueCount']):
                                #        mqfp['VkQueueFamilyProperties']['queueCount'] = qfp['VkQueueFamilyProperties']['queueCount']
                                #    if (qfp['VkQueueFamilyProperties']['timestampValidBits'] < mqfp['VkQueueFamilyProperties']['timestampValidBits']):
                                #        mqfp['VkQueueFamilyProperties']['timestampValidBits'] = qfp['VkQueueFamilyProperties']['timestampValidBits']
                                #    if (qfp['VkQueueFamilyProperties']['minImageTransferGranularity']['width'] > mqfp['VkQueueFamilyProperties']['minImageTransferGranularity']['width']):
                                #        mqfp['VkQueueFamilyProperties']['minImageTransferGranularity']['width'] = qfp['VkQueueFamilyProperties']['minImageTransferGranularity']['width']
                                #    if (qfp['VkQueueFamilyProperties']['minImageTransferGranularity']['height'] > mqfp['VkQueueFamilyProperties']['minImageTransferGranularity']['height']):
                                #        mqfp['VkQueueFamilyProperties']['minImageTransferGranularity']['height'] = qfp['VkQueueFamilyProperties']['minImageTransferGranularity']['height']
                                #    if (qfp['VkQueueFamilyProperties']['minImageTransferGranularity']['depth'] > mqfp['VkQueueFamilyProperties']['minImageTransferGranularity']['depth']):
                                #        mqfp['VkQueueFamilyProperties']['minImageTransferGranularity']['depth'] = qfp['VkQueueFamilyProperties']['minImageTransferGranularity']['depth']
                                for qfp in capability['queueFamiliesProperties']:
                                    if mqfp['VkQueueFamilyProperties']['queueFlags'] != qfp['VkQueueFamilyProperties']['queueFlags']:
                                        continue
                                    if (qfp['VkQueueFamilyProperties']['queueCount'] != mqfp['VkQueueFamilyProperties']['queueCount']):
                                        continue
                                    if (qfp['VkQueueFamilyProperties']['timestampValidBits'] != mqfp['VkQueueFamilyProperties']['timestampValidBits']):
                                        continue
                                    if (qfp['VkQueueFamilyProperties']['minImageTransferGranularity']['width'] != mqfp['VkQueueFamilyProperties']['minImageTransferGranularity']['width']):
                                        continue
                                    if (qfp['VkQueueFamilyProperties']['minImageTransferGranularity']['height'] != mqfp['VkQueueFamilyProperties']['minImageTransferGranularity']['height']):
                                        continue
                                    if (qfp['VkQueueFamilyProperties']['minImageTransferGranularity']['depth'] != mqfp['VkQueueFamilyProperties']['minImageTransferGranularity']['depth']):
                                        continue
                                    found = True
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

                    else:
                        print("ERROR: Unknown combination mode: " + self.mode)

                if 'videoProfiles' in capability:
                    if self.mode == 'intersection':
                        # If this is the first json just append all video profiles
                        if self.first:
                            for video_profile in capability['videoProfiles']:
                                merged_video_profiles.append(video_profile)
                        # Otherwise do an intersect
                        else:
                            def deep_compare(a, b):
                                if isinstance(a, list):
                                    if isinstance(b, list) and len(a) == len(b):
                                        for i in range(len(a)):
                                            if not deep_compare(a[i], b[i]):
                                                return False
                                        return True
                                    else:
                                        return False
                                elif isinstance(a, dict):
                                    if isinstance(b, dict) and len(a.keys()) == len(b.keys()):
                                        for key in a.keys():
                                            if not key in b or not deep_compare(a[key], b[key]):
                                                return False
                                        return True
                                    else:
                                        return False
                                else:
                                    return a == b

                            for merged_video_profile in list(merged_video_profiles):
                                found = False
                                for video_profile in capability['videoProfiles']:
                                    if deep_compare(merged_video_profile, video_profile):
                                        found = True
                                        break
                                if not found:
                                    merged_video_profiles.remove(merged_video_profile)

                    elif self.mode == 'union':
                        for video_profile in capability['videoProfiles']:
                            merged_video_profiles.append(video_profile)

                    else:
                        print("ERROR: Unknown combination mode: " + self.mode)

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
            for property in dict(merged_properties):
                if not merged_properties[property]:
                    del merged_properties[property]

            sorted_properties = collections.OrderedDict(sorted(merged_properties.items()))

            if strip_duplicate_struct:
                if 'VkPhysicalDeviceVulkan11Properties' in sorted_properties:
                    if 'VkPhysicalDeviceIDPropertiesKHR' in sorted_properties:
                        del sorted_properties['VkPhysicalDeviceIDPropertiesKHR']
                    if 'VkPhysicalDeviceSubgroupProperties' in sorted_properties:
                        del sorted_properties['VkPhysicalDeviceSubgroupProperties']
                    if 'VkPhysicalDevicePointClippingPropertiesKHR' in sorted_properties:
                        del sorted_properties['VkPhysicalDevicePointClippingPropertiesKHR']
                    if 'VkPhysicalDeviceMultiviewPropertiesKHR' in sorted_properties:
                        del sorted_properties['VkPhysicalDeviceMultiviewPropertiesKHR']
                    if 'VkPhysicalDeviceProtectedMemoryProperties' in sorted_properties:
                        del sorted_properties['VkPhysicalDeviceProtectedMemoryProperties']
                    if 'VkPhysicalDeviceMaintenance3PropertiesKHR' in sorted_properties:
                        del sorted_properties['VkPhysicalDeviceMaintenance3PropertiesKHR']

                if 'VkPhysicalDeviceVulkan12Properties' in sorted_properties:
                    if 'VkPhysicalDeviceDriverPropertiesKHR' in sorted_properties:
                        del sorted_properties['VkPhysicalDeviceDriverPropertiesKHR']
                    if 'VkPhysicalDeviceFloatControlsPropertiesKHR ' in sorted_properties:
                        del sorted_properties['VkPhysicalDeviceFloatControlsPropertiesKHR ']
                    if 'VkPhysicalDeviceDescriptorIndexingPropertiesEXT' in sorted_properties:
                        del sorted_properties['VkPhysicalDeviceDescriptorIndexingPropertiesEXT']
                    if 'VkPhysicalDeviceDepthStencilResolvePropertiesKHR' in sorted_properties:
                        del sorted_properties['VkPhysicalDeviceDepthStencilResolvePropertiesKHR']
                    if 'VkPhysicalDeviceSamplerFilterMinmaxPropertiesEXT' in sorted_properties:
                        del sorted_properties['VkPhysicalDeviceSamplerFilterMinmaxPropertiesEXT']
                    if 'VkPhysicalDeviceTimelineSemaphorePropertiesKHR' in sorted_properties:
                        del sorted_properties['VkPhysicalDeviceTimelineSemaphorePropertiesKHR']

                if 'VkPhysicalDeviceVulkan13Properties' in sorted_properties:
                    if 'VkPhysicalDeviceInlineUniformBlockPropertiesEXT' in sorted_properties:
                        del sorted_properties['VkPhysicalDeviceInlineUniformBlockPropertiesEXT']
                    if 'VkPhysicalDeviceSubgroupSizeControlPropertiesEXT' in sorted_properties:
                        del sorted_properties['VkPhysicalDeviceSubgroupSizeControlPropertiesEXT']

            capabilities['properties'] = dict(sorted_properties)

        if merged_formats:
            sorted_formats = collections.OrderedDict(sorted(merged_formats.items()))
            capabilities['formats'] = dict(sorted_formats)

            # remove all empty elements
            formatsToRemove = list()

            for format in capabilities['formats']:
                for prop_name in ['VkFormatProperties', 'VkFormatProperties3', 'VkFormatProperties3KHR']:
                    for features in ['linearTilingFeatures', 'optimalTilingFeatures', 'bufferFeatures']:
                        if features in capabilities['formats'][format][prop_name]:
                            if not capabilities['formats'][format][prop_name][features]:
                                del capabilities['formats'][format][prop_name][features]
                    if prop_name in capabilities['formats'][format]:
                        if not capabilities['formats'][format][prop_name]:
                            del capabilities['formats'][format][prop_name]
                if not capabilities['formats'][format]:
                    formatsToRemove.append(format)

            for format in formatsToRemove:
                del capabilities['formats'][format]

        if merged_qfp:
            capabilities['queueFamiliesProperties'] = merged_qfp

        if merged_video_profiles:
            capabilities['videoProfiles'] = merged_video_profiles

        return capabilities

    def compareList(self, l1, l2):
        return collections.Counter(l1) == collections.Counter(l2)

    def merge_format_features(self, merged_formats, format, capability, prop_name, features):
        # Remove all format features not in current json if intersect is used
        if self.mode == 'intersection' and self.first is False:
            for mformat in dict(merged_formats):
                if mformat not in capability['formats']:
                    del merged_formats[mformat]

            # Remove format features not in intersect
            for feature in list(merged_formats[format][prop_name]):
                if prop_name not in capability['formats'][format] or feature not in capability['formats'][format][prop_name]:
                    del merged_formats[format][prop_name][feature]

        # Iterate all format features in current json
        if prop_name in capability['formats'][format]:
            if features in capability['formats'][format][prop_name]:
                # If mode is union or this is the first json when using intersect add the features if not already in merged features
                if features not in merged_formats[format][prop_name]:
                    if self.mode == 'union' or self.first == True:
                        merged_formats[format][prop_name][features] = capability['formats'][format][prop_name][features]
                else:
                    # In union add all aditional features
                    if self.mode == 'union':
                        for feature in capability['formats'][format][prop_name][features]:
                            if feature not in merged_formats[format][prop_name][features]:
                                merged_formats[format][prop_name][features].append(feature)
                    # In intersect removed features which are not set in the current json
                    else:
                        for feature in list(merged_formats[format][prop_name][features]):
                            if feature not in capability['formats'][format][prop_name][features]:
                                merged_formats[format][prop_name][features].remove(feature)

    def promote_structs(self, promoted, merged, feature):
        for struct in dict(merged):
            if self.get_promoted_struct_name(struct, feature) == promoted and struct is not promoted:
                # Union
                if self.mode == 'union':
                    for member in merged[struct]:
                        merged[promoted][member] = merged[struct][member]
                # Intersect
                #elif self.mode == 'intersection':
                #    if promoted in merged:
                #        for member in list(merged[promoted]):
                #            if member not in merged[struct]:
                #                del merged[promoted][member]
                #else:
                #    print("ERROR: Unknown combination mode: " + self.mode)
                #del merged[struct]


    def get_promoted_struct_name(self, struct, feature):
        # Workaround, because Vulkan11 structs were added in vulkan 1.2
        if struct == 'VkPhysicalDeviceFeatures':
            return 'VkPhysicalDeviceFeatures'
        if struct == 'VkPhysicalDeviceProperties':
            return 'VkPhysicalDeviceProperties'
        if struct == 'VkPhysicalDeviceVulkan11Features':
            return 'VkPhysicalDeviceVulkan11Features'
        if struct == 'VkPhysicalDeviceVulkan11Properties':
            return 'VkPhysicalDeviceVulkan11Properties'

        version = None
        if self.registry.structs[struct].definedByVersion:
            version = self.registry.structs[struct].definedByVersion
        else:
            aliases = self.registry.structs[struct].aliases
            for alias in aliases:
                if registry.structs[alias].definedByVersion:
                    version = registry.structs[alias].definedByVersion
                    break
        if version is None:
            return False
        return 'VkPhysicalDeviceVulkan' + str(version.major) + str(version.minor) + 'Features' if feature else 'Properties'

    def add_struct(self, struct_name, struct, merged):
        if struct_name in merged:
            # Union
            if self.mode == 'union':
                for member in struct:
                    if member in merged[struct_name]:
                        merged[struct_name][member] = merged[struct_name][member] or struct[member]
                    else:
                        merged[struct_name][member] = struct[member]
            # Intersect
            elif self.mode == 'intersection':
                if self.first is True:
                    for member in struct:
                        merged[struct_name][member] = struct[member]
                for member in list(merged[struct_name]):
                    if member not in struct:
                        del merged[struct_name][member]
                    elif struct[member] != merged[struct_name][member]:
                        del merged[struct_name][member]
            else:
                print("ERROR: Unknown combination mode: " + self.mode)
        else:
            if self.mode == 'union' or self.first is True:
                merged[struct_name] = struct

    def add_members(self, merged, entry, property = None):
        # First, remove all noauto member, they can't be merged
        if property != None:
            for member in list(merged):
                if (member not in self.registry.structs[property].members):
                    print('member: ' + member)
                    continue

                xmlmember = self.registry.structs[property].members[member]
                if (xmlmember.limittype == 'exact' or xmlmember.limittype == 'noauto') and not xmlmember.isDynamicallySizedArrayWithCap():
                    del merged[member]
                #elif 'mul'  in xmlmember.limittype and xmlmember.type == 'float':
                #    del merged[member]
        for member in entry:
            if property is None:
                if self.mode == 'union' or self.first is True:
                    merged[member] = entry[member]
            elif not member in merged:
                xmlmember = self.registry.structs[property].members[member]
                if (xmlmember.limittype == 'exact' or xmlmember.limittype == 'noauto') and not xmlmember.isDynamicallySizedArrayWithCap():
                    continue
                elif self.mode == 'union' or self.first is True:
                    if xmlmember.type == 'uint64_t' or xmlmember.type == 'VkDeviceSize':
                        merged[member] = int(entry[member])
                    else:
                        merged[member] = entry[member]
            else:
                # Merge properties
                xmlmember = self.registry.structs[property].members[member]
                if xmlmember.limittype == 'struct':
                    s = self.registry.structs[xmlmember.type].members
                    for smember in s:
                        if smember in merged[member]:
                            if smember in entry[member]:
                                self.merge_members(merged[member], smember, entry[member], s[smember])
                        # only add member in union mode or first
                        elif (self.mode == 'union' or self.first is True) and (smember in entry[member]):
                            if xmlmember.type == 'uint64_t' or xmlmember.type == 'VkDeviceSize':
                                merged[member][smember] = int(entry[member][smember])
                            else:
                                merged[member][smember] = entry[member][smember]
                else:
                    self.merge_members(merged, member, entry, xmlmember)

    def merge_members(self, merged, member, entry, xmlmember):
        if (xmlmember.limittype == 'exact' or xmlmember.limittype == 'noauto') and not xmlmember.isDynamicallySizedArrayWithCap():
            del merged[member]
        elif self.mode == 'union':
            #if xmlmember.limittype == 'exact':
                #if merged[member] != entry[member]:
                    # merged.remove(member)
                    # del merged[member]
                    # del entry[member]
                    #print("ERROR: '" + member + " 'values with 'exact' limittype have different values.")
            #if 'mul'  in xmlmember.limittype and xmlmember.type == 'float':
            #    del merged[member]
            if 'max' in xmlmember.limittype or xmlmember.limittype == 'bits':
                if xmlmember.type == 'VkExtent2D':
                    if entry[member]['width'] > merged[member]['width']:
                        merged[member]['width'] = entry[member]['width']
                    if entry[member]['height'] > merged[member]['height']:
                        merged[member]['height'] = entry[member]['height']
                elif xmlmember.type == 'VkBool32':
                    merged[member] = merged[member] or entry[member]
                elif xmlmember.arraySize == 3:
                    if entry[member][0] > merged[member][0]:
                        merged[member][0] = entry[member][0]
                    if entry[member][1] > merged[member][1]:
                        merged[member][1] = entry[member][1]
                    if entry[member][2] > merged[member][2]:
                        merged[member][2] = entry[member][2]
                elif xmlmember.arraySize == 2:
                    if entry[member][0] > merged[member][0]:
                        merged[member][0] = entry[member][0]
                    if entry[member][1] > merged[member][1]:
                        merged[member][1] = entry[member][1]
                else:
                    if entry[member] > merged[member]:
                        merged[member] = entry[member]
            elif 'min' in xmlmember.limittype:
                if xmlmember.type == 'VkExtent2D':
                    if entry[member]['width'] < merged[member]['width']:
                        merged[member]['width'] = entry[member]['width']
                    if entry[member]['height'] < merged[member]['height']:
                        merged[member]['height'] = entry[member]['height']
                elif xmlmember.type == 'VkBool32':
                    merged[member] = merged[member] and entry[member]
                elif xmlmember.arraySize == 3:
                    if entry[member][0] < merged[member][0]:
                        merged[member][0] = entry[member][0]
                    if entry[member][1] < merged[member][1]:
                        merged[member][1] = entry[member][1]
                    if entry[member][2] < merged[member][2]:
                        merged[member][2] = entry[member][2]
                elif xmlmember.arraySize == 2:
                    if entry[member][0] < merged[member][0]:
                        merged[member][0] = entry[member][0]
                    if entry[member][1] < merged[member][1]:
                        merged[member][1] = entry[member][1]
                else:
                    if entry[member] < merged[member]:
                        merged[member] = entry[member]
            elif xmlmember.limittype == 'bitmask':
                for smember in entry[member]:
                    if smember in merged[member]:
                        merged[member] = merged[member] or smember
                    else:
                        merged[member].append(smember)
            elif xmlmember.limittype == 'range':
                if entry[member][0] < merged[member][0]:
                    merged[member][0] = entry[member][0]
                if entry[member][1] > merged[member][1]:
                    merged[member][1] = entry[member][1]
            elif xmlmember.isDynamicallySizedArrayWithCap():
                entry_set = set(merged[member])
                merged_set = set(entry[member])
                union_set = merged_set.union(entry_set)
                merged[member] = list(union_set)
            else:
                print("ERROR: Unknown limitype: " + xmlmember.limittype + " for " + member)
        elif self.mode == 'intersection':
            #if xmlmember.limittype == 'exact':
                #if merged[member] != entry[member]:
                    #merged.remove(member)
                    #del merged[member]
                    #del entry[member]
                    #print("ERROR: '" + member + " 'values with 'exact' limittype have different values.")
            #if 'mul'  in xmlmember.limittype and xmlmember.type == 'float':
            #    del merged[member]
            if 'max' in xmlmember.limittype or xmlmember.limittype == 'bits':
                if xmlmember.type == 'VkExtent2D':
                    if entry[member]['width'] < merged[member]['width']:
                        merged[member]['width'] = entry[member]['width']
                    if entry[member]['height'] < merged[member]['height']:
                        merged[member]['height'] = entry[member]['height']
                elif xmlmember.type == 'VkBool32':
                    merged[member] = merged[member] and entry[member]
                elif xmlmember.arraySize == 3:
                    if entry[member][0] < merged[member][0]:
                        merged[member][0] = entry[member][0]
                    if entry[member][1] < merged[member][1]:
                        merged[member][1] = entry[member][1]
                    if entry[member][2] < merged[member][2]:
                        merged[member][2] = entry[member][2]
                elif xmlmember.arraySize == 2:
                    if entry[member][0] < merged[member][0]:
                        merged[member][0] = entry[member][0]
                    if entry[member][1] < merged[member][1]:
                        merged[member][1] = entry[member][1]
                elif xmlmember.type == 'uint64_t' or xmlmember.type == 'VkDeviceSize':
                    if int(entry[member]) < int(merged[member]):
                        merged[member] = int(entry[member])
                    else:
                        merged[member] = int(merged[member])
                elif xmlmember.type == 'float':
                    if float(entry[member]) < float(merged[member]):
                        merged[member] = float(entry[member])
                elif xmlmember.type == 'uint32_t' or xmlmember.type == 'int32_t'  or xmlmember.type == 'size_t' or 'VkSampleCountFlagBits':
                    if entry[member] < merged[member]:
                        merged[member] = entry[member]
                else:
                    print("ERROR: '" + member + " 'values with 'max' limittype unknown case.")
            elif 'min' in xmlmember.limittype:
                if xmlmember.type == 'VkExtent2D':
                    if entry[member]['width'] < merged[member]['width']:
                        merged[member]['width'] = entry[member]['width']
                    if entry[member]['height'] < merged[member]['height']:
                        merged[member]['height'] = entry[member]['height']
                elif xmlmember.type == 'VkBool32':
                    merged[member] = merged[member] or entry[member]
                elif xmlmember.arraySize == 3:
                    if entry[member][0] < merged[member][0]:
                        merged[member][0] = entry[member][0]
                    if entry[member][1] < merged[member][1]:
                        merged[member][1] = entry[member][1]
                    if entry[member][2] < merged[member][2]:
                        merged[member][2] = entry[member][2]
                elif xmlmember.arraySize == 2:
                    if entry[member][0] < merged[member][0]:
                        merged[member][0] = entry[member][0]
                    if entry[member][1] < merged[member][1]:
                        merged[member][1] = entry[member][1]
                elif xmlmember.type == 'uint64_t' or xmlmember.type == 'VkDeviceSize':
                    if int(entry[member]) > int(merged[member]):
                        merged[member] = int(entry[member])
                    else:
                        merged[member] = int(merged[member])
                elif xmlmember.type == 'float':
                    if float(entry[member]) > float(merged[member]):
                        merged[member] = float(entry[member])
                elif xmlmember.type == 'uint32_t' or xmlmember.type == 'int32_t' or xmlmember.type == 'size_t' or 'VkSampleCountFlagBits':
                    if entry[member] > merged[member]:
                        merged[member] = entry[member]
                else:
                    print("ERROR: '" + member + " 'values with 'min' limittype unknown case.")
            elif xmlmember.limittype == 'bitmask':
                remove_list = []
                for value in merged[member]:
                    if value not in entry[member]:
                        remove_list.append(value)
                for value in remove_list:
                    merged[member].remove(value)
            elif xmlmember.limittype == 'range':
                if entry[member][0] > merged[member][0]:
                    merged[member][0] = entry[member][0]
                if entry[member][1] < merged[member][1]:
                    merged[member][1] = entry[member][1]
                #if member[1] < member[0]:
                #    merged.pop(member, None)
            elif xmlmember.isDynamicallySizedArrayWithCap():
                entry_set = set(merged[member])
                merged_set = set(entry[member])
                intersection_set = merged_set.intersection(entry_set)
                merged[member] = list(intersection_set)
            else:
                print("ERROR: Unknown limitype: " + xmlmember.limittype + " for " + member)
        else:
            print("ERROR: Unknown combination mode: " + self.mode)

    def find_higher_struct(self, struct1, struct2):
        if registry.structs[struct1].definedByVersion:
            return struct1
        if registry.structs[struct2].definedByVersion:
            return struct2
        ext1_ext = False
        ext1_other = False
        ext2_ext = False
        ext2_other = False
        for ext in registry.structs[struct1].definedByExtensions:
            if registry.extensions[ext].name[3:6] == 'EXT':
                ext1_ext = True
            else:
                ext1_other = True
        for ext in registry.structs[struct2].definedByExtensions:
            if registry.extensions[ext].name[3:6] == 'EXT':
                ext2_ext = True
            else:
                ext2_other = True
        
        if not ext1_ext and not ext1_other:
            return struct1
        if not ext2_ext and not ext2_other:
            return struct2
        if not ext1_other:
            return struct1
        if not ext2_other:
            return struct2

        return struct1

    def get_promoted_version(self, vk_version):
        if vk_version is None:
            return None
        version = vk_version[11:]
        underscore = version.find('_')
        major = version[:underscore]
        minor = version[underscore+1:]
        return [major, minor]

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

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Generate Vulkan profile JSON files')

    parser.add_argument('--registry', '-r', action='store', required=True,
                        help='Use specified registry file instead of vk.xml.')
    parser.add_argument('--config', '-c', action='store',
                        help='Use specified a JSON merge config file path instead of using individual arguments.')
    parser.add_argument('--input', '-i', action='store',
                        help='Path to directory with profiles.')
    parser.add_argument('--input-profiles', action='store',
                        help='Comma separated list of profiles.')
    parser.add_argument('--output-path', '-o', action='store', required=True,
                        help='Path to output profile.')
    parser.add_argument('--output-profile', action='store',
                        help='Profile name of the output profile. Deprecated, replaced by `--profile-name`.')
    parser.add_argument('--profile-name', action='store',
                        help='Profile name of the output profile. If the argument is not set, the value is generated.')
    parser.add_argument('--profile-version', action='store',
                        help='Override the Profile version of the generated profile. If the argument is not set, the value is 1.')
    parser.add_argument('--profile-label', action='store',
                        help='Override the Label of the generated profile. If the argument is not set, the value is generated.')
    parser.add_argument('--profile-desc', action='store',
                        help='Override the Description of the generated profile. If the argument is not set, the value is generated.')
    parser.add_argument('--profile-date', action='store',
                        help='Override the release date of the generated profile. If the argument is not set, the value is generated.')
    parser.add_argument('--profile-api-version', action='store',
                        help='Override the Vulkan API version of the generated profile. If the argument is not set, the value is generated.')
    parser.add_argument('--profile-stage', action='store', choices=['ALPHA', 'BETA', 'STABLE'], default='STABLE',
                        help='Override the development stage of the generated profile. If the argument is not set, the value is set to "stable".')
    parser.add_argument('--profile-required-profiles', action='store',
                        help='Comma separated list of required profiles by the generated profile.')
    parser.add_argument('--mode', '-m', action='store', choices=['union', 'intersection'], default='intersection',
                        help='Mode of profile combination. If the argument is not set, the value is set to "intersection".')
    parser.add_argument('--strip-duplicate-structs', action='store_true',
                        help='Strip the duplicated structures in the generated profiles file.')

    parser.set_defaults(mode='intersection')

    args = parser.parse_args()

    profile_configs = list()

    if args.registry is None:
        gen_profiles_solution.Log.e('Merging the profiles requires specifying --registry')
        parser.print_help()
        exit()

    registry = gen_profiles_solution.VulkanRegistry(args.registry)

    if (args.mode.lower() != 'union' and args.mode.lower() != 'intersection'):
        gen_profiles_solution.Log.e('Mode must be either union or intersection')
        parser.print_help()
        exit()

    if args.strip_duplicate_structs:
        gen_profiles_solution.Log.i('Stripping duplicated structures. `--strip-duplicate-structs` is set. Eg the output profiles file will contain VkPhysicalDeviceVulkan11Properties not VkPhysicalDeviceMultiviewPropertiesKHR.')
        strip_duplicate_struct = True
    else:
        strip_duplicate_struct = False

    if args.input_profiles is not None:
        input_profile_names = args.input_profiles.split(',')
    else:
        input_profile_names = list()

    profile_file = ProfileFile()

    if args.config is None:
        profile_config = ProfileConfig(args.input, input_profile_names, args.profile_api_version, args.mode)

        if args.profile_name is not None:
            if not re.match('^VP_[A-Z0-9]+[A-Za-z0-9]+', args.profile_name):
                gen_profiles_solution.Log.e('Invalid profile_name, must follow regex pattern ^VP_[A-Z0-9]+[A-Za-z0-9]+')
                exit()
            else:
                profile_config.name = args.profile_name
        elif args.output_profile is not None:
            if not re.match('^VP_[A-Z0-9]+[A-Za-z0-9]+', args.output_profile):
                gen_profiles_solution.Log.e('Invalid output_profile, must follow regex pattern ^VP_[A-Z0-9]+[A-Za-z0-9]+')
                exit()
            else:
                profile_config.name = args.output_profile

        if args.profile_version is not None:
            profile_config.version = int(args.profile_version)

        if args.profile_label is not None:
            profile_config.label = args.profile_label

        if args.profile_desc is not None:
            profile_config.description = args.profile_desc

        if args.profile_stage is not None:
            profile_config.stage = args.profile_stage

        if args.profile_date is not None:
            profile_config.date = args.profile_date

        if args.profile_required_profiles is not None:
            profile_config.required_profiles = args.profile_required_profiles.split(',')

        profile_configs.append(profile_config)

    else:
        currentdir = os.path.dirname(args.config)
        
        json_file = open(args.config, "r")
        json_data = json.load(json_file)

        if json_data["$schema"]:
            profile_file.set_schema(json_data["$schema"])
        if json_data["contributors"]:
            profile_file.set_contributors(json_data["contributors"])
        if json_data["history"]:
            profile_file.set_history(json_data["history"])

        for profile_name in json_data["profiles"]:
            profile_value = json_data["profiles"][profile_name]
            profile_config = ProfileConfig(currentdir + "/" +  profile_value["input"], list(), profile_value["api-version"], args.mode)
            profile_config.apply_json_value(profile_name, profile_value)
            profile_configs.append(profile_config)

    for config in profile_configs:
        profile_merger = ProfileMerger(registry)
        profile_merger.merge(
            config,
            profile_file,
            args.mode,
            strip_duplicate_struct)

    profile_file.dump(args.output_path)


