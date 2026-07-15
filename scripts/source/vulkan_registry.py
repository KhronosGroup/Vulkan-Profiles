#!/usr/bin/python3
#
# Copyright (c) 2021-2026 LunarG, Inc.
# Copyright (c) 2023-2024 RasterGrid Kft.
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
# - Daniel Rakos <daniel.rakos@rastergrid.com>
# - Christophe Riccio <christophe@lunarg.com>

import os
import re
import copy
import xml.etree.ElementTree as etree
from collections import deque
from typing import OrderedDict

from source.log import Log

class VulkanVersionNumber():
    def __init__(self, versionStr, targetApi = None, versionName = None):
        match = re.search(r"^([1-9][0-9]*)\.([0-9]+)$", versionStr)
        if match != None:
            # Only major and minor version specified
            self.major = int(match.group(1))
            self.minor = int(match.group(2))
            self.patch = None
        else:
            # Otherwise expect major, minor, and patch version
            match = re.search(r"^([1-9][0-9]*)\.([0-9]+)\.([0-9]+)$", versionStr)
            if match != None:
                self.major = int(match.group(1))
                self.minor = int(match.group(2))
                self.patch = int(match.group(3))
            else:
                Log.f("Invalid API version string: '{0}'".format(versionStr))

        # Construct version number pre-processor definition's name
        if targetApi == 'vulkan':
            self.versionName = 'VK_VERSION_{0}_{1}'.format(self.major, self.minor)
            self.versionMacro = 'VK_API_VERSION_{0}_{1}'.format(self.major, self.minor)
            self.versionStructSuffic = '{0}{1}'.format(self.major, self.minor)

        elif targetApi is not None:
            Log.f("Unknown target API '{0}'".format(targetApi))

    def get_api_version_string(self):
        return 'VK_API_VERSION_' + str(self.major) + '_' + str(self.minor)

    def __eq__(self, other):
        if isinstance(other, VulkanVersionNumber):
            # Only consider major and minor version in comparison
            return self.major == other.major and self.minor == other.minor
        else:
            return False

    def __gt__(self, other):
        # Only consider major and minor version in comparison
        return self.major > other.major or (self.major == other.major and self.minor > other.minor)

    def __lt__(self, other):
        # Only consider major and minor version in comparison
        return self.major < other.major or (self.major == other.major and self.minor < other.minor)

    def __ne__(self, other):
        return not self.__eq__(other)

    def __ge__(self, other):
        return self.__eq__(other) or self.__gt__(other)

    def __le__(self, other):
        return self.__eq__(other) or self.__lt__(other)

    def __str__(self):
        if self.patch != None:
            return '{0}.{1}.{2}'.format(self.major, self.minor, self.patch)
        else:
            return '{0}.{1}'.format(self.major, self.minor)

class VulkanPlatform():
    def __init__(self, data):
        self.name = data.get('name')
        self.protect = data.get('protect')

class VulkanDefinitionScope():
    def parseAliases(self, xml):
        self.sTypeAliases = dict()
        for sTypeAlias in xml.findall("./require/enum[@alias]"):
            if re.search(r'^VK_STRUCTURE_TYPE_.*', sTypeAlias.get('name')):
                self.sTypeAliases[sTypeAlias.get('alias')] = sTypeAlias.get('name')

class VulkanVersion(VulkanDefinitionScope):
    def __init__(self, xml, targetApi):
        self.name = xml.get('name')
        self.number = VulkanVersionNumber(xml.get('number'), targetApi, self.name)
        self.extensions = []
        self.features = dict()
        self.limits = dict()
        self.parseAliases(xml)

class VulkanExtension(VulkanDefinitionScope):
    def __init__(self, xml, upperCaseName):
        self.name = xml.get('name')
        self.upperCaseName = upperCaseName
        self.type = xml.get('type')
        self.features = dict()
        self.limits = dict()
        self.platform = xml.get('platform')
        self.provisional = xml.get('provisional')
        self.promotedTo = xml.get('promotedto').split(',') if xml.get('promotedto') is not None else []
        self.obsoletedBy = xml.get('obsoletedby')
        self.deprecatedBy = xml.get('deprecatedby')
        self.spec_version = 1
        for e in xml.findall("./require/enum"):
            if (e.get('name').endswith("SPEC_VERSION")):
                self.spec_version = e.get('value')
                break
        self.parseAliases(xml)


class VulkanStructMember():
    def __init__(self, name, type, limittype, isArray = False):
        self.name = name
        self.type = type
        self.limittype = limittype
        self.isArray = isArray
        self.arraySizeMember = None
        self.nullTerminated = False
        self.arraySize = None
        self.arraySizeCap = None

    def isDynamicallySizedArrayWithCap(self):
            return self.isArray and self.arraySizeCap is not None

class VulkanStruct():
    def __init__(self, name):
        self.name = name
        self.sType = None
        self.extends = []
        self.members = OrderedDict()
        self.aliases = [ name ]
        self.isAlias = False
        self.definedByVersion = None
        self.definedByExtensions = []
        self.isBeta = None


class VulkanEnum():
    def __init__(self, name):
        self.name = name
        self.aliases = [ name ]
        self.isAlias = False
        self.values = []
        self.aliasValues = dict()


class VulkanBitmask():
    def __init__(self, name):
        self.name = name
        self.aliases = [ name ]
        self.isAlias = False
        self.bitsType = None


class VulkanFeature():
    def __init__(self, name):
        self.name = name
        self.structs = set()


class VulkanLimit():
    def __init__(self, name):
        self.name = name
        self.structs = set()


class VulkanVideoRequiredCapabilities():
    def __init__(self, struct, member, value):
        self.struct = struct
        self.member = member
        self.value = value


class VulkanVideoFormat():
    def __init__(self, name, usage):
        self.name = name
        self.usage = usage
        self.properties = OrderedDict()
        self.requiredCaps = list()
        super().__init__()

    def matchesImageUsageFlags(self, flags):
        # Check if the specified list of VkImageUsageFlags matches the usage criteria
        # for this video format category
        return evalConditionFromList(self.usage, flags)

    def hasRequiredCapabilities(self, videoCapabilities, registry):
        hasAllRequiredCaps = True
        for requiredCap in self.requiredCaps:
            capabilitiesData = None
            if requiredCap.struct in videoCapabilities:
                capabilitiesData = videoCapabilities[requiredCap.struct]
            else:
                # Check also for possible aliases
                for alias in registry.structs[requiredCap.struct].aliases:
                    if alias in videoCapabilities:
                        capabilitiesData = videoCapabilities[alias]

            if capabilitiesData is not None:
                if requiredCap.member in capabilitiesData:
                    value = capabilitiesData[requiredCap.member]
                    if isinstance(value, list):
                        hasAllRequiredCaps = evalConditionFromList(requiredCap.value, value)
                    else:
                        hasAllRequiredCaps = (requiredCap.value == value)
                else:
                    # Required capability structure member is missing
                    hasAllRequiredCaps = False
            else:
                # Entire required capability structure is missing
                hasAllRequiredCaps = False
        return hasAllRequiredCaps

class VulkanVideoProfileStructMember():
    def __init__(self, name):
        self.name = name
        self.values = OrderedDict()

class VulkanVideoProfileStruct():
    def __init__(self, struct):
        self.struct = struct
        self.members = OrderedDict()

class VulkanVideoCodec():
    def __init__(self, name, extend = None, value = None):
        self.name = name
        self.value = value
        self.profileStructs = OrderedDict()
        self.capabilities = OrderedDict()
        self.formats = OrderedDict()
        if extend is not None:
            self.profileStructs = copy.deepcopy(extend.profileStructs)
            self.capabilities = copy.deepcopy(extend.capabilities)
            self.formats = copy.deepcopy(extend.formats)

    def isSpecific(self):
        return self.value is not None

    def getVideoFormatCategoriesForFormat(self, videoFormat, videoCapabilities, registry):
        result = list()

        baseProps = registry.getBaseVideoFormatPropertiesFromVideoFormat(videoFormat)

        # Find the the video format categories the video format belongs to
        if 'imageUsageFlags' in baseProps:
            foundVideoFormatCategory = False
            hadMatchWithMissingPrerequisities = None
            for videoFormatCategory in self.formats.values():
                # Check if the video format matches the image usage requirements of the video format category
                if not videoFormatCategory.matchesImageUsageFlags(baseProps['imageUsageFlags']):
                    continue
                # Make sure that the video profile has the required capabilites for this video format category
                if not videoFormatCategory.hasRequiredCapabilities(videoCapabilities, registry):
                    hadMatchWithMissingPrerequisities = videoFormatCategory
                    continue
                # This video format does indeed fall into this video format category
                foundVideoFormatCategory = True
                result.append(videoFormatCategory)
            if not foundVideoFormatCategory:
                if hadMatchWithMissingPrerequisities is not None:
                    Log.e("Video format from category {0} with missing prerequisites:\n{1}".format(hadMatchWithMissingPrerequisities.name, json.dumps(videoFormat, indent=4)))
                else:
                    Log.e("Unrecognized video format category for imageUsageFlags in video format:\n{0}".format(json.dumps(videoFormat, indent=4)))
        else:
            Log.f("Missing imageUsageFlags from video format:\n{0}".format(json.dumps(videoFormat, indent=4)))

        return result

