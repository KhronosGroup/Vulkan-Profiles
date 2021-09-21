import json
import os
import os.path
import sys
import re
import xml.etree.ElementTree as etree

header_template = '''
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
 * - Christophe Riccio <christophe@lunarg.com>
 * - Adam Bodnar
 *
 * DO NOT EDIT: This file is generated from the Khronos Vulkan XML API Registry.
 */

 #include <vulkan/vulkan_core.h>

 {0}

VkResult vpCreateDevice(
    VkPhysicalDevice physicalDevice, 
    const VpProfile profile, 
    const VkDeviceCreateInfo *pCreateInfo, 
    const VkAllocationCallbacks *pAllocator, 
    VkDevice *pDevice);

void vpGetPhysicalDeviceProfiles(
    VkPhysicalDevice physicalDevice, 
    uint32_t *profileCount, 
    VpProfile *pProfiles);

// Implementation details:
#include <iostream>
#include <map>
#include <set>
#include <string>
#include <vector>
'''

helper_code = '''
inline bool isListed(const std::vector<const char*>& extensions, const char *extensionName) 
{
    for (std::size_t i = 0, n = extensions.size(); i < n; ++i) {
        if (extensions[i] == std::string(extensionName))
            return true;
    }

    return false;
}

inline std::vector<const char *> mergeExtensionList(int firstLength,
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

inline std::map<std::string, uint32_t> createExtensionPropMap(const std::vector<VkExtensionProperties> &supportedExts) {
    std::map<std::string, uint32_t> propertiesMap;
    for (const auto &ext : supportedExts) {
        propertiesMap[std::string(ext.extensionName)] = ext.specVersion;
    }
    return propertiesMap;
}

inline bool checkExtensionProperties(const std::vector<VkExtensionProperties> &requestedExts,
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
'''

def _JOIN(*argv):
    return '\n'.join(argv)


