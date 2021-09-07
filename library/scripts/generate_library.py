import json
import os
import os.path
import sys
import re
import xml.etree.ElementTree as etree

license_header = '''
/*
 * Copyright (c) 2021-2022 LunarG, Inc.",
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
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
 * Authors:
 * - Christophe Riccio <christophe@lunarg.com>",
 *
 * DO NOT EDIT: This file is generated from the Khronos Vulkan XML API Registry.
 */
'''

helper_code = '''
#include <iostream>
#include <map>
#include <set>
#include <string>
#include <vector>

typedef struct FakeVulkanStruct {
    VkStructureType sType;
    const void *pNext;
} FakeVulkanStruct;

std::vector<const char *> mergeExtensionList(int firstLength,
                                             const char *const *firstList,
                                             int secondLength,
                                             const char *const *secondList) {
    std::vector<const char *> mergedList;
    std::set<std::string> stringSet;

    for (int i = 0; i < firstLength; ++i) {
        if (stringSet.find(std::string(firstList[i])) == stringSet.end()) {
            stringSet.insert(std::string(firstList[i]));
            mergedList.push_back(firstList[i]);
        }
    }

    for (int i = 0; i < secondLength; ++i) {
        if (stringSet.find(std::string(secondList[i])) == stringSet.end()) {
            stringSet.insert(std::string(secondList[i]));
            mergedList.push_back(secondList[i]);
        }
    }

    return mergedList;
}

std::map<std::string, uint32_t> createExtensionPropMap(const std::vector<VkExtensionProperties> &supportedExts) {
    std::map<std::string, uint32_t> propertiesMap;
    for (const auto &ext : supportedExts) {
        propertiesMap[std::string(ext.extensionName)] = ext.specVersion;
    }
    return propertiesMap;
}

std::vector<const char *> getSupportedExtensionNames(const std::vector<VkExtensionProperties> &requestedExts,
                                                     const std::vector<VkExtensionProperties> &supportedExts) {
    std::vector<const char *> supportedExtNames;
    std::map<std::string, uint32_t> propertiesMap = createExtensionPropMap(supportedExts);
    
    for (const auto &reqExt : requestedExts) {
        auto ext = std::string(reqExt.extensionName);
        if (propertiesMap.count(ext) > 0 && propertiesMap.at(ext) >= reqExt.specVersion) {
            supportedExtNames.push_back(reqExt.extensionName);
        }
    }
    return supportedExtNames;
}

bool checkExtensionProperties(const std::vector<VkExtensionProperties> &requestedExts,
                              const std::vector<VkExtensionProperties> &supportedExts) {
    std::map<std::string, uint32_t> propertiesMap = createExtensionPropMap(supportedExts);
    
    bool extsSupported = true;
    for (const auto &reqExt : requestedExts) {
        auto ext = std::string(reqExt.extensionName);
        if (propertiesMap.count(ext) == 0) {
            extsSupported = false;
        } else if (propertiesMap.at(ext) < reqExt.specVersion) {
            extsSupported = false;
        }
    }
    return extsSupported;
}

template<class T>
uint32_t maskBitFlags(std::vector<T> flags) {
    uint32_t mask = 0;
    for (const auto &flag : flags) {
        mask |= flag;
    }
    return mask;
}
'''

def _JOIN(*argv):
    return '\n'.join(argv)


