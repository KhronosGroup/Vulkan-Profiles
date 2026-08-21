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
from source.vulkan_object_utils import (
    initVulkanObject,
    VK_VERSION,
    gatherCapabilityAliases,
    gatherDependentCapabilityAliases,
    gatherDependentExtensions,
    isStructExtensionEnabled,
    findExtensionVersion,
    gatherDynamicStructs,
    gatherSatisfiedCoreRequiredFeaturesForVersion,
    gatherSatisfiedExtensionRequiredFeatures,
    gatherPromotedExtensionsForVersion,
    gatherPromotedExtensionsForExactVersion,
    gatherRequiredFeaturesForVersion,
    getExtensionPromotedTo,
    getStructDefiningExtensions
)


class TestVulkanObjectUtils(unittest.TestCase):
    registry_path = None

    def testGetExtensionPromotedTo(self):
        vk: VulkanObject = initVulkanObject('vulkan', self.registry_path)

        promoted_multiview = getExtensionPromotedTo(vk, "VK_KHR_multiview")
        self.assertIn("VK_VERSION_1_1", promoted_multiview)

        promoted_dynamic_rendering = getExtensionPromotedTo(vk, "VK_KHR_dynamic_rendering")
        self.assertIn("VK_VERSION_1_3", promoted_dynamic_rendering)

        promoted_none = getExtensionPromotedTo(vk, "VK_EXT_debug_report")
        self.assertEqual(promoted_none, [])

    def testGetStructDefiningExtensions(self):
        vk: VulkanObject = initVulkanObject('vulkan', self.registry_path)

        exts_imageless = getStructDefiningExtensions(vk, "VkPhysicalDeviceImagelessFramebufferFeaturesKHR")
        self.assertIn("VK_KHR_imageless_framebuffer", exts_imageless)

        exts_16bit = getStructDefiningExtensions(vk, "VkPhysicalDevice16BitStorageFeaturesKHR")
        self.assertIn("VK_KHR_16bit_storage", exts_16bit)

        exts_core = getStructDefiningExtensions(vk, "VkPhysicalDeviceFeatures")
        self.assertEqual(exts_core, [])

    def testGatherSatisfiedCoreRequiredFeaturesVersionBoundaries(self):
        vk: VulkanObject = initVulkanObject('vulkan', self.registry_path)

        # On Vulkan 1.0, core Vulkan 1.1+ features must strictly NOT be pulled
        core_features_v10 = gatherSatisfiedCoreRequiredFeaturesForVersion(
            vk, VK_VERSION.V1_1, VK_VERSION.V1_0, enabled_exts=set(), enabled_features=set()
        )
        self.assertNotIn("VkPhysicalDeviceVulkan11Features", core_features_v10)
        self.assertNotIn("VkPhysicalDeviceVulkan12Features", core_features_v10)

        # On Vulkan 1.2, evaluating core 1.2 features for a Vulkan 1.2 profile
        core_features_v12 = gatherSatisfiedCoreRequiredFeaturesForVersion(
            vk, VK_VERSION.V1_2, VK_VERSION.V1_2, enabled_exts=set(), enabled_features=set()
        )
        if "VkPhysicalDeviceVulkan12Features" in core_features_v12:
            self.assertIn("timelineSemaphore", core_features_v12["VkPhysicalDeviceVulkan12Features"])

    def testGatherSatisfiedExtensionRequiredFeatures(self):
        vk: VulkanObject = initVulkanObject('vulkan', self.registry_path)

        # Evaluating VK_KHR_variable_pointers should gather its required features
        ext_features = gatherSatisfiedExtensionRequiredFeatures(
            vk, "VK_KHR_variable_pointers", VK_VERSION.V1_0, 
            enabled_exts={"VK_KHR_variable_pointers"}, enabled_features=set()
        )
        self.assertIn("VkPhysicalDeviceVariablePointerFeaturesKHR", ext_features)
        self.assertTrue(ext_features["VkPhysicalDeviceVariablePointerFeaturesKHR"]["variablePointersStorageBuffer"])

    def testVulkanObjectUtilsStructDependentFeatureAliasesAccess(self):
        vk: VulkanObject = initVulkanObject('vulkan', self.registry_path)
        
        query_id1 = StructCapabilityAlias("VkPhysicalDeviceShaderSubgroupRotateFeatures", "shaderSubgroupRotate")

        member_1A_aliases = gatherDependentCapabilityAliases(vk, VK_VERSION.V1_1, query_id1)
        assert len(member_1A_aliases) == 1
        assert StructCapabilityAlias("VkPhysicalDeviceShaderSubgroupRotateFeaturesKHR", "shaderSubgroupRotate") in member_1A_aliases
      
        member_1B_aliases = gatherDependentCapabilityAliases(vk, VK_VERSION.V1_4, query_id1)
        assert len(member_1B_aliases) == 2
        assert StructCapabilityAlias("VkPhysicalDeviceVulkan14Features", "shaderSubgroupRotate") in member_1B_aliases
        assert StructCapabilityAlias("VkPhysicalDeviceShaderSubgroupRotateFeaturesKHR", "shaderSubgroupRotate") in member_1B_aliases
        
        query_id2 = StructCapabilityAlias("VkPhysicalDeviceVulkan14Features", "shaderSubgroupRotate")

        member_2A_aliases = gatherDependentCapabilityAliases(vk, VK_VERSION.V1_1, query_id2)
        assert len(member_2A_aliases) == 1
        assert StructCapabilityAlias("VkPhysicalDeviceShaderSubgroupRotateFeaturesKHR", "shaderSubgroupRotate") in member_2A_aliases
        
        member_2B_aliases = gatherDependentCapabilityAliases(vk, VK_VERSION.V1_4, query_id2)
        assert len(member_2B_aliases) == 2
        assert StructCapabilityAlias("VkPhysicalDeviceShaderSubgroupRotateFeatures", "shaderSubgroupRotate") in member_2B_aliases
        assert StructCapabilityAlias("VkPhysicalDeviceShaderSubgroupRotateFeaturesKHR", "shaderSubgroupRotate") in member_2B_aliases

        query_id3 = StructCapabilityAlias("VkPhysicalDeviceShaderSubgroupRotateFeaturesKHR", "shaderSubgroupRotate")

        member_3A_aliases = gatherDependentCapabilityAliases(vk, VK_VERSION.V1_1, query_id3)
        assert len(member_3A_aliases) == 0
      
        member_3B_aliases = gatherDependentCapabilityAliases(vk, VK_VERSION.V1_4, query_id3)
        assert len(member_3B_aliases) == 2
        assert StructCapabilityAlias("VkPhysicalDeviceShaderSubgroupRotateFeatures", "shaderSubgroupRotate") in member_3B_aliases
        assert StructCapabilityAlias("VkPhysicalDeviceVulkan14Features", "shaderSubgroupRotate") in member_3B_aliases

        query_id4 = StructCapabilityAlias("VkPhysicalDevice8BitStorageFeatures", "storageBuffer8BitAccess")

        member_4A_aliases = gatherDependentCapabilityAliases(vk, VK_VERSION.V1_1, query_id4)
        assert len(member_4A_aliases) == 1
        assert StructCapabilityAlias("VkPhysicalDevice8BitStorageFeaturesKHR", "storageBuffer8BitAccess") in member_4A_aliases

        member_4B_aliases = gatherDependentCapabilityAliases(vk, VK_VERSION.V1_2, query_id4)
        assert len(member_4B_aliases) == 2
        assert StructCapabilityAlias("VkPhysicalDevice8BitStorageFeaturesKHR", "storageBuffer8BitAccess") in member_4B_aliases
        assert StructCapabilityAlias("VkPhysicalDeviceVulkan12Features", "storageBuffer8BitAccess") in member_4B_aliases

        member_4C_aliases = gatherDependentCapabilityAliases(vk, VK_VERSION.V1_3, query_id4)
        assert len(member_4C_aliases) == 2
        assert StructCapabilityAlias("VkPhysicalDevice8BitStorageFeaturesKHR", "storageBuffer8BitAccess") in member_4C_aliases
        assert StructCapabilityAlias("VkPhysicalDeviceVulkan12Features", "storageBuffer8BitAccess") in member_4C_aliases
        
        query_id5 = StructCapabilityAlias("VkPhysicalDeviceVulkan12Features", "storageBuffer8BitAccess")

        member_5A_aliases = gatherDependentCapabilityAliases(vk, VK_VERSION.V1_1, query_id5)
        assert len(member_5A_aliases) == 1
        assert StructCapabilityAlias("VkPhysicalDevice8BitStorageFeaturesKHR", "storageBuffer8BitAccess") in member_5A_aliases

        member_5B_aliases = gatherDependentCapabilityAliases(vk, VK_VERSION.V1_2, query_id5)
        assert len(member_5B_aliases) == 2
        assert StructCapabilityAlias("VkPhysicalDevice8BitStorageFeaturesKHR", "storageBuffer8BitAccess") in member_5B_aliases
        assert StructCapabilityAlias("VkPhysicalDevice8BitStorageFeatures", "storageBuffer8BitAccess") in member_5B_aliases

        member_5C_aliases = gatherDependentCapabilityAliases(vk, VK_VERSION.V1_3, query_id5)
        assert len(member_5C_aliases) == 2
        assert StructCapabilityAlias("VkPhysicalDevice8BitStorageFeaturesKHR", "storageBuffer8BitAccess") in member_5C_aliases
        assert StructCapabilityAlias("VkPhysicalDevice8BitStorageFeatures", "storageBuffer8BitAccess") in member_5C_aliases
        
        query_id6 = StructCapabilityAlias("VkPhysicalDevice8BitStorageFeaturesKHR", "storageBuffer8BitAccess")

        member_6A_aliases = gatherDependentCapabilityAliases(vk, VK_VERSION.V1_1, query_id6)
        assert len(member_6A_aliases) == 0

        member_6B_aliases = gatherDependentCapabilityAliases(vk, VK_VERSION.V1_2, query_id6)
        assert len(member_6B_aliases) == 2
        assert StructCapabilityAlias("VkPhysicalDevice8BitStorageFeatures", "storageBuffer8BitAccess") in member_6B_aliases
        assert StructCapabilityAlias("VkPhysicalDeviceVulkan12Features", "storageBuffer8BitAccess") in member_6B_aliases

        member_6C_aliases = gatherDependentCapabilityAliases(vk, VK_VERSION.V1_3, query_id6)
        assert len(member_6C_aliases) == 2
        assert StructCapabilityAlias("VkPhysicalDevice8BitStorageFeatures", "storageBuffer8BitAccess") in member_6C_aliases
        assert StructCapabilityAlias("VkPhysicalDeviceVulkan12Features", "storageBuffer8BitAccess") in member_6C_aliases

        query_id7 = StructCapabilityAlias("VkPhysicalDeviceVulkan12Features", "samplerMirrorClampToEdge")

        member_7A_aliases = gatherDependentCapabilityAliases(vk, VK_VERSION.V1_1, query_id7)
        assert len(member_7A_aliases) == 1
        assert ExtensionCapabilityAlias("VK_KHR_sampler_mirror_clamp_to_edge") in member_7A_aliases

        member_8B_aliases = gatherDependentCapabilityAliases(vk, VK_VERSION.V1_2, query_id7)
        assert len(member_8B_aliases) == 0

        query_id8 = StructCapabilityAlias("VkPhysicalDeviceMutableDescriptorTypeFeaturesVALVE", "mutableDescriptorType")
        member_8_aliases = gatherDependentCapabilityAliases(vk, VK_VERSION.V1_1, query_id8)
        assert len(member_8_aliases) == 1
        assert StructCapabilityAlias("VkPhysicalDeviceMutableDescriptorTypeFeaturesEXT", "mutableDescriptorType") in member_8_aliases

        query_id9 = StructCapabilityAlias("VkPhysicalDeviceMutableDescriptorTypeFeaturesEXT", "mutableDescriptorType")
        member_9_aliases = gatherDependentCapabilityAliases(vk, VK_VERSION.V1_4, query_id9)
        assert len(member_9_aliases) == 1
        assert StructCapabilityAlias("VkPhysicalDeviceMutableDescriptorTypeFeaturesVALVE", "mutableDescriptorType") in member_9_aliases
        
        query_id_none1 = StructCapabilityAlias("VkPhysicalDeviceShaderSubgroupRotateFeatures", "shaderSubgroupRotate")
        member_none1_aliases = gatherDependentCapabilityAliases(vk, VK_VERSION.NONE, query_id_none1)
        assert len(member_none1_aliases) == 2
        assert StructCapabilityAlias("VkPhysicalDeviceShaderSubgroupRotateFeaturesKHR", "shaderSubgroupRotate") in member_none1_aliases
        assert StructCapabilityAlias("VkPhysicalDeviceVulkan14Features", "shaderSubgroupRotate") in member_none1_aliases

        query_id_none2 = StructCapabilityAlias("VkPhysicalDeviceVulkan14Features", "shaderSubgroupRotate")
        member_none2_aliases = gatherDependentCapabilityAliases(vk, VK_VERSION.NONE, query_id_none2)
        assert len(member_none2_aliases) == 2
        assert StructCapabilityAlias("VkPhysicalDeviceShaderSubgroupRotateFeatures", "shaderSubgroupRotate") in member_none2_aliases
        assert StructCapabilityAlias("VkPhysicalDeviceShaderSubgroupRotateFeaturesKHR", "shaderSubgroupRotate") in member_none2_aliases

        query_id_none3 = StructCapabilityAlias("VkPhysicalDeviceVulkan12Features", "samplerMirrorClampToEdge")
        member_none3_aliases = gatherDependentCapabilityAliases(vk, VK_VERSION.NONE, query_id_none3)
        assert len(member_none3_aliases) == 1
        assert ExtensionCapabilityAlias("VK_KHR_sampler_mirror_clamp_to_edge") in member_none3_aliases

        query_id_none4 = StructCapabilityAlias("VkPhysicalDeviceMutableDescriptorTypeFeaturesEXT", "mutableDescriptorType")
        member_none4_aliases = gatherDependentCapabilityAliases(vk, VK_VERSION.NONE, query_id_none4)
        assert len(member_none4_aliases) == 1
        assert StructCapabilityAlias("VkPhysicalDeviceMutableDescriptorTypeFeaturesVALVE", "mutableDescriptorType") in member_none4_aliases

    def testVulkanObjectUtilsStructFeatureAliasesAccess(self):
        vk: VulkanObject = initVulkanObject('vulkan', self.registry_path)
        
        query_id2 = StructCapabilityAlias("VkPhysicalDeviceShaderSubgroupRotateFeatures", "shaderSubgroupRotate")
        member_2_aliases = gatherCapabilityAliases(vk, query_id2)
        assert len(member_2_aliases) == 2
        assert StructCapabilityAlias("VkPhysicalDeviceShaderSubgroupRotateFeaturesKHR", "shaderSubgroupRotate") in member_2_aliases
        assert StructCapabilityAlias("VkPhysicalDeviceVulkan14Features", "shaderSubgroupRotate") in member_2_aliases

        query_id3 = StructCapabilityAlias("VkPhysicalDeviceShaderSubgroupRotateFeaturesKHR", "shaderSubgroupRotate")
        member_3_aliases = gatherCapabilityAliases(vk, query_id3)
        assert len(member_3_aliases) == 2
        assert StructCapabilityAlias("VkPhysicalDeviceShaderSubgroupRotateFeatures", "shaderSubgroupRotate") in member_3_aliases
        assert StructCapabilityAlias("VkPhysicalDeviceVulkan14Features", "shaderSubgroupRotate") in member_3_aliases

        query_id4 = StructCapabilityAlias("VkPhysicalDeviceVulkan14Features", "shaderSubgroupRotate")
        member_4_aliases = gatherCapabilityAliases(vk, query_id4)
        assert len(member_4_aliases) == 2
        assert StructCapabilityAlias("VkPhysicalDeviceShaderSubgroupRotateFeatures", "shaderSubgroupRotate") in member_4_aliases
        assert StructCapabilityAlias("VkPhysicalDeviceShaderSubgroupRotateFeaturesKHR", "shaderSubgroupRotate") in member_4_aliases

        query_id5 = StructCapabilityAlias("VkPhysicalDeviceMutableDescriptorTypeFeaturesVALVE", "mutableDescriptorType")
        member_5_aliases = gatherCapabilityAliases(vk, query_id5)
        assert len(member_5_aliases) == 1
        assert StructCapabilityAlias("VkPhysicalDeviceMutableDescriptorTypeFeaturesEXT", "mutableDescriptorType") in member_5_aliases

        query_id6 = StructCapabilityAlias("VkPhysicalDeviceMutableDescriptorTypeFeaturesEXT", "mutableDescriptorType")
        member_6_aliases = gatherCapabilityAliases(vk, query_id6)
        assert len(member_6_aliases) == 1
        assert StructCapabilityAlias("VkPhysicalDeviceMutableDescriptorTypeFeaturesVALVE", "mutableDescriptorType") in member_6_aliases

        query_id7 = StructCapabilityAlias("VkPhysicalDeviceVulkan12Features", "subgroupBroadcastDynamicId")
        member_7_aliases = gatherCapabilityAliases(vk, query_id7)
        assert len(member_7_aliases) == 0

        query_id8 = StructCapabilityAlias("VkPhysicalDeviceCornerSampledImageFeaturesNV", "cornerSampledImage")
        member_8_aliases = gatherCapabilityAliases(vk, query_id8)
        assert len(member_8_aliases) == 0

        query_id9 = StructCapabilityAlias("VkPhysicalDeviceVulkan12Features", "samplerMirrorClampToEdge")
        member_9_aliases = gatherCapabilityAliases(vk, query_id9)
        assert len(member_9_aliases) == 1
        assert ExtensionCapabilityAlias("VK_KHR_sampler_mirror_clamp_to_edge") in member_9_aliases

        query_idA = ExtensionCapabilityAlias("VK_KHR_sampler_mirror_clamp_to_edge")
        member_A_aliases = gatherCapabilityAliases(vk, query_idA)
        assert len(member_A_aliases) == 1
        assert StructCapabilityAlias("VkPhysicalDeviceVulkan12Features", "samplerMirrorClampToEdge") in member_A_aliases

        query_idB = StructCapabilityAlias("VkUnknownFeatures", "globalPriorityQuery")
        member_B_aliases = gatherCapabilityAliases(vk, query_idB)
        assert member_B_aliases == []

        query_idC = StructCapabilityAlias("VkPhysicalDeviceGlobalPriorityQueryFeatures", "unknown")
        member_C_aliases = gatherCapabilityAliases(vk, query_idC)
        assert member_C_aliases == []

        query_idD = ExtensionCapabilityAlias("VK_LUNARG_unknown")
        member_D_aliases = gatherCapabilityAliases(vk, query_idD)
        assert member_D_aliases == []

    def testVulkanObjectUtilsStructPropertyAliasesAccess(self):
        vk: VulkanObject = initVulkanObject('vulkan', self.registry_path)

        query_id1 = StructCapabilityAlias("VkPhysicalDeviceLineRasterizationProperties", "lineSubPixelPrecisionBits")
        member_1_aliases = gatherCapabilityAliases(vk, query_id1)
        assert len(member_1_aliases) == 3
        assert StructCapabilityAlias("VkPhysicalDeviceLineRasterizationPropertiesKHR", "lineSubPixelPrecisionBits") in member_1_aliases
        assert StructCapabilityAlias("VkPhysicalDeviceLineRasterizationPropertiesEXT", "lineSubPixelPrecisionBits") in member_1_aliases
        assert StructCapabilityAlias("VkPhysicalDeviceVulkan14Properties", "lineSubPixelPrecisionBits") in member_1_aliases

        query_id2 = StructCapabilityAlias("VkPhysicalDeviceVulkan14Properties", "lineSubPixelPrecisionBits")
        member_2_aliases = gatherCapabilityAliases(vk, query_id2)
        assert len(member_2_aliases) == 3
        assert StructCapabilityAlias("VkPhysicalDeviceLineRasterizationPropertiesKHR", "lineSubPixelPrecisionBits") in member_2_aliases
        assert StructCapabilityAlias("VkPhysicalDeviceLineRasterizationPropertiesEXT", "lineSubPixelPrecisionBits") in member_2_aliases
        assert StructCapabilityAlias("VkPhysicalDeviceLineRasterizationProperties", "lineSubPixelPrecisionBits") in member_2_aliases

        query_id3 = StructCapabilityAlias("VkPhysicalDeviceLineRasterizationPropertiesKHR", "lineSubPixelPrecisionBits")
        member_3_aliases = gatherCapabilityAliases(vk, query_id3)
        assert len(member_3_aliases) == 3
        assert StructCapabilityAlias("VkPhysicalDeviceLineRasterizationPropertiesEXT", "lineSubPixelPrecisionBits") in member_3_aliases
        assert StructCapabilityAlias("VkPhysicalDeviceLineRasterizationProperties", "lineSubPixelPrecisionBits") in member_3_aliases
        assert StructCapabilityAlias("VkPhysicalDeviceVulkan14Properties", "lineSubPixelPrecisionBits") in member_3_aliases

        query_id3 = StructCapabilityAlias("VkPhysicalDeviceLineRasterizationPropertiesEXT", "lineSubPixelPrecisionBits")
        member_3_aliases = gatherCapabilityAliases(vk, query_id3)
        assert len(member_3_aliases) == 3
        assert StructCapabilityAlias("VkPhysicalDeviceLineRasterizationPropertiesKHR", "lineSubPixelPrecisionBits") in member_3_aliases
        assert StructCapabilityAlias("VkPhysicalDeviceLineRasterizationProperties", "lineSubPixelPrecisionBits") in member_3_aliases
        assert StructCapabilityAlias("VkPhysicalDeviceVulkan14Properties", "lineSubPixelPrecisionBits") in member_3_aliases

        query_id5 = StructCapabilityAlias("VkPhysicalDeviceVulkan13Properties", "maxBufferSize")
        member_5_aliases = gatherCapabilityAliases(vk, query_id5)
        assert len(member_5_aliases) == 0

        query_id6 = StructCapabilityAlias("VkPhysicalDeviceGpaPropertiesAMD", "shaderEngineCount")
        member_6_aliases = gatherCapabilityAliases(vk, query_id6)
        assert len(member_6_aliases) == 0

        query_id7 = StructCapabilityAlias("VkPhysicalDeviceVulkan11Properties", "subgroupQuadOperationsInAllStages")
        member_7_aliases = gatherCapabilityAliases(vk, query_id7)
        assert len(member_7_aliases) == 1
        assert StructCapabilityAlias("VkPhysicalDeviceSubgroupProperties", "quadOperationsInAllStages") in member_7_aliases

        query_id8 = StructCapabilityAlias("VkPhysicalDeviceSubgroupProperties", "quadOperationsInAllStages")
        member_8_aliases = gatherCapabilityAliases(vk, query_id8)
        assert len(member_8_aliases) == 1
        assert StructCapabilityAlias("VkPhysicalDeviceVulkan11Properties", "subgroupQuadOperationsInAllStages") in member_8_aliases

        query_idA = StructCapabilityAlias("VkPhysicalDeviceLineRasterizationPropertiesEXT", "lineSubPixelPrecisionBits")
        member_A_aliases = gatherCapabilityAliases(vk, query_idA)
        assert len(member_A_aliases) == 3
        assert StructCapabilityAlias("VkPhysicalDeviceLineRasterizationProperties", "lineSubPixelPrecisionBits") in member_A_aliases
        assert StructCapabilityAlias("VkPhysicalDeviceLineRasterizationPropertiesKHR", "lineSubPixelPrecisionBits") in member_A_aliases
        assert StructCapabilityAlias("VkPhysicalDeviceVulkan14Properties", "lineSubPixelPrecisionBits") in member_A_aliases

        query_idB = StructCapabilityAlias("VkUnknownProperty", "maxInlineUniformTotalSize")
        member_B_aliases = gatherCapabilityAliases(vk, query_idB)
        assert member_B_aliases == []

        query_idC = StructCapabilityAlias("VkPhysicalDeviceVertexAttributeDivisorProperties", "unknown")
        member_C_aliases = gatherCapabilityAliases(vk, query_idC)
        assert member_C_aliases == []

    def testFindExtensionVersion(self):
        vk: VulkanObject = initVulkanObject('vulkan', self.registry_path)

        extension_version0 = findExtensionVersion(vk, "VK_KHR_dynamic_rendering")
        self.assertEqual(extension_version0, 1)

        extension_version1 = findExtensionVersion(vk, "VK_EXT_debug_report")
        self.assertEqual(extension_version1, 10)

        extension_version2 = findExtensionVersion(vk, "VK_KHR_dedicated_allocation")
        self.assertEqual(extension_version2, 3)

    def testGatherDependentExtensions(self):
        self.maxDiff = 1024

        vk: VulkanObject = initVulkanObject('vulkan', self.registry_path)

        extensions_data = {
            "VK_KHR_dynamic_rendering": 1,
            "VK_EXT_debug_report": 10,
            "VK_KHR_dedicated_allocation": 3
        }

        dependent_extensions0 = gatherDependentExtensions(vk, VK_VERSION.V1_0, False, extensions_data)

        expected_extensions0 = {
            "VK_KHR_get_physical_device_properties2": 2,
            "VK_KHR_depth_stencil_resolve": 1,
            "VK_KHR_dynamic_rendering": 1,
            "VK_EXT_debug_report": 10,
            "VK_KHR_get_memory_requirements2": 1,
            "VK_KHR_dedicated_allocation": 3
        }

        self.assertEqual(dependent_extensions0, expected_extensions0)

        dependent_extensions1 = gatherDependentExtensions(vk, VK_VERSION.V1_1, False, extensions_data)

        expected_extensions1 = {
            "VK_KHR_depth_stencil_resolve": 1,
            "VK_KHR_dynamic_rendering": 1,
            "VK_EXT_debug_report": 10,
            "VK_KHR_dedicated_allocation": 3
        }

        self.assertEqual(dependent_extensions1, expected_extensions1)

        extensions_data2 = {
            "VK_KHR_load_store_op_none": 1,
            "VK_KHR_maintenance6": 1,
            "VK_KHR_map_memory2": 1,
            "VK_KHR_shader_expect_assume": 1,
            "VK_KHR_shader_float_controls2": 1,
            "VK_KHR_shader_maximal_reconvergence": 1,
            "VK_KHR_shader_subgroup_rotate": 1,
            "VK_KHR_shader_subgroup_uniform_control_flow": 1,
            "VK_KHR_swapchain_mutable_format": 1,
            "VK_EXT_host_image_copy": 1,
            "VK_EXT_image_2d_view_of_3d": 1,
            "VK_EXT_pipeline_protected_access": 1,
            "VK_EXT_pipeline_robustness": 1,
            "VK_EXT_transform_feedback": 1
        }

        dependent_extensions2 = gatherDependentExtensions(vk, VK_VERSION.V1_3, False, extensions_data2)

        expected_extensions2 = {
            "VK_KHR_load_store_op_none": 1,
            "VK_KHR_maintenance6": 1,
            "VK_KHR_map_memory2": 1,
            "VK_KHR_shader_expect_assume": 1,
            "VK_KHR_shader_float_controls": 4,
            "VK_KHR_shader_float_controls2": 1,
            "VK_KHR_shader_maximal_reconvergence": 1,
            "VK_KHR_shader_subgroup_rotate": 2,
            "VK_KHR_shader_subgroup_uniform_control_flow": 1,
            "VK_KHR_swapchain": 70,
            "VK_KHR_swapchain_mutable_format": 1,
            "VK_EXT_host_image_copy": 1,
            "VK_EXT_image_2d_view_of_3d": 1,
            "VK_EXT_pipeline_protected_access": 1,
            "VK_EXT_pipeline_robustness": 1,
            "VK_EXT_transform_feedback": 1
        }

        self.assertEqual(dependent_extensions2, expected_extensions2)

        extensions_data3 = {
            "VK_KHR_load_store_op_none": 1,
            "VK_KHR_maintenance6": 1,
            "VK_KHR_map_memory2": 1,
            "VK_KHR_shader_expect_assume": 1,
            "VK_KHR_shader_float_controls2": 1,
            "VK_KHR_shader_maximal_reconvergence": 1,
            "VK_KHR_shader_subgroup_rotate": 2,
            "VK_KHR_shader_subgroup_uniform_control_flow": 1,
            "VK_KHR_swapchain_mutable_format": 1,
            "VK_EXT_host_image_copy": 1,
            "VK_EXT_image_2d_view_of_3d": 1,
            "VK_EXT_pipeline_protected_access": 1,
            "VK_EXT_pipeline_robustness": 1,
            "VK_EXT_transform_feedback": 1
        }

        dependent_extensions3 = gatherDependentExtensions(vk, VK_VERSION.V1_3, True, extensions_data3)

        expected_extensions3 = {
            "VK_KHR_load_store_op_none": 1,
            "VK_KHR_maintenance6": 1,
            "VK_KHR_map_memory2": 1,
            "VK_KHR_shader_expect_assume": 1,
            "VK_KHR_shader_float_controls": 1,
            "VK_KHR_shader_float_controls2": 1,
            "VK_KHR_shader_maximal_reconvergence": 1,
            "VK_KHR_shader_subgroup_rotate": 1,
            "VK_KHR_shader_subgroup_uniform_control_flow": 1,
            "VK_KHR_swapchain": 1,
            "VK_KHR_swapchain_mutable_format": 1,
            "VK_EXT_host_image_copy": 1,
            "VK_EXT_image_2d_view_of_3d": 1,
            "VK_EXT_pipeline_protected_access": 1,
            "VK_EXT_pipeline_robustness": 1,
            "VK_EXT_transform_feedback": 1
        }

        self.assertEqual(dependent_extensions3, expected_extensions3)

    def testGatherDynamicStructs(self):
        vk: VulkanObject = initVulkanObject('vulkan', self.registry_path)

        dynamic_structs = gatherDynamicStructs(vk)

        self.assertIsInstance(dynamic_structs, list)
        self.assertIn("VkPhysicalDeviceHostImageCopyProperties", dynamic_structs)
        self.assertIn("VkPhysicalDeviceLayeredApiPropertiesListKHR", dynamic_structs)
        self.assertIn("VkPhysicalDeviceGpaPropertiesAMD", dynamic_structs)
        self.assertNotIn("VkPhysicalDeviceFeatures2", dynamic_structs)
        self.assertNotIn("VkPhysicalDeviceProperties2", dynamic_structs)
        self.assertNotIn("VkPhysicalDeviceVulkan11Properties", dynamic_structs)

    def testGatherPromotedExtensionsForVersion(self):
        vk: VulkanObject = initVulkanObject('vulkan', self.registry_path)

        none_promoted = gatherPromotedExtensionsForVersion(vk, VK_VERSION.NONE)
        self.assertEqual(none_promoted, {})

        v1_0_promoted = gatherPromotedExtensionsForVersion(vk, VK_VERSION.V1_0)
        self.assertEqual(v1_0_promoted, {})

        v1_1_promoted = gatherPromotedExtensionsForVersion(vk, VK_VERSION.V1_1)
        self.assertIn("VK_KHR_multiview", v1_1_promoted)
        self.assertIn("VK_KHR_maintenance1", v1_1_promoted)
        self.assertNotIn("VK_KHR_dynamic_rendering", v1_1_promoted)
        self.assertEqual(v1_1_promoted["VK_KHR_multiview"], findExtensionVersion(vk, "VK_KHR_multiview"))

        v1_3_promoted = gatherPromotedExtensionsForVersion(vk, VK_VERSION.V1_3)
        self.assertIn("VK_KHR_multiview", v1_3_promoted)
        self.assertIn("VK_KHR_driver_properties", v1_3_promoted)
        self.assertIn("VK_KHR_dynamic_rendering", v1_3_promoted)
        self.assertIn("VK_KHR_format_feature_flags2", v1_3_promoted)
        self.assertEqual(v1_3_promoted["VK_KHR_dynamic_rendering"], findExtensionVersion(vk, "VK_KHR_dynamic_rendering"))

    def testGatherPromotedExtensionsForExactVersion(self):
        vk: VulkanObject = initVulkanObject('vulkan', self.registry_path)

        none_promoted = gatherPromotedExtensionsForExactVersion(vk, VK_VERSION.NONE)
        self.assertEqual(none_promoted, {})

        v1_1_promoted = gatherPromotedExtensionsForExactVersion(vk, VK_VERSION.V1_1)
        self.assertIn("VK_KHR_multiview", v1_1_promoted)
        self.assertIn("VK_KHR_maintenance1", v1_1_promoted)
        self.assertNotIn("VK_KHR_driver_properties", v1_1_promoted)

        v1_2_promoted = gatherPromotedExtensionsForExactVersion(vk, VK_VERSION.V1_2)
        self.assertIn("VK_KHR_driver_properties", v1_2_promoted)
        self.assertNotIn("VK_KHR_multiview", v1_2_promoted)
        self.assertNotIn("VK_KHR_dynamic_rendering", v1_2_promoted)

    def testIsStructExtensionEnabled(self):
        vk: VulkanObject = initVulkanObject('vulkan', self.registry_path)

        self.assertTrue(isStructExtensionEnabled(vk, "VkPhysicalDeviceFeatures", VK_VERSION.V1_0, set()))
        self.assertFalse(isStructExtensionEnabled(vk, "VkPhysicalDeviceDynamicRenderingFeatures", VK_VERSION.V1_1, set()))

        exts = {"VK_KHR_dynamic_rendering"}
        self.assertTrue(isStructExtensionEnabled(vk, "VkPhysicalDeviceDynamicRenderingFeatures", VK_VERSION.V1_1, exts))
        self.assertTrue(isStructExtensionEnabled(vk, "VkPhysicalDeviceDynamicRenderingFeatures", VK_VERSION.V1_3, set()))

    def testGatherRequiredFeaturesForVersion(self):
        vk: VulkanObject = initVulkanObject('vulkan', self.registry_path)

        v1_0_features = gatherRequiredFeaturesForVersion(vk, VK_VERSION.V1_0)
        self.assertEqual(v1_0_features, {})

        v1_2_features = gatherRequiredFeaturesForVersion(vk, VK_VERSION.V1_2)

        if "VkPhysicalDeviceVulkan12Features" in v1_2_features:
            self.assertIn("timelineSemaphore", v1_2_features["VkPhysicalDeviceVulkan12Features"])
            self.assertTrue(v1_2_features["VkPhysicalDeviceVulkan12Features"]["timelineSemaphore"])

    def testGatherSatisfiedRequiredFeaturesWithDepends(self):
        vk: VulkanObject = initVulkanObject('vulkan', self.registry_path)

        # Case 1: Core 1.2 feature evaluation on Vulkan 1.1 API returns empty dict
        features_v11_no_ext = gatherSatisfiedCoreRequiredFeaturesForVersion(
            vk, VK_VERSION.V1_2, VK_VERSION.V1_1, enabled_exts=set(), enabled_features=set()
        )
        v12_features_v11 = features_v11_no_ext.get("VkPhysicalDeviceVulkan12Features", {})
        self.assertNotIn("samplerMirrorClampToEdge", v12_features_v11)

        # Case 2: Core 1.2 feature evaluation on Vulkan 1.2 API with extension enabled gathers samplerMirrorClampToEdge
        exts_with_clamp = {"VK_KHR_sampler_mirror_clamp_to_edge"}
        features_v12_ext = gatherSatisfiedCoreRequiredFeaturesForVersion(
            vk, VK_VERSION.V1_2, VK_VERSION.V1_2, enabled_exts=exts_with_clamp, enabled_features=set()
        )
        v12_features_ext = features_v12_ext.get("VkPhysicalDeviceVulkan12Features", {})
        self.assertIn("samplerMirrorClampToEdge", v12_features_ext)
        self.assertTrue(v12_features_ext["samplerMirrorClampToEdge"])

        # Case 3: Core 1.2 feature evaluation on Vulkan 1.2 API gathers samplerMirrorClampToEdge due to core promotion
        features_v12 = gatherSatisfiedCoreRequiredFeaturesForVersion(
            vk, VK_VERSION.V1_2, VK_VERSION.V1_2, enabled_exts=set(), enabled_features=set()
        )
        v12_features_v12 = features_v12.get("VkPhysicalDeviceVulkan12Features", {})
        self.assertIn("samplerMirrorClampToEdge", v12_features_v12)
        self.assertTrue(v12_features_v12["samplerMirrorClampToEdge"])

    def testGatherSatisfiedRequiredFeaturesWithFeatureDepends(self):
        vk: VulkanObject = initVulkanObject('vulkan', self.registry_path)

        # Case 1: Neither atomic Int64 feature is enabled -> shaderInt64 is NOT gathered
        features_none = gatherSatisfiedCoreRequiredFeaturesForVersion(
            vk, VK_VERSION.V1_2, VK_VERSION.V1_2, enabled_exts=set(), enabled_features=set()
        )
        base_features_none = features_none.get("VkPhysicalDeviceFeatures", {})
        self.assertNotIn("shaderInt64", base_features_none)

        # Case 2: Enable shaderBufferInt64Atomics -> shaderInt64 IS gathered
        input_buffer_atomic = {("VkPhysicalDeviceShaderAtomicInt64Features", "shaderBufferInt64Atomics")}
        features_buffer = gatherSatisfiedCoreRequiredFeaturesForVersion(
            vk, VK_VERSION.V1_2, VK_VERSION.V1_2, enabled_exts=set(), enabled_features=input_buffer_atomic
        )
        base_features_buffer = features_buffer.get("VkPhysicalDeviceFeatures", {})
        self.assertIn("shaderInt64", base_features_buffer)
        self.assertTrue(base_features_buffer["shaderInt64"])

        # Case 3: Enable shaderSharedInt64Atomics -> shaderInt64 IS gathered
        input_shared_atomic = {("VkPhysicalDeviceShaderAtomicInt64Features", "shaderSharedInt64Atomics")}
        features_shared = gatherSatisfiedCoreRequiredFeaturesForVersion(
            vk, VK_VERSION.V1_2, VK_VERSION.V1_2, enabled_exts=set(), enabled_features=input_shared_atomic
        )
        base_features_shared = features_shared.get("VkPhysicalDeviceFeatures", {})
        self.assertIn("shaderInt64", base_features_shared)
        self.assertTrue(base_features_shared["shaderInt64"])


if __name__ == '__main__':
    parser = argparse.ArgumentParser()

    parser.add_argument('--registry', '-r', action='store', required=False,
                        help='Use specified registry file instead of vk.xml.')

    args, unparsed = parser.parse_known_args()
    TestVulkanObjectUtils.registry_path = args.registry

    unittest.main(argv=[sys.argv[0]] + unparsed)
    