class ProfileLibraryBuilder():
    def __init__(self, database_file, profile_json_dir, header_path):
        self._database_tree = etree.parse(database_file)
        self._profileCreateBuilders = {}
        self._profileDicts = {}
        self._scanAndRegisterProfiles(profile_json_dir)
        self._profileSupportCheckBuilder = self.GetPhysicalDeviceProfilesBuilder(
            self._database_tree,
            self._profileDicts, self)
        self._headerPath = os.path.join(
            os.path.abspath(header_path), "vulkan_profiles.hpp")

    def _scanAndRegisterProfiles(self, profile_json_dir):
        json_dir_abspath = os.path.abspath(profile_json_dir)
        json_filenames = os.listdir(json_dir_abspath)
        for json_filename in json_filenames:
            json_abs_path = os.path.join(json_dir_abspath, json_filename)
            if os.path.isfile(json_abs_path) and os.path.splitext(json_filename)[-1] == '.json':
                with open(json_abs_path, 'r') as json_fp:
                    profile_dict = json.load(json_fp)
                    self._registerProfile(profile_dict)

    def _genProfileEnumDecl(self):
        enum_values = ",\n\t".join(sorted(list(self._profileDicts)))
        return f"typedef enum VpProfile {{ \n\t{enum_values} \n}} VpProfile;"

    def _genProfilesSwitch(self):
        cases = []
        for enum, builder in self._profileCreateBuilders.items():
            case = f'''
        case {enum}: {{
            {builder.genProfileImpl()}
        }} break;'''
            cases.append(case)
        return _JOIN(*cases)

    def _genCreateDeviceImpl(self):
        return f'''
inline VkResult vpCreateDevice(VkPhysicalDevice physicalDevice, const VpProfile profile, const VkDeviceCreateInfo *pCreateInfo, const VkAllocationCallbacks *pAllocator, VkDevice *pDevice) {{
    VkResult result;

    VkPhysicalDeviceFeatures deviceFeatures = {{}};
    vkGetPhysicalDeviceFeatures(physicalDevice, &deviceFeatures);

    std::vector<const char *> extensions;

    switch(profile) {{
        {self._genProfilesSwitch()}
    }}
    return result;
}}'''

    def _registerProfile(self, profile_dict):
        profile_name = profile_dict.get('name')
        if profile_name != None and not self._profileDicts.get('name'):
            print("Registering: ", profile_name)
            self._profileDicts[profile_name] = profile_dict
            self._profileCreateBuilders[profile_name] = self.CreateDeviceBuilder(
                profile_dict, self)
        else:
            raise KeyError(f'Missing or duplicate profile name for {name}')

    def genProfilesHeaderFile(self):
        header_str = header_template.format(self._genProfileEnumDecl()) + helper_code + self._genCreateDeviceImpl() + '\n' + _JOIN(self._profileSupportCheckBuilder.genGetPhysicalDeviceProfilesImpl())
        header_dir = os.path.dirname(self._headerPath)
        os.makedirs(header_dir, exist_ok=True)
        with open(self._headerPath, 'w') as header_fp:
            header_fp.write(header_str)

    class GetPhysicalDeviceProfilesBuilder():
        def __init__(self, database_tree, profile_dicts, parent_inst):
            self._database_tree = database_tree
            self._profile_dicts = profile_dicts
            self._parent_inst = parent_inst

        def _genProfileSwitch(self):
            cases = []
            for enum, profile_dict in self._profile_dicts.items():
                case = f'''
            case {enum}: {{
                {self._genProfileCheck(profile_dict)}
            }} break;'''
                cases.append(case)
            return _JOIN(*cases)

        def _genProfileCheck(self, profile_dict):
            return f'''
                {self._genCheckExtensionProperties(profile_dict)}
                {self._genCheckPhysicalDeviceFeatures(profile_dict)}
                {self._getCheckImageFormatSupport(profile_dict)}
                {self._genCheckAlternatives(profile_dict)}'''

        def _genCheckExtensionProperties(self, profile_dict, isAlt=False):
            check_str = ""
            extensions = profile_dict.get('ArrayOfVkExtensionProperties')
            if extensions and len(extensions) > 0:
                literals = "{" + ','.join(map(lambda ext: '\n\t\t\t\t\t\t\tVkExtensionProperties{{"{}", {}}}'.format(ext['extensionName'], ext['specVersion']), extensions)) + "}"
                expr_str = f"checkExtensionProperties(profileExts, extensionProperties)"
                check_str += f'''
                std::vector<VkExtensionProperties> profileExts = {literals};
                {self._genCheckMacroCall(expr_str, isAlt)}'''
            return check_str

        def _genCheckPhysicalDeviceProperties(self, profile_dict, isAlt=False):
            # TODO support members other than just limits

            root = self._database_tree.getroot()

            properties = profile_dict.get('VkPhysicalDeviceProperties')


            check_str = ""
            print("Limits 0")
            if properties:
                print("Limits 1")
                limits = properties.get('limits')
                if limits:
                    print("Limits 2")
                    for member, limit in limits.items():
                        op = '<='

                        for element in root.findall("./types/type[@category='struct']/member"):
                            if element.find('name').text == f'{member}':
                                if element.get('limittype') == 'min':
                                    op = '>='
                                break

                        expr_str = f"deviceProperties.limits.{member} {op} {limit['value']}"
                        check_str += f"\t\t\t\t{self._genCheckMacroCall(expr_str, isAlt)};"
            return check_str

        def _genCheckPhysicalDeviceFeatures(self, profile_dict, isAlt=False):
            check_str = ""
            features = profile_dict.get('VkPhysicalDeviceFeatures')
            if features:
                for member, value in features.items():
                    expr_str = f"deviceFeatures.{member} == {value}"
                    check_str += f"\t\t\t\t{self._genCheckMacroCall(expr_str, isAlt)};\n"
            return check_str

        def _getCheckImageFormatSupport(self, profile_dict, isAlt=False):
            format_reqs = profile_dict.get('ArrayOfImageFormatRequirements')

            if format_reqs and len(format_reqs) > 0:
                format_checks = [
                    f"VkImageFormatProperties imageFormatProperties = {{}};"]
                for format_req in format_reqs:
                    format_name = format_req.get("format")
                    type_name = format_req.get("type")
                    tiling = format_req.get("tiling")
                    usage_flag_names = format_req.get("usage")
                    create_flag_names = format_req.get("flags")
                    if usage_flag_names:
                        usage_flag_bits = '|'.join(
                            map(lambda flag_name: f'{flag_name}', usage_flag_names))
                        format_checks.append(f'''
                        VkImageUsageFlags usage = {usage_flag_bits};''')
                    else:
                        format_checks.append(f'''
                        VkImageUsageFlags usage = 0;''')

                    if create_flag_names:
                        create_flag_bits = '|'.join(
                            map(lambda flag_name: f'{flag_name}', create_flag_names))
                        format_checks.append(f'''
                        VkImageCreateFlags flags = {create_flag_bits};''')
                    else:
                        format_checks.append(f'''
                        VkImageCreateFlags flags = 0;''')
                        format_checks.append(f'''
                        result = vkGetPhysicalDeviceImageFormatProperties(physicalDevice, {format_name}, {type_name}, {tiling}, usage, flags, &imageFormatProperties);''')
                        format_checks.append(
                        f"{self._genCheckMacroCall('result == VK_SUCCESS', isAlt)};")
                    return _JOIN(*format_checks)
            else:
                return ""

        def _genCheckAlternatives(self, profile_dict):
            alt_groups = profile_dict.get('ArrayOfArrayOfAlternatives')
            if alt_groups and len(alt_groups) > 0:
                alt_group_checks = []
                for alt_group in alt_groups:
                    alt_group_checks.append("\t\t\t\t{")
                    if alt_group and len(alt_group) > 0:
                        alts_checks = [
                            "\t\t\t\t\tbool altGroupSupported = false;"]
                        for alt in alt_group:
                            alts_checks.append(f'''
                    {{
                        bool altSupported = true;
                        {self._genCheckExtensionProperties(alt, True)}
                        {self._genCheckPhysicalDeviceProperties(alt, True)}
                        {self._genCheckPhysicalDeviceFeatures(alt, True)}
                        {self._getCheckImageFormatSupport(alt, True)}
                        altGroupSupported = altGroupSupported || altSupported;
                    }}''')

                        alt_group_checks.append(_JOIN(*alts_checks))
                    alt_group_checks.append(_JOIN(
                        f"{self._genCheckMacroCall('altGroupSupported')};", "}"))
                return _JOIN(*alt_group_checks)
            else:
                return ""

        def _genCheckMacroCall(self, expr_str, isAlt=False):
            if isAlt:
                return f'ALT_CHECK_SUPPORT_EXPR({expr_str}, \"{expr_str}\")'
            else:
                return f'CHECK_SUPPORT_EXPR({expr_str}, \"{expr_str}\")'

        def genGetPhysicalDeviceProfilesImpl(self):
            return f'''
inline void vpGetPhysicalDeviceProfiles(VkPhysicalDevice physicalDevice, uint32_t *profileCount, VpProfile *pProfiles) {{
    #define CHECK_SUPPORT_EXPR(expr, msg) supported = supported && expr; \\
        std::cout << msg << " " << (expr ? " PASS" : " FAIL") << std::endl;
    #define ALT_CHECK_SUPPORT_EXPR(expr, msg) altSupported = altSupported && expr;\\
        std::cout << msg << " " << (expr ? " PASS" : " FAIL") << std::endl;

    VkResult result;

    uint32_t extensionPropertyCount;
    result = vkEnumerateInstanceExtensionProperties(nullptr, &extensionPropertyCount, nullptr);
    std::vector<VkExtensionProperties> extensionProperties(extensionPropertyCount);
    result = vkEnumerateInstanceExtensionProperties(nullptr, &extensionPropertyCount, extensionProperties.data());

    uint32_t deviceExtensionCount;
    result = vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &deviceExtensionCount, nullptr);
    std::vector<VkExtensionProperties> deviceExtensions(deviceExtensionCount);
    result = vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &deviceExtensionCount, deviceExtensions.data());

    VkPhysicalDeviceFeatures deviceFeatures = {{}};
    vkGetPhysicalDeviceFeatures(physicalDevice, &deviceFeatures);

    VkPhysicalDeviceProperties deviceProperties = {{}};
    vkGetPhysicalDeviceProperties(physicalDevice, &deviceProperties);

    uint32_t supported_profile_count = 0;
    const int VP_PROFILE_COUNT = {len(self._profile_dicts)};

    for (int profile_index = 0; profile_index < VP_PROFILE_COUNT; ++profile_index) {{
        VkBool32 supported = VK_TRUE;

        switch(profile_index) {{
            {self._genProfileSwitch()}
        }}

        if (supported) {{
            if (pProfiles != nullptr && supported_profile_count < *profileCount) {{
                pProfiles[supported_profile_count] = static_cast<VpProfile>(profile_index);
            }}
            supported_profile_count++;
        }}
    }}

    if (pProfiles == nullptr) {{
        *profileCount = supported_profile_count;
    }}
}}'''

    class CreateDeviceBuilder():
        def __init__(self, profile_dict, parent_inst):
            self._profile_dict = profile_dict
            self._parent_inst = parent_inst

        def _genPhysicalDeviceFeatures(self):
            struct_blocks = '''
            VkPhysicalDeviceFeatures enabledFeatures = {};
            if (pCreateInfo->pEnabledFeatures != nullptr)
                enabledFeatures = *pCreateInfo->pEnabledFeatures;
            '''

            features_list = self._profile_dict.get("VkPhysicalDeviceFeatures")
            if features_list:
                for feature, value in features_list.items():
                    struct_blocks += f'''
            enabledFeatures.{feature} = {value};'''
            alt_groups = self._profile_dict.get('ArrayOfArrayOfAlternatives')
            if alt_groups:
                for alt_group in alt_groups:
                    for alt in alt_group:
                        alt_features = alt.get('VkPhysicalDeviceFeatures')
                        if alt_features:
                            for feature_name, value in alt_features.items():
                                struct_blocks += f'''
            if (deviceFeatures.{feature_name} == {value}) {{
                enabledFeatures.{feature_name} = {value};
            }}'''
            return struct_blocks

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
                literals = "{" + ','.join(map(lambda ext: '\n\t\t\t\tVkExtensionProperties{{"{}", {}}}'.format(ext['extensionName'], ext['specVersion']), extensions)) + "}"
                return f'''
            std::vector<VkExtensionProperties> profileExtProps = {literals};

            for (uint32_t i = 0; i < profileExtProps.size(); ++i) {{
                extensions.push_back(profileExtProps[i].extensionName);
            }}

            for (uint32_t i = 0; i < pCreateInfo->enabledExtensionCount; ++i) {{
                if (isListed(extensions, pCreateInfo->ppEnabledExtensionNames[i]))
                    continue;
                extensions.push_back(pCreateInfo->ppEnabledExtensionNames[i]);
            }}
            '''
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
                    name = "pNext_{0}".format(i)
                    struct_decl = f"\t\t\t{typeName} {name} = {{}};\n"
                    struct_decl += f"\t\t\t{name}.sType = {self._getSType(typeName)};\n"
                    if i == len(next_struct_descs) - 1:
                        struct_decl += f"\t\t\t{name}.pNext = const_cast<void*>(pCreateInfo->pNext);\n"
                    else:
                        struct_decl += f"\t\t\t{name}.pNext = &{'pNext_{0}'.format(i+1)};\n"
                    for field, value in members.items():
                        struct_decl += f"\t\t\t{name}.{field} = {value};\n"
                    struct_decls.insert(0, struct_decl)
                struct_decls.append('''
            void* pNext = &pNext_0;''')
                return _JOIN(*struct_decls)
            else:
                return f'''
            void* pNext = nullptr;'''

        def genProfileImpl(self):
            return f'''
            {self._genEnabledExtensions()}
            {self._genPhysicalDeviceFeatures()}
            {self._genNextChain()}

            VkDeviceCreateInfo deviceCreateInfo = {{}};
            deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
            deviceCreateInfo.pNext = pNext;
            deviceCreateInfo.queueCreateInfoCount = pCreateInfo->queueCreateInfoCount;
            deviceCreateInfo.pQueueCreateInfos = pCreateInfo->pQueueCreateInfos;
            deviceCreateInfo.enabledExtensionCount = extensions.empty() ? 0 : static_cast<uint32_t>(extensions.size());
            deviceCreateInfo.ppEnabledExtensionNames = extensions.empty() ? nullptr : static_cast<const char* const*>(extensions.data());
            deviceCreateInfo.pEnabledFeatures = &enabledFeatures;
            result = vkCreateDevice(physicalDevice, &deviceCreateInfo, pAllocator, pDevice);'''

    class ProfileTestBuilder():
        pass


if __name__ == "__main__":
    # python ./scripts/generate_library.py ./registry/vk.xml ./scripts/profiles ./include/vulkan
    #database = etree.parse(sys.argv[1])
    #root = database.getroot()

    #for element in root.findall("./types/type[@category='struct']/member"):
    #    if element.find('name').text == 'maxImageDimension1D':
    #        print(element.get('limittype'))

    #database_file = sys.argv[1]
    #profile_dir = sys.argv[2]
    #header_path = sys.argv[3]

    dirname = os.path.dirname(__file__)
    database = os.path.join(dirname, 'vk.xml')
    profiles = os.path.join(dirname, '../../profiles')
    output = os.path.join(dirname, '../include/vulkan')

    builder = ProfileLibraryBuilder(database, profiles, output)
    builder.genProfilesHeaderFile()

