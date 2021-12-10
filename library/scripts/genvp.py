#!/usr/bin/python3

import os
import re
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
void vpGetProfiles(uint32_t *pPropertyCount, VpProfileProperties *pProperties);

// List the recommended fallback profiles of a profile
void vpGetProfileFallbacks(const VpProfileProperties *pProfile, uint32_t *pPropertyCount, VpProfileProperties *pProperties);

// Check whether a profile is supported by the physical device
VkResult vpGetDeviceProfileSupport(VkPhysicalDevice physicalDevice, const char *pLayerName, const VpProfileProperties *pProfile,
                                   VkBool32 *pSupported);

// Create a VkDevice with the profile features and extensions enabled
VkResult vpCreateDevice(VkPhysicalDevice physicalDevice, const VpDeviceCreateInfo *pCreateInfo,
                        const VkAllocationCallbacks *pAllocator, VkDevice *pDevice);

// Query the list of extension of a profile
void vpGetProfileExtensionProperties(const VpProfileProperties *pProfile, uint32_t *pPropertyCount,
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
void vpGetProfileStructureProperties(const VpProfileProperties *pProfile, uint32_t *pPropertyCount,
                                     VpStructureProperties *pProperties);

// Query the list of formats with specified requirements by a profile
void vpGetProfileFormats(const VpProfileProperties *pProfile, uint32_t *pFormatCount, VkFormat *pFormats);

// Query the requirements of a format for a profile
void vpGetProfileFormatProperties(const VpProfileProperties *pProfile, VkFormat format, void *pNext);

// Query the requirements of memory types by a profile
void vpGetProfileMemoryTypes(const VpProfileProperties *pProfile, uint32_t *pMemoryTypeCount, VkMemoryPropertyFlags *pMemoryTypes);

// Query the requirements of queue families by a profile
void vpGetProfileQueueFamilies(const VpProfileProperties *pProfile, uint32_t *pPropertyCount, VkQueueFamilyProperties *pProperties);


#ifdef __cplusplus
}
#endif

