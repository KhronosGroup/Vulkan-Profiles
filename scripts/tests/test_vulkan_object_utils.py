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

import argparse
from pathlib import Path
import sys
import unittest

scripts_dir = Path(__file__).resolve().parent.parent
if str(scripts_dir) not in sys.path:
    sys.path.insert(0, str(scripts_dir))

from vulkan_object import VulkanObject, StructCapabilityAlias, ExtensionCapabilityAlias
from source.vulkan_object_utils import getVulkanObject, gatherCapabilityAliases

class TestVulkanObjectUtils(unittest.TestCase):
    registry_path = None

    # Check we can get the list of feature aliases from any feature structure
    def testVulkanObjectUtilsStructFeatureAliasesAccess(self):
        vk: VulkanObject = getVulkanObject(self.registry_path)
        
        # Case 2: Building the list of aliases of an actual struct using the getAliases helper function that hide that not all structs are stored in vk.structs
        query_id2 = StructCapabilityAlias("VkPhysicalDeviceShaderSubgroupRotateFeatures", "shaderSubgroupRotate")
        member_2_aliases = gatherCapabilityAliases(vk, query_id2)
        assert len(member_2_aliases) == 2
        assert StructCapabilityAlias("VkPhysicalDeviceShaderSubgroupRotateFeaturesKHR", "shaderSubgroupRotate") in member_2_aliases
        assert StructCapabilityAlias("VkPhysicalDeviceVulkan14Features", "shaderSubgroupRotate") in member_2_aliases

        # Case 3: Building the list of aliases from an aliased feature structure, not stored in vk.structs
        query_id3 = StructCapabilityAlias("VkPhysicalDeviceShaderSubgroupRotateFeaturesKHR", "shaderSubgroupRotate")
        member_3_aliases = gatherCapabilityAliases(vk, query_id3)
        assert len(member_3_aliases) == 2
        assert StructCapabilityAlias("VkPhysicalDeviceShaderSubgroupRotateFeatures", "shaderSubgroupRotate") in member_3_aliases
        assert StructCapabilityAlias("VkPhysicalDeviceVulkan14Features", "shaderSubgroupRotate") in member_3_aliases

        # Case 4: Building the list of aliases from a "bundle" feature structure stored in vk.structs
        query_id4 = StructCapabilityAlias("VkPhysicalDeviceVulkan14Features", "shaderSubgroupRotate")
        member_4_aliases = gatherCapabilityAliases(vk, query_id4)
        assert len(member_4_aliases) == 2
        assert StructCapabilityAlias("VkPhysicalDeviceShaderSubgroupRotateFeatures", "shaderSubgroupRotate") in member_4_aliases
        assert StructCapabilityAlias("VkPhysicalDeviceShaderSubgroupRotateFeaturesKHR", "shaderSubgroupRotate") in member_4_aliases

        # Case 5: Building the list of aliases from a aliased feature structure of a extension
        query_id5 = StructCapabilityAlias("VkPhysicalDeviceMutableDescriptorTypeFeaturesVALVE", "mutableDescriptorType")
        member_5_aliases = gatherCapabilityAliases(vk, query_id5)
        assert len(member_5_aliases) == 1
        assert StructCapabilityAlias("VkPhysicalDeviceMutableDescriptorTypeFeaturesEXT", "mutableDescriptorType") in member_5_aliases

        # Case 6: Building the list of aliases from a feature structure of a extension
        query_id6 = StructCapabilityAlias("VkPhysicalDeviceMutableDescriptorTypeFeaturesEXT", "mutableDescriptorType")
        member_6_aliases = gatherCapabilityAliases(vk, query_id6)
        assert len(member_6_aliases) == 1
        assert StructCapabilityAlias("VkPhysicalDeviceMutableDescriptorTypeFeaturesVALVE", "mutableDescriptorType") in member_6_aliases

        # Case 7: A feature from a VkPhysicalDeviceVulkan1*Features structure that has NO alias
        query_id7 = StructCapabilityAlias("VkPhysicalDeviceVulkan12Features", "subgroupBroadcastDynamicId")
        member_7_aliases = gatherCapabilityAliases(vk, query_id7)
        assert len(member_7_aliases) == 0

        # Case 8: A feature from an extension that has NO alias
        query_id8 = StructCapabilityAlias("VkPhysicalDeviceCornerSampledImageFeaturesNV", "cornerSampledImage")
        member_8_aliases = gatherCapabilityAliases(vk, query_id8)
        assert len(member_8_aliases) == 0

        # Case 9: A feature from a VkPhysicalDeviceVulkan1*Features structure whose alias is an explicit ExtensionAlias
        query_id9 = StructCapabilityAlias("VkPhysicalDeviceVulkan12Features", "samplerMirrorClampToEdge")
        member_9_aliases = gatherCapabilityAliases(vk, query_id9)
        assert len(member_9_aliases) == 1
        assert ExtensionCapabilityAlias("VK_KHR_sampler_mirror_clamp_to_edge") in member_9_aliases

        # Case A: A feature from a VkPhysicalDeviceVulkan1*Features structure whose alias is an explicit ExtensionAlias
        query_idA = ExtensionCapabilityAlias("VK_KHR_sampler_mirror_clamp_to_edge")
        member_A_aliases = gatherCapabilityAliases(vk, query_idA)
        assert len(member_A_aliases) == 1
        assert StructCapabilityAlias("VkPhysicalDeviceVulkan12Features", "samplerMirrorClampToEdge") in member_A_aliases

        # Case B: Unknown structure
        query_idB = StructCapabilityAlias("VkUnknownFeatures", "globalPriorityQuery")
        member_B_aliases = gatherCapabilityAliases(vk, query_idB)
        assert member_B_aliases == []

        # Case C: Unknown feature
        query_idC = StructCapabilityAlias("VkPhysicalDeviceGlobalPriorityQueryFeatures", "unknown")
        member_C_aliases = gatherCapabilityAliases(vk, query_idC)
        assert member_C_aliases == []

        # Case D: Unknown extension
        query_idD = ExtensionCapabilityAlias("VK_LUNARG_unknown")
        member_D_aliases = gatherCapabilityAliases(vk, query_idD)
        assert member_D_aliases == []

    # Check we can get the list of property aliases from any property structure
    def testVulkanObjectUtilsStructPropertyAliasesAccess(self):
        vk: VulkanObject = getVulkanObject(self.registry_path)

        # Case 1: Building the list of aliases of an actual struct using the getCapabilityAliases helper function that hide that not all structs are stored in vk.structs
        query_id1 = StructCapabilityAlias("VkPhysicalDeviceLineRasterizationProperties", "lineSubPixelPrecisionBits")
        member_1_aliases = gatherCapabilityAliases(vk, query_id1)
        assert len(member_1_aliases) == 3
        assert StructCapabilityAlias("VkPhysicalDeviceLineRasterizationPropertiesKHR", "lineSubPixelPrecisionBits") in member_1_aliases
        assert StructCapabilityAlias("VkPhysicalDeviceLineRasterizationPropertiesEXT", "lineSubPixelPrecisionBits") in member_1_aliases
        assert StructCapabilityAlias("VkPhysicalDeviceVulkan14Properties", "lineSubPixelPrecisionBits") in member_1_aliases

        # Case 2: Building the list of aliases from a "bundle" property structure stored in vk.structs
        query_id2 = StructCapabilityAlias("VkPhysicalDeviceVulkan14Properties", "lineSubPixelPrecisionBits")
        member_2_aliases = gatherCapabilityAliases(vk, query_id2)
        assert len(member_2_aliases) == 3
        assert StructCapabilityAlias("VkPhysicalDeviceLineRasterizationPropertiesKHR", "lineSubPixelPrecisionBits") in member_2_aliases
        assert StructCapabilityAlias("VkPhysicalDeviceLineRasterizationPropertiesEXT", "lineSubPixelPrecisionBits") in member_2_aliases
        assert StructCapabilityAlias("VkPhysicalDeviceLineRasterizationProperties", "lineSubPixelPrecisionBits") in member_2_aliases
        
        # Case 3: Building the list of aliases from an aliased property structure, not stored in vk.structs
        query_id3 = StructCapabilityAlias("VkPhysicalDeviceLineRasterizationPropertiesKHR", "lineSubPixelPrecisionBits")
        member_3_aliases = gatherCapabilityAliases(vk, query_id3)
        assert len(member_3_aliases) == 3
        assert StructCapabilityAlias("VkPhysicalDeviceLineRasterizationPropertiesEXT", "lineSubPixelPrecisionBits") in member_3_aliases
        assert StructCapabilityAlias("VkPhysicalDeviceLineRasterizationProperties", "lineSubPixelPrecisionBits") in member_3_aliases
        assert StructCapabilityAlias("VkPhysicalDeviceVulkan14Properties", "lineSubPixelPrecisionBits") in member_3_aliases

        # Case 4: Building the list of aliases from an aliased property structure, not stored in vk.structs
        query_id3 = StructCapabilityAlias("VkPhysicalDeviceLineRasterizationPropertiesEXT", "lineSubPixelPrecisionBits")
        member_3_aliases = gatherCapabilityAliases(vk, query_id3)
        assert len(member_3_aliases) == 3
        assert StructCapabilityAlias("VkPhysicalDeviceLineRasterizationPropertiesKHR", "lineSubPixelPrecisionBits") in member_3_aliases
        assert StructCapabilityAlias("VkPhysicalDeviceLineRasterizationProperties", "lineSubPixelPrecisionBits") in member_3_aliases
        assert StructCapabilityAlias("VkPhysicalDeviceVulkan14Properties", "lineSubPixelPrecisionBits") in member_3_aliases

        # Case 5: A feature from a VkPhysicalDeviceVulkan1*Properties structure that has NO alias
        query_id5 = StructCapabilityAlias("VkPhysicalDeviceVulkan13Properties", "maxBufferSize")
        member_5_aliases = gatherCapabilityAliases(vk, query_id5)
        assert len(member_5_aliases) == 0

        # Case 6: A feature from an extension that has NO alias
        query_id6 = StructCapabilityAlias("VkPhysicalDeviceGpaPropertiesAMD", "shaderEngineCount")
        member_6_aliases = gatherCapabilityAliases(vk, query_id6)
        assert len(member_6_aliases) == 0

        # Case 7: A property from a VkPhysicalDeviceVulkan1*Properties structure whose alias property was renamed
        query_id7 = StructCapabilityAlias("VkPhysicalDeviceVulkan11Properties", "subgroupQuadOperationsInAllStages")
        member_7_aliases = gatherCapabilityAliases(vk, query_id7)
        assert len(member_7_aliases) == 1
        assert StructCapabilityAlias("VkPhysicalDeviceSubgroupProperties", "quadOperationsInAllStages") in member_7_aliases

        # Case 8: A property from a VkPhysicalDeviceVulkan1*Properties structure whose alias property was renamed
        query_id8 = StructCapabilityAlias("VkPhysicalDeviceSubgroupProperties", "quadOperationsInAllStages")
        member_8_aliases = gatherCapabilityAliases(vk, query_id8)
        assert len(member_8_aliases) == 1
        assert StructCapabilityAlias("VkPhysicalDeviceVulkan11Properties", "subgroupQuadOperationsInAllStages") in member_8_aliases

        # Case A: A property from a KHR/EXT extension whose name is the same as a feature name
        query_idA = StructCapabilityAlias("VkPhysicalDeviceLineRasterizationPropertiesEXT", "lineSubPixelPrecisionBits")
        member_A_aliases = gatherCapabilityAliases(vk, query_idA)
        assert len(member_A_aliases) == 3
        assert StructCapabilityAlias("VkPhysicalDeviceLineRasterizationProperties", "lineSubPixelPrecisionBits") in member_A_aliases
        assert StructCapabilityAlias("VkPhysicalDeviceLineRasterizationPropertiesKHR", "lineSubPixelPrecisionBits") in member_A_aliases
        assert StructCapabilityAlias("VkPhysicalDeviceVulkan14Properties", "lineSubPixelPrecisionBits") in member_A_aliases

        # Case B: Unknown structure
        query_idB = StructCapabilityAlias("VkUnknownProperty", "maxInlineUniformTotalSize")
        member_B_aliases = gatherCapabilityAliases(vk, query_idB)
        assert member_B_aliases == []

        # Case C: Unknown feature
        query_idC = StructCapabilityAlias("VkPhysicalDeviceVertexAttributeDivisorProperties", "unknown")
        member_C_aliases = gatherCapabilityAliases(vk, query_idC)
        assert member_C_aliases == []

if __name__ == '__main__':
    parser = argparse.ArgumentParser()

    parser.add_argument('--registry', '-r', action='store', required=True,
                        help='Use specified registry file instead of vk.xml (video.xml must be present in the same directory for video support).')

    args, unparsed = parser.parse_known_args()
    TestVulkanObjectUtils.registry_path = args.registry

    unittest.main(argv=[sys.argv[0]] + unparsed)

