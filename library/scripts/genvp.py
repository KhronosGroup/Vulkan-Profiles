#!/usr/bin/python3
#
# Copyright (c) 2021-2022 LunarG, Inc.
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
# Author: Daniel Rakos, RasterGrid

import os
import argparse
import xml.etree.ElementTree as etree
import json

COPYRIGHT_HEADER = '''
/**
 * Copyright (c) 2021-2022 LunarG, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License")
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
 * DO NOT EDIT: This file is generated.
 */
'''

C_HEADER_PRE_DEFS = '''
#ifndef VULKAN_PROFILES_
#define VULKAN_PROFILES_ 1

#define VP_INLINE

#ifdef __cplusplus
    extern "C" {
#endif
#include <vulkan/vulkan_core.h>
#ifdef VK_ENABLE_BETA_EXTENSIONS
#include <vulkan/vulkan_beta.h>
#endif
'''

C_HEADER_POST_DEFS = '''
#define VP_MAX_PROFILE_NAME_SIZE 256U

typedef struct VpProfileProperties {
    char profileName[VP_MAX_PROFILE_NAME_SIZE];
    uint32_t specVersion;
} VpProfileProperties;

typedef enum VpDeviceCreateFlagBits {
    VP_DEVICE_CREATE_DISABLE_ROBUST_BUFFER_ACCESS_BIT = 0x00000001,
    VP_DEVICE_CREATE_DISABLE_ROBUST_IMAGE_ACCESS_BIT = 0x00000002,
    VP_DEVICE_CREATE_MERGE_EXTENSIONS_BIT = 0x00000004,
    VP_DEVICE_CREATE_OVERRIDE_EXTENSIONS_BIT = 0x00000008,

    VP_DEVICE_CREATE_DISABLE_ROBUST_ACCESS_BIT =
        VP_DEVICE_CREATE_DISABLE_ROBUST_BUFFER_ACCESS_BIT | VP_DEVICE_CREATE_DISABLE_ROBUST_IMAGE_ACCESS_BIT,
    VP_DEVICE_CREATE_FLAG_BITS_MAX_ENUM = 0x7FFFFFFF
} VpDeviceCreateFlagBits;
typedef VkFlags VpDeviceCreateFlags;

typedef struct VpDeviceCreateInfo {
    const VkDeviceCreateInfo *pCreateInfo;
    const VpProfileProperties *pProfile;
    VpDeviceCreateFlags flags;
} VpDeviceCreateInfo;

// Query the list of available profiles in the library
VkResult vpGetProfiles(uint32_t *pPropertyCount, VpProfileProperties *pProperties);

// List the recommended fallback profiles of a profile
VkResult vpGetProfileFallbacks(const VpProfileProperties *pProfile, uint32_t *pPropertyCount, VpProfileProperties *pProperties);

// Check whether a profile is supported by the physical device
VkResult vpGetDeviceProfileSupport(VkPhysicalDevice physicalDevice, const char *pLayerName, const VpProfileProperties *pProfile,
                                   VkBool32 *pSupported);

// Create a VkDevice with the profile features and extensions enabled
VkResult vpCreateDevice(VkPhysicalDevice physicalDevice, const VpDeviceCreateInfo *pCreateInfo,
                        const VkAllocationCallbacks *pAllocator, VkDevice *pDevice);

// Query the list of extension of a profile
VkResult vpGetProfileExtensionProperties(const VpProfileProperties *pProfile, uint32_t *pPropertyCount,
                                         VkExtensionProperties *pProperties);

// Fill the pNext Vulkan structures with the requirements of a profile
void vpGetProfileStructures(const VpProfileProperties *pProfile, void *pNext);

typedef enum VpStructureArea {
    VP_STRUCTURE_FEATURES = 0,  // A Vulkan structure specified to expose features
    VP_STRUCTURE_PROPERTIES     // A Vulkan structure specified to expose properties
} VpStructureArea;

typedef struct VpStructureProperties {
    VkStructureType type;
    VpStructureArea area;
} VpStructureProperties;

// Query the list of structures used to specify requirements of a profile
VkResult vpGetProfileStructureProperties(const VpProfileProperties *pProfile, uint32_t *pPropertyCount,
                                         VpStructureProperties *pProperties);

// Query the list of formats with specified requirements by a profile
VkResult vpGetProfileFormats(const VpProfileProperties *pProfile, uint32_t *pFormatCount, VkFormat *pFormats);

// Query the requirements of a format for a profile
void vpGetProfileFormatProperties(const VpProfileProperties *pProfile, VkFormat format, void *pNext);

// Query the requirements of queue families by a profile
VkResult vpGetProfileQueueFamilies(const VpProfileProperties *pProfile, uint32_t *pPropertyCount, VkQueueFamilyProperties *pProperties);

#ifdef __cplusplus
}
#endif

#endif//VULKAN_PROFILES_
'''

CPP_IMPL_PREDEFS = '''
#include <vulkan/vulkan_profiles.h>
#include <stddef.h>
#include <string.h>
#include <assert.h>
#include <stdint.h>
#include <vector>
#include <algorithm>

#define _vpArraySize(arr) static_cast<uint32_t>(sizeof(arr) / sizeof(arr[0]))
'''

CPP_IMPL_POSTDEFS = '''
VP_INLINE bool _vpCheckExtension(const VkExtensionProperties *supportedProperties, size_t supportedSize,
                              const char *requestedExtension) {
    for (size_t i = 0, n = supportedSize; i < n; ++i) {
        if (strcmp(supportedProperties[i].extensionName, requestedExtension) == 0) {
            return true;
        }
    }

    return false;
}

VP_INLINE bool _vpCheckMemoryProperty(const VkPhysicalDeviceMemoryProperties &memoryProperties,
                                      const VkMemoryPropertyFlags &memoryPropertyFlags) {
    assert(&memoryProperties != nullptr);

    for (uint32_t i = 0, n = memoryProperties.memoryTypeCount; i < n; ++i) {
        if ((memoryProperties.memoryTypes[i].propertyFlags & memoryPropertyFlags) == memoryPropertyFlags) return true;
    }

    return false;
}

VP_INLINE bool _vpCheckFormatProperty(const VkFormatProperties2 *deviceProps, const VpFormatProperties &profileProps) {
    if ((deviceProps->formatProperties.linearTilingFeatures & profileProps.linearTilingFeatures) !=
        profileProps.linearTilingFeatures) {
        return false;
    } else if ((deviceProps->formatProperties.optimalTilingFeatures & profileProps.optimalTilingFeatures) !=
               profileProps.optimalTilingFeatures) {
        return false;
    } else if ((deviceProps->formatProperties.bufferFeatures & profileProps.bufferFeatures) != profileProps.bufferFeatures) {
        return false;
    }

    return true;
}

VP_INLINE bool _vpCheckQueueFamilyProperty(const VkQueueFamilyProperties *queueFamilyProperties,
                                           uint32_t queueFamilyPropertiesCount,
                                           const VkQueueFamilyProperties &profileQueueFamilyPropertie) {
    assert(queueFamilyProperties != nullptr);

    for (uint32_t i = 0, n = queueFamilyPropertiesCount; i < n; ++i) {
        if (queueFamilyProperties[i].queueCount < profileQueueFamilyPropertie.queueCount) {
            continue;
        } else if (queueFamilyProperties[i].timestampValidBits < profileQueueFamilyPropertie.timestampValidBits) {
            continue;
        } else if (queueFamilyProperties[i].minImageTransferGranularity.width >
                   profileQueueFamilyPropertie.minImageTransferGranularity.width) {
            continue;
        } else if (queueFamilyProperties[i].minImageTransferGranularity.height >
                   profileQueueFamilyPropertie.minImageTransferGranularity.height) {
            continue;
        } else if (queueFamilyProperties[i].minImageTransferGranularity.depth >
                   profileQueueFamilyPropertie.minImageTransferGranularity.depth) {
            continue;
        } else if ((queueFamilyProperties[i].queueFlags & profileQueueFamilyPropertie.queueFlags) !=
                   profileQueueFamilyPropertie.queueFlags) {
            continue;
        }

        return true;
    }

    return false;
}

VP_INLINE void _vpGetExtensions(const VpDeviceCreateInfo *pCreateInfo, uint32_t propertyCount,
                             const VkExtensionProperties *pProperties, std::vector<const char *> &extensions) {
    if (pCreateInfo->flags & VP_DEVICE_CREATE_MERGE_EXTENSIONS_BIT) {
        for (int i = 0, n = propertyCount; i < n; ++i) {
            extensions.push_back(pProperties[i].extensionName);
        }

        for (uint32_t i = 0; i < pCreateInfo->pCreateInfo->enabledExtensionCount; ++i) {
            if (_vpCheckExtension(pProperties, propertyCount, pCreateInfo->pCreateInfo->ppEnabledExtensionNames[i])) {
                continue;
            }
            extensions.push_back(pCreateInfo->pCreateInfo->ppEnabledExtensionNames[i]);
        }
    } else {  // or VP_DEVICE_CREATE_OVERRIDE_EXTENSIONS_BIT
        for (int i = 0, n = pCreateInfo->pCreateInfo->enabledExtensionCount; i < n; ++i) {
            extensions.push_back(pCreateInfo->pCreateInfo->ppEnabledExtensionNames[i]);
        }
    }
}

VP_INLINE bool _vpHasStructure(const void* pNext, VkStructureType type) {
    const VkBaseOutStructure *p = static_cast<const VkBaseOutStructure*>(pNext);
    while (p != nullptr) {
        if (p->sType == type) return true;
        p = p->pNext;
    }
    return false;
}
'''

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