#endif//VULKAN_PROFILES_
'''

class VulkanProfilesBuilder():
    def __init__(self, registryFile, profilesDir):
        self.loadRegistryFile(registryFile)
        self.loadProfileFiles(profilesDir)
        self.crossValidate()


    def logMsg(self, level, *args):
        if level == 'info':
            print(*args)
        elif level == 'warn':
            print('WARNING:' *args)
        elif level == 'error':
            print('ERROR:' *args)
            raise Exception(*args)
        else:
            raise Exception("Unexpected log level '{0}'".format(level))


    def loadRegistryFile(self, registryFile):
        self.logMsg('info', "Loading registry file: '{0}'".format(registryFile))
        xml = etree.parse(registryFile)
        self.registry = dict()
        self.parsePlatformInfo(xml)
        self.parseExtensionInfo(xml)


    def parsePlatformInfo(self, xml):
        self.registry['platforms'] = dict()
        for plat in xml.findall("./platforms/platform"):
            self.registry['platforms'][plat.get('name')] = {
                'protect': plat.get('protect')
            }


    def parseExtensionInfo(self, xml):
        self.registry['extensions'] = dict()
        for ext in xml.findall("./extensions/extension"):
            if ext.get('supported') == "vulkan":
                name = ext.get('name')

                foundNameEnum = False
                matches = ext.findall("./require/enum[@value='\"" + name + "\"']")
                for match in matches:
                    if match.get('name').endswith("_EXTENSION_NAME"):
                        self.registry['extensions'][name] = {
                            'upperCaseName': match.get('name')[:-len("_EXTENSION_NAME")]
                        }
                        foundNameEnum = True
                        break

                if foundNameEnum:
                    platform = ext.get('platform')
                    if platform != None:
                        self.registry['extensions'][name]['platform'] = platform
                else:
                    self.logMsg('error', "Cannot find name enum for extension '{0}'".format(name))
                    raise()


    def loadProfileFiles(self, profilesDir):
        self.profiles = dict()
        dirAbsPath = os.path.abspath(profilesDir)
        filenames = os.listdir(dirAbsPath)
        for filename in filenames:
            fileAbsPath = os.path.join(dirAbsPath, filename)
            if os.path.isfile(fileAbsPath) and os.path.splitext(filename)[-1] == '.json':
                self.logMsg('info', "Loading profile file: '{0}'".format(filename))
                with open(fileAbsPath, 'r') as f:
                    jsonData = json.load(f)
                    caps = self.parseCapabilities(jsonData['capabilities'])
                    self.parseProfiles(jsonData['profiles'], caps)


    def parseCapabilities(self, capData):
        caps = dict()
        for cap in capData:
            name = cap['name']
            if not name in caps:
                caps[name] = cap
            else:
                self.logMsg('error', "Capability with name '{0}' already exists".format(name))
        return caps


    def parseProfiles(self, profiles, caps):
        for profile in profiles:
            name = profile['name']
            if not name in self.profiles:
                self.logMsg('info', "Registering profile '{0}'".format(name))
                self.profiles[name] = profile
                self.profiles[name]['capabilities'] = self.mergeProfileCaps(profile, caps)
            else:
                self.logMsg('error', "Profile with name '{0}' already exists".format(name))


    def mergeProfileCaps(self, profile, caps):
        mergedCaps = dict()
        for capName in profile['capabilities']:
            if capName in caps:
                self.mergeProfileExtensions(mergedCaps, caps[capName])
                self.mergeProfileFeatures(mergedCaps, caps[capName])
                self.mergeProfileQueueFamiliesProperties(mergedCaps, caps[capName])
                self.mergeProfileMemoryProperties(mergedCaps, caps[capName])
            else:
                self.logMsg('error', "Capability '{0}' needed by profile '{1}' is missing".format(capName, profile['name']))
        return mergedCaps


    def mergeProfileExtensions(self, dst, src):
        if src.get('extensions') != None:
            if dst.get('extensions') == None:
                dst['extensions'] = dict()
            for ext in src['extensions']:
                name = ext['extensionName']
                dst['extensions'][name] = ext


    def mergeProfileFeatures(self, dst, src):
        if src.get('features') != None:
            if dst.get('features') == None:
                dst['features'] = dict()
            for feats in src['features']:
                for key in feats:
                    dst['features'][key] = feats[key]


    def mergeProfileProperties(self, dst, src):
        if src.get('properties') != None:
            if dst.get('properties') == None:
                dst['properties'] = dict()
            for props in src['properties']:
                for key in props:
                    dst['properties'][key] = props[key]


    def mergeProfileFormats(self, dst, src):
        if src.get('formats') != None:
            if dst.get('formats') == None:
                dst['formats'] = dict()
            for fmt in src['formats']:
                for key in fmt:
                    dst['formats'][key] = dict()
                    for props in fmt[key]:
                        for key2 in props:
                            dst['formats'][key][key2] = props[key2]


    def mergeProfileQueueFamiliesProperties(self, dst, src):
        if src.get('queueFamiliesProperties') != None:
            if dst.get('queueFamiliesProperties') == None:
                dst['queueFamiliesProperties'] = dict()
            for props in src['queueFamiliesProperties']:
                for key in props:
                    dst['queueFamiliesProperties'][key] = props[key]


    def mergeProfileMemoryProperties(self, dst, src):
        if src.get('memoryProperties') != None:
            if dst.get('memoryProperties') == None:
                dst['memoryProperties'] = dict()
            for props in src['memoryProperties']:
                for key in props:
                    dst['memoryProperties'][key] = props[key]


    def crossValidate(self):
        for profileName in self.profiles:
            self.crossValidateProfileExtensions(self.profiles[profileName])


    def crossValidateProfileExtensions(self, profile):
        if profile['capabilities']['extensions'] != None:
            for extName, ext in profile['capabilities']['extensions'].items():
                if extName in self.registry['extensions']:
                    platform = self.registry['extensions'][extName].get('platform')
                    if platform != None:
                        profile['platform'] = platform
                else:
                    self.logMsg('error', "Extension '{0}' required by profile '{1}' does not exist".format(extName, profile['name']))


    def generate(self, outIncDir, outSrcDir):
        self.generate_h(outIncDir)
        self.generate_cpp(outSrcDir)


    def generate_h(self, outDir):
        fileAbsPath = os.path.join(os.path.abspath(outDir), 'vulkan_profiles.h')
        self.logMsg('info', "Generating '{0}'...".format(fileAbsPath))
        with open(fileAbsPath, 'w') as f:
            f.write(COPYRIGHT_HEADER)
            f.write(C_HEADER_PRE_DEFS)
            f.write(self.gen_profileDefs())
            f.write(C_HEADER_POST_DEFS)


    def generate_cpp(self, outDir):
        fileAbsPath = os.path.join(os.path.abspath(outDir), 'vulkan_profiles.cpp')
        self.logMsg('info', "Generating '{0}'...".format(fileAbsPath))
        with open(fileAbsPath, 'w') as f:
            f.write(COPYRIGHT_HEADER)
            f.write('\n#include <vulkan/vulkan_profiles.h>\n')
            f.write('\n#include <algorithm>\n')
            f.write(self.gen_extensionLists())

            f.write(self.gen_vpGetProfiles())
            f.write(self.gen_vpGetProfileFallbacks())
            f.write(self.gen_vpGetDeviceProfileSupport())
            f.write(self.gen_vpCreateDevice())
            f.write(self.gen_vpGetProfileExtensionProperties())
            f.write(self.gen_vpGetProfileStructures())
            f.write(self.gen_vpGetProfileStructureProperties())
            f.write(self.gen_vpGetProfileFormats())
            f.write(self.gen_vpGetProfileFormatProperties())
            f.write(self.gen_vpGetProfileMemoryTypes())
            f.write(self.gen_vpGetProfileQueueFamilies())


    def gen_profileDefs(self):
        gen = ''
        for name, profile in self.profiles.items():
            uname = name.upper()
            gen += '\n'
            gen += '#define {0} 1\n'.format(name)
            gen += '#define {0}_NAME "{1}"\n'.format(uname, name)
            gen += '#define {0}_SPEC_VERSION {1}\n'.format(uname, profile['version'])
            gen += '#define {0}_MIN_API_VERSION VK_MAKE_VERSION({1})\n'.format(uname, profile['api-version'].replace(".", ", "))
        return gen


    def gen_extensionLists(self):
        gen = ''
        for name, profile in self.profiles.items():
            if profile['capabilities']['extensions'] != None:
                gen += '\n'

                platform = profile.get('platform')
                if platform != None:
                    platformDef = self.registry['platforms'][platform]['protect']
                    gen += '#ifdef {0}\n'.format(platformDef)

                gen += 'static const VkExtensionProperties _{0}_EXTENSIONS[] = {{\n'.format(name.upper())
                for extName, ext in profile['capabilities']['extensions'].items():
                    extInfo = self.registry['extensions'][extName]
                    gen += '    VkExtensionProperties{{ {0}_EXTENSION_NAME, {1} }},\n'.format(extInfo['upperCaseName'], ext['specVersion'])
                gen += '};\n'

                if platform != None:
                    gen += '#endif\n'
        return gen


    def gen_vpGetProfiles(self):
        gen = '\n'
        gen += ('VP_INLINE void vpGetProfiles(uint32_t *pPropertyCount, VpProfileProperties *pProperties) {\n'
                '    static const VpProfileProperties profiles[] = {\n')

        for name, profile in self.profiles.items():
            gen += '        {{ {0}_NAME, {0}_SPEC_VERSION }},\n'.format(name.upper())

        gen += ('    };\n'
                '    const uint32_t profilesCount = static_cast<uint32_t>(sizeof(profiles) / sizeof(profiles[0]));\n'
                '\n'
                '    if (pProperties == nullptr) {\n'
                '        *pPropertyCount = profilesCount;\n'
                '        return;\n'
                '    }\n'
                '\n'
                '    *pPropertyCount = std::min(profilesCount, *pPropertyCount);\n'
                '    for (uint32_t i = 0; i < *pPropertyCount; ++i) {\n'
                '        pProperties[i] = profiles[i];\n'
                '    }\n'
                '}\n')
        return gen


    def gen_vpGetProfileFallbacks(self):
        gen = '\n'
        gen += 'VP_INLINE void vpGetProfileFallbacks(const VpProfileProperties *pProfile, uint32_t *pPropertyCount, VpProfileProperties *pProperties) {\n'

        for name, profile in self.profiles.items():
            uname = name.upper()
            if profile.get('fallback') != None:
                gen += ('    if (strcmp(pProfile->profileName, {0}_NAME) == 0) {{\n'
                        '        static const VpProfileProperties {0}_fallbacks[] = {{\n').format(uname)
                for fallback in profile['fallback']:
                    gen += '            {{ {0}_NAME, {0}_SPEC_VERSION }},\n'.format(fallback.upper())
                gen += ('        }};\n'
                        '        const uint32_t {0}_fallbacksCount = static_cast<uint32_t>(sizeof({0}_fallbacks) / sizeof({0}_fallbacks[0]));\n'
                        '\n'
                        '        if (pProperties == nullptr) {{\n'
                        '            *pPropertyCount = {0}_fallbacksCount;\n'
                        '            return;\n'
                        '        }}\n'
                        '\n'
                        '        *pPropertyCount = std::min({0}_fallbacksCount, *pPropertyCount);\n'
                        '        for (uint32_t i = 0; i < *pPropertyCount; ++i) {{\n'
                        '            pProperties[i] = {0}_fallbacks[i];\n'
                        '        }}\n'
                        '    }} else\n').format(uname)

        gen += ('    {\n'
                '        *pPropertyCount = 0;\n'
                '    }\n'
                '}\n')
        return gen


    def gen_vpGetDeviceProfileSupport(self):
        gen = '\n'
        gen += ('VP_INLINE VkResult vpGetDeviceProfileSupport(VkPhysicalDevice physicalDevice, const char *pLayerName,\n'
                '                                             const VpProfileProperties *pProfile, VkBool32 *pSupported) {\n')
        # TODO: implement this
        gen += '    return VK_ERROR_INITIALIZATION_FAILED;\n'
        gen += '}\n'
        return gen


    def gen_vpCreateDevice(self):
        gen = '\n'
        gen += ('VP_INLINE VkResult vpCreateDevice(VkPhysicalDevice physicalDevice, const VpDeviceCreateInfo *pCreateInfo,\n'
                '                                  const VkAllocationCallbacks *pAllocator, VkDevice *pDevice) {\n')
        # TODO: implement this
        gen += '    return VK_ERROR_INITIALIZATION_FAILED;\n'
        gen += '}\n'
        return gen

    def gen_vpGetProfileExtensionProperties(self):
        gen = '\n'
        gen += ('VP_INLINE void vpGetProfileExtensionProperties(const VpProfileProperties *pProfile, uint32_t *pPropertyCount,\n'
                '                                               VkExtensionProperties *pProperties) {\n')
        # TODO: implement this
        gen += '}\n'
        return gen


    def gen_vpGetProfileStructures(self):
        gen = '\n'
        gen += 'VP_INLINE void vpGetProfileStructures(const VpProfileProperties *pProfile, void *pNext) {\n'
        # TODO: implement this
        gen += '}\n'
        return gen


    def gen_vpGetProfileStructureProperties(self):
        gen = '\n'
        gen += ('VP_INLINE void vpGetProfileStructureProperties(const VpProfileProperties *pProfile, uint32_t *pPropertyCount,\n'
                '                                               VpStructureProperties *pProperties) {\n')
        # TODO: implement this
        gen += '}\n'
        return gen


    def gen_vpGetProfileFormats(self):
        gen = '\n'
        gen += 'VP_INLINE void vpGetProfileFormats(const VpProfileProperties *pProfile, uint32_t *pFormatCount, VkFormat *pFormat) {\n'
        # TODO: implement this
        gen += '}\n'
        return gen


    def gen_vpGetProfileFormatProperties(self):
        gen = '\n'
        gen += 'VP_INLINE void vpGetProfileFormatProperties(const VpProfileProperties *pProfile, VkFormat format, void *pNext) {\n'
        # TODO: implement this
        gen += '}\n'
        return gen


    def gen_vpGetProfileMemoryTypes(self):
        gen = '\n'
        gen += ('VP_INLINE void vpGetProfileMemoryTypes(const VpProfileProperties *pProfile, uint32_t *pMemoryPropertyFlagsCount,\n'
                '                                       VkMemoryPropertyFlags *pMemoryPropertyFlags) {\n')
        # TODO: implement this
        gen += '}\n'
        return gen


    def gen_vpGetProfileQueueFamilies(self):
        gen = '\n'
        gen += ('VP_INLINE void vpGetProfileQueueFamilies(const VpProfileProperties *pProfile, uint32_t *pQueueFamilyPropertiesCount,\n'
                '                                         VkQueueFamilyProperties *pQueueFamilyProperties) {\n')
        # TODO: implement this
        gen += '}\n'
        return gen


if __name__ == '__main__':
    parser = argparse.ArgumentParser()

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

    builder = VulkanProfilesBuilder(args.registry, args.profiles)
    builder.generate(args.outIncDir, args.outSrcDir)