class VulkanDefinitions():
    def __init__(self):
        self.enums = set()
        self.types = set()

    def add(self, elements):
        for element in elements:
            for enum in element.findall("./enum"):
                self.enums.add(enum.get('name'))
            for type in element.findall("./type"):
                self.types.add(type.get('name'))

    def addDependencies(self, xml, targetApi):
        # Add types that are required by required types as dependency
        for type in xml.findall("./types/type[@requires]"):
            apiList = type.get('api')

            # Skip dependency if it does not apply to the target API
            if apiList is not None and not targetApi in apiList.split(','):
                continue

            name = type.find('./name')
            if name is not None and name.text in self.types:
                self.types.add(type.get('requires'))

        # Add types that contain the definition of required alias types as dependency
        for type in xml.findall("./types/type[@alias]"):

            # Skip dependency if it does not apply to the target API
            if apiList is not None and not targetApi in apiList.split(','):
                continue

            name = type.get('name')
            if name in self.types:
                self.types.add(type.get('alias'))

def apiNameMatch(str, supported):
    """Return whether a required api name matches a pattern specified for an
    XML <feature> 'api' attribute or <extension> 'supported' attribute.
    - str - API name such as 'vulkan' or 'openxr'. May be None, in which
        case it never matches (this should not happen).
    - supported - comma-separated list of XML API names. May be None, in
        which case str always matches (this is the usual case)."""

    if str is not None:
        return supported is None or str in supported.split(',')

    # Fallthrough case - either str is None or the test failed
    return False

def stripNonmatchingAPIs(tree, apiName, actuallyDelete = True):
    """Remove tree Elements with 'api' attributes matching apiName.
        tree - Element at the root of the hierarchy to strip. Only its
            children can actually be removed, not the tree itself.
        apiName - string which much match a command-separated component of
            the 'api' attribute.
        actuallyDelete - only delete matching elements if True."""

    stack = deque()
    stack.append(tree)

    while len(stack) > 0:
        parent = stack.pop()

        for child in parent.findall('*'):
            api = child.get('api')

            if apiNameMatch(apiName, api):
                # Add child to the queue
                stack.append(child)
            elif not apiNameMatch(apiName, api):
                # Child does not match requested api. Remove it.
                if actuallyDelete:
                    parent.remove(child)

# Dynamic arrays are ill-formed, but some of them still have a maximum size that can be used
struct_with_valid_dynamic_array = ["VkQueueFamilyGlobalPriorityProperties"]
# These dynamic arrays have a known maximum possible size
struct_with_dynamic_array_size_cap = ["VkPhysicalDeviceHostImageCopyProperties", "VkPhysicalDeviceHostImageCopyPropertiesEXT", "VkPhysicalDeviceVulkan14Properties"]

# Evaluates that a condition is satisfied per the specified list of values
# e.g.:
#   condition = '(A+B),C'
#   evaluates to True for values = [ 'A', 'B' ]
#   evaluates to True for values = [ 'C' ]
#   evaluates to False for values = [ 'A' ]
#   evaluates to False for values = [ 'B' ]
def evalConditionFromList(condition, values):
    evalstr = ""
    value = ""

    def genExpressionFromValue(value):
        return value if value == "" else "('{0}' in values)".format(value)

    for char in condition:
        if char in ['(', ')', '+', ',']:
            evalstr += genExpressionFromValue(value)
            value = ""
            if char == '+':
                # '+' means AND
                evalstr += ' and '
            elif char == ',':
                # ',' means OR
                evalstr += ' or '
            else:
                evalstr += char
        else:
            value += char
    evalstr += genExpressionFromValue(value)

    return eval(evalstr)