class VulkanPlatform():
    def __init__(self, data):
        self.name = data.get('name')
        self.protect = data.get('protect')


class VulkanStructMember():
    def __init__(self, name, type, limittype, isArray = False):
        self.name = name
        self.type = type
        self.limittype = limittype
        self.isArray = isArray


class VulkanStruct():
    def __init__(self, name):
        self.name = name
        self.sType = None
        self.extends = []
        self.members = dict()
        self.aliases = [ name ]


class VulkanExtension():
    def __init__(self, name, upperCaseName, type, platform):
        self.name = name
        self.upperCaseName = upperCaseName
        self.type = type
        self.platform = platform


class VulkanRegistry():
    def __init__(self, registryFile):
        Log.i("Loading registry file: '{0}'".format(registryFile))
        xml = etree.parse(registryFile)
        self.parsePlatformInfo(xml)
        self.parseExtensionInfo(xml)
        self.parseStructInfo(xml)
        self.applyWorkarounds()

    def parsePlatformInfo(self, xml):
        self.platforms = dict()
        for plat in xml.findall("./platforms/platform"):
            self.platforms[plat.get('name')] = VulkanPlatform(plat)


    def parseStructInfo(self, xml):
        self.structs = dict()
        for struct in xml.findall("./types/type[@category='struct']"):
            # Define base struct information
            structDef = VulkanStruct(struct.get('name'))

            # Find out whether it's an extension structure
            extends = struct.get('structextends')
            if extends != None:
                structDef.extends = extends.split(',')

            # Find sType value
            sType = struct.find("./member[name='sType']")
            if sType != None:
                structDef.sType = sType.get('values')

            # Parse struct members
            for member in struct.findall('./member'):
                name = member.find('./name').text
                tail = member.find('./name').tail
                type = member.find('./type').text

                # Only add real members (skip sType and pNext)
                if name != 'sType' and name != 'pNext':
                    # Define base member information
                    structDef.members[name] = VulkanStructMember(
                        name,
                        type,
                        member.get('limittype')
                    )

                    # Detect if it's an array
                    if tail != None and tail[0] == '[':
                        structDef.members[name].isArray = True

            # Store struct definition
            self.structs[struct.get('name')] = structDef

        # Find any aliases and copy the constructed data for them too
        for struct in xml.findall("./types/type[@category='struct']"):
            alias = struct.get('alias')
            if alias != None:
                if self.structs[alias] != None:
                    self.structs[struct.get('name')] = self.structs[alias]
                    self.structs[alias].aliases.append(struct.get('name'))
                else:
                    Log.f("Failed to find alias '{0}' of struct '{0}'".format(alias, struct.get('name')))


    def applyWorkarounds(self):
        # TODO: We currently have to apply workarounds due to "noauto" limittypes and other bugs related to limittypes in the vk.xml
        # These can only be solved permanently if we make modifications to the registry xml itself
        self.structs['VkPhysicalDeviceLimits'].members['bufferImageGranularity'].limittype = 'min' # should be maxalign
        self.structs['VkPhysicalDeviceLimits'].members['subPixelPrecisionBits'].limittype = 'max'
        self.structs['VkPhysicalDeviceLimits'].members['subTexelPrecisionBits'].limittype = 'max'
        self.structs['VkPhysicalDeviceLimits'].members['mipmapPrecisionBits'].limittype = 'max'
        self.structs['VkPhysicalDeviceLimits'].members['viewportSubPixelBits'].limittype = 'max'
        self.structs['VkPhysicalDeviceLimits'].members['minMemoryMapAlignment'].limittype = 'max' # should be minalign
        self.structs['VkPhysicalDeviceLimits'].members['minTexelBufferOffsetAlignment'].limittype = 'min' # should be maxalign
        self.structs['VkPhysicalDeviceLimits'].members['minUniformBufferOffsetAlignment'].limittype = 'min' # should be maxalign
        self.structs['VkPhysicalDeviceLimits'].members['minStorageBufferOffsetAlignment'].limittype = 'min' # should be maxalign
        self.structs['VkPhysicalDeviceLimits'].members['subPixelInterpolationOffsetBits'].limittype = 'max'
        self.structs['VkPhysicalDeviceLimits'].members['timestampPeriod'].limittype = 'IGNORE' # good question what this should be...
        self.structs['VkPhysicalDeviceLimits'].members['nonCoherentAtomSize'].limittype = 'min' # should be maxalign
        self.structs['VkPhysicalDeviceLimits'].members['maxColorAttachments'].limittype = 'max' # vk.xml declares this with 'bitmask' limittype for some reason
        self.structs['VkPhysicalDeviceLimits'].members['pointSizeGranularity'].limittype = 'min' # should be maxmul
        self.structs['VkPhysicalDeviceLimits'].members['lineWidthGranularity'].limittype = 'min' # should be maxmul
        self.structs['VkPhysicalDeviceVulkan11Properties'].members['subgroupSize'].limittype = 'IGNORE' # good question what this should be...
        self.structs['VkPhysicalDevicePortabilitySubsetPropertiesKHR'].members['minVertexInputBindingStrideAlignment'].limittype = 'min' # should be maxalign

        # TODO: There are also some bugs in the vk.xml, like parameters having "bitmask" limittype but actually VkBool32 type
        # This is non-sense, so we patch them
        for structName in self.structs:
            for memberName in self.structs[structName].members:
                memberDef = self.structs[structName].members[memberName]
                if memberDef.limittype == 'bitmask' and memberDef.type == 'VkBool32':
                    self.structs[structName].members[memberName].limittype = 'noauto'


    def parseExtensionInfo(self, xml):
        self.extensions = dict()
        for ext in xml.findall("./extensions/extension"):
            # Only care about enabled extensions
            if ext.get('supported') == "vulkan":
                name = ext.get('name')

                # Find name enum (due to inconsistencies in lower case and upper case names this is non-trivial)
                foundNameEnum = False
                matches = ext.findall("./require/enum[@value='\"" + name + "\"']")
                for match in matches:
                    if match.get('name').endswith("_EXTENSION_NAME"):
                        # Add extension definition
                        self.extensions[name] = VulkanExtension(
                            name,
                            match.get('name')[:-len("_EXTENSION_NAME")],
                            ext.get('type'),
                            ext.get('platform')
                        )
                        foundNameEnum = True
                        break
                if not foundNameEnum:
                    Log.f("Cannot find name enum for extension '{0}'".format(name))


