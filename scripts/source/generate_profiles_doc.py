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

import functools
import itertools
import logging
import re
from collections import OrderedDict

from vulkan_object import VulkanObject, VideoCodec
from source.vulkan_object_utils import getStructByName
from source.vulkan_object_version import VK_VERSION
from source.generate_api_library import LibraryRegistry, LibraryMember, VulkanProfile, VulkanProfileCapabilities, VulkanProfilesDatabase


DOC_MD_HEADER = '''
<!-- markdownlint-disable MD041 -->
<p align="left"><img src="https://vulkan.lunarg.com/img/NewLunarGLogoBlack.png" alt="LunarG" width=263 height=113 /></p>
<p align="left">Copyright (c) 2021-2026 LunarG, Inc.</p>

<p align="center"><img src="./images/logo.png" width=400 /></p>

[![Creative Commons][3]][4]

[3]: https://i.creativecommons.org/l/by-nd/4.0/88x31.png "Creative Commons License"
[4]: https://creativecommons.org/licenses/by-nd/4.0/
'''


def formatVersion(version: VK_VERSION) -> str:
    return '{0}.{1}'.format(version.major, version.minor)


class DocStruct():
    """A structure referenced by one of its names (possibly an alias), with the API version and extensions defining that name."""
    def __init__(self, name: str, isAlias: bool, aliases: list[str], members: dict[str, LibraryMember], definedByVersion: VK_VERSION | None, definedByExtensions: list[str]):
        self.name = name
        self.isAlias = isAlias
        self.aliases = aliases
        self.members = members
        self.definedByVersion = definedByVersion
        self.definedByExtensions = definedByExtensions


class DocCapability():
    """A feature or a limit/property member name, with all the structures defining a member by this name."""
    def __init__(self, name: str):
        self.name = name
        # Ordered set of the structure names, the legacy generator used a set making the documentation depend on the string hash seed
        # when a profile defines a member in several of these structures
        # TODO: The order is the VulkanObject structures order, check which structure should be preferred when a profile defines several
        self.structs: dict[str, None] = dict()

    def add(self, structNames: list[str]):
        self.structs.update(dict.fromkeys(structNames))

    def isSupersetOf(self, other: 'DocCapability') -> bool:
        return self.structs.keys() >= other.structs.keys()


class DocScope():
    """The features and the limits/properties defined by an API version or an extension."""
    def __init__(self, name: str, number: VK_VERSION | None = None, type: str | None = None, promotedTo: str | None = None):
        self.name = name
        self.number = number
        self.type = type
        self.promotedTo = promotedTo.split(',') if promotedTo is not None else []
        self.features: dict[str, DocCapability] = dict()
        self.limits: dict[str, DocCapability] = dict()


class DocRegistry(LibraryRegistry):
    """Lookups over VulkanObject for the data needed by the documentation generator."""
    def __init__(self, vk: VulkanObject):
        super().__init__(vk)
        self.docStructs: dict[str, DocStruct] = dict()

        # VK_FORMAT aliases to the formats they alias, and formats to their aliases
        self.formatAliases = { alias: field.name for field in self.vk.enums['VkFormat'].fields for alias in field.aliases }
        self.formatAliasesByFormat = { field.name: field.aliases for field in self.vk.enums['VkFormat'].fields }

        # Video codecs, None when the video registry is not loaded
        # TODO: The legacy generator only skipped video support when video.xml is missing, VulkanObject has no video codecs in that case
        self.videoCodecs = self.vk.videoCodecs if len(self.vk.videoCodecs) > 0 else None

        # Names of the structures defined by each API version and extension
        structNamesByRequirement: dict[str, list[str]] = dict()
        for struct in self.vk.structs.values():
            if struct.union:
                continue
            for name in [ struct.name ] + struct.aliases:
                for requirement in self.getStructNameRequirements(name):
                    structNamesByRequirement.setdefault(requirement, []).append(name)

        self.versions: dict[str, DocScope] = OrderedDict()
        for versionName in sorted(self.vk.versions.keys(), key = lambda versionName: VK_VERSION(versionName)):
            self.versions[versionName] = DocScope(versionName, number = VK_VERSION(versionName))

        self.extensions: dict[str, DocScope] = OrderedDict()
        for extension in self.vk.extensions.values():
            type = 'instance' if extension.instance else 'device' if extension.device else None
            self.extensions[extension.name] = DocScope(extension.name, type = type, promotedTo = extension.promotedTo)

        self.parseFeatures(structNamesByRequirement)
        self.parseLimits(structNamesByRequirement)

    def getStructNameRequirements(self, name: str) -> dict[str, str | None]:
        # Returns the API versions and extensions defining a structure by this exact name, excluding its other names
        struct = getStructByName(self.vk.structs, name)
        return struct.definingRequirements if name == struct.name else self.vk.aliasTypeRequirements.get(name, {})

    def getDocStruct(self, name: str) -> DocStruct | None:
        if name in self.docStructs:
            return self.docStructs[name]

        struct = getStructByName(self.vk.structs, name)
        if struct is None:
            return None

        if struct.name not in self.members:
            self.members[struct.name] = self.getMembers(struct)

        definedByVersion, definedByExtensions = self.getStructRequirements(name)
        if name == struct.name and definedByVersion is None and len(definedByExtensions) == 0:
            # Use alias structure dependencies as the structure dependencies if the latter has none
            # TODO: The legacy generator used the dependencies of the last alias it processed, this uses the first alias having some
            for alias in struct.aliases:
                definedByVersion, definedByExtensions = self.getStructRequirements(alias)
                if definedByVersion is not None or len(definedByExtensions) > 0:
                    break

        # TODO: The aliases are in VulkanObject order, the legacy generator used the vk.xml types order
        aliases = [ struct.name ] + struct.aliases
        self.docStructs[name] = DocStruct(name, name != struct.name, aliases, self.members[struct.name], definedByVersion, definedByExtensions)
        return self.docStructs[name]

    def getNonAliasStructName(self, name: str) -> str:
        return getStructByName(self.vk.structs, name).name

    def getStructExtends(self, name: str) -> list[str]:
        return getStructByName(self.vk.structs, name).extends

    def addCapabilities(self, capabilities: dict[str, DocCapability], structName: str, createAlways: bool):
        structDef = self.getDocStruct(structName)
        for memberName in structDef.members.keys():
            if createAlways or not memberName in capabilities:
                capabilities[memberName] = DocCapability(memberName)
            capabilities[memberName].add(structDef.aliases)

    def parseFeatures(self, structNamesByRequirement: dict[str, list[str]]):
        # First, parse features specific to Vulkan versions
        for version in self.versions.values():
            if version.number == VK_VERSION.V1_0:
                # For version 1.0 use VkPhysicalDeviceFeatures
                for memberName in self.getDocStruct('VkPhysicalDeviceFeatures').members.keys():
                    version.features[memberName] = DocCapability(memberName)
                    version.features[memberName].add([ 'VkPhysicalDeviceFeatures' ])
            else:
                # For all other versions use the feature structures required by it
                featureStructNames = [ name for name in structNamesByRequirement.get(version.name, []) if 'VkPhysicalDeviceFeatures2' in self.getStructExtends(name) ]
                # VkPhysicalDeviceVulkan11Features is defined in Vulkan 1.2, but it actually
                # contains Vulkan 1.1 features, so treat it as such
                if version.number == VK_VERSION.V1_1:
                    featureStructNames.append('VkPhysicalDeviceVulkan11Features')
                elif version.number == VK_VERSION.V1_2:
                    if 'VkPhysicalDeviceVulkan11Features' in featureStructNames:
                        featureStructNames.remove('VkPhysicalDeviceVulkan11Features')
                # For each feature collect all feature structures containing them, and their aliases
                for featureStructName in featureStructNames:
                    self.addCapabilities(version.features, featureStructName, False)

        # Then parse features specific to extensions
        for extension in self.extensions.values():
            featureStructNames = [ name for name in structNamesByRequirement.get(extension.name, []) if 'VkPhysicalDeviceFeatures2' in self.getStructExtends(name) ]
            # For each feature collect all feature structures containing them, and their aliases
            for featureStructName in featureStructNames:
                self.addCapabilities(extension.features, featureStructName, True)
            # For each feature we also have to check whether it's part of core so that
            # any not strictly alias struct (i.e. the VkPhysicalDeviceVulkanXXFeatures)
            # get included as well
            for memberName, feature in extension.features.items():
                for version in self.versions.values():
                    if memberName in version.features and version.features[memberName].isSupersetOf(extension.features[memberName]):
                        extension.features[memberName] = version.features[memberName]

    def parseLimits(self, structNamesByRequirement: dict[str, list[str]]):
        # First, parse properties/limits specific to Vulkan versions
        for version in self.versions.values():
            if version.number == VK_VERSION.V1_0:
                # The properties extension structures are a misnomer, as they contain limits,
                # however, the naming will stay with us, so in order to avoid nested
                # "properties" (limits), we simply use VkPhysicalDeviceLimits directly here
                # for version 1.0 limits, plus, not having a better place to put them, we
                # also include VkPhysicalDeviceSparseProperties here (even though they are
                # more like features)
                limitStructNames = [ 'VkPhysicalDeviceLimits', 'VkPhysicalDeviceSparseProperties' ]
            else:
                # For all other versions use the property structures required by it
                limitStructNames = [ name for name in structNamesByRequirement.get(version.name, []) if 'VkPhysicalDeviceProperties2' in self.getStructExtends(name) ]
                # VkPhysicalDeviceVulkan11Properties is defined in Vulkan 1.2, but it actually
                # contains Vulkan 1.1 limits, so treat it as such
                if version.number == VK_VERSION.V1_1:
                    limitStructNames.append('VkPhysicalDeviceVulkan11Properties')
                elif version.number == VK_VERSION.V1_2:
                    if 'VkPhysicalDeviceVulkan11Properties' in limitStructNames:
                        limitStructNames.remove('VkPhysicalDeviceVulkan11Properties')
            # For each limit collect all property/limit structures containing them, and their aliases
            for limitStructName in limitStructNames:
                self.addCapabilities(version.limits, limitStructName, False)

        # Then parse properties/limits specific to extensions
        for extension in self.extensions.values():
            limitStructNames = [ name for name in structNamesByRequirement.get(extension.name, []) if 'VkPhysicalDeviceProperties2' in self.getStructExtends(name) ]
            # For each limit collect all property/limit structures containing them, and their aliases
            for limitStructName in limitStructNames:
                self.addCapabilities(extension.limits, limitStructName, True)
            # For each limit we also have to check whether it's part of core so that
            # any not strictly alias struct (i.e. the VkPhysicalDeviceVulkanXXProperties)
            # get included as well
            for memberName, limit in extension.limits.items():
                for version in self.versions.values():
                    if memberName in version.limits and version.limits[memberName].isSupersetOf(extension.limits[memberName]):
                        extension.limits[memberName] = version.limits[memberName]

    def getExtensionPromotedToVersion(self, extensionName: str) -> DocScope | None:
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