class VulkanRegistry():
    def __init__(self, registryFile, api = 'vulkan'):
        Log.i("Loading registry file: '{0}'".format(registryFile))
        xml = etree.parse(registryFile)
        stripNonmatchingAPIs(xml.getroot(), api, actuallyDelete = True)

        videoRegistryFile = registryFile.replace('vk.xml', 'video.xml')
        if os.path.isfile(videoRegistryFile):
            Log.i("Loading video registry file: '{0}'".format(videoRegistryFile))
            videoxml = etree.parse(videoRegistryFile)
        else:
            Log.w("Video registry file '{0}' does not exist, building without video support".format(videoRegistryFile))
            videoxml = None

        self.api = api
        self.require = VulkanDefinitions()
        self.remove = VulkanDefinitions()

        self.parsePlatformInfo(xml)
        self.parseVersionInfo(xml)
        self.parseExtensionInfo(xml)

        self.require.addDependencies(xml, self.api)

        self.parseStructInfo(xml)
        self.parsePrerequisites(xml)
        self.parseEnums(xml)
        self.parseFormats(xml)
        self.parseBitmasks(xml)
        self.parseConstants(xml)
        self.parseAliases(xml)
        self.parseExternalTypes(xml)
        self.parseFeatures(xml)
        self.parseLimits(xml)
        self.parseHeaderVersion(xml)
        self.parseVideoCodecs(xml, videoxml)
        self.applyWorkarounds()

    def findAllFeatures(self, xml, xpath = None):
        results = []
        for feature in xml.findall("./feature"):
            apiList = feature.get('api')
            if self.api in apiList.split(','):
                if xpath is None:
                    results.append(feature)
                else:
                    results.extend(feature.findall(xpath))
        return results

    def findAllExtensions(self, xml, xpath = None):
        results = []
        for extension in xml.findall("./extensions/extension"):
            apiList = extension.get('supported')
            if self.api in apiList.split(','):
                if xpath is None:
                    results.append(extension)
                else:
                    results.extend(extension.findall(xpath))
        return results

    def parseRequireRemove(self, xml):
        self.require.add(xml.findall("./require"))
        self.remove.add(xml.findall("./remove"))

    def parsePlatformInfo(self, xml):
        self.platforms = dict()
        for plat in xml.findall("./platforms/platform"):
            self.platforms[plat.get('name')] = VulkanPlatform(plat)

    def parseVersionInfo(self, xml):
        self.versions = dict()
        for feature in self.findAllFeatures(xml):
            if re.search(r"^[1-9][0-9]*\.[0-9]+$", feature.get('number')):
                self.versions[feature.get('name')] = VulkanVersion(feature, self.api)
                self.parseRequireRemove(feature)
            else:
                Log.f("Unsupported feature with number '{0}'".format(feature.get('number')))

    def parseExtensionInfo(self, xml):
        self.extensions = dict()
        for ext in self.findAllExtensions(xml):
            name = ext.get('name')

            # Find name enum (due to inconsistencies in lower case and upper case names this is non-trivial)
            foundNameEnum = False
            matches = ext.findall("./require/enum[@value='\"" + name + "\"']")
            for match in matches:
                if match.get('name').endswith("_EXTENSION_NAME"):
                    # Add extension definition
                    self.extensions[name] = VulkanExtension(ext, match.get('name')[:-len("_EXTENSION_NAME")])
                    foundNameEnum = True
                    break
            if not foundNameEnum:
                Log.f("Cannot find name enum for extension '{0}'".format(name))

            self.parseRequireRemove(ext)

    def parseStructInfo(self, xml):
        self.structs = dict()
        for struct in xml.findall("./types/type[@category='struct']"):
            name = struct.get('name')

            # Don't process structure if it is not required or if it is removed
            if name not in self.require.types or name in self.remove.types:
                continue

            # Define base struct information
            structDef = VulkanStruct(name)

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
                        match1D = re.search(r"^\[([0-9]+)\]$", tail)
                        match2D = re.search(r"^\[([0-9]+)\]\[([0-9]+)\]$", tail)
                        enum = member.find('./enum')
                        if match1D != None:
                            # [<number>] case
                            structDef.members[name].arraySize = int(match1D.group(1))
                        elif match2D != None:
                            # [<number>][<number>] case
                            structDef.members[name].arraySize = [ int(match2D.group(1)), int(match2D.group(2)) ]
                        elif tail == '[' and enum != None and enum.tail == ']':
                            # [<enum>] case
                            structDef.members[name].arraySize = enum.text
                        elif structDef.name == 'VkPhysicalDeviceDataGraphOperationSupportARM':
                            # Handle xml bug
                            structDef.members['name'].arraySize = 'VK_MAX_PHYSICAL_DEVICE_DATA_GRAPH_OPERATION_SET_NAME_SIZE_ARM'
                        else:
                            Log.f("Unsupported array format for struct member '{0}::{1}'".format(structDef.name, name))

                    # If it has a "len" attribute then it's also an array, just a dynamically sized one
                    if member.get('len') != None:
                        lenMeta = member.get('len').split(',')
                        for len in lenMeta:
                            if len == 'null-terminated':
                                # Values are null-terminated
                                structDef.members[name].nullTerminated = True
                            else:
                                # This is a pointer to an array with a corresponding count member
                                structDef.members[name].isArray = True
                                structDef.members[name].arraySizeMember = len

                                # Some arrays have a natural maximum size even if they are dynamic.  For example, a list
                                # of VkImageLayouts, because that enum itself is limited.
                                if structDef.members[name].type == 'VkImageLayout':
                                    structDef.members[name].arraySizeCap = 64

            # If any of the members is a dynamic array then we should remove the corresponding count member
            for member in list(structDef.members.values()):
                if member.isArray and member.arraySizeMember != None and struct.get('name') not in struct_with_valid_dynamic_array and struct.get('name') not in struct_with_dynamic_array_size_cap:
                    structDef.members.pop(member.arraySizeMember, None)

            # Store struct definition
            self.structs[struct.get('name')] = structDef

    def parsePrerequisites(self, xml):
        # Check features (i.e. API versions)
        for feature in self.findAllFeatures(xml):
            for requireType in feature.findall('./require/type'):
                # Add feature as the source of the definition of a struct
                if requireType.get('name') in self.structs:
                    self.structs[requireType.get('name')].definedByVersion = VulkanVersionNumber(feature.get('number'), self.api, feature.get('name'))

        # Check extensions
        for extension in self.findAllExtensions(xml):
            for requireType in extension.findall('./require/type'):
                # Add extension as the source of the definition of a struct
                if requireType.get('name') in self.structs:
                    self.structs[requireType.get('name')].definedByExtensions.append(extension.get('name'))

    def parseEnums(self, xml):
        self.enums = dict()
        # Find enum definitions
        for enum in xml.findall("./types/type[@category='enum']"):
            name = enum.get('name')

            # Don't process enum type if it is not required or if it is removed
            if name not in self.require.types or name in self.remove.types:
                continue

            # Create enum type
            enumDef = VulkanEnum(name)

            # First collect base values
            values = xml.find("./enums[@name='" + enumDef.name + "']")
            if values is not None:
                for value in values.findall("./enum"):
                    if value.get('alias') is None:
                        enumDef.values.append(value.get('name'))

            # Then find extension values
            for value in self.findAllFeatures(xml, "./require/enum[@extends='" + enumDef.name + "']"):
                if value.get('alias') is None:
                    enumDef.values.append(value.get('name'))
            for value in self.findAllExtensions(xml, "./require/enum[@extends='" + enumDef.name + "']"):
                if value.get('alias') is None:
                    enumDef.values.append(value.get('name'))

            # Remove any values that are marked as removed
            removedValues = []
            for name in enumDef.values:
                if name in self.remove.enums:
                    removedValues.append(name)
            for name in removedValues:
                enumDef.values.remove(name)

            # Finally store it in the registry
            self.enums[enumDef.name] = enumDef

    def parseFormats(self, xml):
        self.formatCompression = dict()
        for enum in xml.findall("./formats/format"):
            if enum.get('compressed'):
                self.formatCompression[enum.get('name')] = enum.get('compressed')

        self.aliasFormats = list()
        for format in self.findAllExtensions(xml, "./require/enum[@extends='VkFormat'][@alias]"):
            self.aliasFormats.append(format.attrib["name"])

        self.betaFormatFeatures = list()
        for format_feature in self.findAllExtensions(xml, "./require/enum[@protect='VK_ENABLE_BETA_EXTENSIONS']"):
            self.betaFormatFeatures.append(format_feature.attrib["name"])

    def parseBitmasks(self, xml):
        self.bitmasks = dict()
        # Find bitmask definitions
        for bitmask in xml.findall("./types/type[@category='bitmask']"):
            # Only consider non-alias bitmasks
            name = bitmask.find("./name")
            if bitmask.get('alias') is None and name != None:
                # Don't process bitmask type if it is not required or if it is removed
                if name.text not in self.require.types or name.text in self.remove.types:
                    continue

                bitmaskDef = VulkanBitmask(name.text)

                # Get the name of the corresponding FlagBits type
                bitsName = bitmask.get('bitvalues')
                if bitsName is None:
                    # Currently some definitions use "requires", not "bitvalues"
                    bitsName = bitmask.get('requires')

                if bitsName != None:
                    if bitsName in self.enums:
                        bitmaskDef.bitsType = self.enums[bitsName]
                    else:
                        Log.f("Could not find bits enum '{0}' for bitmask '{1}'".format(bitsName, bitmaskDef.name))
                else:
                    # This bitmask doesn't have any bits defined
                    pass

                # Finally store it in the registry
                self.bitmasks[bitmaskDef.name] = bitmaskDef

    def parseConstants(self, xml):
        self.constants = dict()
        # Find constant definitions
        constants = xml.find("./enums[@name='API Constants']").findall("./enum[@value]")
        if constants != None:
            for constant in constants:
                self.constants[constant.get('name')] = constant.get('value')
        else:
            Log.f("Failed to find API constants in the registry")

    def parseAliases(self, xml):
        # Find any struct aliases
        for struct in xml.findall("./types/type[@category='struct']"):
            name = struct.get('name')

            # Don't process structure if it is not required or if it is removed
            if name not in self.require.types or name in self.remove.types:
                continue

            alias = struct.get('alias')
            if alias != None:
                # Don't process alias if it is not required or if it is removed
                if alias not in self.require.types or alias in self.remove.types:
                    continue

                if alias in self.structs:
                    baseStructDef = self.structs[alias]
                    aliasStructDef = self.structs[name]

                    # Set as alias
                    aliasStructDef.isAlias = True

                    # Fill missing struct information for the alias
                    aliasStructDef.extends = baseStructDef.extends
                    aliasStructDef.members = baseStructDef.members
                    aliasStructDef.aliases = baseStructDef.aliases
                    aliasStructDef.aliases.append(name)

                    # Use alias structure dependencies as the structure dependencies if the latter has none
                    # This is needed to handle the case when the structure is not part of the target API
                    # but is a dependency of the alias
                    if baseStructDef.definedByVersion is None and len(baseStructDef.definedByExtensions) == 0:
                        baseStructDef.definedByVersion = aliasStructDef.definedByVersion
                        baseStructDef.definedByExtensions = aliasStructDef.definedByExtensions

                    if baseStructDef.sType != None:
                        sTypeAlias = None

                        # First try to find sType alias in core versions
                        if aliasStructDef.definedByVersion != None:
                            for versionName in self.versions:
                                version = self.versions[versionName]
                                if version.number <= aliasStructDef.definedByVersion:
                                    sTypeAlias = version.sTypeAliases.get(baseStructDef.sType)
                                    if sTypeAlias != None:
                                        break

                        # Otherwise need to find sType alias in extension
                        if sTypeAlias == None:
                            for extName in aliasStructDef.definedByExtensions:
                                sTypeAlias = self.extensions[extName].sTypeAliases.get(baseStructDef.sType)
                                if sTypeAlias != None:
                                    break

                        #Workaround due to a vk.xml issue that was resolved with 1.1.119
                        if alias == 'VkPhysicalDeviceVariablePointersFeatures':
                            sTypeAlias = 'VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VARIABLE_POINTERS_FEATURES'
                        
                        if sTypeAlias != None:
                            aliasStructDef.sType = sTypeAlias

        # Find any enum aliases
        for enum in xml.findall("./types/type[@category='enum']"):
            name = enum.get('name')

            # Don't process enum type if it is not required or if it is removed
            if name not in self.require.types or name in self.remove.types:
                continue

            alias = enum.get('alias')
            if alias != None:
                # Don't process alias if it is not required or if it is removed
                if alias not in self.require.types or alias in self.remove.types:
                    continue

                if alias in self.enums:
                    baseEnumDef = self.enums[alias]
                    aliasEnumDef = self.enums[name]

                    # Set as alias
                    aliasEnumDef.isAlias = True

                    # Merge aliases
                    aliasEnumDef.aliases = baseEnumDef.aliases
                    aliasEnumDef.aliases.append(name)

                    # Merge values respecting original order
                    for value in aliasEnumDef.values:
                        if not value in baseEnumDef.values:
                            baseEnumDef.values.append(value)
                    aliasEnumDef.values = baseEnumDef.values
                else:
                    Log.f("Failed to find alias '{0}' of enum '{1}'".format(alias, enum.get('name')))

        # Find any enum value aliases
        for enum in xml.findall("./enums"):
            if enum.get('name') in self.enums.keys():
                enumDef = self.enums[enum.get('name')]
                for aliasValue in enum.findall("./enum[@alias]"):
                    name = aliasValue.get('name')
                    alias = aliasValue.get('alias')
                    enumDef.values.append(name)
                    enumDef.aliasValues[name] = alias
        for aliasValue in self.findAllExtensions(xml, "./require/enum[@alias]"):
            if aliasValue.get('extends'):
                enumDef = self.enums[aliasValue.get('extends')]
                name = aliasValue.get('name')
                alias = aliasValue.get('alias')
                enumDef.values.append(name)
                enumDef.aliasValues[name] = alias

        # Find any bitmask (flags) aliases
        for bitmask in xml.findall("./types/type[@category='bitmask']"):
            name = bitmask.get('name')

            # Don't process bitmask if it is not required or if it is removed
            if name not in self.require.types or name in self.remove.types:
                continue

            alias = bitmask.get('alias')
            if alias != None:
                # Don't process alias if it is not required or if it is removed
                if alias not in self.require.types or alias in self.remove.types:
                    continue

                if alias in self.bitmasks:
                    # Duplicate bitmask definition
                    baseBitmaskDef = self.bitmasks[alias]
                    aliasBitmaskDef = VulkanBitmask(name)
                    aliasBitmaskDef.bitsType = baseBitmaskDef.bitsType

                    # Set as alias
                    aliasBitmaskDef.isAlias = True

                    # Merge aliases
                    aliasBitmaskDef.aliases = baseBitmaskDef.aliases
                    aliasBitmaskDef.aliases.append(name)
                else:
                    Log.f("Failed to find alias '{0}' of bitmask '{1}'".format(alias, bitmask.get('name')))

        # Find any constant aliases
        for constant in xml.find("./enums[@name='API Constants']").findall("./enum[@alias]"):
            self.constants[constant.get('name')] = self.constants[constant.get('alias')]

    def parseExternalTypes(self, xml):
        self.includes = set()
        self.externalTypes = set()

        # Find all include definitions
        for include in xml.findall("./types/type[@category='include']"):
            self.includes.add(include.get('name'))

        # Find all types depending on the includes
        for type in xml.findall("./types/type[@requires]"):
            if type.get('requires') in self.includes:
                self.externalTypes.add(type.get('name'))

    def parseFeatures(self, xml):
        # First, parse features specific to Vulkan versions
        for version in self.versions.values():
            if version.number.major == 1 and version.number.minor == 0:
                # For version 1.0 use VkPhysicalDeviceFeatures
                structDef = self.structs['VkPhysicalDeviceFeatures']
                for memberDef in structDef.members.values():
                    version.features[memberDef.name] = VulkanFeature(memberDef.name)
                    version.features[memberDef.name].structs.add('VkPhysicalDeviceFeatures')
            else:
                # For all other versions use the feature structures required by it
                featureStructNames = []
                xmlVersion = xml.find("./feature[@name='" + version.name + "']")
                for type in xmlVersion.findall("./require/type"):
                    name = type.get('name')
                    if name in self.structs and 'VkPhysicalDeviceFeatures2' in self.structs[name].extends:
                        featureStructNames.append(name)
                # VkPhysicalDeviceVulkan11Features is defined in Vulkan 1.2, but it actually
                # contains Vulkan 1.1 features, so treat it as such
                if version.number.major == 1 and version.number.minor == 1:
                    featureStructNames.append('VkPhysicalDeviceVulkan11Features')
                elif version.number.major == 1 and version.number.minor == 2:
                    if 'VkPhysicalDeviceVulkan11Features' in featureStructNames:
                        featureStructNames.remove('VkPhysicalDeviceVulkan11Features')
                # For each feature collect all feature structures containing them, and their aliases
                for featureStructName in featureStructNames:
                    if (featureStructName in self.structs):
                        structDef = self.structs[featureStructName]
                        for memberName in structDef.members.keys():
                            if not memberName in version.features:
                                version.features[memberName] = VulkanFeature(memberName)
                            version.features[memberName].structs.update(structDef.aliases)

        # Then parse features specific to extensions
        for extension in self.extensions.values():
            featureStructNames = []
            xmlExtension = xml.find("./extensions/extension[@name='" + extension.name + "']")
            for type in xmlExtension.findall("./require/type"):
                name = type.get('name')
                if name in self.structs and 'VkPhysicalDeviceFeatures2' in self.structs[name].extends:
                    featureStructNames.append(name)
            # For each feature collect all feature structures containing them, and their aliases
            for featureStructName in featureStructNames:
                structDef = self.structs[featureStructName]
                for memberName in structDef.members.keys():
                    extension.features[memberName] = VulkanFeature(memberName)
                    extension.features[memberName].structs.update(structDef.aliases)
                    # For each feature we also have to check whether it's part of core so that
                    # any not strictly alias struct (i.e. the VkPhysicalDeviceVulkanXXFeatures)
                    # get included as well
                    for version in self.versions.values():
                        if memberName in version.features and version.features[memberName].structs >= extension.features[memberName].structs:
                            extension.features[memberName].structs = version.features[memberName].structs

    def parseLimits(self, xml):
        # First, parse properties/limits specific to Vulkan versions
        for version in self.versions.values():
            if version.number.major == 1 and version.number.minor == 0:
                # The properties extension structures are a misnomer, as they contain limits,
                # however, the naming will stay with us, so in order to avoid nested
                # "properties" (limits), we simply use VkPhysicalDeviceLimits directly here
                # for version 1.0 limits, plus, not having a better place to put them, we
                # also include VkPhysicalDeviceSparseProperties here (even though they are
                # more like features)
                limitStructNames = [ 'VkPhysicalDeviceLimits', 'VkPhysicalDeviceSparseProperties' ]
            else:
                # For all other versions use the property structures required by it
                limitStructNames = []
                xmlVersion = xml.find("./feature[@name='" + version.name + "']")
                for type in xmlVersion.findall("./require/type"):
                    name = type.get('name')
                    if name in self.structs and 'VkPhysicalDeviceProperties2' in self.structs[name].extends:
                        limitStructNames.append(name)
                # VkPhysicalDeviceVulkan11Properties is defined in Vulkan 1.2, but it actually
                # contains Vulkan 1.1 limits, so treat it as such
                if version.number.major == 1 and version.number.minor == 1:
                    limitStructNames.append('VkPhysicalDeviceVulkan11Properties')
                elif version.number.major == 1 and version.number.minor == 2:
                    if 'VkPhysicalDeviceVulkan11Properties' in limitStructNames:
                        limitStructNames.remove('VkPhysicalDeviceVulkan11Properties')
            # For each limit collect all property/limit structures containing them, and their aliases
            for limitStructName in limitStructNames:
                if (limitStructName in self.structs):
                    structDef = self.structs[limitStructName]
                    for memberName in structDef.members.keys():
                        if not memberName in version.limits:
                            version.limits[memberName] = VulkanLimit(memberName)
                        version.limits[memberName].structs.update(structDef.aliases)

        # Then parse properties/limits specific to extensions
        for extension in self.extensions.values():
            limitStructNames = []
            xmlExtension = xml.find("./extensions/extension[@name='" + extension.name + "']")
            for type in xmlExtension.findall("./require/type"):
                name = type.get('name')
                if name in self.structs and 'VkPhysicalDeviceProperties2' in self.structs[name].extends:
                    limitStructNames.append(name)
            # For each limit collect all property/limit structures containing them, and their aliases
            for limitStructName in limitStructNames:
                structDef = self.structs[limitStructName]
                for memberName in structDef.members.keys():
                    extension.limits[memberName] = VulkanLimit(memberName)
                    extension.limits[memberName].structs.update(structDef.aliases)
                    # For each limit we also have to check whether it's part of core so that
                    # any not strictly alias struct (i.e. the VkPhysicalDeviceVulkanXXProperties)
                    # get included as well
                    for version in self.versions.values():
                        if memberName in version.limits and version.limits[memberName].structs >= extension.limits[memberName].structs:
                            extension.limits[memberName].structs = version.limits[memberName].structs

    def parseHeaderVersion(self, xml):
        # Find the largest version number
        maxVersionNumber = self.versions[max(self.versions, key = lambda version: self.versions[version].number)].number
        self.headerVersionNumber = VulkanVersionNumber(str(maxVersionNumber))
        # Add patch from VK_HEADER_VERSION define
        for define in xml.findall("./types/type[@category='define']"):
            name = define.find('./name')
            if name != None and name.text == 'VK_HEADER_VERSION':
                self.headerVersionNumber.patch = int(name.tail.lstrip())
                return

    def parseVideoConstants(self, videoxml):
        for constant in videoxml.findall("./extensions/extension/require/enum[@value]"):
            self.constants[constant.get('name')] = constant.get('value')

    def parseVideoEnums(self, videoxml):
        # Find enum definitions
        for enum in videoxml.findall("./enums[@name]"):
            name = enum.get('name')

            # Only add video enum type if it is a required external type
            if name in self.externalTypes:
                # Create enum type
                enumDef = VulkanEnum(name)

                # First collect base values
                for value in enum.findall("./enum"):
                    if value.get('alias') is None:
                        enumDef.values.append(value.get('name'))

                # Store video enum type in the registry
                self.enums[name] = enumDef

                # Remove video enum type from the set of external types
                self.externalTypes.remove(name)

    def parseVideoCodecs(self, xml, videoxml):
        self.videoCodecs = dict()

        # Used to look up video codecs based on the video codec op value
        self.videoCodecsByValue = dict()

        # Used to reverse look up video codecs by the defined structure names if no video codec op value is available
        self.videoCodecsByStructName = dict()

        if videoxml is None:
            return

        self.parseVideoConstants(videoxml)
        self.parseVideoEnums(videoxml)

        xmlVideoCodecs = xml.find("./videocodecs")
        for xmlVideoCodec in xmlVideoCodecs.findall("./videocodec"):
            name = xmlVideoCodec.get('name')
            extend = xmlVideoCodec.get('extend')
            value = xmlVideoCodec.get('value')
            if value is None:
                # Video codec category
                self.videoCodecs[name] = VulkanVideoCodec(name)
            else:
                # Specific video codec
                self.videoCodecs[name] = VulkanVideoCodec(name, self.videoCodecs[extend], value)
                self.videoCodecsByValue[value] = self.videoCodecs[name]
            videoCodec = self.videoCodecs[name]

            for xmlVideoProfiles in xmlVideoCodec.findall("./videoprofiles"):
                videoProfileStructName = xmlVideoProfiles.get('struct')
                videoCodec.profileStructs[videoProfileStructName] = VulkanVideoProfileStruct(videoProfileStructName)
                videoProfileStruct = videoCodec.profileStructs[videoProfileStructName]
                self.videoCodecsByStructName[videoProfileStructName] = videoCodec

                for xmlVideoProfileMember in xmlVideoProfiles.findall("./videoprofilemember"):
                    memberName = xmlVideoProfileMember.get('name')
                    videoProfileStruct.members[memberName] = VulkanVideoProfileStructMember(memberName)
                    videoProfileStructMember = videoProfileStruct.members[memberName]

                    for xmlVideoProfile in xmlVideoProfileMember.findall("./videoprofile"):
                        videoProfileStructMember.values[xmlVideoProfile.get('value')] = xmlVideoProfile.get('name')

            for xmlVideoCapabilities in xmlVideoCodec.findall("./videocapabilities"):
                capabilityStructName = xmlVideoCapabilities.get('struct')
                videoCodec.capabilities[capabilityStructName] = capabilityStructName
                self.videoCodecsByStructName[capabilityStructName] = videoCodec

            for xmlVideoFormat in xmlVideoCodec.findall("./videoformat"):
                videoFormatName = xmlVideoFormat.get('name')
                videoFormatExtend = xmlVideoFormat.get('extend')
                if videoFormatName is not None:
                    # This is a new video format category
                    videoFormatUsage = xmlVideoFormat.get('usage')
                    videoCodec.formats[videoFormatName] = VulkanVideoFormat(videoFormatName, videoFormatUsage)
                    videoFormat = videoCodec.formats[videoFormatName]
                elif videoFormatExtend is not None:
                    # This is an extension to an already defined video format category
                    if videoFormatExtend in videoCodec.formats:
                        videoFormat = videoCodec.formats[videoFormatExtend]
                    else:
                        Log.f("Video format category '{0}' not found but it is attempted to be extended".format(videoFormatExtend))
                else:
                    Log.f('"name" or "extend" is attribute is required for "videoformat" element')

                for xmlVideoFormatProperties in xmlVideoFormat.findall("./videoformatproperties"):
                    propertiesStructName = xmlVideoFormatProperties.get('struct')
                    videoFormat.properties[propertiesStructName] = propertiesStructName
                    self.videoCodecsByStructName[propertiesStructName] = videoCodec

                for xmlVideoFormatRequiredCap in xmlVideoFormat.findall("./videorequirecapabilities"):
                    requiredCapStruct = xmlVideoFormatRequiredCap.get('struct')
                    requiredCapMember = xmlVideoFormatRequiredCap.get('member')
                    requiredCapValue = xmlVideoFormatRequiredCap.get('value')
                    videoFormat.requiredCaps.append(VulkanVideoRequiredCapabilities(requiredCapStruct, requiredCapMember, requiredCapValue))

    def getBaseVideoProfileInfoFromVideoProfile(self, videoProfile):
        if not 'profile' in videoProfile:
            return None
        profile = videoProfile['profile']
        if 'VkVideoProfileInfoKHR' in profile:
            return profile['VkVideoProfileInfoKHR']
        else:
            # Check also for possible aliases
            for alias in self.structs['VkVideoProfileInfoKHR'].aliases:
                if alias in profile:
                    return profile[alias]
            return None

    def getBaseVideoFormatPropertiesFromVideoFormat(self, format):
        if 'VkVideoFormatPropertiesKHR' in format:
            return format['VkVideoFormatPropertiesKHR']
        else:
            # Check also for possible aliases
            for alias in self.structs['VkVideoFormatPropertiesKHR'].aliases:
                if alias in format:
                    return format[alias]
            Log.f("Did not find base video format properties in video format:\n{0}".format(json.dumps(format, indent=4)))
            return None

    def getVideoCodecFromVideoProfile(self, videoProfile):
        base = self.getBaseVideoProfileInfoFromVideoProfile(videoProfile)
        if base is not None and 'videoCodecOperation' in base:
            if base['videoCodecOperation'] not in self.videoCodecsByValue:
                Log.f("Unrecognized videoCodecOperation in video profile:\n{0}".format(json.dumps(videoProfile['profile'], indent=4)))
            return self.videoCodecsByValue[base['videoCodecOperation']]
        else:
            # No VkVideoProfileInfoKHR in the profile definition or no videoCodecOperation specified
            # We do a reverse lookup based on the defined structures
            videoCodec = None
            structNames = set()
            if 'profile' in videoProfile:
                structNames = structNames.union(set(videoProfile['profile'].keys()))
            if 'capabilities' in videoProfile:
                structNames = structNames.union(set(videoProfile['capabilities'].keys()))
            if 'formats' in videoProfile:
                for videoFormat in videoProfile['formats']:
                    structNames = structNames.union(set(videoFormat.keys()))
            for structName in structNames:
                if structName in self.videoCodecsByStructName:
                    newMatchingVideoCodec = self.videoCodecsByStructName[structName]
                    if videoCodec is None or not videoCodec.isSpecific():
                        videoCodec = newMatchingVideoCodec
            if videoCodec is None:
                # No match found, create an empty video codec to represent general requirements
                videoCodec = VulkanVideoCodec("General")
            return videoCodec


    def getVideoProfileNameFromVideoProfile(self, videoProfile):
        videoCodec = self.getVideoCodecFromVideoProfile(videoProfile)
        base = self.getBaseVideoProfileInfoFromVideoProfile(videoProfile)

        # Video profile name always contains the codec name which is either the specific codec name,
        # "General" to indicate no specific codec profile, or one of the codec categories like "Decode" and "Encode"
        profileName = videoCodec.name

        if base is not None:
            profile = videoProfile['profile']

            # Helper function populating lookup tables with alias values
            def genAliasValues(flagBitsTypeName, map):
                flagBitsTypeName = self.enums[self.getNonAliasTypeName(flagBitsTypeName, self.enums)]
                for alias, value in flagBitsTypeName.aliasValues.items():
                    if alias in map:
                        map[value] = map[alias]
                    elif value in map:
                        map[value] = map[alias]
                return map

            formatModifiers = []

            chromaSubsamplingMap = genAliasValues('VkVideoChromaSubsamplingFlagBitsKHR', {
                "VK_VIDEO_CHROMA_SUBSAMPLING_420_BIT_KHR": "4:2:0",
                "VK_VIDEO_CHROMA_SUBSAMPLING_422_BIT_KHR": "4:2:2",
                "VK_VIDEO_CHROMA_SUBSAMPLING_444_BIT_KHR": "4:4:4",
                "VK_VIDEO_CHROMA_SUBSAMPLING_MONOCHROME_BIT_KHR": "monochrome"
            })
            if 'chromaSubsampling' in base:
                # Include chroma subsampling info in the name as it is present
                if len(base['chromaSubsampling']) != 1:
                    Log.f("Expected chromaSubsampling to only contain a single value in video profile:\n{0}".format(json.dumps(profile, indent=4)))
                if base['chromaSubsampling'][0] not in chromaSubsamplingMap:
                    Log.f("Unrecognized chromaSubsampling in video profile:\n%s".format(json.dumps(profile, indent=4)))
                chromaSubsampling = chromaSubsamplingMap[base['chromaSubsampling'][0]]
            else:
                chromaSubsampling = None

            if chromaSubsampling is not None:
                formatModifiers.append(chromaSubsampling)

            bitDepthMap = genAliasValues('VkVideoComponentBitDepthFlagBitsKHR', {
                "VK_VIDEO_COMPONENT_BIT_DEPTH_8_BIT_KHR": 8,
                "VK_VIDEO_COMPONENT_BIT_DEPTH_10_BIT_KHR": 10,
                "VK_VIDEO_COMPONENT_BIT_DEPTH_12_BIT_KHR": 12
            })
            if 'lumaBitDepth' in base:
                if len(base['lumaBitDepth']) != 1:
                    Log.f("Expected lumaBitDepth to only contain a single value in video profile:\n{0}".format(json.dumps(profile, indent=4)))
                if base['lumaBitDepth'][0] not in bitDepthMap:
                    Log.f("Unrecognized lumaBitDepth in profile:\n{0}".format(json.dumps(profile, indent=4)))
                lumaBitDepth = bitDepthMap[base['lumaBitDepth'][0]]
            else:
                lumaBitDepth = None

            if chromaSubsampling != 'monochrome' and 'chromaBitDepth' in base:
                if len(base['chromaBitDepth']) != 1:
                    Log.f("Expected chromaBitDepth to only contain a single value in video profile:\n{0}".format(json.dumps(profile, indent=4)))
                if base['chromaBitDepth'][0] not in bitDepthMap:
                    Log.f("Unrecognized chromaBitDepth in profile:\n{0}".format(json.dumps(profile, indent=4)))
                chromaBitDepth = bitDepthMap[base['chromaBitDepth'][0]]
            else:
                # For monochrome chromaBitDepth is ignored
                # This case works also if lumaBitDepth is None because it was not present
                chromaBitDepth = lumaBitDepth

            if lumaBitDepth == chromaBitDepth:
                if lumaBitDepth is not None:
                    formatModifiers.append("{0}-bit".format(lumaBitDepth))
            else:
                formatModifiers.append("{0}:{1}-bit".format(lumaBitDepth if lumaBitDepth is not None else "*",
                                                            chromaBitDepth if chromaBitDepth is not None else "*"))

            # If there is format information, then include it in the video profile name in parantheses
            if len(formatModifiers) > 0:
                profileName += " ({0})".format(" ".join(formatModifiers))

        for profileStruct in videoCodec.profileStructs.values():
            profileStructData = None
            if profileStruct.struct in profile:
                profileStructData = profile[profileStruct.struct]
            else:
                # Check also for possible aliases
                for alias in self.structs[profileStruct.struct].aliases:
                    if alias in profile:
                        profileStructData = profile[alias]

            if profileStructData is None:
                # Profile struct is not present, this is a "wildcard" video profile definition
                continue

            for profileStructMember in profileStruct.members.values():
                if not profileStructMember.name in profileStructData:
                    # Profile struct member is not present, this is a "wildcard" video profile definition
                    continue

                profileStructMemberValue = profileStructData[profileStructMember.name]
                if isinstance(profileStructMemberValue, bool):
                    profileStructMemberValue = 'VK_TRUE' if profileStructMemberValue else 'VK_FALSE'
                if profileStructMemberValue not in profileStructMember.values:
                    Log.f("Unrecognized profile struct member value for '{0}::{1}' in video profile:\n{2}".format(profileStruct.struct, profileStructMember.name, json.dumps(profile, indent=4)))

                # Append codec-specific profile information to the profile name
                profileName += " {0}".format(profileStructMember.values[profileStructMemberValue])

        return profileName

    def overwrite(self, structName, memberName, invalid_values, correct_value):
        if structName in self.structs:
            if (self.structs[structName].members[memberName].limittype == None or
                self.structs[structName].members[memberName].limittype in invalid_values):
                self.structs[structName].members[memberName].limittype = correct_value
            elif (self.structs[structName].members[memberName].limittype != correct_value):
                Log.w("Profiles is overwriting {0}::{1} to {2}, but current XML value is {3}".format(structName, memberName, correct_value, self.structs[structName].members[memberName].limittype))

    def applyWorkarounds(self):
        if self.headerVersionNumber.patch < 207: # vk.xml declares maxColorAttachments with 'bitmask' limittype before header 207
            self.structs['VkPhysicalDeviceLimits'].members['maxColorAttachments'].limittype = 'max'

        # TODO: We currently have to apply workarounds due to "noauto" limittypes and other bugs related to limittypes in the vk.xml
        # These can only be solved permanently if we make modifications to the registry xml itself
        self.overwrite('VkPhysicalDeviceLimits', 'subPixelPrecisionBits', ['noauto'], 'bits')
        self.overwrite('VkPhysicalDeviceLimits', 'subTexelPrecisionBits', ['noauto'], 'bits')
        self.overwrite('VkPhysicalDeviceLimits', 'mipmapPrecisionBits', ['noauto'], 'bits')
        self.overwrite('VkPhysicalDeviceLimits', 'viewportSubPixelBits', ['noauto'], 'bits')
        self.overwrite('VkPhysicalDeviceLimits', 'subPixelInterpolationOffsetBits', ['noauto'], 'bits')
        self.overwrite('VkPhysicalDeviceLimits', 'minMemoryMapAlignment', ['noauto'], 'max,pot')
        self.overwrite('VkPhysicalDeviceLimits', 'minTexelBufferOffsetAlignment', ['noauto'], 'min,pot')
        self.overwrite('VkPhysicalDeviceLimits', 'minUniformBufferOffsetAlignment', ['noauto'], 'min,pot')
        self.overwrite('VkPhysicalDeviceLimits', 'minStorageBufferOffsetAlignment', ['noauto'], 'min,pot')
        self.overwrite('VkPhysicalDeviceLimits', 'optimalBufferCopyOffsetAlignment', ['noauto'], 'min,pot')
        self.overwrite('VkPhysicalDeviceLimits', 'optimalBufferCopyRowPitchAlignment', ['noauto'], 'min,pot')
        self.overwrite('VkPhysicalDeviceLimits', 'nonCoherentAtomSize', ['noauto'], 'min,pot')
        self.overwrite('VkPhysicalDeviceLimits', 'timestampPeriod', ['noauto', 'min,mul'], 'exact') # resolve https://github.com/KhronosGroup/Vulkan-Profiles/issues/769
        self.overwrite('VkPhysicalDeviceLimits', 'bufferImageGranularity', ['noauto'], 'min,mul')
        self.overwrite('VkPhysicalDeviceLimits', 'pointSizeGranularity', ['max'], 'min,mul')
        self.overwrite('VkPhysicalDeviceLimits', 'lineWidthGranularity', ['max'], 'min,mul')
        self.overwrite('VkPhysicalDeviceLimits', 'strictLines', ['noauto', 'bitmask', 'exact'], 'max')
        self.overwrite('VkPhysicalDeviceLimits', 'standardSampleLocations', ['noauto', 'bitmask', 'exact'], 'max')

        self.overwrite('VkPhysicalDeviceSparseProperties', 'residencyAlignedMipSize', ['bitmask', 'not'], 'min')

        self.overwrite('VkPhysicalDeviceVulkan11Properties', 'deviceUUID', ['None'], 'exact')
        self.overwrite('VkPhysicalDeviceVulkan11Properties', 'driverUUID', ['None'], 'exact')
        self.overwrite('VkPhysicalDeviceVulkan11Properties', 'deviceLUID', ['None'], 'exact')
        self.overwrite('VkPhysicalDeviceVulkan11Properties', 'deviceNodeMask', ['None'], 'exact')
        self.overwrite('VkPhysicalDeviceVulkan11Properties', 'deviceLUIDValid', ['None'], 'exact')
        self.overwrite('VkPhysicalDeviceVulkan11Properties', 'subgroupSize', ['None'], 'max,pot')
        self.overwrite('VkPhysicalDeviceVulkan11Properties', 'pointClippingBehavior', ['None'], 'exact')
        self.overwrite('VkPhysicalDeviceVulkan11Properties', 'protectedNoFault', ['None'], 'exact')

        self.overwrite('VkPhysicalDeviceVulkan12Properties', 'driverID', ['None'], 'noauto')
        self.overwrite('VkPhysicalDeviceVulkan12Properties', 'driverName', ['None'], 'noauto')
        self.overwrite('VkPhysicalDeviceVulkan12Properties', 'driverInfo', ['None'], 'noauto')
        self.overwrite('VkPhysicalDeviceVulkan12Properties', 'conformanceVersion', ['None'], 'noauto')
        self.overwrite('VkPhysicalDeviceVulkan12Properties', 'denormBehaviorIndependence', ['None'], 'exact')
        self.overwrite('VkPhysicalDeviceVulkan12Properties', 'roundingModeIndependence', ['None'], 'exact')

        self.overwrite('VkPhysicalDeviceVulkan13Properties', 'storageTexelBufferOffsetAlignmentBytes', ['noauto'], 'min,pot')
        self.overwrite('VkPhysicalDeviceVulkan13Properties', 'storageTexelBufferOffsetSingleTexelAlignment', ['noauto'], 'exact')
        self.overwrite('VkPhysicalDeviceVulkan13Properties', 'uniformTexelBufferOffsetAlignmentBytes', ['noauto'], 'min,pot')
        self.overwrite('VkPhysicalDeviceVulkan13Properties', 'uniformTexelBufferOffsetSingleTexelAlignment', ['noauto'], 'exact')
        self.overwrite('VkPhysicalDeviceVulkan13Properties', 'minSubgroupSize', ['min'], 'min,pot')
        self.overwrite('VkPhysicalDeviceVulkan13Properties', 'maxSubgroupSize', ['max'], 'max,pot')

        self.overwrite('VkPhysicalDeviceVulkan14Properties', 'maxCombinedImageSamplerDescriptorCount', ['None'], 'max')

        self.overwrite('VkPhysicalDeviceTexelBufferAlignmentProperties', 'storageTexelBufferOffsetAlignmentBytes', ['None'], 'min,pot')
        self.overwrite('VkPhysicalDeviceTexelBufferAlignmentProperties', 'storageTexelBufferOffsetSingleTexelAlignment', ['None'], 'exact')
        self.overwrite('VkPhysicalDeviceTexelBufferAlignmentProperties', 'uniformTexelBufferOffsetAlignmentBytes', ['None'], 'min,pot')
        self.overwrite('VkPhysicalDeviceTexelBufferAlignmentProperties', 'uniformTexelBufferOffsetSingleTexelAlignment', ['None'], 'exact')

        self.overwrite('VkPhysicalDeviceProperties', 'apiVersion', ['None'], 'noauto')
        self.overwrite('VkPhysicalDeviceProperties', 'driverVersion', ['None'], 'noauto')
        self.overwrite('VkPhysicalDeviceProperties', 'vendorID', ['None'], 'noauto')
        self.overwrite('VkPhysicalDeviceProperties', 'deviceID', ['None'], 'noauto')
        self.overwrite('VkPhysicalDeviceProperties', 'deviceType', ['None'], 'noauto')
        self.overwrite('VkPhysicalDeviceProperties', 'deviceName', ['None'], 'noauto')
        self.overwrite('VkPhysicalDeviceProperties', 'pipelineCacheUUID', ['None'], 'noauto')

        self.overwrite('VkPhysicalDeviceToolProperties', 'name', ['None'], 'noauto')
        self.overwrite('VkPhysicalDeviceToolProperties', 'version', ['None'], 'noauto')
        self.overwrite('VkPhysicalDeviceToolProperties', 'purposes', ['None'], 'noauto')
        self.overwrite('VkPhysicalDeviceToolProperties', 'description', ['None'], 'noauto')
        self.overwrite('VkPhysicalDeviceToolProperties', 'layer', ['None'], 'noauto')

        self.overwrite('VkPhysicalDeviceSubgroupSizeControlProperties', 'minSubgroupSize', ['None'], 'min,pot')
        self.overwrite('VkPhysicalDeviceSubgroupSizeControlProperties', 'maxSubgroupSize', ['None'], 'max,pot')

        self.overwrite('VkPhysicalDeviceDriverProperties', 'driverID', ['noauto'], 'exact')
        self.overwrite('VkPhysicalDeviceDriverProperties', 'driverName', ['noauto'], 'exact')
        self.overwrite('VkPhysicalDeviceDriverProperties', 'driverInfo', ['noauto'], 'exact')
        self.overwrite('VkPhysicalDeviceDriverProperties', 'conformanceVersion', ['noauto'], 'exact')

        self.overwrite('VkPhysicalDeviceIDProperties', 'deviceUUID', ['None'], 'noauto')
        self.overwrite('VkPhysicalDeviceIDProperties', 'driverUUID', ['None'], 'noauto')
        self.overwrite('VkPhysicalDeviceIDProperties', 'deviceLUID', ['None', 'noauto'], 'max')
        self.overwrite('VkPhysicalDeviceIDProperties', 'deviceNodeMask', ['None'], 'noauto')
        self.overwrite('VkPhysicalDeviceIDProperties', 'deviceLUIDValid', ['None', 'noauto'], 'max')

        self.overwrite('VkPhysicalDeviceSubgroupProperties', 'subgroupSize', ['None'], 'max,pot')

        self.overwrite('VkPhysicalDevicePointClippingProperties', 'pointClippingBehavior', ['None'], 'exact')

        self.overwrite('VkPhysicalDeviceProtectedMemoryProperties', 'protectedNoFault', ['None'], 'exact')

        self.overwrite('VkPhysicalDeviceFloatControlsProperties', 'denormBehaviorIndependence', ['None'], 'exact')
        self.overwrite('VkPhysicalDeviceFloatControlsProperties', 'roundingModeIndependence', ['None'], 'exact')

        self.overwrite('VkPhysicalDeviceTexelBufferAlignmentProperties', 'storageTexelBufferOffsetSingleTexelAlignment', ['None'], 'exact')
        self.overwrite('VkPhysicalDeviceTexelBufferAlignmentProperties', 'uniformTexelBufferOffsetSingleTexelAlignment', ['None'], 'exact')

        self.overwrite('VkPhysicalDevicePortabilitySubsetPropertiesKHR', 'minVertexInputBindingStrideAlignment', ['None'], 'min,pot')

        self.overwrite('VkPhysicalDeviceFragmentShadingRatePropertiesKHR', 'maxFragmentShadingRateAttachmentTexelSizeAspectRatio', ['None'], 'max,pot')
        self.overwrite('VkPhysicalDeviceFragmentShadingRatePropertiesKHR', 'maxFragmentSizeAspectRatio', ['None'], 'max,pot')
        self.overwrite('VkPhysicalDeviceFragmentShadingRatePropertiesKHR', 'maxFragmentShadingRateCoverageSamples', ['None'], 'max')

        self.overwrite('VkPhysicalDeviceRayTracingPipelinePropertiesKHR', 'shaderGroupHandleSize', ['None'], 'exact')
        self.overwrite('VkPhysicalDeviceRayTracingPipelinePropertiesKHR', 'shaderGroupBaseAlignment', ['None'], 'exact')
        self.overwrite('VkPhysicalDeviceRayTracingPipelinePropertiesKHR', 'shaderGroupHandleCaptureReplaySize', ['None'], 'exact')
        self.overwrite('VkPhysicalDeviceRayTracingPipelinePropertiesKHR', 'shaderGroupHandleAlignment', ['None'], 'min,pot')

        self.overwrite('VkPhysicalDeviceFragmentShadingRatePropertiesKHR', 'maxFragmentShadingRateRasterizationSamples', ['None'], 'max')

        self.overwrite('VkPhysicalDeviceConservativeRasterizationPropertiesEXT', 'primitiveOverestimationSize', ['None'], 'exact')
        self.overwrite('VkPhysicalDeviceConservativeRasterizationPropertiesEXT', 'extraPrimitiveOverestimationSizeGranularity', ['None'], 'min,mul')
        self.overwrite('VkPhysicalDeviceConservativeRasterizationPropertiesEXT', 'conservativePointAndLineRasterization', ['None', 'bitmask'], 'max')
        self.overwrite('VkPhysicalDeviceConservativeRasterizationPropertiesEXT', 'degenerateTrianglesRasterized', ['None'], 'exact')
        self.overwrite('VkPhysicalDeviceConservativeRasterizationPropertiesEXT', 'degenerateLinesRasterized', ['None'], 'exact')

        self.overwrite('VkPhysicalDeviceLineRasterizationPropertiesEXT', 'lineSubPixelPrecisionBits', ['None'], 'bits')

        self.overwrite('VkPhysicalDeviceTransformFeedbackPropertiesEXT', 'maxTransformFeedbackBufferDataStride', ['None'], 'max')

        self.overwrite('VkPhysicalDeviceExternalMemoryHostPropertiesEXT', 'minImportedHostPointerAlignment', ['None'], 'min,pot')

        self.overwrite('VkPhysicalDevicePCIBusInfoPropertiesEXT', 'pciDomain', ['None'], 'noauto')
        self.overwrite('VkPhysicalDevicePCIBusInfoPropertiesEXT', 'pciBus', ['None'], 'noauto')
        self.overwrite('VkPhysicalDevicePCIBusInfoPropertiesEXT', 'pciDevice', ['None'], 'noauto')
        self.overwrite('VkPhysicalDevicePCIBusInfoPropertiesEXT', 'pciFunction', ['None'], 'noauto')

        self.overwrite('VkPhysicalDeviceDrmPropertiesEXT', 'hasPrimary', ['None', 'bitmask'], 'max')
        self.overwrite('VkPhysicalDeviceDrmPropertiesEXT', 'hasRender', ['None', 'bitmask'], 'max')
        self.overwrite('VkPhysicalDeviceDrmPropertiesEXT', 'primaryMajor', ['None'], 'noauto')
        self.overwrite('VkPhysicalDeviceDrmPropertiesEXT', 'primaryMinor', ['None'], 'noauto')
        self.overwrite('VkPhysicalDeviceDrmPropertiesEXT', 'renderMajor', ['None'], 'noauto')
        self.overwrite('VkPhysicalDeviceDrmPropertiesEXT', 'renderMinor', ['None'], 'noauto')

        self.overwrite('VkPhysicalDeviceFragmentDensityMap2PropertiesEXT', 'subsampledLoads', ['noauto'], 'exact')
        self.overwrite('VkPhysicalDeviceFragmentDensityMap2PropertiesEXT', 'subsampledCoarseReconstructionEarlyAccess', ['noauto'], 'exact')

        self.overwrite('VkPhysicalDeviceSampleLocationsPropertiesEXT', 'sampleLocationSubPixelBits', ['noauto'], 'bits')

        self.overwrite('VkPhysicalDeviceRobustness2PropertiesEXT', 'robustStorageBufferAccessSizeAlignment', ['noauto'], 'min,pot')
        self.overwrite('VkPhysicalDeviceRobustness2PropertiesEXT', 'robustUniformBufferAccessSizeAlignment', ['noauto'], 'min,pot')

        self.overwrite('VkPhysicalDeviceShaderCorePropertiesAMD', 'shaderEngineCount', ['max'], 'exact')
        self.overwrite('VkPhysicalDeviceShaderCorePropertiesAMD', 'shaderArraysPerEngineCount', ['max'], 'exact')
        self.overwrite('VkPhysicalDeviceShaderCorePropertiesAMD', 'computeUnitsPerShaderArray', ['max'], 'exact')
        self.overwrite('VkPhysicalDeviceShaderCorePropertiesAMD', 'simdPerComputeUnit', ['max'], 'exact')
        self.overwrite('VkPhysicalDeviceShaderCorePropertiesAMD', 'wavefrontsPerSimd', ['max'], 'exact')
        self.overwrite('VkPhysicalDeviceShaderCorePropertiesAMD', 'sgprsPerSimd', ['max'], 'exact')
        self.overwrite('VkPhysicalDeviceShaderCorePropertiesAMD', 'sgprAllocationGranularity', ['noauto'], 'min,mul')
        self.overwrite('VkPhysicalDeviceShaderCorePropertiesAMD', 'vgprsPerSimd', ['max'], 'exact')
        self.overwrite('VkPhysicalDeviceShaderCorePropertiesAMD', 'vgprAllocationGranularity', ['noauto'], 'min,mul')

        self.overwrite('VkPhysicalDeviceSubpassShadingPropertiesHUAWEI', 'maxSubpassShadingWorkgroupSizeAspectRatio', ['noauto'], 'max,pot')

        self.overwrite('VkPhysicalDeviceRayTracingPropertiesNV', 'shaderGroupHandleSize', ['noauto'], 'exact')
        self.overwrite('VkPhysicalDeviceRayTracingPropertiesNV', 'shaderGroupBaseAlignment', ['noauto'], 'exact')

        self.overwrite('VkPhysicalDeviceShadingRateImagePropertiesNV', 'shadingRateTexelSize', ['noauto'], 'exact')

        self.overwrite('VkPhysicalDeviceMeshShaderPropertiesNV', 'meshOutputPerVertexGranularity', ['noauto'], 'min,mul')
        self.overwrite('VkPhysicalDeviceMeshShaderPropertiesNV', 'meshOutputPerPrimitiveGranularity', ['noauto'], 'min,mul')

        self.overwrite('VkPhysicalDevicePipelineRobustnessPropertiesEXT', 'defaultRobustnessStorageBuffers', ['noauto'], 'exact')
        self.overwrite('VkPhysicalDevicePipelineRobustnessPropertiesEXT', 'defaultRobustnessUniformBuffers', ['noauto'], 'exact')
        self.overwrite('VkPhysicalDevicePipelineRobustnessPropertiesEXT', 'defaultRobustnessVertexInputs', ['noauto'], 'exact')
        self.overwrite('VkPhysicalDevicePipelineRobustnessPropertiesEXT', 'defaultRobustnessImages', ['noauto'], 'exact')

        self.overwrite('VkPhysicalDeviceDeviceGeneratedCommandsPropertiesNV', 'minSequencesCountBufferOffsetAlignment', ['noauto'], 'min')
        self.overwrite('VkPhysicalDeviceDeviceGeneratedCommandsPropertiesNV', 'minSequencesIndexBufferOffsetAlignment', ['noauto'], 'min')
        self.overwrite('VkPhysicalDeviceDeviceGeneratedCommandsPropertiesNV', 'minIndirectCommandsBufferOffsetAlignment', ['noauto'], 'min')

        self.overwrite('VkPhysicalDeviceFragmentDensityMapOffsetPropertiesQCOM', 'fragmentDensityOffsetGranularity', ['max'], 'min,mul')

        self.overwrite('VkPhysicalDeviceSchedulingControlsPropertiesARM', 'schedulingControlsFlags', ['None'], 'bitmask')

        self.overwrite('VkPhysicalDeviceExternalFormatResolvePropertiesANDROID', 'nullColorAttachmentWithExternalFormatResolve', ['noauto', 'not'], 'min')

        self.overwrite('VkPhysicalDeviceRenderPassStripedPropertiesARM', 'renderPassStripeGranularity', ['None', 'min', 'max,mul'], 'min,mul')
        self.overwrite('VkPhysicalDeviceRenderPassStripedPropertiesARM', 'maxRenderPassStripes', ['None'], 'max')

        self.overwrite('VkPhysicalDeviceMaintenance6PropertiesKHR', 'maxCombinedImageSamplerDescriptorCount', ['None'], 'max')

        self.overwrite('VkPhysicalDeviceDeviceGeneratedCommandsPropertiesEXT', 'supportedIndirectCommandsInputModes', ['None'], 'bitmask')
        self.overwrite('VkPhysicalDeviceDeviceGeneratedCommandsPropertiesEXT', 'supportedIndirectCommandsShaderStages', ['None'], 'bitmask')
        self.overwrite('VkPhysicalDeviceDeviceGeneratedCommandsPropertiesEXT', 'supportedIndirectCommandsShaderStagesPipelineBinding', ['None'], 'bitmask')
        self.overwrite('VkPhysicalDeviceDeviceGeneratedCommandsPropertiesEXT', 'supportedIndirectCommandsShaderStagesShaderBinding', ['None'], 'bitmask')

        self.overwrite('VkPhysicalDeviceCooperativeVectorPropertiesNV', 'maxCooperativeVectorComponents', ['None'], 'max')

        self.overwrite('VkPhysicalDeviceGpaPropertiesAMD', 'flags', ['noauto'], 'bitmask')

        # TODO: The registry xml is also missing limittype definitions for format and queue family properties
        # For now we just add the important ones, this needs a larger overhaul in the vk.xml
        self.overwrite('VkFormatProperties', 'linearTilingFeatures', ['None'], 'bitmask')
        self.overwrite('VkFormatProperties', 'optimalTilingFeatures', ['None'], 'bitmask')
        self.overwrite('VkFormatProperties', 'bufferFeatures', ['None'], 'bitmask')
        self.overwrite('VkFormatProperties3', 'linearTilingFeatures', ['None'], 'bitmask')
        self.overwrite('VkFormatProperties3', 'optimalTilingFeatures', ['None'], 'bitmask')
        self.overwrite('VkFormatfProperties3', 'bufferFeatures', ['None'], 'bitmask')

        self.overwrite('VkQueueFamilyProperties', 'queueFlags', ['None'], 'bitmask')
        self.overwrite('VkQueueFamilyProperties', 'queueCount', ['None'], 'max')
        self.overwrite('VkQueueFamilyProperties', 'timestampValidBits', ['None'], 'bits')
        self.overwrite('VkQueueFamilyProperties', 'minImageTransferGranularity', ['None'], 'min,mul')

        self.overwrite('VkSparseImageFormatProperties', 'aspectMask', ['None'], 'bitmask')
        self.overwrite('VkSparseImageFormatProperties', 'imageGranularity', ['None'], 'min,mul')
        self.overwrite('VkSparseImageFormatProperties', 'flags', ['None'], 'bitmask')

        self.overwrite('VkPhysicalDeviceDescriptorBufferTensorPropertiesARM', 'tensorCaptureReplayDescriptorDataSize', ['None'], 'noauto')
        self.overwrite('VkPhysicalDeviceDescriptorBufferTensorPropertiesARM', 'tensorViewCaptureReplayDescriptorDataSize', ['None'], 'noauto')
        self.overwrite('VkPhysicalDeviceDescriptorBufferTensorPropertiesARM', 'tensorDescriptorSize', ['None'], 'max')

        # TODO: The registry xml contains some return structures that contain count + pointers to arrays
        # While the script itself is prepared to drop those, as they are ill-formed, as return structures
        # should never contain such pointers, some of the structures (e.g. 'VkVideoProfilesKHR') actually
        # doesn't even have the proper 'len' attribute to be able to detect the dynamic array
        # Hence here we simply remove such "disallow-listed" structs so that they don't get in the way
        self.structs.pop('VkDrmFormatModifierPropertiesListEXT', None)
        self.structs.pop('VkDrmFormatModifierPropertiesList2EXT', None)

    def getExtensionPromotedToVersion(self, extensionName):
        promotedTo = self.extensions[extensionName].promotedTo.copy()
        version = None
        while len(promotedTo) > 0:
            target = promotedTo[0]
            if target in self.extensions:
                # Functionality was promoted to another extension, continue with that
                promotedTo.remove(target)
                promotedTo.extend(self.extensions[target].promotedTo)
            elif target in self.versions:
                # Found extension in a core API version, we're done
                version = self.versions[target]
                break
            else:
                # Version or extension is not included in the target API
                promotedTo.remove(target)
        return version

    def getExtensionPromotedToExtensionList(self, extensionName):
        promotedTo = self.extensions[extensionName].promotedTo.copy()
        extensions = []
        while len(promotedTo) > 0:
            target = promotedTo[0]
            if target in self.extensions:
                # Functionality was promoted to another extension, add to list and continue with that
                extensions.append(target)
                promotedTo.remove(target)
                promotedTo.extend(self.extensions[target].promotedTo)
            else:
                # Extension is not included in the target API or is a version, skip
                promotedTo.remove(target)
        return extensions

    def getChainableStructDef(self, name, extends):
        structDef = self.structs.get(name)
        if structDef == None:
            Log.f("Structure '{0}' does not exist".format(name))
        if structDef.sType == None:
            Log.f("Structure '{0}' is not chainable".format(name))
        if not extends in structDef.extends + [ name ]:
            Log.f("Structure '{0}' does not extend '{1}'".format(name, extends))
        return structDef

    def evalArraySize(self, arraySize):
        if isinstance(arraySize, str):
            if arraySize in self.constants:
                return int(self.constants[arraySize])
            else:
                Log.f("Invalid array size '{0}'".format(arraySize))
        else:
            return arraySize

    def getNonAliasTypeName(self, alias, types):
        typeDef = types[alias]
        if typeDef.isAlias:
            for alias in typeDef.aliases:
                if not types[alias].isAlias:
                    return alias
        else:
            return alias