class VulkanProfileCapabilities():
    def __init__(self, data, caps):
        self.extensions = dict()
        self.features = dict()
        self.properties = dict()
        self.formats = dict()
        self.queueFamiliesProperties = dict()
        self.memoryProperties = dict()
        for capName in data['capabilities']:
            if capName in caps:
                self.mergeCaps(caps[capName])
            else:
                Log.f("Capability '{0}' needed by profile '{1}' is missing".format(capName, data['name']))


    def mergeCaps(self, caps):
        self.mergeProfileExtensions(caps)
        self.mergeProfileFeatures(caps)
        self.mergeProfileProperties(caps)
        self.mergeProfileFormats(caps)
        self.mergeProfileQueueFamiliesProperties(caps)
        self.mergeProfileMemoryProperties(caps)


    def mergeProfileCapData(self, dst, src):
        if type(src) != type(dst):
            Log.f("Data type confict during profile capability data merge (src is '{0}', dst is '{1}')".format(type(src), type(dst)))
        elif type(src) == dict:
            for key, val in src.items():
                if type(val) == dict:
                    if not key in dst:
                        dst[key] = dict()
                    self.mergeProfileCapData(dst[key], val)

                elif type(val) == list:
                    if not key in dst:
                        dst[key] = []
                    dst[key].extend(val)

                else:
                    if key in dst and type(dst[key]) != type(val):
                        Log.f("Data type confict during profile capability data merge (src is '{0}', dst is '{1}')".format(type(val), type(dst[key])))
                    dst[key] = val
        else:
            Log.f("Unexpected data type during profile capability data merge (src is '{0}', dst is '{1}')".format(type(src), type(dst)))


    def mergeProfileExtensions(self, data):
        if data.get('extensions') != None:
            for extName, specVer in data['extensions'].items():
                self.extensions[extName] = {
                    'specVersion': specVer
                }


    def mergeProfileFeatures(self, data):
        if data.get('features') != None:
            self.mergeProfileCapData(self.features, data['features'])


    def mergeProfileProperties(self, data):
        if data.get('properties') != None:
            self.mergeProfileCapData(self.properties, data['properties'])


    def mergeProfileFormats(self, data):
        if data.get('formats') != None:
            self.mergeProfileCapData(self.formats, data['formats'])


    def mergeProfileQueueFamiliesProperties(self, data):
        if data.get('queueFamiliesProperties') != None:
            self.mergeProfileCapData(self.queueFamiliesProperties, data['queueFamiliesProperties'])


    def mergeProfileMemoryProperties(self, data):
        if data.get('memoryProperties') != None:
            self.mergeProfileCapData(self.memoryProperties, data['memoryProperties'])


class VulkanProfile():
    def __init__(self, registry, name, data, caps):
        self.name = name
        self.version = data['version']
        self.apiVersion = data['api-version']
        self.fallback = data.get('fallback')
        self.platform = None
        self.capabilities = VulkanProfileCapabilities(data, caps)
        self.validateProfileExtensions(registry)


    def validateProfileExtensions(self, registry):
        if self.capabilities.extensions:
            for extName in self.capabilities.extensions.keys():
                if extName in registry.extensions:
                    platform = registry.extensions[extName].platform
                    if platform != None:
                        self.platform = platform
                else:
                    Log.f("Extension '{0}' required by profile '{1}' does not exist".format(extName, self.name))


class VulkanProfiles():
    def loadFromDir(registry, profilesDir):
        profiles = dict()
        dirAbsPath = os.path.abspath(profilesDir)
        filenames = os.listdir(dirAbsPath)
        for filename in filenames:
            fileAbsPath = os.path.join(dirAbsPath, filename)
            if os.path.isfile(fileAbsPath) and os.path.splitext(filename)[-1] == '.json':
                Log.i("Loading profile file: '{0}'".format(filename))
                with open(fileAbsPath, 'r') as f:
                    jsonData = json.load(f)
                    VulkanProfiles.parseProfiles(registry, profiles, jsonData['profiles'], jsonData['capabilities'])
        return profiles


    def parseProfiles(registry, profiles, json, caps):
        for name, data in json.items():
            Log.i("Registering profile '{0}'".format(name))
            profiles[name] = VulkanProfile(registry, name, data, caps)


class ProfilePlatformGuard():
    def __init__(self, registry, profile):
        platform = profile.platform
        if platform != None:
            platformDef = registry.platforms[platform].protect
            self.begin = '#ifdef {0}\n'.format(platformDef)
            self.end = '#endif\n'
        else:
            self.begin = ''
            self.end = ''