class VulkanProfilesDocGenerator():
    def __init__(self, registry: DocRegistry, profiles: dict[str, VulkanProfile], doc_capabilities: dict[str, VulkanProfileCapabilities]):
        self.registry = registry
        self.doc_capabilities = doc_capabilities
        sorted_profiles = sorted(profiles.values(), key = self.sort_KHR_EXT_first)

        self.profiles = []
        for profile in sorted_profiles:
            if (not profile.multiple_variants):
                self.profiles.append(profile)

        if not self.profiles:
            logging.error("No valid non-variant profiles were found to generate documentation.")
            self.maxRequiredCoreVersion = VK_VERSION.V1_0
            self.coreInstanceExtensions = []
            self.coreDeviceExtensions = []
            return

        # Determine maximum core version required across all profiles
        self.maxRequiredCoreVersion = max(profile.apiVersionNumber for profile in self.profiles)

        # Collect extensions required by core versions up to the maximum core version required
        # across all profiles so that we can include related data in the relevant tables
        self.coreInstanceExtensions = []
        self.coreDeviceExtensions = []
        for extension in self.registry.extensions.values():
            version = self.registry.getExtensionPromotedToVersion(extension.name)
            if version != None and version.number <= self.maxRequiredCoreVersion:
                if extension.type == 'instance':
                    self.coreInstanceExtensions.append(extension.name)
                elif extension.type == 'device':
                    self.coreDeviceExtensions.append(extension.name)


    def sort_KHR_EXT_first(self, profileOrExtName):
        # Make sure KHR profiles and extensions come first and EXT extensions come next
        key = profileOrExtName.key if isinstance(profileOrExtName, VulkanProfile) else profileOrExtName
        if key[2:7] == '_KHR_':
            return 'A' + key
        elif key[2:7] == '_KHX_':
            return 'B' + key
        elif key[2:7] == '_EXT_':
            return 'C' + key
        else:
            return key


    def getDocCapabilities(self, profile: VulkanProfile) -> VulkanProfileCapabilities:
        return self.doc_capabilities[profile.key]


    def generate(self, outDoc: str):
        logging.info("Generating '{0}'...".format(outDoc))
        with open(outDoc, 'w') as f:
            f.write(self.gen_doc())


    def gen_doc(self):
        gen = DOC_MD_HEADER
        gen += '\n# Vulkan Profiles Definitions\n'
        gen += self.gen_profilesList()
        gen += self.gen_extensions()
        gen += self.gen_features()
        gen += self.gen_limits()
        gen += self.gen_queueFamilies()
        gen += self.gen_formats()
        gen += self.gen_videoProfiles()
        return gen


    def gen_manPageLink(self, entry, text):
        # The version is irrelevant currently in the man page base link as it gets redirected to
        # the latest version's corresponding page, so we simply use version 1.1 as convention
        return '[{0}](https://www.khronos.org/registry/vulkan/specs/1.1-extensions/man/html/{1}.html)'.format(text, entry)


    def gen_table(self, rowHandlers):
        gen = '| Profiles |'
        cellFmt = ' {0} |'
        for profile in self.profiles:
            gen += cellFmt.format(profile.key)
        gen += '\n{0}'.format(re.sub(r"[^|]", '-', gen))
        for row, rowHandler in rowHandlers.items():
            gen += '\n| {0} |'.format(row)
            for profile in self.profiles:
                gen += cellFmt.format(rowHandler(row, profile))
        return gen


    def gen_sectionedTable(self, rowHandlers):
        gen = '| Profiles |'
        cellFmt = ' {0} |'
        for profile in self.profiles:
            gen += cellFmt.format(profile.key)
        gen += '\n{0}'.format(re.sub(r"[^|]", '-', gen))
        for section, sectionRowHandlers in rowHandlers.items():
            gen += '\n| **{0}** |'.format(section)
            for row, rowHandler in sectionRowHandlers.items():
                gen += '\n| {0} |'.format(rowHandler(section, row))
                for profile in self.profiles:
                    gen += cellFmt.format(rowHandler(section, row, profile))
        return gen


    def gen_profilesList(self):
        return '\n## Vulkan Profiles List\n\n{0}\n'.format(self.gen_table(OrderedDict({
            'Label': lambda _, profile : profile.label,
            'Description': lambda _, profile : profile.description,
            'Version': lambda _, profile : profile.version,
            'Required API version': lambda _, profile : profile.apiVersion,
            # TODO: The legacy generator checked for None, VulkanProfile always sets a (possibly empty) list of required profiles
            'Required profiles': lambda _, profile : ', '.join(profile.profileRequirements) if profile.profileRequirements != None else '-',
            'Fallback profiles': lambda _, profile : ', '.join(profile.fallbacks) if profile.fallbacks != None else '-'
        })))

    def gen_extension(self, section, extension, profile = None):
        # If no profile was specified then this is the first column so return the extension name
        # with a link to the extension's manual page
        if profile is None:
            return self.gen_manPageLink(extension, extension)

        # If it's an extension explicitly required by the profile then this is a supported extension
        if extension in profile.merge_capabilities.extensions:
            return ':heavy_check_mark:'

        # Otherwise check if this extension has been promoted to a core API version that the profile requires
        version = self.registry.getExtensionPromotedToVersion(extension)
        # If core API version found and is required by the profile then this extension is supported as being core
        if version != None and version.number <= profile.apiVersionNumber:
            return formatVersion(version.number) + ' Core'

        # Otherwise it's unsupported
        return ':x:'


    def gen_extensions(self):
        # Collect instance extensions defined by the profiles
        instanceExtensions = self.coreInstanceExtensions + list(itertools.chain(*[
            profile.merge_capabilities.instanceExtensions.keys() for profile in self.profiles
        ]))
        instanceExtensions.sort(key = self.sort_KHR_EXT_first)

        # Collect device extensions defined by the profiles
        deviceExtensions = self.coreDeviceExtensions + list(itertools.chain(*[
            profile.merge_capabilities.deviceExtensions.keys() for profile in self.profiles
        ]))
        deviceExtensions.sort(key = self.sort_KHR_EXT_first)

        # Generate table legend
        legend = (
            '* :heavy_check_mark: indicates that the extension is defined in the profile\n'
            '* "X.X Core" indicates that the extension is not defined in the profile but '
            'the extension is promoted to the specified core API version that is smaller than '
            'or equal to the minimum required API version of the profile\n'
            '* :x: indicates that the extension is neither defined in the profile nor it is '
            'promoted to a core API version that is smaller than or equal to the minimum '
            'required API version of the profile\n'
        )

        # Generate table
        table = self.gen_sectionedTable(OrderedDict({
            'Instance extensions': OrderedDict({ row: self.gen_extension for row in instanceExtensions }),
            'Device extensions': OrderedDict({ row: self.gen_extension for row in deviceExtensions })
        }))
        return '\n## Vulkan Profiles Extensions\n\n{0}\n{1}\n'.format(legend, table)


    def has_nestedFeatureData(self, data):
        for key in data:
            if not isinstance(data[key], bool):
                return True
        return None


    def getDefinedWhere(self, structDef: DocStruct, section, vulkan11StructName: str):
        # Returns where a structure is defined and whether this matches the section
        # VkPhysicalDeviceVulkan11Features/Properties are defined in Vulkan 1.2, but actually define Vulkan 1.1 features/properties
        if structDef.name == vulkan11StructName:
            where = 'Vulkan 1.1'
            isExactMatch = (section == where)
        elif structDef.definedByVersion != None:
            where = 'Vulkan {0}'.format(formatVersion(structDef.definedByVersion))
            isExactMatch = (section == where)
        elif len(structDef.definedByExtensions) > 0:
            where = '/'.join(structDef.definedByExtensions)
            isExactMatch = (section in structDef.definedByExtensions)
        else:
            where = 'Vulkan 1.0'
            isExactMatch = (section == where)
        return where, isExactMatch


    def formatFeatureSupport(self, supported, struct, section):
        where, isExactMatch = self.getDefinedWhere(self.registry.getDocStruct(struct), section, 'VkPhysicalDeviceVulkan11Features')
        if supported:
            if isExactMatch:
                return '<span title="defined in {0} ({1})">:heavy_check_mark:</span>'.format(struct, where)
            else:
                return '<span title="equivalent defined in {0} ({1})">:warning:</span>'.format(struct, where)
        else:
            return ':x:'


    def getFeatureStructSynonyms(self, struct, member):
        structDef = self.registry.getDocStruct(struct)
        if structDef.definedByVersion != None:
            # For 1.1+ core features we always have two structures defining the feature, one is
            # the feature specific structure, the other is VkPhysicalDeviceVulkanXXFeatures
            if struct == 'VkPhysicalDeviceVulkan11Features':
                # VkPhysicalDeviceVulkan11Features is defined in Vulkan 1.2, but actually it
                # defines Vulkan 1.1 features
                version = self.registry.versions['VK_VERSION_1_1']
            else:
                # For other structures find the version defining the structure
                for version in self.registry.versions.values():
                    if version.number == structDef.definedByVersion:
                        break
            # Return all the structures defining this feature member
            return version.features[member].structs
        else:
            # In all other cases we're talking about a non-promoted extension, as the structure
            # we receive here is always a non-alias structure, so we can simply return the
            # aliases of the structure
            return structDef.aliases


    def getFeatureStructForManPageLink(self, struct, member):
        # We don't want to link to the man page VkPhysicalDeviceVulkanXXFeatures structures,
        # instead we prefer to use the more specific non-alias structure if possible
        # TODO: If several non-alias structures define the feature, the last one in VulkanObject order is selected, check which is preferred
        for alias in self.getFeatureStructSynonyms(struct, member):
            if re.match(r"^VkPhysicalDeviceVulkan[0-9]+Features$", alias) is None:
                structDef = self.registry.getDocStruct(alias)
                if not structDef.isAlias:
                    struct = alias
        return struct


    def gen_feature(self, struct, section, member, profile = None):
        # If no profile was specified then this is the first column so return the member name
        # with a link to the encompassing structure's manual page
        if profile is None:
            return self.gen_manPageLink(self.getFeatureStructForManPageLink(struct, member),
                                        member)

        doc_capabilities = self.getDocCapabilities(profile)

        # If this feature struct member is defined in the profile as is, consider it supported
        if struct in doc_capabilities.features:
            featureStruct = doc_capabilities.features[struct]
            if member in featureStruct:
                return self.formatFeatureSupport(featureStruct[member], struct, section)

        # If the struct is VkPhysicalDeviceFeatures then check if the feature is defined in
        # VkPhysicalDeviceFeatures2 or VkPhysicalDeviceFeatures2KHR for the profile and then
        # consider it supported
        if struct == 'VkPhysicalDeviceFeatures':
            for wrapperStruct in [ 'VkPhysicalDeviceFeatures2', 'VkPhysicalDeviceFeatures2KHR' ]:
                if wrapperStruct in doc_capabilities.features:
                    featureStruct = doc_capabilities.features[wrapperStruct]['features']
                    if member in featureStruct:
                        return self.formatFeatureSupport(featureStruct[member], struct, section)

        # If the struct has aliases and the feature struct member is defined in the profile in
        # one of those, consider it supported
        for alias in self.getFeatureStructSynonyms(struct, member):
            if alias in doc_capabilities.features:
                featureStruct = doc_capabilities.features[alias]
                if member in featureStruct:
                    return self.formatFeatureSupport(featureStruct[member], alias, section)

        return self.formatFeatureSupport(False, struct, section)


    def gen_featuresSection(self, features, definedFeatures, sectionHeader, tableData):
        # Go through defined feature structures
        for definedFeatureStructName, definedFeatureList in definedFeatures.items():
            # Go through defined features within those structures
            for definedFeature in definedFeatureList:
                # Check if there's a feature with a matching name in the features to consider
                if definedFeature in features.keys():
                    feature = features[definedFeature]
                    # Check that the feature structure actually matches one of the structures
                    # this feature is defined in (this is needed because the registry xml doesn't
                    # prevent multiple structures defining features with identical names so we
                    # have to check whether we actually talk about a synonym or a completely
                    # different feature with the same name)
                    if definedFeatureStructName in feature.structs:
                        if not sectionHeader in tableData:
                            tableData[sectionHeader] = OrderedDict()
                        # Feature is defined, add it to the table
                        # TODO: When a profile defines a feature in several structures (e.g. VkPhysicalDeviceDescriptorIndexingFeatures and
                        # VkPhysicalDeviceVulkan12Features), the last one is reported. The legacy generator reported the specific structure
                        # only because the VK_BASE/COMPUTE/GRAPHICS_VERSION_X_Y split of vk.xml hid the core structures it requires
                        tableData[sectionHeader][definedFeature] = functools.partial(self.gen_feature, definedFeatureStructName)


    def gen_features(self):
        # Merge all feature references across the profiles to collect the relevant features to look at
        definedFeatures = dict()
        for profile in self.profiles:
            for featureStructName, features in self.getDocCapabilities(profile).features.items():
                # VkPhysicalDeviceFeatures2 is an exception, as it contains a nested structure
                # No other structure is allowed to have this
                if featureStructName in [ 'VkPhysicalDeviceFeatures2', 'VkPhysicalDeviceFeatures2KHR' ]:
                    featureStructName = 'VkPhysicalDeviceFeatures'
                    features = features['features']
                elif self.has_nestedFeatureData(features):
                    logging.error("Unexpected nested feature data in profile '{0}' structure '{1}'".format(profile.key, featureStructName))
                # If this is an alias structure then find the non-alias one and use that
                featureStructName = self.registry.getNonAliasStructName(featureStructName)
                # Copy defined feature structure data
                if not featureStructName in definedFeatures:
                    definedFeatures[featureStructName] = []
                definedFeatures[featureStructName].extend(features.keys())

        tableData = OrderedDict()

        # First, go through core features
        for version in self.registry.versions.values():
            self.gen_featuresSection(version.features, definedFeatures, 'Vulkan ' + formatVersion(version.number), tableData)

        # Then, go through extensions
        for extension in sorted(self.registry.extensions.values(), key = lambda extension: self.sort_KHR_EXT_first(extension.name)):
            self.gen_featuresSection(extension.features, definedFeatures, extension.name, tableData)

        # Sort individual features within the sections by name
        for sectionName in tableData.keys():
            tableData[sectionName] = OrderedDict(sorted(tableData[sectionName].items()))

        # TODO: Currently we don't include features that are required by the minimum required API
        # version of a profile, or features required by extensions required by the profile, as
        # that would necessitate the inclusion of the information currently only available
        # textually in the "Feature Requirements" section of the Vulkan Specification
        disclaimer = (
            '> **NOTE**: The table below only contains features explicitly defined by the '
            'corresponding profile. Further features may be supported by the profiles in '
            'accordance to the requirements defined in the "Feature Requirements" section '
            'of the appropriate version of the Vulkan API Specification.'
        )

        # Generate table legend
        legend = (
            '* :heavy_check_mark: indicates that the feature is defined in the profile (hover '
            'over the symbol to view the structure and corresponding extension or core API '
            'version where the feature is defined in the profile)\n'
            '* :warning: indicates that the feature is not defined in the profile but an '
            'equivalent feature is (hover over the symbol to view the structure and '
            'corresponding extension or core API version where the feature is defined in the '
            'profile)\n'
            '* :x: indicates that neither the feature nor an equivalent feature is defined in '
            'the profile\n'
        )

        # Generate table
        table = self.gen_sectionedTable(tableData)
        return '\n## Vulkan Profile Features\n\n{0}\n\n{1}\n{2}\n'.format(disclaimer, legend, table)


    def formatValue(self, value):
        if type(value) == bool:
            # Boolean
            return 'VK_TRUE' if value else 'VK_FALSE'
        elif type(value) == dict:
            # Structure, match the Vulkan Specification's formatting
            return '({0})'.format(','.join(str(el) for el in value.values()))
        elif type(value) == list:
            if len(value) == 0:
                # Empty array, not much to return
                return '-'
            elif type(value[0]) == str:
                # Bitmask, match the Vulkan Specification's formatting
                return '({0})'.format(' \\| '.join(value))
            elif type(value[0]) == dict:
                # Array of structures (e.g. VkDrmFormatModifierPropertiesListEXT::pDrmFormatModifierProperties), one structure per line
                return '<br>'.join(
                    '{{{0}}}'.format(', '.join('{0}: {1}'.format(name, self.formatValue(member)) for name, member in element.items()))
                    for element in value)
            else:
                # Array, match the Vulkan Specification's formatting
                return '({0})'.format(','.join(str(el) for el in value))
        else:
            return str(value)


    def formatProperty(self, value, struct, section = None):
        where, isExactMatch = self.getDefinedWhere(self.registry.getDocStruct(struct), section, 'VkPhysicalDeviceVulkan11Properties')
        if isExactMatch or section == None:
            return '<span title="defined in {0} ({1})">{2}</span>'.format(struct, where, self.formatValue(value))
        else:
            return '<span title="equivalent defined in {0} ({1})">_{2}_</span>'.format(struct, where, self.formatValue(value))


    def formatLimitName(self, struct, member):
        structDef = self.registry.getDocStruct(struct)
        memberDef = structDef.members[member]
        limittype = memberDef.limittype

        if limittype in [ None, 'noauto', 'bitmask', 'struct' ]:
            return member
        elif limittype == 'exact':
            return member + ' (exact)'
        elif limittype == 'max':
            return member + ' (max)'
        elif limittype == 'max,pot' or limittype == 'pot,max':
            return member + ' (max,pot)'
        elif limittype in [ 'min' ]:
            return member + ' (min)'
        elif limittype == 'min,pot' or limittype == 'pot,min':
            return member + ' (min,pot)'
        elif limittype == 'min,mul' or limittype == 'mul,min':
            return member + ' (min,mul)'
        elif limittype == 'bits':
            return member + ' (bits)'
        elif limittype == 'range':
            return member + ' (min-max)'
        else:
            logging.error("Unexpected limittype '{0}'".format(limittype))


    def getLimitStructSynonyms(self, struct, member):
        structDef = self.registry.getDocStruct(struct)
        if structDef.definedByVersion != None:
            # For 1.1+ core limits we always have two structures defining the limit, one is
            # the limit specific structure, the other is VkPhysicalDeviceVulkanXXProperties
            if struct == 'VkPhysicalDeviceVulkan11Properties':
                # VkPhysicalDeviceVulkan11Properties is defined in Vulkan 1.2, but actually it
                # defines Vulkan 1.1 limits
                version = self.registry.versions['VK_VERSION_1_1']
            else:
                # For other structures find the version defining the structure
                for version in self.registry.versions.values():
                    if version.number == structDef.definedByVersion and member in version.limits:
                        break
            # Return all the structures defining this limit member
            return version.limits[member].structs
        else:
            # In all other cases we're talking about a non-promoted extension, as the structure
            # we receive here is always a non-alias structure, so we can simply return the
            # aliases of the structure
            return structDef.aliases


    def getLimitStructForManPageLink(self, struct, member):
        # If the structure at hand is VkPhysicalDeviceProperties then we should rather link
        # to the underlying nested structure that actually defines the limit
        if struct == 'VkPhysicalDeviceProperties':
            structs = self.registry.versions['VK_VERSION_1_0'].limits[member].structs
            for nestedStruct in [ 'VkPhysicalDeviceLimits', 'VkPhysicalDeviceSparseProperties' ]:
                if nestedStruct in structs:
                    return nestedStruct

        # We don't want to link to the man page VkPhysicalDeviceVulkanXXProperties structures,
        # instead we prefer to use the more specific non-alias structure if possible
        for alias in self.getLimitStructSynonyms(struct, member):
            if re.match(r"^VkPhysicalDeviceVulkan[0-9]+Properties$", alias) is None:
                structDef = self.registry.getDocStruct(alias)
                if not structDef.isAlias:
                    struct = alias
        return struct


    def gen_limit(self, struct, section, member, profile = None):
        # If no profile was specified then this is the first column so return the member name
        # decorated with the corresponding limittype specific info and a link to the
        # encompassing structure's manual page
        if profile is None:
            return self.gen_manPageLink(self.getLimitStructForManPageLink(struct, member),
                                        self.formatLimitName(struct, member))

        doc_capabilities = self.getDocCapabilities(profile)

        # If this limit/property struct member is defined in the profile as is, include it
        if struct in doc_capabilities.properties:
            limitStruct = doc_capabilities.properties[struct]
            if member in limitStruct:
                return self.formatProperty(limitStruct[member], struct, section)

        # If the struct is VkPhysicalDeviceLimits or VkPhysicalDeviceSparseProperties then check
        # if the limit/property is defined somewhere nested in VkPhysicalDeviceProperties,
        # VkPhysicalDeviceProperties2, or VkPhysicalDeviceProperties2KHR for the profile then
        # include it
        if struct == 'VkPhysicalDeviceLimits' or struct == 'VkPhysicalDeviceSparseProperties':
            if struct == 'VkPhysicalDeviceLimits':
                memberStruct = 'limits'
            else:
                memberStruct = 'sparseProperties'
            propertyStruct = None
            if 'VkPhysicalDeviceProperties' in doc_capabilities.properties:
                propertyStructName = 'VkPhysicalDeviceProperties'
                propertyStruct = doc_capabilities.properties[propertyStructName]
            for wrapperStruct in [ 'VkPhysicalDeviceProperties2', 'VkPhysicalDeviceProperties2KHR' ]:
                if wrapperStruct in doc_capabilities.properties:
                    propertyStructName = wrapperStruct
                    propertyStruct = doc_capabilities.properties[wrapperStruct]['properties']
            if propertyStruct != None:
                if memberStruct in propertyStruct:
                    limitStruct = propertyStruct[memberStruct]
                    if member in limitStruct:
                        return self.formatProperty(limitStruct[member], propertyStructName, section)

        # If the struct has aliases and the limit/property struct member is defined in the profile
        # in one of those then include it
        # TODO: Unlike the checks above, falsy values (e.g. 0, VK_FALSE, empty lists) defined in an alias are skipped, as with the legacy generator
        for alias in self.getLimitStructSynonyms(struct, member):
            if alias in doc_capabilities.properties:
                limitStruct = doc_capabilities.properties[alias]
                if member in limitStruct and limitStruct[member]:
                    return self.formatProperty(limitStruct[member], alias, section)

        return '-'


    def gen_limitsSection(self, limits, definedLimits, sectionHeader, tableData):
        # Go through defined limit/property structures
        for definedLimitStructName, definedLimitList in definedLimits.items():
            # Go through defined limits within those structures
            for definedLimit in definedLimitList:
                # Check if there's a limit with a matching name in the limits to consider
                if definedLimit in limits.keys():
                    limit = limits[definedLimit]
                    # Check that the limit/property structure actually matches one of the
                    # structures this limit is defined in (this is needed because the registry xml
                    # doesn't prevent multiple structures defining limits/properties with
                    # identical names so we have to check whether we actually talk about a synonym
                    # or a completely different limit/property with the same name)
                    if definedLimitStructName in limit.structs:
                        if not sectionHeader in tableData:
                            tableData[sectionHeader] = OrderedDict()
                        # Limit/property is defined, add it to the table
                        # TODO: When a profile defines a limit in several structures (e.g. VkPhysicalDeviceDescriptorIndexingProperties and
                        # VkPhysicalDeviceVulkan12Properties), the last one is reported, see gen_featuresSection
                        tableData[sectionHeader][definedLimit] = functools.partial(self.gen_limit, definedLimitStructName)


    def gen_limits(self):
        # Merge all limit/property references across the profiles to collect the relevant limits to look at
        definedLimits = dict()
        for profile in self.profiles:
            for propertyStructName, properties in self.getDocCapabilities(profile).properties.items():
                # VkPhysicalDeviceProperties and VkPhysicalDeviceProperties2 are exceptions,
                # need custom handling due to only using their nested structures
                if propertyStructName in [ 'VkPhysicalDeviceProperties2', 'VkPhysicalDeviceProperties2KHR' ]:
                    propertyStructName = 'VkPhysicalDeviceProperties'
                    properties = properties['properties']
                if propertyStructName == 'VkPhysicalDeviceProperties':
                    for member, struct in { 'limits': 'VkPhysicalDeviceLimits', 'sparseProperties': 'VkPhysicalDeviceSparseProperties' }.items():
                        if member in properties:
                            if not struct in definedLimits:
                                definedLimits[struct] = []
                            definedLimits[struct].extend(properties[member].keys())
                    continue

                # If this is an alias structure then find the non-alias one and use that
                propertyStructName = self.registry.getNonAliasStructName(propertyStructName)
                # Copy defined limit/property structure data
                if not propertyStructName in definedLimits:
                    definedLimits[propertyStructName] = []
                definedLimits[propertyStructName].extend(properties.keys())

        tableData = OrderedDict()

        # First, go through core limits/properties
        for version in self.registry.versions.values():
            self.gen_limitsSection(version.limits, definedLimits, 'Vulkan ' + formatVersion(version.number), tableData)

        # Then, go through extensions
        for extension in sorted(self.registry.extensions.values(), key = lambda extension: self.sort_KHR_EXT_first(extension.name)):
            self.gen_limitsSection(extension.limits, definedLimits, extension.name, tableData)

        # Sort individual limits within the sections by name
        for sectionName in tableData.keys():
            tableData[sectionName] = OrderedDict(sorted(tableData[sectionName].items()))

        # TODO: Currently we don't include limits/properties that are required by the minimum
        # required API version of a profile, or limits/properties required by extensions required
        # by the profile, as that would necessitate the inclusion of information currently only
        # available textually in the "Limit Requirements" section of the Vulkan Specification
        disclaimer = (
            '> **NOTE**: The table below only contains properties/limits explicitly defined '
            'by the corresponding profile. Further properties/limits may be supported by the '
            'profiles in accordance to the requirements defined in the "Limit Requirements" '
            'section of the appropriate version of the Vulkan API Specification.'
        )

        # Generate table legend
        legend = (
            '* "valueWithRegularFont" indicates that the limit/property is defined in the profile '
            '(hover over the value to view the structure and corresponding extension or core API '
            'version where the limit/property is defined in the profile)\n'
            '* "_valueWithItalicFont_" indicates that the limit/property is not defined in the profile '
            'but an equivalent limit/property is (hover over the symbol to view the structure '
            'and corresponding extension or core API version where the limit/property is defined '
            'in the profile)\n'
            '* "-" indicates that neither the limit/property nor an equivalent limit/property is '
            'defined in the profile\n'
        )

        # Generate table
        table = self.gen_sectionedTable(tableData)
        return '\n## Vulkan Profile Limits (Properties)\n\n{0}\n\n{1}\n{2}\n'.format(disclaimer, legend, table)


    def gen_queueFamily(self, index, struct, section, member, profile = None):
        # If no profile was specified then this is the first column so return the member name
        # decorated with the corresponding limittype specific info and a link to the
        # encompassing structure's manual page
        if profile is None:
            return self.gen_manPageLink(struct, self.formatLimitName(struct, member))

        doc_capabilities = self.getDocCapabilities(profile)

        # If this profile doesn't even define this queue family index then early out
        if len(doc_capabilities.queueFamiliesProperties) <= index:
            return ''

        # If this queue family property struct member is defined in the profile as is, include it
        if struct in doc_capabilities.queueFamiliesProperties[index]:
            propertyStruct = doc_capabilities.queueFamiliesProperties[index][struct]
            if member in propertyStruct:
                return self.formatProperty(propertyStruct[member], struct)

        # If the struct is VkQueueFamilyProperties then check if the property is defined in
        # VkQueueFamilyProperties2 or VkQueueFamilyProperties2KHR for the profile and then include it
        # The legacy generator checked VkPhysicalDeviceQueueFamilyProperties[2[KHR]] names that don't exist in vk.xml
        if struct == 'VkQueueFamilyProperties':
            for wrapperStruct in [ 'VkQueueFamilyProperties2', 'VkQueueFamilyProperties2KHR' ]:
                if wrapperStruct in doc_capabilities.queueFamiliesProperties[index]:
                    propertyStruct = doc_capabilities.queueFamiliesProperties[index][wrapperStruct]['queueFamilyProperties']
                    if member in propertyStruct and propertyStruct[member]:
                        return self.formatProperty(propertyStruct[member], wrapperStruct)

        # If the struct has aliases and the property struct member is defined in the profile
        # in one of those then include it
        structDef = self.registry.getDocStruct(struct)
        for alias in structDef.aliases:
            if alias in doc_capabilities.queueFamiliesProperties[index]:
                propertyStruct = doc_capabilities.queueFamiliesProperties[index][alias]
                if member in propertyStruct and propertyStruct[member]:
                    return self.formatProperty(propertyStruct[member], alias)

        return '-'


    def gen_queueFamilies(self):
        # Merge all queue family property references across the profiles to collect the relevant
        # properties to look at for each queue family definition index
        definedQueueFamilies = []
        for profile in self.profiles:
            for index, queueFamily in enumerate(self.getDocCapabilities(profile).queueFamiliesProperties):
                definedQueueFamilyProperties = OrderedDict()
                for structName, properties in queueFamily.items():
                    # VkQueueFamilyProperties2 is an exception, as it contains a nested structure
                    # No other structure is allowed to have this
                    if structName in [ 'VkQueueFamilyProperties2', 'VkQueueFamilyProperties2KHR' ]:
                        structName = 'VkQueueFamilyProperties'
                        properties = properties['queueFamilyProperties']
                    # If this is an alias structure then find the non-alias one and use that
                    structName = self.registry.getNonAliasStructName(structName)
                    # Copy defined limit/property structure data
                    if not structName in definedQueueFamilyProperties:
                        definedQueueFamilyProperties[structName] = []
                    definedQueueFamilyProperties[structName].extend(sorted(properties.keys()))
                # Add queue family to the list
                if len(definedQueueFamilies) <= index:
                    definedQueueFamilies.append(dict())
                definedQueueFamilies[index].update(definedQueueFamilyProperties)

        # Construct table data
        tableData = OrderedDict()
        for index, queueFamilyProperties in enumerate(definedQueueFamilies):
            section = tableData['Queue family #' + str(index)] = OrderedDict()
            for structName, members in queueFamilyProperties.items():
                section.update({ row: functools.partial(self.gen_queueFamily, index, structName) for row in members })

        # Generate table legend
        legend = (
            '* "valueWithRegularFont" indicates that the queue family property is defined in the '
            'profile (hover over the value to view the structure and corresponding extension or '
            'core API version where the queue family property is defined in the profile)\n'
            '* "_valueWithItalicFont_" indicates that the queue family property is not defined in the '
            'profile but an equivalent queue family property is (hover over the symbol to view '
            'the structure and corresponding extension or core API version where the queue family '
            'property is defined in the profile)\n'
            '* "-" indicates that neither the queue family property nor an equivalent queue '
            'family property is defined in the profile\n'
            '* Empty cells next to the properties of a particular queue family definition section '
            'indicate that the profile does not have a corresponding queue family definition\n'
        )

        # Generate table
        table = self.gen_sectionedTable(tableData)
        return '\n## Vulkan Profile Queue Families\n\n{0}\n{1}\n'.format(legend, table)


    def getFormatStructForManPageLink(self, struct):
        # We prefer returning VkFormatProperties3 instead of VkFormatProperties as even though
        # they are technically not strictly aliases, the former is the one that should be used
        # going forward and the feature flags are anyway defined to be usable as synonyms for
        # the legacy 32-bit flags
        return 'VkFormatProperties3' if struct == 'VkFormatProperties' else struct


    def gen_formatRow(self, format, struct, member, section, row, profile = None):
        return self.gen_format(format, struct, section, member, profile)


    def gen_format(self, format, struct, section, member, profile = None):
        # If no profile was specified then this is the first column so return the member name
        # decorated with the corresponding limittype specific info and a link to the
        # encompassing structure's manual page
        if profile is None:
            return self.gen_manPageLink(self.getFormatStructForManPageLink(struct),
                                        self.formatLimitName(struct, member))

        doc_capabilities = self.getDocCapabilities(profile)

        # 'format' is a non-alias format name, the profile may define the format with one of its aliases
        # The legacy generator looked up the alias of 'format' itself, so the data of a format defined with an alias was not shown
        formatProperties = doc_capabilities.formats.get(format)
        if formatProperties is None:
            for alias in self.registry.formatAliasesByFormat.get(format, []):
                if alias in doc_capabilities.formats:
                    formatProperties = doc_capabilities.formats[alias]
                    break

        # If this profile doesn't even define this format then early out
        if formatProperties is None:
            return ''

        # If this format property struct member is defined in the profile as is, include it
        if struct in formatProperties:
            propertyStruct = formatProperties[struct]
            if member in propertyStruct:
                return self.formatProperty(propertyStruct[member], struct)

        # If the struct is VkFormatProperties then 'member' also contains the trimmed name of
        # the flag bit to check for, so we check for that, or any of its aliases
        if struct == 'VkFormatProperties':
            for alternative in [ 'VkFormatProperties', 'VkFormatProperties2', 'VkFormatProperties2KHR', 'VkFormatProperties3', 'VkFormatProperties3KHR' ]:
                if alternative in formatProperties:
                    propertyStruct = formatProperties[alternative]
                    # VkFormatProperties2[KHR] wrap the real structure in a member
                    if 'formatProperties' in propertyStruct:
                        propertyStruct = propertyStruct['formatProperties']
                    if member in propertyStruct:
                        return self.formatProperty(propertyStruct[member], alternative)

        # If the struct has aliases and the property struct member is defined in the profile
        # in one of those then include it
        structDef = self.registry.getDocStruct(struct)
        for alias in structDef.aliases:
            if alias in formatProperties:
                propertyStruct = formatProperties[alias]
                if member in propertyStruct and propertyStruct[member]:
                    return self.formatProperty(propertyStruct[member], alias)

        return '-'


    def gen_formats(self):
        # Merge all format property references across the profiles to collect the relevant
        # properties to look at for each format
        definedFormats = dict()
        for profile in self.profiles:
            for format, formatProperties in self.getDocCapabilities(profile).formats.items():
                # This may be an alias of a format name, so get the real name
                formatAliases = self.registry.formatAliases
                format = formatAliases[format] if format in formatAliases else format

                definedFormatProperties = OrderedDict()
                for structName, properties in formatProperties.items():
                    # VkFormatProperties, VkFormatProperties2, and VkFormatProperties3 are special
                    if structName in [ 'VkFormatProperties2', 'VkFormatProperties2KHR' ]:
                        structName = 'VkFormatProperties'
                        properties = properties['formatProperties']
                    if structName in [ 'VkFormatProperties3', 'VkFormatProperties3KHR' ]:
                        structName = 'VkFormatProperties'
                    # If this is an alias structure then find the non-alias one and use that
                    structName = self.registry.getNonAliasStructName(structName)
                    # Copy defined format property structure data, the count of a dynamic array
                    # (e.g. drmFormatModifierCount) is not a member, it is defined by the array elements
                    if not structName in definedFormatProperties:
                        definedFormatProperties[structName] = []
                    structMembers = self.registry.getDocStruct(structName).members
                    definedFormatProperties[structName].extend(sorted(m for m in properties.keys() if m in structMembers))

                # Add format information
                if not format in definedFormats:
                    definedFormats[format] = OrderedDict()
                # Merge the members defined by each profile
                for structName, members in definedFormatProperties.items():
                    definedMembers = definedFormats[format].setdefault(structName, [])
                    definedMembers.extend(m for m in members if m not in definedMembers)


        # Construct table data
        tableData = OrderedDict()
        for format in sorted(definedFormats.keys()):
            section = tableData[format] = OrderedDict()
            for structName, members in definedFormats[format].items():
                # Rows are keyed by structure and member, different structures may have members with the same name
                # (e.g. VkDrmFormatModifierPropertiesListEXT and VkDrmFormatModifierPropertiesList2EXT)
                section.update({ (structName, row): functools.partial(self.gen_formatRow, format, structName, row) for row in members })

        # TODO: Currently we don't include format properties that are required by the minimum
        # required API version of a profile, or those required by extensions required by the
        # profile, as that would necessitate the inclusion of information currently only
        # available textually in the "Required Format Support" section of the Vulkan Specification
        disclaimer = (
            '> **NOTE**: The table below only contains formats and properties explicitly defined '
            'by the corresponding profile. Further formats and properties may be supported by the '
            'profiles in accordance to the requirements defined in the "Required Format Support" '
            'section of the appropriate version of the Vulkan API Specification.'
        )

        # Generate table legend
        legend = (
            '* "valueWithRegularFont" indicates that the format property is defined in the '
            'profile (hover over the value to view the structure and corresponding extension or '
            'core API version where the format property is defined in the profile)\n'
            '* "_valueWithItalicFont_" indicates that the format property is not defined in the '
            'profile but an equivalent format property is (hover over the symbol to view the '
            'structure and corresponding extension or core API version where the format property '
            'is defined in the profile)\n'
            '* "-" indicates that neither the format property nor an equivalent format property '
            'is defined in the profile\n'
            '* Empty cells next to the properties of a particular format definition section '
            'indicate that the profile does not have a corresponding format definition\n'
        )

        # Generate table
        table = self.gen_sectionedTable(tableData)
        return '\n## Vulkan Profile Formats\n\n{0}\n\n{1}\n{2}\n'.format(disclaimer, legend, table)


    def gen_videoProfile(self, videoProfilesPerProfileKey, videoProfileName, profile = None):
        if videoProfileName in videoProfilesPerProfileKey[profile.key]:
            # Video profile is supported by the given profile
            return ':heavy_check_mark:'
        else:
            # Video profile is NOT supported by the given profile
            return ':x:'


    def gen_videoProfiles(self):
        if self.registry.videoCodecs is None:
            return ""

        definedVideoProfiles = OrderedDict()
        videoProfilesPerProfileKey = OrderedDict()

        # Merge all defined video profile names
        for profile in self.profiles:
            videoProfilesPerProfileKey[profile.key] = OrderedDict()
            for videoProfileName, videoProfile in self.getDocCapabilities(profile).videoProfilesByName.items():
                videoProfilesPerProfileKey[profile.key][videoProfileName] = videoProfile
                if not videoProfileName in definedVideoProfiles:
                    definedVideoProfiles[videoProfileName] = videoProfile

        # Construct table data
        tableData = OrderedDict()
        for videoProfileName in sorted(definedVideoProfiles.keys()):
            tableData[videoProfileName] = functools.partial(self.gen_videoProfile, videoProfilesPerProfileKey)

        # Generate table legend
        legend = (
            'The table below lists the video profiles (or class of video profiles) that any profile defines requirements for.\n'
            'Video profile names take the following form:\n'
            '> `"(General|[<codec>] <category>) [([<subsampling>] [<bits>])] [<codecSpecificProfileInfo>]"`\n'
            '* "General" refers to general requirements for all video profiles.\n'
            '* `<category>` is the category of the video profile requirements (e.g. "Decode" or "Encode").\n'
            '* `<codec>` identifies the specific codec (e.g. H.264, H.265, AV1) the requirements apply to. If not indicated, '
            'the requirements apply to all supported video profiles falling in the specified category.\n'
            '* `<subsampling>` indicates the chroma subsampling (e.g. 4:2:0, 4:2:2, 4:4:4, or monochrome). If not indicated, '
            'the requirements apply to all supported chroma subsampling modes.\n'
            '* `<bits>` indicates the luma and chroma bit depth (e.g. 8-bit, 10-bit, or e.g. 10:8-bit for mixed bit depths). '
            'If not indicated, the requirements apply to all supported bit depths. Partial, luma-only or chroma-only "wildcard" '
            'requirements are indicated with an asterisk in place of the corresponding bit depth (e.g. 10:\\*-bit or \\*:8-bit).\n'
            '* `<codecSpecificProfileInfo>` indicates additional codec-specific video profile information (e.g. "Main", "High"). '
            'If not indicated, the requirements apply to all video profiles of the codec.\n'
            '* :heavy_check_mark: indicates that the profile defines requirements for the video profile (or class of video profiles)\n'
            '* :x: indicates that that the profile does not define requirements for the video profile (or class of video profiles)\n'
        )

        # Generate per video profile sections
        videoProfileSections = ""
        for videoProfileName in sorted(definedVideoProfiles.keys()):
            videoProfile = definedVideoProfiles[videoProfileName]
            videoCodec = self.registry.getVideoCodecFromVideoProfile(videoProfile)

            videoProfileSections += "\n### {0}\n\n".format(videoProfileName)
            videoProfileSections += self.gen_videoProfileDefinition(videoProfile, videoCodec)
            videoProfileSections += self.gen_videoCapabilities(videoProfileName, videoCodec)
            videoProfileSections += self.gen_videoFormats(videoProfileName, videoCodec)

        # Generate table
        table = self.gen_table(tableData)
        return '\n## Vulkan Profile Video Profiles\n\n{0}\n{1}\n{2}'.format(legend, table, videoProfileSections)


    def gen_videoProfileDefinition(self, videoProfile, videoCodec: VideoCodec):
        base = self.registry.getBaseVideoProfileInfoFromVideoProfile(videoProfile)
        baseStructName = self.registry.getNonAliasStructName('VkVideoProfileInfoKHR')

        # Construct table data
        table = '| Profile member | Value |\n'
        table += '|----------------|-------|\n'
        table += '| **{0}** |\n'.format(baseStructName)
        for member in self.registry.getDocStruct(baseStructName).members:
            # Asterisk marks unspecified members (wildcard members)
            value = base[member] if base is not None and member in base else "*"
            table += '| {0} | {1} |\n'.format(self.gen_manPageLink(baseStructName, member), value)

        if 'profile' in videoProfile:
            videoProfileDesc = videoProfile['profile']
        else:
            videoProfileDesc = dict()

        for profileStruct in videoCodec.profiles.values():
            extStructName = self.registry.getNonAliasStructName(profileStruct.name)
            table += '| **{0}** |\n'.format(extStructName)

            videoProfileStruct = None
            if extStructName in videoProfileDesc:
                videoProfileStruct = videoProfileDesc[extStructName]
            else:
                # Check also for possible aliases
                for alias in self.registry.getDocStruct(extStructName).aliases:
                    if alias in videoProfileDesc:
                        videoProfileStruct = videoProfileDesc[alias]

            for profileStructMember in profileStruct.members.values():
                # Asterisk marks unspecified members (wildcard members)
                value = videoProfileStruct[profileStructMember.name] if videoProfileStruct is not None and profileStructMember.name in videoProfileStruct else "*"
                table += '| {0} | {1} |\n'.format(self.gen_manPageLink(extStructName, profileStructMember.name), value)

        return '\n#### Video Profile Definition\n\n{0}'.format(table)


    def formatVideoProfileProperty(self, value, struct):
        structDef = self.registry.getDocStruct(struct)
        # TODO: As with the legacy generator, 'where' is undefined for a structure defined by neither an API version nor an extension
        if structDef.definedByVersion != None:
            where = 'Vulkan {0}'.format(formatVersion(structDef.definedByVersion))
        elif len(structDef.definedByExtensions) > 0:
            where = '/'.join(structDef.definedByExtensions)
        return '<span title="defined in {0} ({1})">{2}</span>'.format(struct, where, self.formatValue(value))


    def gen_videoCapability(self, videoProfileName, struct, member, profile = None):
        # If no profile was specified then this is the first column so return the member name
        # decorated with the corresponding limittype specific info and a link to the
        # encompassing capability structure's manual page
        if profile is None:
            return self.gen_manPageLink(struct, self.formatLimitName(struct, member))

        doc_capabilities = self.getDocCapabilities(profile)

        # Check if the capability is defined in the profile
        if videoProfileName in doc_capabilities.videoProfilesByName:
            videoProfile = doc_capabilities.videoProfilesByName[videoProfileName]
            capabilityStruct = None
            capabilityStructName = None
            if struct in videoProfile['capabilities']:
                capabilityStruct = videoProfile['capabilities'][struct]
                capabilityStructName = struct
            else:
                # Check also for possible aliases
                for alias in self.registry.getDocStruct(struct).aliases:
                    if alias in videoProfile['capabilities']:
                        capabilityStruct = videoProfile['capabilities'][alias]
                        capabilityStructName = alias
                        break

            if capabilityStruct is not None and member in capabilityStruct:
                return self.formatVideoProfileProperty(capabilityStruct[member], capabilityStructName)

        return '-'


    def gen_videoCapabilities(self, videoProfileName, videoCodec: VideoCodec):
        # Merge all capability references across the profiles to collect the relevant capabilities to look at
        definedCapabilities = dict()
        for profile in self.profiles:
            for videoProfile in self.getDocCapabilities(profile).videoProfiles:
                if videoProfileName != self.registry.getVideoProfileNameFromVideoProfile(videoProfile):
                    continue
                if not 'capabilities' in videoProfile:
                    continue
                for capabilityStructName, capabilities in videoProfile['capabilities'].items():
                    # If this is an alias structure then find the non-alias one and use that
                    capabilityStructName = self.registry.getNonAliasStructName(capabilityStructName)
                    # Copy defined capability structure data
                    if not capabilityStructName in definedCapabilities:
                        definedCapabilities[capabilityStructName] = []
                    definedCapabilities[capabilityStructName].extend(capabilities.keys())

        # Construct table
        tableData = OrderedDict()
        for capabilityStructName in ['VkVideoCapabilitiesKHR'] + list(videoCodec.capabilities.keys()):
            capabilityStruct = self.registry.getDocStruct(self.registry.getNonAliasStructName(capabilityStructName))
            if not capabilityStruct.name in definedCapabilities:
                continue
            tableData[capabilityStruct.name] = OrderedDict()
            for capability in capabilityStruct.members:
                if not capability in definedCapabilities[capabilityStruct.name]:
                    continue
                # Capability is defined, add it to the table
                tableData[capabilityStruct.name][capability] = functools.partial(self.gen_videoCapability, videoProfileName)

        # Generate table
        table = self.gen_sectionedTable(tableData)
        return '\n#### Video Capabilities\n\n{0}'.format(table)


    def gen_videoFormat(self, definedVideoFormatsPerProfile, videoProfileName, videoFormatCategory, index, struct, section, member, profile = None):
        # If no profile was specified then this is the first column so return the member name
        # decorated with the corresponding limittype specific info and a link to the
        # encompassing structure's manual page
        if profile is None:
            return self.gen_manPageLink(struct, self.formatLimitName(struct, member))

        # If this profile doesn't even define this video format category or this video format index then early out
        if not profile.key in definedVideoFormatsPerProfile:
            return ''
        if not videoFormatCategory.name in definedVideoFormatsPerProfile[profile.key]:
            return ''
        if len(definedVideoFormatsPerProfile[profile.key][videoFormatCategory.name]) <= index:
            return ''

        videoFormatProps = definedVideoFormatsPerProfile[profile.key][videoFormatCategory.name][index]

        # Find the video format properties structure data in the profile, if it exists
        videoFormatPropsStruct = None
        videoFormatPropsStructName = None
        if struct in videoFormatProps:
            videoFormatPropsStruct = videoFormatProps[struct]
            videoFormatPropsStructName = struct
        else:
            # Check also for alises
            for alias in self.registry.getDocStruct(struct).aliases:
                if alias in videoFormatProps:
                    videoFormatPropsStruct = videoFormatProps[alias]
                    videoFormatPropsStructName = alias

        if videoFormatPropsStruct is not None and member in videoFormatPropsStruct:
            return self.formatVideoProfileProperty(videoFormatPropsStruct[member], videoFormatPropsStructName)

        return '-'


    def gen_videoFormats(self, videoProfileName, videoCodec: VideoCodec):
        # Merge all video format categories across the profiles to collect the relevant video format categories to look at
        # Also collect the video format properties that have the necessary prerequisites at least in one Vulkan profile
        # Finally, Collect for each video format category the maximum number of video formats per Vulkan profile
        definedVideoFormats = dict()
        definedVideoFormatProps = set()
        definedVideoFormatsPerProfile = dict()
        for profile in self.profiles:
            doc_capabilities = self.getDocCapabilities(profile)
            definedVideoFormatsCount = dict()
            definedVideoFormatsPerProfile[profile.key] = dict()
            for videoProfile in doc_capabilities.videoProfiles:
                if videoProfileName != self.registry.getVideoProfileNameFromVideoProfile(videoProfile):
                    continue
                if not 'formats' in videoProfile:
                    continue
                for videoFormat in videoProfile['formats']:
                    # Collect the video format properties structures that have their prerequisites met
                    for videoFormatProps in videoFormat:
                        nonAliasStructName = self.registry.getNonAliasStructName(videoFormatProps)
                        if nonAliasStructName in definedVideoFormatProps:
                            continue
                        # Check also for aliases
                        # TODO: As with the legacy generator, the structure is compared with '<' to the profile API version, '<=' looks intended
                        for structName in [ videoFormatProps ] + self.registry.getDocStruct(videoFormatProps).aliases:
                            structDef = self.registry.getDocStruct(structName)
                            if structDef.definedByVersion is not None and structDef.definedByVersion < profile.apiVersionNumber:
                                definedVideoFormatProps.add(nonAliasStructName)
                            for definedByExtension in structDef.definedByExtensions:
                                if definedByExtension in doc_capabilities.extensions:
                                    definedVideoFormatProps.add(nonAliasStructName)

                    for videoFormatCategory in self.registry.getVideoFormatCategoriesForFormat(videoCodec, videoFormat, videoProfile['capabilities'] if 'capabilities' in videoProfile else {}):
                        # Include the video format in the defined list for the video format category
                        if not videoFormatCategory.name in definedVideoFormatsCount:
                            definedVideoFormatsCount[videoFormatCategory.name] = 0
                            definedVideoFormatsPerProfile[profile.key][videoFormatCategory.name] = []
                        definedVideoFormatsCount[videoFormatCategory.name] += 1
                        definedVideoFormatsPerProfile[profile.key][videoFormatCategory.name].append(videoFormat)

            # Check if this profile increased the maximum format counts per video format category
            for videoFormatCategoryName, count in definedVideoFormatsCount.items():
                if not videoFormatCategoryName in definedVideoFormats or count > definedVideoFormats[videoFormatCategoryName]:
                    definedVideoFormats[videoFormatCategoryName] = count

        # Construct table
        tableData = OrderedDict()
        for videoFormatCategory in videoCodec.formats.values():
            if videoFormatCategory.name in definedVideoFormats:
                # There are formats falling in this video format category
                for index in range(definedVideoFormats[videoFormatCategory.name]):
                    section = tableData["{0} Format #{1}".format(videoFormatCategory.name, index + 1)] = OrderedDict()
                    # Include all video format properties that apply to this video format category
                    for videoFormatProps in [ 'VkVideoFormatPropertiesKHR' ] + list(videoFormatCategory.properties.keys()):
                        propsStruct = self.registry.getDocStruct(self.registry.getNonAliasStructName(videoFormatProps))
                        # Skip video format properties not defined by profile prequisites
                        if not propsStruct.name in definedVideoFormatProps:
                            continue

                        # Setup row handler function and add rows for each property
                        rowHandler = functools.partial(self.gen_videoFormat, definedVideoFormatsPerProfile, videoProfileName, videoFormatCategory, index, propsStruct.name)
                        section.update({ row: rowHandler for row in propsStruct.members.keys()})

        # Generate table
        table = self.gen_sectionedTable(tableData)
        return '\n#### Video Formats\n\n{0}'.format(table)



def generate_profiles_doc(vk: VulkanObject, json_files: list[dict], input_profiles: list[str] | None, output: str):
    """Generates the Vulkan Profiles markdown documentation of the profiles listed by input_profiles, or of all the profiles if None."""
    registry = DocRegistry(vk)
    # All the profiles files are kept in the database so that the required profiles of the documented profiles are found
    database = VulkanProfilesDatabase(json_files)

    profiles = dict()
    doc_capabilities = dict()
    for json_file in json_files:
        for json_profile_key, json_profile_value in json_file['profiles'].items():
            if input_profiles and json_profile_key not in input_profiles:
                continue
            if json_profile_key not in profiles:
                profiles[json_profile_key] = VulkanProfile(registry, database, json_profile_key, json_profile_value, json_file['capabilities'])
                # The documentation shows all the capabilities of the profile, including those of its required profiles
                collected_json_capabilities = database.collectProfileCapabilities(database.collectRequiredProfiles(json_profile_key))
                doc_capabilities[json_profile_key] = VulkanProfileCapabilities(registry, '"DOC"', collected_json_capabilities, False, True)

    generator = VulkanProfilesDocGenerator(registry, profiles, doc_capabilities)
    generator.generate(output)