class ProfileLibraryBuilder():
    def __init__(self, profile_json_dir, header_path):
        self._profileCreateBuilders = {}
        self._profileDicts = {}
        self._scanAndRegisterProfiles(profile_json_dir)
        self._profileSupportCheckBuilder = self.ProfileSupportCheckBuilder(
            self._profileDicts, self)
        self._headerPath = os.path.join(
            os.path.abspath(header_path), "vulkan_profiles.h")

    def _scanAndRegisterProfiles(self, profile_json_dir):
        json_dir_abspath = os.path.abspath(profile_json_dir)
        json_filenames = os.listdir(json_dir_abspath)
        for json_filename in json_filenames:
            json_abs_path = os.path.join(json_dir_abspath, json_filename)
            if os.path.isfile(json_abs_path) and os.path.splitext(json_filename)[-1] == '.json':
                with open(json_abs_path, 'r') as json_fp:
                    profile_dict = json.load(json_fp)
                    self._registerProfile(profile_dict)

    def _genCIncludes(self):
        return "#include <vulkan/vulkan_core.h>\n"

    def _genCPPIncludes(self):
        return _JOIN("#include <vector>",
                     helper_code)

    def _genEnumDecl(self):
        symbol_decl = ",\n\t".join(sorted(list(self._profileDicts)))
        return f"typedef enum VpProfile {{ \n\t{symbol_decl} \n}} VpProfile;\n"

    def _genProfilesSwitch(self):
        cases = []
        for enum, builder in self._profileCreateBuilders.items():
            case = _JOIN(f"\t\tcase {enum}: {{", 
                         f"{builder.genProfileImpl()}",
                         "\t\t} break;\n")
            cases.append(case)
        return _JOIN(f"\tswitch(profile) {{",
                     *cases,
                     "\t}")

    def _getProfileCreateSign(self):
        return _JOIN(f'VkResult vpCreateDevice(VkPhysicalDevice physicalDevice, const VpProfile profile, const VkDeviceCreateInfo *pCreateInfo, const VkAllocationCallbacks *pAllocator, VkDevice *pDevice)')

    def _genProfileCreateDecl(self):
        return f"{self._getProfileCreateSign()};\n"

    def _genEnumerateInstanceExtensionProperties(self):
        return '''
    uint32_t extensionPropertyCount;
    result = vkEnumerateInstanceExtensionProperties(NULL, &extensionPropertyCount, NULL);
    std::vector<VkExtensionProperties> extensionProperties(extensionPropertyCount);
    result = vkEnumerateInstanceExtensionProperties(NULL, &extensionPropertyCount, extensionProperties.data());
        '''

    def _genGetPhysicalDeviceFeatures(self):
        return '''
    VkPhysicalDeviceFeatures deviceFeatures = {{}};
    vkGetPhysicalDeviceFeatures(physicalDevice, &deviceFeatures);
        '''

    def _genProfileCreateImpl(self):
        return _JOIN(f"inline {self._getProfileCreateSign()} {{",
                     f"\tVkResult result;",
                     f"{self._genEnumerateInstanceExtensionProperties()}",
                     f"{self._genGetPhysicalDeviceFeatures()}",
                     f"{self._genProfilesSwitch()}",
                     f"\treturn result;",
                     "}")

    def _registerProfile(self, profile_dict):
        enum = profile_dict.get('enum')
        if enum != None and not self._profileDicts.get('enum'):
            print("Registering: ", enum)
            self._profileDicts[enum] = profile_dict
            self._profileCreateBuilders[enum] = self.ProfileCreateBuilder(
                profile_dict, self)
        else:
            raise KeyError(f'Missing or duplicate profile enum for {enum}')

    def genProfilesHeader(self):
        """
        docstring
        """
        header_str = _JOIN(
            license_header,
            self._genCIncludes(),
            self._genEnumDecl(),
            self._genProfileCreateDecl(),
            self._profileSupportCheckBuilder.genSupportCheckDecl(),
            self._genCPPIncludes(),
            self._genProfileCreateImpl(),
            "\n",
            self._profileSupportCheckBuilder.genSupportCheckImpl(),
            "\n")
        with open(self._headerPath, 'w') as header_fp:
            header_fp.write(header_str)

    class ProfileSupportCheckBuilder():
        def __init__(self, profile_dicts, parent_inst):
            self._profile_dicts = profile_dicts
            self._parent_inst = parent_inst

        def _genMacros(self):
            return '''
    #define CHECK_SUPPORT_EXPR(expr, msg) supported = supported && expr;\\
        std::cout << msg << " " << (expr ? " PASS" : " FAIL") << std::endl;
    #define ALT_CHECK_SUPPORT_EXPR(expr, msg) altSupported = altSupported && expr;\\
        std::cout << msg << " " << (expr ? " PASS" : " FAIL") << std::endl;'''

        def _genProfileSwitch(self):
            cases = []
            for enum, profile_dict in self._profile_dicts.items():
                case = f'''
            case {enum}: {{
                {self._genProfileCheck(profile_dict)}\n
            }} break;'''
                cases.append(case)
            return _JOIN("switch(profile) {",
                         *cases,
                         "\t\t}\n")

        def _genProfileCheckLoop(self):
            return f'''
    for (int i = 0; i < profileCount; ++i) {{
        VkBool32 supported = VK_TRUE;
        VpProfile profile = pProfiles[i];
        {self._genProfileSwitch()}
        pSupportedProfiles[i] = supported;
    }}'''

        def _genProfileCheck(self, profile_dict):
            return _JOIN(f"{self._genCheckExtensionProperties(profile_dict)}",
                         f"{self._genCheckPhysicalDeviceProperties(profile_dict)}",
                         f"{self._genCheckPhysicalDeviceFeatures(profile_dict)}",
                         f"{self._getCheckImageFormatSupport(profile_dict)}"
                         f"{self._genCheckAlternatives(profile_dict)}")

        def _genCheckExtensionProperties(self, profile_dict, isAlt=False):
            check_str = ""
            extensions = profile_dict.get('ArrayOfVkExtensionProperties')
            if extensions and len(extensions) > 0:
                literals = "{" + ',\n'.join(
                    map(lambda ext: 'VkExtensionProperties{{"{}", {}}}'.format(ext['extensionName'], ext['specVersion']), extensions)) + "}"
                expr_str = f"checkExtensionProperties(profileExts, extensionProperties)"
                check_str += f"std::vector<VkExtensionProperties> profileExts = {literals};\n"\
                    f"{self._genCheckMacroCall(expr_str, isAlt)}\n"
            return check_str

        def _genCheckPhysicalDeviceProperties(self, profile_dict, isAlt=False):
            # TODO support members other than just limits
            properties = profile_dict.get('VkPhysicalDeviceProperties')
            check_str = ""
            if properties:
                limits = properties.get('limits')
                if limits:
                    for member, limit in limits.items():
                        if limit['type'] == 'min':
                            op = '>='
                        else:
                            op = '<='
                        expr_str = f"deviceProperties.limits.{member} {op} {limit['value']}"
                        check_str += f"{self._genCheckMacroCall(expr_str, isAlt)};\n"
            return check_str

        def _genCheckPhysicalDeviceFeatures(self, profile_dict, isAlt=False):
            check_str = ""
            features = profile_dict.get('VkPhysicalDeviceFeatures')
            if features:
                for member, value in features.items():
                    expr_str = f"deviceFeatures.{member} == {value}"
                    check_str += f"{self._genCheckMacroCall(expr_str, isAlt)};\n"
            return check_str

        def _getCheckImageFormatSupport(self, profile_dict, isAlt=False):
            IMG_FMT_PROPS_NAME = "imageFormatProperties"
            USAGE_FLAGS_NAME = "usage"
            CREATE_FLAGS_NAME = "flags"
            format_reqs = profile_dict.get('ArrayOfImageFormatRequirements')

            if format_reqs and len(format_reqs) > 0:
                format_checks = [
                    f"VkImageFormatProperties {IMG_FMT_PROPS_NAME} = {{}};"]
                for format_req in format_reqs:
                    format_name = format_req.get("format")
                    type_name = format_req.get("type")
                    tiling = format_req.get("tiling")
                    usage_flag_names = format_req.get("usage")
                    create_flag_names = format_req.get("flags")
                    if usage_flag_names:
                        usage_flag_bits = "{" + ',\n'.join(
                            map(lambda flag_name: f'{flag_name}', usage_flag_names)) + "}"
                        format_checks.append(
                            f"std::vector<VkImageUsageFlagBits> usageFlagBits = {usage_flag_bits};")
                        format_checks.append(
                            f"VkImageUsageFlags {USAGE_FLAGS_NAME} = maskBitFlags(usageFlagBits);")
                    else:
                        format_checks.append(
                            f"VkImageUsageFlags {USAGE_FLAGS_NAME} = 0;")

                    if create_flag_names:
                        create_flag_bits = "{" + ',\n'.join(
                            map(lambda flag_name: f'{flag_name}', create_flag_names)) + "}"
                        format_checks.append(
                            f"std::vector<VkImageCreateFlagBits> createFlagBits = {create_flag_bits};")
                        format_checks.append(
                            f"VkImageCreateFlags {CREATE_FLAGS_NAME} = maskBitFlags(createFlagBits);")
                    else:
                        format_checks.append(
                            f"VkImageCreateFlags {CREATE_FLAGS_NAME} = 0;")
                    format_checks.append(_JOIN(
                        f"result = vkGetPhysicalDeviceImageFormatProperties(",
                        f"physicalDevice,",
                        f"{format_name},",
                        f"{type_name},",
                        f"{tiling},",
                        f"{USAGE_FLAGS_NAME},",
                        f"{CREATE_FLAGS_NAME},",
                        f"&{IMG_FMT_PROPS_NAME}",
                        f");",
                    ))
                    check_expr = f"result == VK_SUCCESS"
                    format_checks.append(
                        f"{self._genCheckMacroCall(check_expr, isAlt)};")
                    return _JOIN(*format_checks)
            else:
                return ""

        def _genCheckAlternatives(self, profile_dict):
            ALT_GROUP_SUPPORTED_NAME = "altGroupSupported"
            alt_groups = profile_dict.get('ArrayOfArrayOfAlternatives')
            if alt_groups and len(alt_groups) > 0:
                alt_group_checks = []
                for alt_group in alt_groups:
                    alt_group_checks.append("{")
                    if alt_group and len(alt_group) > 0:
                        alts_checks = [
                            f"bool {ALT_GROUP_SUPPORTED_NAME} = false;"]
                        for alt in alt_group:
                            alts_checks.append(_JOIN(
                                "{",
                                f"bool altSupported = true;",
                                f"{self._genCheckExtensionProperties(alt, True)}",
                                f"{self._genCheckPhysicalDeviceProperties(alt, True)}",
                                f"{self._genCheckPhysicalDeviceFeatures(alt, True)}",
                                f"{self._getCheckImageFormatSupport(alt, True)}"
                                f"{ALT_GROUP_SUPPORTED_NAME} = {ALT_GROUP_SUPPORTED_NAME} || altSupported;",
                                "}"))

                        alt_group_checks.append(_JOIN(*alts_checks))
                    alt_group_checks.append(_JOIN(
                        f"{self._genCheckMacroCall(ALT_GROUP_SUPPORTED_NAME)};", "}"))
                return _JOIN(*alt_group_checks)
            else:
                return ""

        def _genCheckMacroCall(self, expr_str, isAlt=False):
            if isAlt:
                return f"ALT_CHECK_SUPPORT_EXPR({expr_str}, \"{expr_str}\")"
            else:
                return f"CHECK_SUPPORT_EXPR({expr_str}, \"{expr_str}\")"

        def _genGetPhysicalDeviceProperties(self):
            return '''
    VkPhysicalDeviceProperties deviceProperties = {};
    vkGetPhysicalDeviceProperties(physicalDevice, &deviceProperties);'''

        def _getSupportCheckSign(self):
            return "void vpGetPhysicalDeviceProfiles(VkPhysicalDevice physicalDevice, uint32_t profileCount, const VpProfile *pProfiles, VkBool32 *pSupportedProfiles)"

        def genSupportCheckDecl(self):
            return f"{self._getSupportCheckSign()};\n"

        def genSupportCheckImpl(self):
            return _JOIN(f"{self._getSupportCheckSign()} {{",
                         f"{self._genMacros()}",
                         f"\tVkResult result;",
                         f"{self._parent_inst._genEnumerateInstanceExtensionProperties()}",
                         f"{self._parent_inst._genGetPhysicalDeviceFeatures()}",
                         f"{self._genGetPhysicalDeviceProperties()}",
                         f"{self._genProfileCheckLoop()}}}")

    class ProfileCreateBuilder():
        NEXT_STRUCT_NAME_FMT = "pNext_{}"

        def __init__(self, profile_dict, parent_inst):
            self._profile_dict = profile_dict
            self._parent_inst = parent_inst

        def _genPhysicalDeviceFeatures(self):
            struct_blocks = ["\t\t\tVkPhysicalDeviceFeatures enabledFeatures = {};",
                             "\t\t\tif (pCreateInfo->pEnabledFeatures != NULL)",
                             "\t\t\t\tenabledFeatures = *pCreateInfo->pEnabledFeatures;"]
            features_list = self._profile_dict.get("VkPhysicalDeviceFeatures")
            if features_list:
                for feature, value in features_list.items():
                    struct_blocks.append(
                        f"enabledFeatures.{feature} = {value};")
            alt_groups = self._profile_dict.get('ArrayOfArrayOfAlternatives')
            if alt_groups:
                for alt_group in alt_groups:
                    for alt in alt_group:
                        alt_features = alt.get('VkPhysicalDeviceFeatures')
                        if alt_features:
                            for feature_name, value in alt_features.items():
                                struct_blocks += [f'if (deviceFeatures.{feature_name} == {value}) {{',
                                                  f'enabledFeatures.{feature_name} = {value};',
                                                  '}']
            return _JOIN(*struct_blocks)

        def _genEnabledExtensions(self):
            extensions = []
            profile_extensions = self._profile_dict.get(
                'ArrayOfVkExtensionProperties')
            if profile_extensions:
                extensions += profile_extensions
            alt_groups = self._profile_dict.get('ArrayOfArrayOfAlternatives')
            if alt_groups:
                for alt_group in alt_groups:
                    for alt in alt_group:
                        alt_exts = alt.get('ArrayOfVkExtensionProperties')
                        if alt_exts:
                            for alt_ext in alt_exts:
                                extensions.append(alt_ext)
            if extensions:
                literals = "{" + ',\n'.join(
                    map(lambda ext: 'VkExtensionProperties{{"{}", {}}}'.format(ext['extensionName'], ext['specVersion']), extensions)) + "}"
                return f'''
            std::vector<VkExtensionProperties> profileExtProps = {literals};
            std::vector<const char*> profileExtNames = getSupportedExtensionNames(profileExtProps, extensionProperties);
            std::vector<const char*> extensions = mergeExtensionList(profileExtNames.size(), profileExtNames.data(),
            pCreateInfo->enabledExtensionCount, pCreateInfo->ppEnabledExtensionNames);
            const char* const* enabledExtensions = extensions.data();
            uint32_t enabledExtensionCount = extensions.size();'''
            else:
                return '''
            const char* const* enabledExtensions = pCreateInfo->ppEnabledExtensionNames;
            uint32_t enabledExtensionCount = pCreateInfo->enabledExtensionCount;'''

        def _getSType(self, vk_type):
            tokens = list(map(lambda s: s.upper(), re.split(
                r'(?<=[a-z0-9])(?=[A-Z0-9])', vk_type)))
            tokens.insert(1, 'STRUCTURE_TYPE')
            return '_'.join(tokens)

        def _genNextChain(self):
            next_struct_descs = self._profile_dict.get('extended')
            if next_struct_descs and len(next_struct_descs) > 0:
                struct_decls = []
                for i, next_struct_desc in enumerate(next_struct_descs):
                    typeName = next_struct_desc['type']
                    members = next_struct_desc['members']
                    name = self.NEXT_STRUCT_NAME_FMT.format(i)
                    struct_decl = f"{typeName} {name} = {{}};\n"
                    struct_decl += f"{name}.sType = {self._getSType(typeName)};\n"
                    if i == len(next_struct_descs) - 1:
                        struct_decl += f"{name}.pNext = const_cast<void*>(pCreateInfo->pNext);\n"
                    else:
                        struct_decl += f"{name}.pNext = &{self.NEXT_STRUCT_NAME_FMT.format(i+1)};\n"
                    for field, value in members.items():
                        struct_decl += f"{name}.{field} = {value};\n"
                    struct_decls.insert(0, struct_decl)
                struct_decls.append(
                    f"void* pNext = &{self.NEXT_STRUCT_NAME_FMT.format(0)};")
                return _JOIN(*struct_decls)
            else:
                return "\t\t\tvoid* pNext = NULL;"

        def _genDeviceCreateInfo(self):
            return '''
            VkDeviceCreateInfo deviceCreateInfo = {};
            deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
            deviceCreateInfo.pNext = pNext;
            deviceCreateInfo.queueCreateInfoCount = pCreateInfo->queueCreateInfoCount;
            deviceCreateInfo.pQueueCreateInfos = pCreateInfo->pQueueCreateInfos;
            deviceCreateInfo.enabledExtensionCount = enabledExtensionCount;
            deviceCreateInfo.ppEnabledExtensionNames = enabledExtensions;
            deviceCreateInfo.pEnabledFeatures = &enabledFeatures;'''

        def _genCreateDevice(self):
            return _JOIN(self._genEnabledExtensions(),
                         self._genPhysicalDeviceFeatures(),
                         self._genNextChain(),
                         self._genDeviceCreateInfo(),
                         "\t\t\tresult = vkCreateDevice(physicalDevice, &deviceCreateInfo, pAllocator, pDevice);")

        def genProfileImpl(self):
            """
            docstring
            """
            return f"{self._genCreateDevice()}"

    class ProfileTestBuilder():
        pass


if __name__ == "__main__":
    # python ./scripts/generate_library.py ./registry/vk.xml ./scripts/profiles ./include/vulkan
    print(sys.argv[1])
    xml_tree = etree.parse(sys.argv[1])
    profile_dir = sys.argv[2]
    header_path = sys.argv[3]
    builder = ProfileLibraryBuilder(profile_dir, header_path)
    builder.genProfilesHeader()