class VulkanProfilesBuilder():
    def __init__(self, registry, profiles):
        self.registry = registry
        self.profiles = profiles


    def generate(self, outIncDir, outSrcDir):
        self.generate_h(outIncDir)
        self.generate_cpp(outSrcDir)


    def generate_h(self, outDir):
        fileAbsPath = os.path.join(os.path.abspath(outDir), 'vulkan_profiles.h')
        Log.i("Generating '{0}'...".format(fileAbsPath))
        with open(fileAbsPath, 'w') as f:
            f.write(COPYRIGHT_HEADER)
            f.write(C_HEADER_PRE_DEFS)
            f.write(self.gen_profileDefs())
            f.write(C_HEADER_POST_DEFS)


    def generate_cpp(self, outDir):
        fileAbsPath = os.path.join(os.path.abspath(outDir), 'vulkan_profiles.cpp')
        Log.i("Generating '{0}'...".format(fileAbsPath))
        with open(fileAbsPath, 'w') as f:
            f.write(COPYRIGHT_HEADER)
            f.write(CPP_IMPL_PREDEFS)
            f.write(self.gen_extensionLists())
            f.write(self.gen_structPropLists())
            f.write(self.gen_formatLists())
            # TODO: Memory types removed for now
            #f.write(self.gen_memoryTypeLists())
            f.write(self.gen_queueFamilyLists())
            f.write(CPP_IMPL_POSTDEFS)
            f.write(self.gen_vpGetProfiles())
            f.write(self.gen_vpGetProfileFallbacks())
            f.write(self.gen_vpGetDeviceProfileSupport())
            f.write(self.gen_vpCreateDevice())
            f.write(self.gen_vpGetProfileExtensionProperties())
            f.write(self.gen_vpGetProfileStructures())
            f.write(self.gen_vpGetProfileStructureProperties())
            f.write(self.gen_vpGetProfileFormats())
            f.write(self.gen_vpGetProfileFormatProperties())
            # TODO: Memory types removed for now
            #f.write(self.gen_vpGetProfileMemoryTypes())
            f.write(self.gen_vpGetProfileQueueFamilies())


    def gen_profileDefs(self):
        gen = ''
        for name, profile in self.profiles.items():
            uname = name.upper()
            gen += '\n'
            gen += '#define {0} 1\n'.format(name)
            gen += '#define {0}_NAME "{1}"\n'.format(uname, name)
            gen += '#define {0}_SPEC_VERSION {1}\n'.format(uname, profile.version)
            gen += '#define {0}_MIN_API_VERSION VK_MAKE_VERSION({1})\n'.format(uname, profile.apiVersion.replace(".", ", "))
        return gen


    def gen_extensionLists(self):
        gen = ''
        # TODO: Probably we should separate instance vs device extensions
        for name, profile in self.profiles.items():
            if profile.capabilities.extensions:
                gen += '\n'
                guard = ProfilePlatformGuard(self.registry, profile)
                gen += guard.begin
                gen += 'static const VkExtensionProperties _{0}_EXTENSIONS[] = {{\n'.format(name.upper())
                for extName, ext in profile.capabilities.extensions.items():
                    extInfo = self.registry.extensions[extName]
                    gen += '    VkExtensionProperties{{ {0}_EXTENSION_NAME, {1} }},\n'.format(extInfo.upperCaseName, ext['specVersion'])
                gen += '};\n'
                gen += guard.end
        return gen


    def gen_structPropLists(self):
        gen = ''
        for name, profile in self.profiles.items():
            features = profile.capabilities.features
            properties = profile.capabilities.properties
            if features != None or properties != None:
                gen += '\n'
                guard = ProfilePlatformGuard(self.registry, profile)
                gen += guard.begin
                gen += 'static const VpStructureProperties _{0}_STRUCTURE_PROPERTIES[] = {{\n'.format(name.upper())

                if features != None:
                    for featureStructName in features:
                        if featureStructName == 'VkPhysicalDeviceFeatures':
                            # Special case, as it's wrapped into VkPhysicalDeviceFeatures2
                            featureStructName = 'VkPhysicalDeviceFeatures2'

                        structDef = self.registry.structs.get(featureStructName)
                        if structDef == None:
                            Log.f("Feature structure '{0}' does not exist".format(featureStructName))

                        if structDef.sType == None:
                            Log.f("Feature structure '{0}' is not chainable".format(featureStructName))

                        if not 'VkPhysicalDeviceFeatures2' in structDef.extends + [ structDef.name ]:
                            Log.f("Feature structure '{0}' does not extend VkPhysicalDeviceFeatures2".format(featureStructName))

                        gen += '    {{ {0}, VP_STRUCTURE_FEATURES }},\n'.format(structDef.sType)

                if properties != None:
                    for propertyStructName in properties:
                        if propertyStructName == 'VkPhysicalDeviceProperties':
                            # Special case, as it's wrapped into VkPhysicalDeviceProperties2
                            propertyStructName = 'VkPhysicalDeviceProperties2'

                        structDef = self.registry.structs.get(propertyStructName)
                        if structDef == None:
                            Log.f("Properties structure '{0}' does not exist".format(propertyStructName))

                        if structDef.sType == None:
                            Log.f("Properties structure '{0}' is not chainable".format(propertyStructName))

                        if not 'VkPhysicalDeviceProperties2' in [ structDef.name ] + structDef.extends:
                            Log.f("Properties structure '{0}' does not extend VkPhysicalDeviceProperties2".format(propertyStructName))

                        gen += '    {{ {0}, VP_STRUCTURE_PROPERTIES }},\n'.format(structDef.sType)

                gen += '};\n'
                gen += guard.end
        return gen


    def gen_formatLists(self):
        gen = '\n'
        # TODO: Make format properties extensible
        gen += ('struct VpFormatProperties {\n'
                '    VkFormat format;\n'
                '    VkFlags64 linearTilingFeatures;\n'
                '    VkFlags64 optimalTilingFeatures;\n'
                '    VkFlags64 bufferFeatures;\n'
                '};\n')
        for name, profile in self.profiles.items():
            if profile.capabilities.formats:
                gen += '\n'
                guard = ProfilePlatformGuard(self.registry, profile)
                gen += guard.begin
                gen += 'static const VpFormatProperties _{0}_FORMATS[] = {{\n'.format(name.upper())
                for format, props in profile.capabilities.formats.items():
                    for propStructName in props:
                        if propStructName != 'VkFormatProperties':
                            Log.f("Unsupported format properties structure '{0}'".format(propStructName))

                    formatProps = props['VkFormatProperties']

                    gen += ('    {{\n'
                            '        {0},\n'
                            '        {1},\n'
                            '        {2},\n'
                            '        {3},\n'
                            '    }},\n').format(format,
                                                self.gen_listValue(formatProps.get('linearTilingFeatures')),
                                                self.gen_listValue(formatProps.get('optimalTilingFeatures')),
                                                self.gen_listValue(formatProps.get('bufferFeatures')))
                gen += '};\n'
                gen += guard.end
        return gen


    def gen_memoryTypeLists(self):
        gen = ''
        # TODO: Make memory properties extensible if necessary
        for name, profile in self.profiles.items():
            if profile.capabilities.memoryProperties:
                gen += '\n'
                guard = ProfilePlatformGuard(self.registry, profile)
                gen += guard.begin
                gen += 'static const VkMemoryPropertyFlags _{0}_MEMORY_TYPES[] = {{\n'.format(name.upper())
                for propStructName, members in profile.capabilities.memoryProperties.items():
                    if propStructName != 'VkPhysicalDeviceMemoryProperties':
                        Log.f("Unsupported memory properties structure '{0}'".format(propStructName))
                    for member in members:
                        if member != 'memoryTypes':
                            Log.f("Unsupported memory properties struct member '{0}'".format(member))

                    memoryTypes = members['memoryTypes']
                    for memoryType in memoryTypes:
                        gen += '    {0},\n'.format(self.gen_listValue(memoryType['propertyFlags']))
                gen += '};\n'
                gen += guard.end
        return gen


    def gen_queueFamilyLists(self):
        gen = ''
        # TODO: Make queue family properties extensible
        for name, profile in self.profiles.items():
            if profile.capabilities.queueFamiliesProperties:
                gen += '\n'
                guard = ProfilePlatformGuard(self.registry, profile)
                gen += guard.begin
                gen += 'static const VkQueueFamilyProperties _{0}_QUEUE_FAMILY_PROPERTIES[] = {{\n'.format(name.upper())
                for propStructName, members in profile.capabilities.queueFamiliesProperties.items():
                    if propStructName != 'VkQueueFamilyProperties':
                        Log.f("Unsupported memory properties structure '{0}'".format(propStructName))

                    gen += '    {0},\n'.format(
                        self.gen_listValue(
                            [
                                self.gen_listValue(members['queueFlags']),
                                members['queueCount'],
                                members['timestampValidBits'],
                                self.gen_listValue(
                                    [
                                        members['minImageTransferGranularity']['width'],
                                        members['minImageTransferGranularity']['height'],
                                        members['minImageTransferGranularity']['depth']
                                    ], False)
                            ], False))
                gen += '};\n'
                gen += guard.end
        return gen


    def gen_vpGetProfiles(self):
        gen = '\n'
        gen += ('VP_INLINE VkResult vpGetProfiles(uint32_t *pPropertyCount, VpProfileProperties *pProperties) {\n'
                '    VkResult result = VK_SUCCESS;\n'
                '    static const VpProfileProperties profiles[] = {\n')

        for name, profile in self.profiles.items():
            guard = ProfilePlatformGuard(self.registry, profile)
            gen += guard.begin
            gen += '        {{ {0}_NAME, {0}_SPEC_VERSION }},\n'.format(name.upper())
            gen += guard.end

        gen += ('    };\n'
                '\n'
                '    if (pProperties == nullptr) {\n'
                '        *pPropertyCount = _vpArraySize(profiles);\n'
                '    } else {\n'
                '        if (*pPropertyCount < _vpArraySize(profiles)) {\n'
                '            result = VK_INCOMPLETE;\n'
                '        } else {\n'
                '            *pPropertyCount = _vpArraySize(profiles);\n'
                '        }\n'
                '        for (uint32_t i = 0; i < *pPropertyCount; ++i) {\n'
                '            pProperties[i] = profiles[i];\n'
                '        }\n'
                '    }\n'
                '    return result;\n'
                '}\n')
        return gen


    def gen_vpGetProfileFallbacks(self):
        gen = '\n'
        gen += ('VP_INLINE VkResult vpGetProfileFallbacks(const VpProfileProperties *pProfile, uint32_t *pPropertyCount, VpProfileProperties *pProperties) {\n'
                '    VkResult result = VK_SUCCESS;\n')

        for name, profile in self.profiles.items():
            uname = name.upper()
            if profile.fallback:
                guard = ProfilePlatformGuard(self.registry, profile)
                gen += guard.begin
                gen += ('    if (strcmp(pProfile->profileName, {0}_NAME) == 0) {{\n'
                        '        static const VpProfileProperties {0}_fallbacks[] = {{\n').format(uname)
                for fallback in profile.fallback:
                    gen += '            {{ {0}_NAME, {0}_SPEC_VERSION }},\n'.format(fallback.upper())
                gen += ('        }};\n'
                        '\n'
                        '        if (pProperties == nullptr) {{\n'
                        '            *pPropertyCount = _vpArraySize({0}_fallbacks);\n'
                        '        }} else {{\n'
                        '            if (*pPropertyCount < _vpArraySize({0}_fallbacks)) {{\n'
                        '                result = VK_INCOMPLETE;\n'
                        '            }} else {{\n'
                        '                *pPropertyCount = _vpArraySize({0}_fallbacks);\n'
                        '            }}\n'
                        '            for (uint32_t i = 0; i < *pPropertyCount; ++i) {{\n'
                        '                pProperties[i] = {0}_fallbacks[i];\n'
                        '            }}\n'
                        '        }}\n'
                        '    }} else\n').format(uname)
                gen += guard.end

        gen += ('    {\n'
                '        *pPropertyCount = 0;\n'
                '    }\n'
                '    return result;\n'
                '}\n')
        return gen


    def gen_vpGetDeviceProfileSupport(self):
        gen = '\n'
        gen += ('VP_INLINE VkResult vpGetDeviceProfileSupport(VkPhysicalDevice physicalDevice, const char *pLayerName,\n'
                '                                             const VpProfileProperties *pProfile, VkBool32 *pSupported) {\n'
                '    assert(pProfile != nullptr);\n'
                '    assert(pSupported != nullptr);\n'
                '    assert(physicalDevice != VK_NULL_HANDLE);\n'
                '\n'
                '    VkResult result = VK_SUCCESS;\n'
                '\n'
                '    uint32_t instanceExtensionCount;\n'
                '    result = vkEnumerateInstanceExtensionProperties(pLayerName, &instanceExtensionCount, nullptr);\n'
                '    if (result != VK_SUCCESS) return result;\n'
                '    std::vector<VkExtensionProperties> instanceExtensions(instanceExtensionCount);\n'
                '    result = vkEnumerateInstanceExtensionProperties(pLayerName, &instanceExtensionCount, instanceExtensions.data());\n'
                '    if (result != VK_SUCCESS) return result;\n'
                '\n'
                '    uint32_t deviceExtensionCount;\n'
                '    result = vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &deviceExtensionCount, nullptr);\n'
                '    if (result != VK_SUCCESS) return result;\n'
                '    std::vector<VkExtensionProperties> deviceExtensions(deviceExtensionCount);\n'
                '    result = vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &deviceExtensionCount, deviceExtensions.data());\n'
                '    if (result != VK_SUCCESS) return result;\n'
                '\n'
                '    *pSupported = VK_FALSE;\n'
                '\n')

        for name, profile in self.profiles.items():
            uname = name.upper()
            guard = ProfilePlatformGuard(self.registry, profile)
            gen += guard.begin
            gen += ('    if (strcmp(pProfile->profileName, {0}_NAME) == 0) {{\n'
                    '        if ({0}_SPEC_VERSION < pProfile->specVersion) return result;\n'
                    '\n'
                    '        VkPhysicalDeviceProperties devProps;\n'
                    '        vkGetPhysicalDeviceProperties(physicalDevice, &devProps);\n'
                    '        if (VK_VERSION_PATCH(devProps.apiVersion) < VK_VERSION_PATCH({0}_MIN_API_VERSION)) return result;\n').format(uname)

            # Check extensions
            # TODO: Eventually we should check instance vs device extensions separately
            if profile.capabilities.extensions:
                gen += ('\n'
                        '        bool extensionsSupported = true;\n'
                        '        for (uint32_t i = 0; i < _vpArraySize(_{0}_EXTENSIONS); ++i) {{\n'
                        '            const bool supportedInstanceExt = _vpCheckExtension(instanceExtensions.data(), instanceExtensions.size(),\n'
                        '                                                                _{0}_EXTENSIONS[i].extensionName);\n'
                        '            const bool supportedDeviceExt = _vpCheckExtension(deviceExtensions.data(), deviceExtensions.size(),\n'
                        '                                                              _{0}_EXTENSIONS[i].extensionName);\n'
                        '            if (!supportedInstanceExt && !supportedDeviceExt) {{\n'
                        '                extensionsSupported = false;\n'
                        '                break;\n'
                        '            }}\n'
                        '        }}\n'
                        '        if (!extensionsSupported) return result;\n').format(uname)

            # Check features
            features = profile.capabilities.features
            if features:
                pNextDevice = None
                pNextProfile = None
                gen += '\n'
                genDef = ''
                genCheck = '        bool featuresSupported = true;\n'
                for featureStructName, feature in features.items():
                    structDef = self.registry.structs.get(featureStructName)
                    if structDef == None:
                        Log.f("Feature structure '{0}' does not exist".format(featureStructName))

                    if featureStructName == 'VkPhysicalDeviceFeatures':
                        # Special case, as it's wrapped into VkPhysicalDeviceFeatures2
                        featureStructName = 'VkPhysicalDeviceFeatures2'
                        varAccessSuffix = '.features.'
                        sType = 'VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2'
                    else:
                        varAccessSuffix = '.'
                        sType = structDef.sType

                    deviceVarName = 'device' + featureStructName[2:]
                    genDef += '        {0} {1}{{ {2} }};\n'.format(featureStructName, deviceVarName, sType)
                    if featureStructName != 'VkPhysicalDeviceFeatures2':
                        if pNextDevice != None:
                            genDef += '        {0}.pNext = &{1};\n'.format(deviceVarName, pNextDevice)
                        pNextDevice = deviceVarName

                    profileVarName = 'profile' + featureStructName[2:]
                    genDef += '        {0} {1}{{ {2} }};\n'.format(featureStructName, profileVarName, sType)
                    if pNextProfile != None:
                        genDef += '        {0}.pNext = &{1};\n'.format(profileVarName, pNextProfile)
                    pNextProfile = profileVarName

                    genCheck += self.gen_compareStructVar('        featuresSupported = featuresSupported && {0};\n', structDef, deviceVarName + varAccessSuffix, profileVarName + varAccessSuffix, feature)

                genCheck = genCheck[:-1] + ';\n'

                if not 'VkPhysicalDeviceFeatures' in features and not 'VkPhysicalDeviceFeatures2' in features:
                    # We have to manually add VkPhysicalDeviceFeatures2 as it's not used in the profile
                    genDef += '        VkPhysicalDeviceFeatures2 devicePhysicalDeviceFeatures2{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2 };\n'
                if pNextDevice != None:
                    # For vkGetPhysicalDeviceFeatures2 everything has to be chained to VkPhysicalDeviceFeatures2
                    genDef += '        devicePhysicalDeviceFeatures2.pNext = &{0};\n'.format(pNextDevice)

                gen += genDef
                gen += '        vkGetPhysicalDeviceFeatures2(physicalDevice, &devicePhysicalDeviceFeatures2);\n'
                gen += '        vpGetProfileStructures(pProfile, &{0});\n'.format(pNextProfile)
                gen += genCheck
                gen += '        if (!featuresSupported) return result;\n'

            # Check properties
            properties = profile.capabilities.properties
            if properties:
                pNextDevice = None
                pNextProfile = None
                gen += '\n'
                genDef = ''
                genCheck = '        bool propertiesSupported = true;\n'
                for propertyStructName, property in properties.items():
                    structDef = self.registry.structs.get(propertyStructName)
                    if structDef == None:
                        Log.f("Properties structure '{0}' does not exist".format(propertyStructName))

                    if propertyStructName == 'VkPhysicalDeviceProperties':
                        # Special case, as it's wrapped into VkPhysicalDeviceProperties2
                        propertyStructName = 'VkPhysicalDeviceProperties2'
                        varAccessSuffix = '.properties.'
                        sType = 'VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2'
                    else:
                        varAccessSuffix = '.'
                        sType = structDef.sType

                    deviceVarName = 'device' + propertyStructName[2:]
                    genDef += '        {0} {1}{{ {2} }};\n'.format(propertyStructName, deviceVarName, sType)
                    if propertyStructName != 'VkPhysicalDeviceProperties2':
                        if pNextDevice != None:
                            genDef += '        {0}.pNext = &{1};\n'.format(deviceVarName, pNextDevice)
                        pNextDevice = deviceVarName

                    profileVarName = 'profile' + propertyStructName[2:]
                    genDef += '        {0} {1}{{ {2} }};\n'.format(propertyStructName, profileVarName, sType)
                    if pNextProfile != None:
                        genDef += '        {0}.pNext = &{1};\n'.format(profileVarName, pNextProfile)
                    pNextProfile = profileVarName

                    genCheck += self.gen_compareStructVar('        propertiesSupported = propertiesSupported && {0};\n', structDef, deviceVarName + varAccessSuffix, profileVarName + varAccessSuffix, property)

                genCheck = genCheck[:-1] + ';\n'

                if not 'VkPhysicalDeviceProperties' in properties and not 'VkPhysicalDeviceProperties2' in properties:
                    # We have to manually add VkPhysicalDeviceProperies2 as it's not used in the profile
                    genDef += '        VkPhysicalDeviceProperties2 devicePhysicalDeviceProperties2{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2 };\n'
                if pNextDevice != None:
                    # For vkGetPhysicalDeviceProperties2 everything has to be chained to VkPhysicalDeviceProperties2
                    genDef += '        devicePhysicalDeviceProperties2.pNext = &{0};\n'.format(pNextDevice)

                gen += genDef
                gen += '        vkGetPhysicalDeviceProperties2(physicalDevice, &devicePhysicalDeviceProperties2);\n'
                gen += '        vpGetProfileStructures(pProfile, &{0});\n'.format(pNextProfile)
                gen += genCheck
                gen += '        if (!propertiesSupported) return result;\n'

            # Check queue family properties
            if profile.capabilities.queueFamiliesProperties:
                gen += ('\n'
                        '        uint32_t queueFamilyCount = 0;\n'
                        '        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);\n'
                        '        std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);\n'
                        '        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilies.data());\n'
                        '\n'
                        '        bool queueFamiliesSupported = true;\n'
                        '        for (uint32_t i = 0; i < _vpArraySize(_{0}_QUEUE_FAMILY_PROPERTIES); ++i) {{\n'
                        '            if (!_vpCheckQueueFamilyProperty(&queueFamilies[0], queueFamilyCount, _{0}_QUEUE_FAMILY_PROPERTIES[i])) {{\n'
                        '                queueFamiliesSupported = false;\n'
                        '                break;\n'
                        '            }}\n'
                        '        }}\n'
                        '        if (!queueFamiliesSupported) return result;\n').format(uname)

            # Check memory types
            if profile.capabilities.memoryProperties:
                gen += ('\n'
                        '        VkPhysicalDeviceMemoryProperties memoryProperties;\n'
                        '        vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memoryProperties);\n'
                        '\n'
                        '        bool memoryTypesSupported = true;\n'
                        '        for (uint32_t i = 0; i < _vpArraySize(_{0}_MEMORY_TYPES); ++i) {{\n'
                        '            if (!_vpCheckMemoryProperty(memoryProperties, _{0}_MEMORY_TYPES[i])) {{\n'
                        '                memoryTypesSupported = false;\n'
                        '                break;\n'
                        '            }}\n'
                        '        }}\n'
                        '        if (!memoryTypesSupported) return result;\n').format(uname)

            gen += '    } else\n'

            gen += guard.end

        gen += ('    {\n'
                '        return result;\n'
                '    }\n'
                '\n'
                '    *pSupported = VK_TRUE;\n'
                '    return result;\n'
                '}\n')
        return gen


    def gen_vpCreateDevice(self):
        gen = '\n'
        gen += ('VP_INLINE VkResult vpCreateDevice(VkPhysicalDevice physicalDevice, const VpDeviceCreateInfo *pCreateInfo,\n'
                '                                  const VkAllocationCallbacks *pAllocator, VkDevice *pDevice) {\n'
                '    assert(pCreateInfo != nullptr);\n'
                '\n'
                '    if (physicalDevice == VK_NULL_HANDLE || pCreateInfo == nullptr || pDevice == nullptr) {\n'
                '        return vkCreateDevice(physicalDevice, pCreateInfo == nullptr ? nullptr : pCreateInfo->pCreateInfo, pAllocator, pDevice);\n'
                '    } else if (pCreateInfo->pProfile == nullptr || strcmp(pCreateInfo->pProfile->profileName, "") == 0) {\n'
                '        return vkCreateDevice(physicalDevice, pCreateInfo->pCreateInfo, pAllocator, pDevice);\n'
                '    } else\n')

        for name, profile in self.profiles.items():
            uname = name.upper()
            guard = ProfilePlatformGuard(self.registry, profile)
            gen += guard.begin

            # TODO: Well, this is bogus now, as we add all extensions here even though some may be instance
            # and not device extensions, but we'll keep it as is for now to maintain existing behavior
            gen += ('    if (strcmp(pCreateInfo->pProfile->profileName, {0}_NAME) == 0) {{\n'
                    '        std::vector<const char*> extensions;\n'
                    '        _vpGetExtensions(pCreateInfo, _vpArraySize(_{0}_EXTENSIONS), &_{0}_EXTENSIONS[0], extensions);\n'
                    '\n'
                    '        void *pNext = const_cast<void*>(pCreateInfo->pCreateInfo->pNext);\n').format(uname)

            # Add profile feature structures if they aren't overridden by application
            features = profile.capabilities.features
            if features:
                genDef = ''
                genCheck = ''
                pNext = None
                for featureStructName, feature in features.items():
                    structDef = self.registry.structs.get(featureStructName)
                    if structDef == None:
                        Log.f("Feature structure '{0}' does not exist".format(featureStructName))

                    if featureStructName == 'VkPhysicalDeviceFeatures':
                        # Special case, as it's wrapped into VkPhysicalDeviceFeatures2
                        featureStructName = 'VkPhysicalDeviceFeatures2'
                        sType = 'VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2'
                    else:
                        varAccessSuffix = '.'
                        sType = structDef.sType

                    profileVarName = 'profile' + featureStructName[2:]
                    genDef += '        {0} {1}{{ {2} }};\n'.format(featureStructName, profileVarName, sType)
                    if pNext != None:
                        genDef += '        {0}.pNext = &{1};\n'.format(profileVarName, pNext)
                    pNext = profileVarName

                    if featureStructName == 'VkPhysicalDeviceFeatures2':
                        genCheck += ('        if (pCreateInfo->pCreateInfo->pEnabledFeatures != nullptr) {{\n'
                                     '            profilePhysicalDeviceFeatures2.features = *pCreateInfo->pCreateInfo->pEnabledFeatures;\n'
                                     '        }}\n'
                                     '        if (pCreateInfo->flags & VP_DEVICE_CREATE_DISABLE_ROBUST_BUFFER_ACCESS_BIT) {{\n'
                                     '            profilePhysicalDeviceFeatures2.features.robustBufferAccess = VK_FALSE;\n'
                                     '        }}\n'
                                     '        if (!_vpHasStructure(pNext, {0}) && pCreateInfo->pCreateInfo->pEnabledFeatures == nullptr) {{\n').format(sType)
                    else:
                        genCheck += '        if (!_vpHasStructure(pNext, {0})) {{\n'.format(sType)
                    genCheck += ('            {0}.pNext = pNext;\n'
                                 '            pNext = &{0};\n'
                                 '        }}\n').format(profileVarName)

                gen += genDef
                gen += '        vpGetProfileStructures(pCreateInfo->pProfile, &{0});\n'.format(pNext)
                gen += genCheck

            gen += ('\n'
                    '        VkDeviceCreateInfo deviceCreateInfo{ VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO };\n'
                    '        deviceCreateInfo.pNext = pNext;\n'
                    '        deviceCreateInfo.queueCreateInfoCount = pCreateInfo->pCreateInfo->queueCreateInfoCount;\n'
                    '        deviceCreateInfo.pQueueCreateInfos = pCreateInfo->pCreateInfo->pQueueCreateInfos;\n'
                    '        deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());\n'
                    '        deviceCreateInfo.ppEnabledExtensionNames = extensions.data();\n')

            if not 'VkPhysicalDeviceFeatures' in features and not 'VkPhysicalDeviceFeatures2' in features:
                gen += ('        VkPhysicalDeviceFeatures enabledFeatures;\n'
                        '        if (pCreateInfo->pCreateInfo->pEnabledFeatures != nullptr) {\n'
                        '            enabledFeatures = *pCreateInfo->pCreateInfo->pEnabledFeatures;\n'
                        '            if (pCreateInfo->flags & VP_DEVICE_CREATE_DISABLE_ROBUST_BUFFER_ACCESS_BIT) {\n'
                        '                enabledFeatures.robustBufferAccess = VK_FALSE;\n'
                        '            }\n'
                        '            deviceCreateInfo.pEnabledFeatures = &enabledFeatures;\n'
                        '        }\n')

            gen += ('        return vkCreateDevice(physicalDevice, &deviceCreateInfo, pAllocator, pDevice);\n'
                    '    } else\n')
            gen += guard.end

        gen += ('    {\n'
                '        return VK_ERROR_UNKNOWN;\n'
                '    }\n'
                '}\n')
        return gen

    def gen_vpGetProfileExtensionProperties(self):
        gen = '\n'
        # TODO: We should probably have separate APIs for device vs instance extensions
        gen += ('VP_INLINE VkResult vpGetProfileExtensionProperties(const VpProfileProperties *pProfile, uint32_t *pPropertyCount,\n'
                '                                                   VkExtensionProperties *pProperties) {\n'
                '    VkResult result = VK_SUCCESS;\n')

        for name, profile in self.profiles.items():
            uname = name.upper()
            if profile.capabilities.extensions:
                guard = ProfilePlatformGuard(self.registry, profile)
                gen += guard.begin
                gen += ('    if (strcmp(pProfile->profileName, {0}_NAME) == 0) {{\n'
                        '        if (pProperties == nullptr) {{\n'
                        '            *pPropertyCount = _vpArraySize(_{0}_EXTENSIONS);\n'
                        '        }} else {{\n'
                        '            if (*pPropertyCount < _vpArraySize(_{0}_EXTENSIONS)) {{\n'
                        '                result = VK_INCOMPLETE;\n'
                        '            }} else {{\n'
                        '                *pPropertyCount = _vpArraySize(_{0}_EXTENSIONS);\n'
                        '            }}\n'
                        '            for (uint32_t i = 0; i < *pPropertyCount; ++i) {{\n'
                        '                pProperties[i] = _{0}_EXTENSIONS[i];\n'
                        '            }}\n'
                        '        }}\n'
                        '    }} else\n').format(uname)
                gen += guard.end

        gen += ('    {\n'
                '        *pPropertyCount = 0;\n'
                '    }\n'
                '    return result;\n'
                '}\n')
        return gen


    def gen_listValue(self, values, isEnum = True):
        gen = ''
        if not isEnum:
            gen += '{ '

        separator = ''
        if values != None and len(values) > 0:
            for value in values:
                gen += separator + str(value)
                if isEnum:
                    separator = ' | '
                else:
                    separator = ', '
        elif isEnum:
            gen += '0'

        if not isEnum:
            gen += ' }'
        return gen


    def gen_assignStructVar(self, structDef, var, values):
        gen = ''
        for member, value in values.items():
            if member in structDef.members:
                if type(value) == dict:
                    # Nested structure
                    memberDef = self.registry.structs.get(structDef.members[member].type)
                    if memberDef != None:
                        gen += self.gen_assignStructVar(memberDef, var + member + '.', value)
                    else:
                        Log.f("Member '{0}' in structure '{1}' is not a struct".format(member, structDef.name))

                elif type(value) == list:
                    # Some sort of list (enums or integer/float list for structure initialization)
                    if structDef.members[member].isArray:
                        # If it's an array we have to generate per-element assignment code
                        for i, v in enumerate(value):
                            gen += '{0}{1}[{2}] = {3};\n'.format(var, member, i, v)
                    else:
                        # For enums and struct initialization, most of the code can be shared
                        gen += '{0}{1} = '.format(var, member)
                        isEnum = isinstance(value[0], str)
                        gen += '{0};\n'.format(self.gen_listValue(value, isEnum))

                elif type(value) == bool:
                    # Boolean
                    gen += '{0}{1} = {2};\n'.format(var, member, 'VK_TRUE' if value else 'VK_FALSE')

                else:
                    # Everything else
                    gen += '{0}{1} = {2};\n'.format(var, member, value)
            else:
                Log.f("No member '{0}' in structure '{1}'".format(member, structDef.name))
        return gen


    def gen_compareStructVar(self, fmt, structDef, deviceVar, profileVar, values):
        gen = ''
        for member, value in values.items():
            if member in structDef.members:
                limittype = structDef.members[member].limittype
                if limittype == 'IGNORE':
                    # Skip this member as we don't know how to validate it
                    continue
                if limittype == 'bitmask':
                    # Compare bitmask by checking if device value contains every bit of profile value
                    comparePredFmt = '(({0} & {1}) == {1})'
                elif limittype == 'max':
                    # Compare max limit by checking if device value is greater than or equal to profile value
                    comparePredFmt = '({0} >= {1})'
                elif limittype == 'min':
                    # Compare min limit by checking if device value is less than or equal to profile value
                    comparePredFmt = '({0} <= {1})'
                elif limittype == 'range':
                    # Compare range limit by checking if device range is larger than or equal to profile range
                    comparePredFmt = [ '({0} <= {1})', '({0} >= {1})' ]
                elif limittype is None or limittype == 'noauto' or limittype == 'struct':
                    # Compare everything else with equality
                    comparePredFmt = '({0} == {1})'
                else:
                    Log.f("Unsupported limittype '{0}' in member '{1}' of structure '{2}'".format(limittype, member, structDef.name))

                if type(value) == dict:
                    # Nested structure
                    memberDef = self.registry.structs.get(structDef.members[member].type)
                    if memberDef != None:
                        gen += self.gen_compareStructVar(fmt, memberDef, deviceVar + member + '.', profileVar + member + '.', value)
                    else:
                        Log.f("Member '{0}' in structure '{1}' is not a struct".format(member, structDef.name))

                elif type(value) == list:
                    # Some sort of list (enums or integer/float list for structure initialization)
                    if structDef.members[member].isArray:
                        # If it's an array we have to generate per-element comparison code
                        for i in range(len(value)):
                            if limittype == 'range':
                                gen += fmt.format(comparePredFmt[i].format('{0}{1}[{2}]'.format(deviceVar, member, i),
                                                                           '{0}{1}[{2}]'.format(profileVar, member, i)))
                            else:
                                gen += fmt.format(comparePredFmt.format('{0}{1}[{2}]'.format(deviceVar, member, i),
                                                                        '{0}{1}[{2}]'.format(profileVar, member, i)))
                    else:
                        # Enum flags and basic structs can be compared directly
                        gen += fmt.format(comparePredFmt.format('{0}{1}'.format(deviceVar, member),
                                                                '{0}{1}'.format(profileVar, member)))

                else:
                    # Everything else
                    gen += fmt.format(comparePredFmt.format('{0}{1}'.format(deviceVar, member),
                                                            '{0}{1}'.format(profileVar, member)))
            else:
                Log.f("No member '{0}' in structure '{1}'".format(member, structDef.name))
        return gen


    def gen_vpGetProfileStructures(self):
        gen = '\n'
        gen += ('VP_INLINE void vpGetProfileStructures(const VpProfileProperties *pProfile, void *pNext) {\n'
                '    if (pProfile == nullptr || pNext == nullptr) return;\n'
                '    VkBaseOutStructure* p = static_cast<VkBaseOutStructure*>(pNext);\n')

        for name, profile in self.profiles.items():
            uname = name.upper()
            guard = ProfilePlatformGuard(self.registry, profile)
            gen += guard.begin
            gen += ('    if (strcmp(pProfile->profileName, {0}_NAME) == 0) {{\n'
                    '        while (p != nullptr) {{\n'
                    '            switch (p->sType) {{\n').format(uname)

            # Generate feature structure data
            features = profile.capabilities.features
            if features:
                for featureStructName in features:
                    structDef = self.registry.structs.get(featureStructName)
                    if structDef == None:
                        Log.f("Feature structure '{0}' does not exist".format(featureStructName))

                    if featureStructName == 'VkPhysicalDeviceFeatures':
                        # Special case, as it's wrapped into VkPhysicalDeviceFeatures2
                        gen += ('                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2: {\n'
                                '                    VkPhysicalDeviceFeatures2* wrap = reinterpret_cast<VkPhysicalDeviceFeatures2*>(p);\n'
                                '                    VkPhysicalDeviceFeatures* features = &wrap->features;\n')
                    elif structDef.sType:
                        gen += '                case {0}: {{\n'.format(structDef.sType)
                        gen += '                    {0}* features = reinterpret_cast<{0}*>(p);\n'.format(featureStructName)
                    else:
                        Log.f("Feature structure '{0}' is not chainable".format(featureStructName))

                    gen += self.gen_assignStructVar(structDef, '                    features->', features[featureStructName])
                    gen += '                } break;\n'

            # Generate property structure data
            properties = profile.capabilities.properties
            if properties:
                for propertyStructName in properties:
                    structDef = self.registry.structs.get(propertyStructName)
                    if structDef == None:
                        Log.f("Properties structure '{0}' does not exist".format(propertyStructName))

                    if propertyStructName == 'VkPhysicalDeviceProperties':
                        # Special case, as it's wrapped into VkPhysicalDeviceProperties2
                        gen += ('                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2: {\n'
                                '                    VkPhysicalDeviceProperties2* wrap = reinterpret_cast<VkPhysicalDeviceProperties2*>(p);\n'
                                '                    VkPhysicalDeviceProperties* props = &wrap->properties;\n')
                    elif structDef.sType:
                        gen += '                case {0}: {{\n'.format(structDef.sType)
                        gen += '                    {0}* props = reinterpret_cast<{0}*>(p);\n'.format(propertyStructName)
                    else:
                        Log.f("Properties structure '{0}' is not chainable".format(propertyStructName))

                    gen += self.gen_assignStructVar(structDef, '                    props->', properties[propertyStructName])
                    gen += '                } break;\n'

            gen += ('                default: break;\n'
                    '            }\n'
                    '            p = p->pNext;\n'
                    '        }\n'
                    '    } else\n')
            gen += guard.end

        gen += ('    {\n'
                '    }\n'
                '}\n')
        return gen


    def gen_vpGetProfileStructureProperties(self):
        gen = '\n'
        gen += ('VP_INLINE VkResult vpGetProfileStructureProperties(const VpProfileProperties *pProfile, uint32_t *pPropertyCount,\n'
                '                                                   VpStructureProperties *pProperties) {\n'
                '    VkResult result = VK_SUCCESS;\n')

        for name, profile in self.profiles.items():
            uname = name.upper()
            if profile.capabilities.features or profile.capabilities.properties:
                guard = ProfilePlatformGuard(self.registry, profile)
                gen += guard.begin
                gen += ('    if (strcmp(pProfile->profileName, {0}_NAME) == 0) {{\n'
                        '        if (pProperties == nullptr) {{\n'
                        '            *pPropertyCount = _vpArraySize(_{0}_STRUCTURE_PROPERTIES);\n'
                        '        }} else {{\n'
                        '            if (*pPropertyCount < _vpArraySize(_{0}_STRUCTURE_PROPERTIES)) {{\n'
                        '                result = VK_INCOMPLETE;\n'
                        '            }} else {{\n'
                        '                *pPropertyCount = _vpArraySize(_{0}_STRUCTURE_PROPERTIES);\n'
                        '            }}\n'
                        '            for (uint32_t i = 0; i < *pPropertyCount; ++i) {{\n'
                        '                pProperties[i] = _{0}_STRUCTURE_PROPERTIES[i];\n'
                        '            }}\n'
                        '        }}\n'
                        '    }} else\n').format(uname)
                gen += guard.end

        gen += ('    {\n'
                '        *pPropertyCount = 0;\n'
                '    }\n'
                '    return result;\n'
                '}\n')
        return gen


    def gen_vpGetProfileFormats(self):
        gen = '\n'
        gen += ('VP_INLINE VkResult vpGetProfileFormats(const VpProfileProperties *pProfile, uint32_t *pFormatCount, VkFormat *pFormats) {\n'
                '    VkResult result = VK_SUCCESS;\n')

        for name, profile in self.profiles.items():
            uname = name.upper()
            if profile.capabilities.formats:
                guard = ProfilePlatformGuard(self.registry, profile)
                gen += guard.begin
                gen += ('    if (strcmp(pProfile->profileName, {0}_NAME) == 0) {{\n'
                        '        if (pFormats == nullptr) {{\n'
                        '            *pFormatCount = _vpArraySize(_{0}_FORMATS);\n'
                        '        }} else {{\n'
                        '            if (*pFormatCount < _vpArraySize(_{0}_FORMATS)) {{\n'
                        '                result = VK_INCOMPLETE;\n'
                        '            }} else {{\n'
                        '                *pFormatCount = _vpArraySize(_{0}_FORMATS);\n'
                        '            }}\n'
                        '            for (uint32_t i = 0; i < *pFormatCount; ++i) {{\n'
                        '                pFormats[i] = _{0}_FORMATS[i].format;\n'
                        '            }}\n'
                        '        }}\n'
                        '    }} else\n').format(uname)
                gen += guard.end

        gen += ('    {\n'
                '        *pFormatCount = 0;\n'
                '    }\n'
                '    return result;\n'
                '}\n')
        return gen


    def gen_vpGetProfileFormatProperties(self):
        gen = '\n'
        gen += ('VP_INLINE void vpGetProfileFormatProperties(const VpProfileProperties *pProfile, VkFormat format, void *pNext) {\n'
                '    if (pProfile == nullptr || pNext == nullptr) return;\n'
                '    VkBaseOutStructure* p = static_cast<VkBaseOutStructure*>(pNext);\n')

        for name, profile in self.profiles.items():
            uname = name.upper()
            if profile.capabilities.formats:
                guard = ProfilePlatformGuard(self.registry, profile)
                gen += guard.begin
                gen += ('    if (strcmp(pProfile->profileName, {0}_NAME) == 0) {{\n'
                        '        for (uint32_t i = 0; i < _vpArraySize(_{0}_FORMATS); ++i) {{\n'
                        '            const VpFormatProperties& props = _{0}_FORMATS[i];\n'
                        '            if (props.format != format) continue;\n'
                        '\n'
                        '            while (p != nullptr) {{\n'
                        '                switch (p->sType) {{\n').format(uname)

                # TODO: Make format properties extensible
                formatPropTypes = [
                    {
                        'sType': 'VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2',
                        'varDecl': 'VkFormatProperties2* pProps = reinterpret_cast<VkFormatProperties2*>(p);',
                        'varRef': 'pProps->formatProperties.',
                        'valCast': 'static_cast<VkFormatFeatureFlags>'
                    },
                    {
                        'sType': 'VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_3_KHR',
                        'varDecl': 'VkFormatProperties3KHR* pProps = reinterpret_cast<VkFormatProperties3KHR*>(p);',
                        'varRef': 'pProps->',
                        'valCast': ''
                    }
                ]
                for types in formatPropTypes:
                    gen += ('                    case {0}: {{\n'
                            '                        {1};\n'
                            '                        {2}linearTilingFeatures = {3}(props.linearTilingFeatures);\n'
                            '                        {2}optimalTilingFeatures = {3}(props.optimalTilingFeatures);\n'
                            '                        {2}bufferFeatures = {3}(props.bufferFeatures);\n'
                            '                    }} break;\n').format(types['sType'], types['varDecl'], types['varRef'], types['valCast'])

                gen += ('                    default: break;\n'
                        '                }\n'
                        '                p = p->pNext;\n'
                        '            }\n'
                        '        }\n'
                        '    } else\n')
                gen += guard.end

        gen += ('    {\n'
                '    }\n'
                '}\n')
        return gen


    def gen_vpGetProfileMemoryTypes(self):
        gen = '\n'
        gen += ('VP_INLINE VkResult vpGetProfileMemoryTypes(const VpProfileProperties *pProfile, uint32_t *pMemoryPropertyFlagsCount,\n'
                '                                           VkMemoryPropertyFlags *pMemoryPropertyFlags) {\n'
                '    VkResult result = VK_SUCCESS;\n')

        for name, profile in self.profiles.items():
            uname = name.upper()
            if profile.capabilities.memoryProperties:
                guard = ProfilePlatformGuard(self.registry, profile)
                gen += guard.begin
                gen += ('    if (strcmp(pProfile->profileName, {0}_NAME) == 0) {{\n'
                        '        if (pMemoryPropertyFlags == nullptr) {{\n'
                        '            *pMemoryPropertyFlagsCount = _vpArraySize(_{0}_MEMORY_TYPES);\n'
                        '        }} else {{\n'
                        '            if (*pMemoryPropertyFlagsCount < _vpArraySize(_{0}_MEMORY_TYPES)) {{\n'
                        '                result = VK_INCOMPLETE;\n'
                        '            }} else {{\n'
                        '                *pMemoryPropertyFlagsCount = _vpArraySize(_{0}_MEMORY_TYPES);\n'
                        '            }}\n'
                        '            for (uint32_t i = 0; i < *pMemoryPropertyFlagsCount; ++i) {{\n'
                        '                pMemoryPropertyFlags[i] = _{0}_MEMORY_TYPES[i];\n'
                        '            }}\n'
                        '        }}\n'
                        '    }} else\n').format(uname)
                gen += guard.end

        gen += ('    {\n'
                '        *pMemoryPropertyFlagsCount = 0;\n'
                '    }\n'
                '    return result;\n'
                '}\n')
        return gen


    def gen_vpGetProfileQueueFamilies(self):
        gen = '\n'
        gen += ('VP_INLINE VkResult vpGetProfileQueueFamilies(const VpProfileProperties *pProfile, uint32_t *pQueueFamilyPropertiesCount,\n'
                '                                             VkQueueFamilyProperties *pQueueFamilyProperties) {\n'
                '    VkResult result = VK_SUCCESS;\n')

        for name, profile in self.profiles.items():
            uname = name.upper()
            if profile.capabilities.queueFamiliesProperties:
                guard = ProfilePlatformGuard(self.registry, profile)
                gen += guard.begin
                gen += ('    if (strcmp(pProfile->profileName, {0}_NAME) == 0) {{\n'
                        '        if (pQueueFamilyProperties == nullptr) {{\n'
                        '            *pQueueFamilyPropertiesCount = _vpArraySize(_{0}_QUEUE_FAMILY_PROPERTIES);\n'
                        '        }} else {{\n'
                        '            if (*pQueueFamilyPropertiesCount < _vpArraySize(_{0}_QUEUE_FAMILY_PROPERTIES)) {{\n'
                        '                result = VK_INCOMPLETE;\n'
                        '            }} else {{\n'
                        '                *pQueueFamilyPropertiesCount = _vpArraySize(_{0}_QUEUE_FAMILY_PROPERTIES);\n'
                        '            }}\n'
                        '            for (uint32_t i = 0; i < *pQueueFamilyPropertiesCount; ++i) {{\n'
                        '                pQueueFamilyProperties[i] = _{0}_QUEUE_FAMILY_PROPERTIES[i];\n'
                        '            }}\n'
                        '        }}\n'
                        '    }} else\n').format(uname)
                gen += guard.end

        gen += ('    {\n'
                '        *pQueueFamilyPropertiesCount = 0;\n'
                '    }\n'
                '    return result;\n'
                '}\n')
        return gen


if __name__ == '__main__':
    parser = argparse.ArgumentParser()

    # TODO: Remove temporary default values used for testing purposes only
    parser.add_argument('-registry', action='store',
                        default='./library/scripts/vk.xml',
                        help='Use specified registry file instead of vk.xml')
    parser.add_argument('-profiles', action='store',
                        default='./profiles',
                        help='Generate based on profiles in the specified directory')
    parser.add_argument('-outIncDir', action='store',
                        default='./library/include/vulkan',
                        help='Output include directory')
    parser.add_argument('-outSrcDir', action='store',
                        default='./library/source',
                        help='Output source directory')

    args = parser.parse_args()

    registry = VulkanRegistry(args.registry)
    profiles = VulkanProfiles.loadFromDir(registry, args.profiles)
    builder = VulkanProfilesBuilder(registry, profiles)
    builder.generate(args.outIncDir, args.outSrcDir)
