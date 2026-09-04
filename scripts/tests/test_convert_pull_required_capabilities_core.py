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
import json
from pathlib import Path
import sys
import unittest

scripts_dir = Path(__file__).resolve().parent.parent
if str(scripts_dir) not in sys.path:
    sys.path.insert(0, str(scripts_dir))

from vulkan_object import VulkanObject
from source.vulkan_object_utils import initVulkanObject
from source.main_convert import pull_required_capabilities_profiles_files


class TestConvertPullRequiredCapabilitiesCore(unittest.TestCase):
    registry_path = None

    def setUp(self):
        self.vk: VulkanObject = initVulkanObject('vulkan', self.registry_path)

    def assertProfileDataEqual(self, gen_data: dict, exp_data: dict):
        self.assertEqual(
            list(gen_data.keys()),
            list(exp_data.keys()),
            f"Top-level keys mismatch: {list(gen_data.keys())} vs {list(exp_data.keys())}"
        )

        if "profiles" in exp_data:
            self.assertEqual(
                list(gen_data.get("profiles", {}).keys()),
                list(exp_data["profiles"].keys()),
                "Profile name list mismatch"
            )
            for prof_name, exp_prof in exp_data["profiles"].items():
                gen_prof = gen_data["profiles"].get(prof_name, {})
                self.assertEqual(
                    gen_prof.get("capabilities"),
                    exp_prof.get("capabilities"),
                    f"Capabilities block reference list mismatch in profile '{prof_name}'"
                )

        if "capabilities" in exp_data:
            self.assertEqual(
                list(gen_data.get("capabilities", {}).keys()),
                list(exp_data["capabilities"].keys()),
                "Capability block name list mismatch"
            )

            for cap_name, exp_block in exp_data["capabilities"].items():
                gen_block = gen_data["capabilities"].get(cap_name, {})

                self.assertEqual(
                    list(gen_block.keys()),
                    list(exp_block.keys()),
                    f"Section mismatch in capability block '{cap_name}'"
                )

                for section in ("features", "properties"):
                    if section in exp_block:
                        exp_section = exp_block[section]
                        gen_section = gen_block.get(section, {})

                        self.assertEqual(
                            list(gen_section.keys()),
                            list(exp_section.keys()),
                            f"Structure name list mismatch in capability block '{cap_name}', section '{section}'"
                        )

                        for struct_name, exp_struct in exp_section.items():
                            gen_struct = gen_section.get(struct_name, {})
                            if isinstance(exp_struct, dict):
                                self.assertEqual(
                                    list(gen_struct.keys()),
                                    list(exp_struct.keys()),
                                    f"Member key list mismatch in '{cap_name}.{section}.{struct_name}'"
                                )
                                for member_name, exp_val in exp_struct.items():
                                    gen_val = gen_struct.get(member_name)
                                    self.assertEqual(
                                        gen_val,
                                        exp_val,
                                        f"Value mismatch in '{cap_name}.{section}.{struct_name}.{member_name}'"
                                    )
                            else:
                                self.assertEqual(
                                    gen_struct,
                                    exp_struct,
                                    f"Value mismatch in '{cap_name}.{section}.{struct_name}'"
                                )

        self.assertEqual(gen_data, exp_data)

    def test_pull_required_capabilities_vulkan10(self):
        original_json_text = """{
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8.0-106.json#",
            "profiles": {
                "VP_TEST_profile_v10": {
                    "version": 1,
                    "api-version": "1.0.68",
                    "capabilities": ["baseline"]
                }
            },
            "capabilities": {
                "baseline": {}
            }
        }"""

        expected_json_text = """{
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8.0-106.json#",
            "profiles": {
                "VP_TEST_profile_v10": {
                    "version": 1,
                    "api-version": "1.0.68",
                    "capabilities": ["baseline"]
                }
            },
            "capabilities": {
                "baseline": {
                    "features": {
                        "VkPhysicalDeviceFeatures": {
                            "robustBufferAccess": true
                        }
                    },
                    "properties": {
                        "VkPhysicalDeviceProperties": {
                            "limits": {
                                "maxImageDimension1D": 4096,
                                "maxImageDimension2D": 4096,
                                "maxImageDimension3D": 256,
                                "maxImageDimensionCube": 4096,
                                "maxImageArrayLayers": 256,
                                "maxTexelBufferElements": 65536,
                                "maxUniformBufferRange": 16384,
                                "maxStorageBufferRange": 134217728,
                                "maxPushConstantsSize": 128,
                                "maxMemoryAllocationCount": 4096,
                                "maxSamplerAllocationCount": 4000,
                                "bufferImageGranularity": 131072,
                                "sparseAddressSpaceSize": 0,
                                "maxBoundDescriptorSets": 4,
                                "maxPerStageDescriptorSamplers": 16,
                                "maxPerStageDescriptorUniformBuffers": 12,
                                "maxPerStageDescriptorStorageBuffers": 4,
                                "maxPerStageDescriptorSampledImages": 16,
                                "maxPerStageDescriptorStorageImages": 4,
                                "maxPerStageDescriptorInputAttachments": 4,
                                "maxPerStageResources": 128,
                                "maxDescriptorSetSamplers": 96,
                                "maxDescriptorSetUniformBuffers": 72,
                                "maxDescriptorSetUniformBuffersDynamic": 8,
                                "maxDescriptorSetStorageBuffers": 24,
                                "maxDescriptorSetStorageBuffersDynamic": 4,
                                "maxDescriptorSetSampledImages": 96,
                                "maxDescriptorSetStorageImages": 24,
                                "maxDescriptorSetInputAttachments": 4,
                                "maxVertexInputAttributes": 16,
                                "maxVertexInputBindings": 16,
                                "maxVertexInputAttributeOffset": 2047,
                                "maxVertexInputBindingStride": 2048,
                                "maxVertexOutputComponents": 64,
                                "maxTessellationGenerationLevel": 0,
                                "maxTessellationPatchSize": 0,
                                "maxTessellationControlPerVertexInputComponents": 0,
                                "maxTessellationControlPerVertexOutputComponents": 0,
                                "maxTessellationControlPerPatchOutputComponents": 0,
                                "maxTessellationControlTotalOutputComponents": 0,
                                "maxTessellationEvaluationInputComponents": 0,
                                "maxTessellationEvaluationOutputComponents": 0,
                                "maxGeometryShaderInvocations": 0,
                                "maxGeometryInputComponents": 0,
                                "maxGeometryOutputComponents": 0,
                                "maxGeometryOutputVertices": 0,
                                "maxGeometryTotalOutputComponents": 0,
                                "maxFragmentInputComponents": 64,
                                "maxFragmentOutputAttachments": 4,
                                "maxFragmentDualSrcAttachments": 0,
                                "maxFragmentCombinedOutputResources": 4,
                                "maxComputeSharedMemorySize": 16384,
                                "maxComputeWorkGroupCount": [65535, 65535, 65535],
                                "maxComputeWorkGroupInvocations": 128,
                                "maxComputeWorkGroupSize": [128, 128, 64],
                                "subPixelPrecisionBits": 4,
                                "subTexelPrecisionBits": 4,
                                "mipmapPrecisionBits": 4,
                                "maxDrawIndexedIndexValue": 16777216,
                                "maxDrawIndirectCount": 1,
                                "maxSamplerLodBias": 2,
                                "maxSamplerAnisotropy": 1,
                                "maxViewports": 1,
                                "maxViewportDimensions": [4096, 4096],
                                "viewportBoundsRange": [-8192, 8192],
                                "viewportSubPixelBits": 0,
                                "minMemoryMapAlignment": 64,
                                "minTexelBufferOffsetAlignment": 256,
                                "minUniformBufferOffsetAlignment": 256,
                                "minStorageBufferOffsetAlignment": 256,
                                "minTexelOffset": -8,
                                "maxTexelOffset": 7,
                                "minTexelGatherOffset": -8,
                                "maxTexelGatherOffset": 7,
                                "minInterpolationOffset": 0.0,
                                "maxInterpolationOffset": 0.0,
                                "subPixelInterpolationOffsetBits": 0,
                                "maxFramebufferWidth": 4096,
                                "maxFramebufferHeight": 4096,
                                "maxFramebufferLayers": 256,
                                "framebufferColorSampleCounts": ["VK_SAMPLE_COUNT_1_BIT", "VK_SAMPLE_COUNT_4_BIT"],
                                "framebufferDepthSampleCounts": ["VK_SAMPLE_COUNT_1_BIT", "VK_SAMPLE_COUNT_4_BIT"],
                                "framebufferStencilSampleCounts": ["VK_SAMPLE_COUNT_1_BIT", "VK_SAMPLE_COUNT_4_BIT"],
                                "framebufferNoAttachmentsSampleCounts": ["VK_SAMPLE_COUNT_1_BIT", "VK_SAMPLE_COUNT_4_BIT"],
                                "maxColorAttachments": 4,
                                "sampledImageColorSampleCounts": ["VK_SAMPLE_COUNT_1_BIT", "VK_SAMPLE_COUNT_4_BIT"],
                                "sampledImageIntegerSampleCounts": ["VK_SAMPLE_COUNT_1_BIT", "VK_SAMPLE_COUNT_4_BIT"],
                                "sampledImageDepthSampleCounts": ["VK_SAMPLE_COUNT_1_BIT", "VK_SAMPLE_COUNT_4_BIT"],
                                "sampledImageStencilSampleCounts": ["VK_SAMPLE_COUNT_1_BIT", "VK_SAMPLE_COUNT_4_BIT"],
                                "storageImageSampleCounts": ["VK_SAMPLE_COUNT_1_BIT"],
                                "maxSampleMaskWords": 1,
                                "maxClipDistances": 0,
                                "maxCullDistances": 0,
                                "maxCombinedClipAndCullDistances": 0,
                                "discreteQueuePriorities": 2,
                                "pointSizeRange": [1.0, 1.0],
                                "lineWidthRange": [1.0, 1.0],
                                "pointSizeGranularity": 1.0,
                                "lineWidthGranularity": 1.0,
                                "nonCoherentAtomSize": 256
                            },
                            "sparseProperties": {
                                "residencyStandard2DBlockShape": false,
                                "residencyStandard2DMultisampleBlockShape": false,
                                "residencyStandard3DBlockShape": false,
                                "residencyNonResidentStrict": false
                            }
                        }
                    }
                }
            }
        }"""

        json_files_dict = {"test_profile.json": json.loads(original_json_text)}
        pull_required_capabilities_profiles_files(self.vk, json_files_dict)
        self.assertProfileDataEqual(json_files_dict["test_profile.json"], json.loads(expected_json_text))

    def test_pull_required_capabilities_vulkan11(self):
        original_json_text = """{
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8.0-106.json#",
            "profiles": {
                "VP_TEST_profile_v11": {
                    "version": 1,
                    "api-version": "1.1.108",
                    "capabilities": ["baseline"]
                }
            },
            "capabilities": {
                "baseline": {}
            }
        }"""

        expected_json_text = """{
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8.0-106.json#",
            "profiles": {
                "VP_TEST_profile_v11": {
                    "version": 1,
                    "api-version": "1.1.108",
                    "capabilities": ["baseline"]
                }
            },
            "capabilities": {
                "baseline": {
                    "features": {
                        "VkPhysicalDeviceFeatures": {
                            "robustBufferAccess": true
                        },
                        "VkPhysicalDeviceMultiviewFeatures": {
                            "multiview": true
                        }
                    },
                    "properties": {
                        "VkPhysicalDeviceProperties": {
                            "limits": {
                                "maxImageDimension1D": 4096,
                                "maxImageDimension2D": 4096,
                                "maxImageDimension3D": 256,
                                "maxImageDimensionCube": 4096,
                                "maxImageArrayLayers": 256,
                                "maxTexelBufferElements": 65536,
                                "maxUniformBufferRange": 16384,
                                "maxStorageBufferRange": 134217728,
                                "maxPushConstantsSize": 128,
                                "maxMemoryAllocationCount": 4096,
                                "maxSamplerAllocationCount": 4000,
                                "bufferImageGranularity": 131072,
                                "sparseAddressSpaceSize": 0,
                                "maxBoundDescriptorSets": 4,
                                "maxPerStageDescriptorSamplers": 16,
                                "maxPerStageDescriptorUniformBuffers": 12,
                                "maxPerStageDescriptorStorageBuffers": 4,
                                "maxPerStageDescriptorSampledImages": 16,
                                "maxPerStageDescriptorStorageImages": 4,
                                "maxPerStageDescriptorInputAttachments": 4,
                                "maxPerStageResources": 128,
                                "maxDescriptorSetSamplers": 96,
                                "maxDescriptorSetUniformBuffers": 72,
                                "maxDescriptorSetUniformBuffersDynamic": 8,
                                "maxDescriptorSetStorageBuffers": 24,
                                "maxDescriptorSetStorageBuffersDynamic": 4,
                                "maxDescriptorSetSampledImages": 96,
                                "maxDescriptorSetStorageImages": 24,
                                "maxDescriptorSetInputAttachments": 4,
                                "maxVertexInputAttributes": 16,
                                "maxVertexInputBindings": 16,
                                "maxVertexInputAttributeOffset": 2047,
                                "maxVertexInputBindingStride": 2048,
                                "maxVertexOutputComponents": 64,
                                "maxTessellationGenerationLevel": 0,
                                "maxTessellationPatchSize": 0,
                                "maxTessellationControlPerVertexInputComponents": 0,
                                "maxTessellationControlPerVertexOutputComponents": 0,
                                "maxTessellationControlPerPatchOutputComponents": 0,
                                "maxTessellationControlTotalOutputComponents": 0,
                                "maxTessellationEvaluationInputComponents": 0,
                                "maxTessellationEvaluationOutputComponents": 0,
                                "maxGeometryShaderInvocations": 0,
                                "maxGeometryInputComponents": 0,
                                "maxGeometryOutputComponents": 0,
                                "maxGeometryOutputVertices": 0,
                                "maxGeometryTotalOutputComponents": 0,
                                "maxFragmentInputComponents": 64,
                                "maxFragmentOutputAttachments": 4,
                                "maxFragmentDualSrcAttachments": 0,
                                "maxFragmentCombinedOutputResources": 4,
                                "maxComputeSharedMemorySize": 16384,
                                "maxComputeWorkGroupCount": [65535, 65535, 65535],
                                "maxComputeWorkGroupInvocations": 128,
                                "maxComputeWorkGroupSize": [128, 128, 64],
                                "subPixelPrecisionBits": 4,
                                "subTexelPrecisionBits": 4,
                                "mipmapPrecisionBits": 4,
                                "maxDrawIndexedIndexValue": 16777216,
                                "maxDrawIndirectCount": 1,
                                "maxSamplerLodBias": 2,
                                "maxSamplerAnisotropy": 1,
                                "maxViewports": 1,
                                "maxViewportDimensions": [4096, 4096],
                                "viewportBoundsRange": [-8192, 8192],
                                "viewportSubPixelBits": 0,
                                "minMemoryMapAlignment": 64,
                                "minTexelBufferOffsetAlignment": 256,
                                "minUniformBufferOffsetAlignment": 256,
                                "minStorageBufferOffsetAlignment": 256,
                                "minTexelOffset": -8,
                                "maxTexelOffset": 7,
                                "minTexelGatherOffset": -8,
                                "maxTexelGatherOffset": 7,
                                "minInterpolationOffset": 0.0,
                                "maxInterpolationOffset": 0.0,
                                "subPixelInterpolationOffsetBits": 0,
                                "maxFramebufferWidth": 4096,
                                "maxFramebufferHeight": 4096,
                                "maxFramebufferLayers": 256,
                                "framebufferColorSampleCounts": ["VK_SAMPLE_COUNT_1_BIT", "VK_SAMPLE_COUNT_4_BIT"],
                                "framebufferDepthSampleCounts": ["VK_SAMPLE_COUNT_1_BIT", "VK_SAMPLE_COUNT_4_BIT"],
                                "framebufferStencilSampleCounts": ["VK_SAMPLE_COUNT_1_BIT", "VK_SAMPLE_COUNT_4_BIT"],
                                "framebufferNoAttachmentsSampleCounts": ["VK_SAMPLE_COUNT_1_BIT", "VK_SAMPLE_COUNT_4_BIT"],
                                "maxColorAttachments": 4,
                                "sampledImageColorSampleCounts": ["VK_SAMPLE_COUNT_1_BIT", "VK_SAMPLE_COUNT_4_BIT"],
                                "sampledImageIntegerSampleCounts": ["VK_SAMPLE_COUNT_1_BIT", "VK_SAMPLE_COUNT_4_BIT"],
                                "sampledImageDepthSampleCounts": ["VK_SAMPLE_COUNT_1_BIT", "VK_SAMPLE_COUNT_4_BIT"],
                                "sampledImageStencilSampleCounts": ["VK_SAMPLE_COUNT_1_BIT", "VK_SAMPLE_COUNT_4_BIT"],
                                "storageImageSampleCounts": ["VK_SAMPLE_COUNT_1_BIT"],
                                "maxSampleMaskWords": 1,
                                "maxClipDistances": 0,
                                "maxCullDistances": 0,
                                "maxCombinedClipAndCullDistances": 0,
                                "discreteQueuePriorities": 2,
                                "pointSizeRange": [1.0, 1.0],
                                "lineWidthRange": [1.0, 1.0],
                                "pointSizeGranularity": 1.0,
                                "lineWidthGranularity": 1.0,
                                "nonCoherentAtomSize": 256
                            },
                            "sparseProperties": {
                                "residencyStandard2DBlockShape": false,
                                "residencyStandard2DMultisampleBlockShape": false,
                                "residencyStandard3DBlockShape": false,
                                "residencyNonResidentStrict": false
                            }
                        },
                        "VkPhysicalDeviceSubgroupProperties": {
                            "subgroupSize": 1,
                            "supportedStages": ["VK_SHADER_STAGE_COMPUTE_BIT"],
                            "supportedOperations": ["VK_SUBGROUP_FEATURE_BASIC_BIT"]
                        },
                        "VkPhysicalDeviceMultiviewProperties": {
                            "maxMultiviewViewCount": 6,
                            "maxMultiviewInstanceIndex": 134217727
                        },
                        "VkPhysicalDeviceMaintenance3Properties": {
                            "maxPerSetDescriptors": 1024,
                            "maxMemoryAllocationSize": 1073741824
                        }
                    }
                }
            }
        }"""

        json_files_dict = {"test_profile.json": json.loads(original_json_text)}
        pull_required_capabilities_profiles_files(self.vk, json_files_dict)
        self.assertProfileDataEqual(json_files_dict["test_profile.json"], json.loads(expected_json_text))

    def test_pull_required_capabilities_vulkan12(self):
        original_json_text = """{
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8.0-131.json#",
            "profiles": {
                "VP_TEST_profile_v12": {
                    "version": 1,
                    "api-version": "1.2.131",
                    "capabilities": ["baseline"]
                }
            },
            "capabilities": {
                "baseline": {}
            }
        }"""

        expected_json_text = """{
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8.0-131.json#",
            "profiles": {
                "VP_TEST_profile_v12": {
                    "version": 1,
                    "api-version": "1.2.131",
                    "capabilities": ["baseline"]
                }
            },
            "capabilities": {
                "baseline": {
                    "features": {
                        "VkPhysicalDeviceFeatures": {
                            "robustBufferAccess": true
                        },
                        "VkPhysicalDeviceVulkan11Features": {
                            "multiview": true
                        },
                        "VkPhysicalDeviceVulkan12Features": {
                            "subgroupBroadcastDynamicId": true,
                            "imagelessFramebuffer": true,
                            "uniformBufferStandardLayout": true,
                            "shaderSubgroupExtendedTypes": true,
                            "separateDepthStencilLayouts": true,
                            "hostQueryReset": true,
                            "timelineSemaphore": true
                        }
                    },
                    "properties": {
                        "VkPhysicalDeviceProperties": {
                            "limits": {
                                "maxImageDimension1D": 4096,
                                "maxImageDimension2D": 4096,
                                "maxImageDimension3D": 256,
                                "maxImageDimensionCube": 4096,
                                "maxImageArrayLayers": 256,
                                "maxTexelBufferElements": 65536,
                                "maxUniformBufferRange": 16384,
                                "maxStorageBufferRange": 134217728,
                                "maxPushConstantsSize": 128,
                                "maxMemoryAllocationCount": 4096,
                                "maxSamplerAllocationCount": 4000,
                                "bufferImageGranularity": 131072,
                                "sparseAddressSpaceSize": 0,
                                "maxBoundDescriptorSets": 4,
                                "maxPerStageDescriptorSamplers": 16,
                                "maxPerStageDescriptorUniformBuffers": 12,
                                "maxPerStageDescriptorStorageBuffers": 4,
                                "maxPerStageDescriptorSampledImages": 16,
                                "maxPerStageDescriptorStorageImages": 4,
                                "maxPerStageDescriptorInputAttachments": 4,
                                "maxPerStageResources": 128,
                                "maxDescriptorSetSamplers": 96,
                                "maxDescriptorSetUniformBuffers": 72,
                                "maxDescriptorSetUniformBuffersDynamic": 8,
                                "maxDescriptorSetStorageBuffers": 24,
                                "maxDescriptorSetStorageBuffersDynamic": 4,
                                "maxDescriptorSetSampledImages": 96,
                                "maxDescriptorSetStorageImages": 24,
                                "maxDescriptorSetInputAttachments": 4,
                                "maxVertexInputAttributes": 16,
                                "maxVertexInputBindings": 16,
                                "maxVertexInputAttributeOffset": 2047,
                                "maxVertexInputBindingStride": 2048,
                                "maxVertexOutputComponents": 64,
                                "maxTessellationGenerationLevel": 0,
                                "maxTessellationPatchSize": 0,
                                "maxTessellationControlPerVertexInputComponents": 0,
                                "maxTessellationControlPerVertexOutputComponents": 0,
                                "maxTessellationControlPerPatchOutputComponents": 0,
                                "maxTessellationControlTotalOutputComponents": 0,
                                "maxTessellationEvaluationInputComponents": 0,
                                "maxTessellationEvaluationOutputComponents": 0,
                                "maxGeometryShaderInvocations": 0,
                                "maxGeometryInputComponents": 0,
                                "maxGeometryOutputComponents": 0,
                                "maxGeometryOutputVertices": 0,
                                "maxGeometryTotalOutputComponents": 0,
                                "maxFragmentInputComponents": 64,
                                "maxFragmentOutputAttachments": 4,
                                "maxFragmentDualSrcAttachments": 0,
                                "maxFragmentCombinedOutputResources": 4,
                                "maxComputeSharedMemorySize": 16384,
                                "maxComputeWorkGroupCount": [65535, 65535, 65535],
                                "maxComputeWorkGroupInvocations": 128,
                                "maxComputeWorkGroupSize": [128, 128, 64],
                                "subPixelPrecisionBits": 4,
                                "subTexelPrecisionBits": 4,
                                "mipmapPrecisionBits": 4,
                                "maxDrawIndexedIndexValue": 16777216,
                                "maxDrawIndirectCount": 1,
                                "maxSamplerLodBias": 2,
                                "maxSamplerAnisotropy": 1,
                                "maxViewports": 1,
                                "maxViewportDimensions": [4096, 4096],
                                "viewportBoundsRange": [-8192, 8192],
                                "viewportSubPixelBits": 0,
                                "minMemoryMapAlignment": 64,
                                "minTexelBufferOffsetAlignment": 256,
                                "minUniformBufferOffsetAlignment": 256,
                                "minStorageBufferOffsetAlignment": 256,
                                "minTexelOffset": -8,
                                "maxTexelOffset": 7,
                                "minTexelGatherOffset": -8,
                                "maxTexelGatherOffset": 7,
                                "minInterpolationOffset": 0.0,
                                "maxInterpolationOffset": 0.0,
                                "subPixelInterpolationOffsetBits": 0,
                                "maxFramebufferWidth": 4096,
                                "maxFramebufferHeight": 4096,
                                "maxFramebufferLayers": 256,
                                "framebufferColorSampleCounts": ["VK_SAMPLE_COUNT_1_BIT", "VK_SAMPLE_COUNT_4_BIT"],
                                "framebufferDepthSampleCounts": ["VK_SAMPLE_COUNT_1_BIT", "VK_SAMPLE_COUNT_4_BIT"],
                                "framebufferStencilSampleCounts": ["VK_SAMPLE_COUNT_1_BIT", "VK_SAMPLE_COUNT_4_BIT"],
                                "framebufferNoAttachmentsSampleCounts": ["VK_SAMPLE_COUNT_1_BIT", "VK_SAMPLE_COUNT_4_BIT"],
                                "maxColorAttachments": 4,
                                "sampledImageColorSampleCounts": ["VK_SAMPLE_COUNT_1_BIT", "VK_SAMPLE_COUNT_4_BIT"],
                                "sampledImageIntegerSampleCounts": ["VK_SAMPLE_COUNT_1_BIT", "VK_SAMPLE_COUNT_4_BIT"],
                                "sampledImageDepthSampleCounts": ["VK_SAMPLE_COUNT_1_BIT", "VK_SAMPLE_COUNT_4_BIT"],
                                "sampledImageStencilSampleCounts": ["VK_SAMPLE_COUNT_1_BIT", "VK_SAMPLE_COUNT_4_BIT"],
                                "storageImageSampleCounts": ["VK_SAMPLE_COUNT_1_BIT"],
                                "maxSampleMaskWords": 1,
                                "maxClipDistances": 0,
                                "maxCullDistances": 0,
                                "maxCombinedClipAndCullDistances": 0,
                                "discreteQueuePriorities": 2,
                                "pointSizeRange": [1.0, 1.0],
                                "lineWidthRange": [1.0, 1.0],
                                "pointSizeGranularity": 1.0,
                                "lineWidthGranularity": 1.0,
                                "nonCoherentAtomSize": 256
                            },
                            "sparseProperties": {
                                "residencyStandard2DBlockShape": false,
                                "residencyStandard2DMultisampleBlockShape": false,
                                "residencyStandard3DBlockShape": false,
                                "residencyNonResidentStrict": false
                            }
                        },
                        "VkPhysicalDeviceVulkan11Properties": {
                            "subgroupSize": 1,
                            "subgroupSupportedStages": ["VK_SHADER_STAGE_COMPUTE_BIT"],
                            "subgroupSupportedOperations": ["VK_SUBGROUP_FEATURE_BASIC_BIT"],
                            "maxMultiviewViewCount": 6,
                            "maxMultiviewInstanceIndex": 134217727,
                            "maxPerSetDescriptors": 1024,
                            "maxMemoryAllocationSize": 1073741824
                        },
                        "VkPhysicalDeviceVulkan12Properties": {
                            "shaderSignedZeroInfNanPreserveFloat16": false,
                            "shaderSignedZeroInfNanPreserveFloat32": false,
                            "shaderSignedZeroInfNanPreserveFloat64": false,
                            "shaderDenormPreserveFloat16": false,
                            "shaderDenormPreserveFloat32": false,
                            "shaderDenormPreserveFloat64": false,
                            "shaderDenormFlushToZeroFloat16": false,
                            "shaderDenormFlushToZeroFloat32": false,
                            "shaderDenormFlushToZeroFloat64": false,
                            "shaderRoundingModeRTEFloat16": false,
                            "shaderRoundingModeRTEFloat32": false,
                            "shaderRoundingModeRTEFloat64": false,
                            "shaderRoundingModeRTZFloat16": false,
                            "shaderRoundingModeRTZFloat32": false,
                            "shaderRoundingModeRTZFloat64": false,
                            "maxUpdateAfterBindDescriptorsInAllPools": 0,
                            "shaderUniformBufferArrayNonUniformIndexingNative": false,
                            "shaderSampledImageArrayNonUniformIndexingNative": false,
                            "shaderStorageBufferArrayNonUniformIndexingNative": false,
                            "shaderStorageImageArrayNonUniformIndexingNative": false,
                            "shaderInputAttachmentArrayNonUniformIndexingNative": false,
                            "robustBufferAccessUpdateAfterBind": false,
                            "quadDivergentImplicitLod": false,
                            "maxPerStageDescriptorUpdateAfterBindSamplers": 0,
                            "maxPerStageDescriptorUpdateAfterBindUniformBuffers": 0,
                            "maxPerStageDescriptorUpdateAfterBindStorageBuffers": 0,
                            "maxPerStageDescriptorUpdateAfterBindSampledImages": 0,
                            "maxPerStageDescriptorUpdateAfterBindStorageImages": 0,
                            "maxPerStageDescriptorUpdateAfterBindInputAttachments": 0,
                            "maxPerStageUpdateAfterBindResources": 0,
                            "maxDescriptorSetUpdateAfterBindSamplers": 0,
                            "maxDescriptorSetUpdateAfterBindUniformBuffers": 0,
                            "maxDescriptorSetUpdateAfterBindUniformBuffersDynamic": 0,
                            "maxDescriptorSetUpdateAfterBindStorageBuffers": 0,
                            "maxDescriptorSetUpdateAfterBindStorageBuffersDynamic": 0,
                            "maxDescriptorSetUpdateAfterBindSampledImages": 0,
                            "maxDescriptorSetUpdateAfterBindStorageImages": 0,
                            "maxDescriptorSetUpdateAfterBindInputAttachments": 0,
                            "supportedDepthResolveModes": [],
                            "supportedStencilResolveModes": [],
                            "independentResolveNone": false,
                            "independentResolve": false,
                            "filterMinmaxSingleComponentFormats": false,
                            "filterMinmaxImageComponentMapping": false,
                            "maxTimelineSemaphoreValueDifference": 2147483647,
                            "framebufferIntegerColorSampleCounts": ["VK_SAMPLE_COUNT_1_BIT"]
                        }
                    }
                }
            }
        }"""

        json_files_dict = {"test_profile.json": json.loads(original_json_text)}
        pull_required_capabilities_profiles_files(self.vk, json_files_dict)
        self.assertProfileDataEqual(json_files_dict["test_profile.json"], json.loads(expected_json_text))

    def test_pull_required_capabilities_vulkan13(self):
        original_json_text = """{
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8.0-204.json#",
            "profiles": {
                "VP_TEST_profile_v13": {
                    "version": 1,
                    "api-version": "1.3.204",
                    "capabilities": ["baseline"]
                }
            },
            "capabilities": {
                "baseline": {}
            }
        }"""

        expected_json_text = """{
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8.0-204.json#",
            "profiles": {
                "VP_TEST_profile_v13": {
                    "version": 1,
                    "api-version": "1.3.204",
                    "capabilities": ["baseline"]
                }
            },
            "capabilities": {
                "baseline": {
                    "features": {
                        "VkPhysicalDeviceFeatures": {
                            "robustBufferAccess": true
                        },
                        "VkPhysicalDeviceVulkan11Features": {
                            "multiview": true
                        },
                        "VkPhysicalDeviceVulkan12Features": {
                            "subgroupBroadcastDynamicId": true,
                            "imagelessFramebuffer": true,
                            "uniformBufferStandardLayout": true,
                            "shaderSubgroupExtendedTypes": true,
                            "separateDepthStencilLayouts": true,
                            "hostQueryReset": true,
                            "timelineSemaphore": true,
                            "vulkanMemoryModel": true,
                            "vulkanMemoryModelDeviceScope": true,
                            "bufferDeviceAddress": true
                        },
                        "VkPhysicalDeviceVulkan13Features": {
                            "shaderTerminateInvocation": true,
                            "shaderDemoteToHelperInvocation": true,
                            "privateData": true,
                            "pipelineCreationCacheControl": true,                                           
                            "synchronization2": true,
                            "shaderZeroInitializeWorkgroupMemory": true,
                            "robustImageAccess": true,
                            "subgroupSizeControl": true,
                            "computeFullSubgroups": true,
                            "dynamicRendering": true,
                            "shaderIntegerDotProduct": true,
                            "maintenance4": true,
                            "inlineUniformBlock": true
                        }
                    },
                    "properties": {
                        "VkPhysicalDeviceProperties": {
                            "limits": {
                                "maxImageDimension1D": 4096,
                                "maxImageDimension2D": 4096,
                                "maxImageDimension3D": 256,
                                "maxImageDimensionCube": 4096,
                                "maxImageArrayLayers": 256,
                                "maxTexelBufferElements": 65536,
                                "maxUniformBufferRange": 16384,
                                "maxStorageBufferRange": 134217728,
                                "maxPushConstantsSize": 128,
                                "maxMemoryAllocationCount": 4096,
                                "maxSamplerAllocationCount": 4000,
                                "bufferImageGranularity": 131072,
                                "sparseAddressSpaceSize": 0,
                                "maxBoundDescriptorSets": 4,
                                "maxPerStageDescriptorSamplers": 16,
                                "maxPerStageDescriptorUniformBuffers": 12,
                                "maxPerStageDescriptorStorageBuffers": 4,
                                "maxPerStageDescriptorSampledImages": 16,
                                "maxPerStageDescriptorStorageImages": 4,
                                "maxPerStageDescriptorInputAttachments": 4,
                                "maxPerStageResources": 128,
                                "maxDescriptorSetSamplers": 96,
                                "maxDescriptorSetUniformBuffers": 72,
                                "maxDescriptorSetUniformBuffersDynamic": 8,
                                "maxDescriptorSetStorageBuffers": 24,
                                "maxDescriptorSetStorageBuffersDynamic": 4,
                                "maxDescriptorSetSampledImages": 96,
                                "maxDescriptorSetStorageImages": 24,
                                "maxDescriptorSetInputAttachments": 4,
                                "maxVertexInputAttributes": 16,
                                "maxVertexInputBindings": 16,
                                "maxVertexInputAttributeOffset": 2047,
                                "maxVertexInputBindingStride": 2048,
                                "maxVertexOutputComponents": 64,
                                "maxTessellationGenerationLevel": 0,
                                "maxTessellationPatchSize": 0,
                                "maxTessellationControlPerVertexInputComponents": 0,
                                "maxTessellationControlPerVertexOutputComponents": 0,
                                "maxTessellationControlPerPatchOutputComponents": 0,
                                "maxTessellationControlTotalOutputComponents": 0,
                                "maxTessellationEvaluationInputComponents": 0,
                                "maxTessellationEvaluationOutputComponents": 0,
                                "maxGeometryShaderInvocations": 0,
                                "maxGeometryInputComponents": 0,
                                "maxGeometryOutputComponents": 0,
                                "maxGeometryOutputVertices": 0,
                                "maxGeometryTotalOutputComponents": 0,
                                "maxFragmentInputComponents": 64,
                                "maxFragmentOutputAttachments": 4,
                                "maxFragmentDualSrcAttachments": 0,
                                "maxFragmentCombinedOutputResources": 4,
                                "maxComputeSharedMemorySize": 16384,
                                "maxComputeWorkGroupCount": [65535, 65535, 65535],
                                "maxComputeWorkGroupInvocations": 128,
                                "maxComputeWorkGroupSize": [128, 128, 64],
                                "subPixelPrecisionBits": 4,
                                "subTexelPrecisionBits": 4,
                                "mipmapPrecisionBits": 4,
                                "maxDrawIndexedIndexValue": 16777216,
                                "maxDrawIndirectCount": 1,
                                "maxSamplerLodBias": 2,
                                "maxSamplerAnisotropy": 1,
                                "maxViewports": 1,
                                "maxViewportDimensions": [4096, 4096],
                                "viewportBoundsRange": [-8192, 8192],
                                "viewportSubPixelBits": 0,
                                "minMemoryMapAlignment": 64,
                                "minTexelBufferOffsetAlignment": 256,
                                "minUniformBufferOffsetAlignment": 256,
                                "minStorageBufferOffsetAlignment": 256,
                                "minTexelOffset": -8,
                                "maxTexelOffset": 7,
                                "minTexelGatherOffset": -8,
                                "maxTexelGatherOffset": 7,
                                "minInterpolationOffset": 0.0,
                                "maxInterpolationOffset": 0.0,
                                "subPixelInterpolationOffsetBits": 0,
                                "maxFramebufferWidth": 4096,
                                "maxFramebufferHeight": 4096,
                                "maxFramebufferLayers": 256,
                                "framebufferColorSampleCounts": ["VK_SAMPLE_COUNT_1_BIT", "VK_SAMPLE_COUNT_4_BIT"],
                                "framebufferDepthSampleCounts": ["VK_SAMPLE_COUNT_1_BIT", "VK_SAMPLE_COUNT_4_BIT"],
                                "framebufferStencilSampleCounts": ["VK_SAMPLE_COUNT_1_BIT", "VK_SAMPLE_COUNT_4_BIT"],
                                "framebufferNoAttachmentsSampleCounts": ["VK_SAMPLE_COUNT_1_BIT", "VK_SAMPLE_COUNT_4_BIT"],
                                "maxColorAttachments": 4,
                                "sampledImageColorSampleCounts": ["VK_SAMPLE_COUNT_1_BIT", "VK_SAMPLE_COUNT_4_BIT"],
                                "sampledImageIntegerSampleCounts": ["VK_SAMPLE_COUNT_1_BIT", "VK_SAMPLE_COUNT_4_BIT"],
                                "sampledImageDepthSampleCounts": ["VK_SAMPLE_COUNT_1_BIT", "VK_SAMPLE_COUNT_4_BIT"],
                                "sampledImageStencilSampleCounts": ["VK_SAMPLE_COUNT_1_BIT", "VK_SAMPLE_COUNT_4_BIT"],
                                "storageImageSampleCounts": ["VK_SAMPLE_COUNT_1_BIT"],
                                "maxSampleMaskWords": 1,
                                "maxClipDistances": 0,
                                "maxCullDistances": 0,
                                "maxCombinedClipAndCullDistances": 0,
                                "discreteQueuePriorities": 2,
                                "pointSizeRange": [1.0, 1.0],
                                "lineWidthRange": [1.0, 1.0],
                                "pointSizeGranularity": 1.0,
                                "lineWidthGranularity": 1.0,
                                "nonCoherentAtomSize": 256
                            },
                            "sparseProperties": {
                                "residencyStandard2DBlockShape": false,
                                "residencyStandard2DMultisampleBlockShape": false,
                                "residencyStandard3DBlockShape": false,
                                "residencyNonResidentStrict": false
                            }
                        },
                        "VkPhysicalDeviceVulkan11Properties": {
                            "subgroupSize": 1,
                            "subgroupSupportedStages": ["VK_SHADER_STAGE_COMPUTE_BIT"],
                            "subgroupSupportedOperations": ["VK_SUBGROUP_FEATURE_BASIC_BIT"],
                            "maxMultiviewViewCount": 6,
                            "maxMultiviewInstanceIndex": 134217727,
                            "maxPerSetDescriptors": 1024,
                            "maxMemoryAllocationSize": 1073741824
                        },
                        "VkPhysicalDeviceVulkan12Properties": {
                            "shaderSignedZeroInfNanPreserveFloat16": false,
                            "shaderSignedZeroInfNanPreserveFloat32": false,
                            "shaderSignedZeroInfNanPreserveFloat64": false,
                            "shaderDenormPreserveFloat16": false,
                            "shaderDenormPreserveFloat32": false,
                            "shaderDenormPreserveFloat64": false,
                            "shaderDenormFlushToZeroFloat16": false,
                            "shaderDenormFlushToZeroFloat32": false,
                            "shaderDenormFlushToZeroFloat64": false,
                            "shaderRoundingModeRTEFloat16": false,
                            "shaderRoundingModeRTEFloat32": false,
                            "shaderRoundingModeRTEFloat64": false,
                            "shaderRoundingModeRTZFloat16": false,
                            "shaderRoundingModeRTZFloat32": false,
                            "shaderRoundingModeRTZFloat64": false,
                            "maxUpdateAfterBindDescriptorsInAllPools": 0,
                            "shaderUniformBufferArrayNonUniformIndexingNative": false,
                            "shaderSampledImageArrayNonUniformIndexingNative": false,
                            "shaderStorageBufferArrayNonUniformIndexingNative": false,
                            "shaderStorageImageArrayNonUniformIndexingNative": false,
                            "shaderInputAttachmentArrayNonUniformIndexingNative": false,
                            "robustBufferAccessUpdateAfterBind": false,
                            "quadDivergentImplicitLod": false,
                            "maxPerStageDescriptorUpdateAfterBindSamplers": 0,
                            "maxPerStageDescriptorUpdateAfterBindUniformBuffers": 0,
                            "maxPerStageDescriptorUpdateAfterBindStorageBuffers": 0,
                            "maxPerStageDescriptorUpdateAfterBindSampledImages": 0,
                            "maxPerStageDescriptorUpdateAfterBindStorageImages": 0,
                            "maxPerStageDescriptorUpdateAfterBindInputAttachments": 0,
                            "maxPerStageUpdateAfterBindResources": 0,
                            "maxDescriptorSetUpdateAfterBindSamplers": 0,
                            "maxDescriptorSetUpdateAfterBindUniformBuffers": 0,
                            "maxDescriptorSetUpdateAfterBindUniformBuffersDynamic": 0,
                            "maxDescriptorSetUpdateAfterBindStorageBuffers": 0,
                            "maxDescriptorSetUpdateAfterBindStorageBuffersDynamic": 0,
                            "maxDescriptorSetUpdateAfterBindSampledImages": 0,
                            "maxDescriptorSetUpdateAfterBindStorageImages": 0,
                            "maxDescriptorSetUpdateAfterBindInputAttachments": 0,
                            "supportedDepthResolveModes": [],
                            "supportedStencilResolveModes": [],
                            "independentResolveNone": false,
                            "independentResolve": false,
                            "filterMinmaxSingleComponentFormats": false,
                            "filterMinmaxImageComponentMapping": false,
                            "maxTimelineSemaphoreValueDifference": 2147483647,
                            "framebufferIntegerColorSampleCounts": ["VK_SAMPLE_COUNT_1_BIT"]
                        },
                        "VkPhysicalDeviceVulkan13Properties": {
                            "maxComputeWorkgroupSubgroups": 0,
                            "requiredSubgroupSizeStages": [],
                            "maxInlineUniformBlockSize": 256,
                            "maxPerStageDescriptorInlineUniformBlocks": 4,
                            "maxPerStageDescriptorUpdateAfterBindInlineUniformBlocks": 4,
                            "maxDescriptorSetInlineUniformBlocks": 4,
                            "maxDescriptorSetUpdateAfterBindInlineUniformBlocks": 4,
                            "maxInlineUniformTotalSize": 256,
                            "integerDotProduct8BitUnsignedAccelerated": false,
                            "integerDotProduct8BitSignedAccelerated": false,
                            "integerDotProduct8BitMixedSignednessAccelerated": false,
                            "integerDotProduct4x8BitPackedUnsignedAccelerated": false,
                            "integerDotProduct4x8BitPackedSignedAccelerated": false,
                            "integerDotProduct4x8BitPackedMixedSignednessAccelerated": false,
                            "integerDotProduct16BitUnsignedAccelerated": false,
                            "integerDotProduct16BitSignedAccelerated": false,
                            "integerDotProduct16BitMixedSignednessAccelerated": false,
                            "integerDotProduct32BitUnsignedAccelerated": false,
                            "integerDotProduct32BitSignedAccelerated": false,
                            "integerDotProduct32BitMixedSignednessAccelerated": false,
                            "integerDotProduct64BitUnsignedAccelerated": false,
                            "integerDotProduct64BitSignedAccelerated": false,
                            "integerDotProduct64BitMixedSignednessAccelerated": false,
                            "integerDotProductAccumulatingSaturating8BitUnsignedAccelerated": false,
                            "integerDotProductAccumulatingSaturating8BitSignedAccelerated": false,
                            "integerDotProductAccumulatingSaturating8BitMixedSignednessAccelerated": false,
                            "integerDotProductAccumulatingSaturating4x8BitPackedUnsignedAccelerated": false,
                            "integerDotProductAccumulatingSaturating4x8BitPackedSignedAccelerated": false,
                            "integerDotProductAccumulatingSaturating4x8BitPackedMixedSignednessAccelerated": false,
                            "integerDotProductAccumulatingSaturating16BitUnsignedAccelerated": false,
                            "integerDotProductAccumulatingSaturating16BitSignedAccelerated": false,
                            "integerDotProductAccumulatingSaturating16BitMixedSignednessAccelerated": false,
                            "integerDotProductAccumulatingSaturating32BitUnsignedAccelerated": false,
                            "integerDotProductAccumulatingSaturating32BitSignedAccelerated": false,
                            "integerDotProductAccumulatingSaturating32BitMixedSignednessAccelerated": false,
                            "integerDotProductAccumulatingSaturating64BitUnsignedAccelerated": false,
                            "integerDotProductAccumulatingSaturating64BitSignedAccelerated": false,
                            "integerDotProductAccumulatingSaturating64BitMixedSignednessAccelerated": false,
                            "maxBufferSize": 1073741824
                        }
                    }
                }
            }
        }"""

        json_files_dict = {"test_profile.json": json.loads(original_json_text)}
        pull_required_capabilities_profiles_files(self.vk, json_files_dict)
        self.assertProfileDataEqual(json_files_dict["test_profile.json"], json.loads(expected_json_text))

    def test_pull_required_capabilities_vulkan14(self):
        original_json_text = """{
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8.0-304.json#",
            "profiles": {
                "VP_TEST_profile_v14": {
                    "version": 1,
                    "api-version": "1.4.304",
                    "capabilities": ["baseline"]
                }
            },
            "capabilities": {
                "baseline": {}
            }
        }"""

        expected_json_text = """{
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8.0-304.json#",
            "profiles": {
                "VP_TEST_profile_v14": {
                    "version": 1,
                    "api-version": "1.4.304",
                    "capabilities": ["baseline"]
                }
            },
            "capabilities": {
                "baseline": {
                    "features": {
                        "VkPhysicalDeviceFeatures": {
                            "robustBufferAccess": true,
                            "fullDrawIndexUint32": true,
                            "imageCubeArray": true,
                            "independentBlend": true,
                            "sampleRateShading": true,
                            "drawIndirectFirstInstance": true,
                            "depthClamp": true,
                            "depthBiasClamp": true,
                            "samplerAnisotropy": true,
                            "fragmentStoresAndAtomics": true,
                            "shaderStorageImageExtendedFormats": true,
                            "shaderUniformBufferArrayDynamicIndexing": true,
                            "shaderSampledImageArrayDynamicIndexing": true,
                            "shaderStorageBufferArrayDynamicIndexing": true,
                            "shaderStorageImageArrayDynamicIndexing": true,
                            "shaderImageGatherExtended": true,
                            "shaderInt16": true,
                            "largePoints": true
                        },
                        "VkPhysicalDeviceVulkan11Features": {
                            "multiview": true,
                            "samplerYcbcrConversion": true,
                            "storageBuffer16BitAccess": true,
                            "variablePointers": true,
                            "variablePointersStorageBuffer": true
                        },
                        "VkPhysicalDeviceVulkan12Features": {
                            "subgroupBroadcastDynamicId": true,
                            "imagelessFramebuffer": true,
                            "uniformBufferStandardLayout": true,
                            "shaderSubgroupExtendedTypes": true,
                            "separateDepthStencilLayouts": true,
                            "hostQueryReset": true,
                            "timelineSemaphore": true,
                            "vulkanMemoryModel": true,
                            "vulkanMemoryModelDeviceScope": true,
                            "bufferDeviceAddress": true,
                            "samplerMirrorClampToEdge": true,
                            "scalarBlockLayout": true,                            
                            "shaderUniformTexelBufferArrayDynamicIndexing": true,
                            "shaderStorageTexelBufferArrayDynamicIndexing": true,
                            "shaderInt8": true,                            
                            "storageBuffer8BitAccess": true
                        },
                        "VkPhysicalDeviceVulkan13Features": {
                            "shaderTerminateInvocation": true,
                            "shaderDemoteToHelperInvocation": true,
                            "privateData": true,
                            "pipelineCreationCacheControl": true,                                           
                            "synchronization2": true,
                            "shaderZeroInitializeWorkgroupMemory": true,
                            "robustImageAccess": true,
                            "subgroupSizeControl": true,
                            "computeFullSubgroups": true,
                            "dynamicRendering": true,
                            "shaderIntegerDotProduct": true,
                            "maintenance4": true,
                            "inlineUniformBlock": true
                        },
                        "VkPhysicalDeviceVulkan14Features": {
                            "globalPriorityQuery": true,
                            "shaderSubgroupRotate": true,
                            "shaderSubgroupRotateClustered": true,
                            "shaderFloatControls2": true,
                            "shaderExpectAssume": true,
                            "bresenhamLines": true,
                            "vertexAttributeInstanceRateDivisor": true,
                            "indexTypeUint8": true,
                            "maintenance5": true,
                            "pushDescriptor": true,
                            "dynamicRenderingLocalRead": true,
                            "maintenance6": true,
                            "pipelineRobustness": true
                        }
                    },
                    "properties": {
                        "VkPhysicalDeviceProperties": {
                            "limits": {
                                "maxImageDimension1D": 8192,
                                "maxImageDimension2D": 8192,
                                "maxImageDimension3D": 512,
                                "maxImageDimensionCube": 8192,
                                "maxImageArrayLayers": 2048,
                                "maxTexelBufferElements": 65536,
                                "maxUniformBufferRange": 65536,
                                "maxStorageBufferRange": 134217728,
                                "maxPushConstantsSize": 256,
                                "maxMemoryAllocationCount": 4096,
                                "maxSamplerAllocationCount": 4000,
                                "bufferImageGranularity": 4096,
                                "sparseAddressSpaceSize": 0,
                                "maxBoundDescriptorSets": 7,
                                "maxPerStageDescriptorSamplers": 16,
                                "maxPerStageDescriptorUniformBuffers": 15,
                                "maxPerStageDescriptorStorageBuffers": 4,
                                "maxPerStageDescriptorSampledImages": 16,
                                "maxPerStageDescriptorStorageImages": 4,
                                "maxPerStageDescriptorInputAttachments": 4,
                                "maxPerStageResources": 200,
                                "maxDescriptorSetSamplers": 96,
                                "maxDescriptorSetUniformBuffers": 90,
                                "maxDescriptorSetUniformBuffersDynamic": 8,
                                "maxDescriptorSetStorageBuffers": 96,
                                "maxDescriptorSetStorageBuffersDynamic": 4,
                                "maxDescriptorSetSampledImages": 96,
                                "maxDescriptorSetStorageImages": 144,
                                "maxDescriptorSetInputAttachments": 4,
                                "maxVertexInputAttributes": 16,
                                "maxVertexInputBindings": 16,
                                "maxVertexInputAttributeOffset": 2047,
                                "maxVertexInputBindingStride": 2048,
                                "maxVertexOutputComponents": 64,
                                "maxTessellationGenerationLevel": 0,
                                "maxTessellationPatchSize": 0,
                                "maxTessellationControlPerVertexInputComponents": 0,
                                "maxTessellationControlPerVertexOutputComponents": 0,
                                "maxTessellationControlPerPatchOutputComponents": 0,
                                "maxTessellationControlTotalOutputComponents": 0,
                                "maxTessellationEvaluationInputComponents": 0,
                                "maxTessellationEvaluationOutputComponents": 0,
                                "maxGeometryShaderInvocations": 0,
                                "maxGeometryInputComponents": 0,
                                "maxGeometryOutputComponents": 0,
                                "maxGeometryOutputVertices": 0,
                                "maxGeometryTotalOutputComponents": 0,
                                "maxFragmentInputComponents": 64,
                                "maxFragmentOutputAttachments": 4,
                                "maxFragmentDualSrcAttachments": 0,
                                "maxFragmentCombinedOutputResources": 16,
                                "maxComputeSharedMemorySize": 16384,
                                "maxComputeWorkGroupCount": [65535, 65535, 65535],
                                "maxComputeWorkGroupInvocations": 256,
                                "maxComputeWorkGroupSize": [256, 256, 64],
                                "subPixelPrecisionBits": 4,
                                "subTexelPrecisionBits": 8,
                                "mipmapPrecisionBits": 6,
                                "maxDrawIndexedIndexValue": 16777216,
                                "maxDrawIndirectCount": 1,
                                "maxSamplerLodBias": 14,
                                "maxSamplerAnisotropy": 1,
                                "maxViewports": 1,
                                "maxViewportDimensions": [7680, 7680],
                                "viewportBoundsRange": [-15360, 15359],
                                "viewportSubPixelBits": 0,
                                "minMemoryMapAlignment": 64,
                                "minTexelBufferOffsetAlignment": 256,
                                "minUniformBufferOffsetAlignment": 256,
                                "minStorageBufferOffsetAlignment": 256,
                                "minTexelOffset": -8,
                                "maxTexelOffset": 7,
                                "minTexelGatherOffset": -8,
                                "maxTexelGatherOffset": 7,
                                "minInterpolationOffset": 0.0,
                                "maxInterpolationOffset": 0.0,
                                "subPixelInterpolationOffsetBits": 0,
                                "maxFramebufferWidth": 7680,
                                "maxFramebufferHeight": 7680,
                                "maxFramebufferLayers": 256,
                                "framebufferColorSampleCounts": ["VK_SAMPLE_COUNT_1_BIT", "VK_SAMPLE_COUNT_4_BIT"],
                                "framebufferDepthSampleCounts": ["VK_SAMPLE_COUNT_1_BIT", "VK_SAMPLE_COUNT_4_BIT"],
                                "framebufferStencilSampleCounts": ["VK_SAMPLE_COUNT_1_BIT", "VK_SAMPLE_COUNT_4_BIT"],
                                "framebufferNoAttachmentsSampleCounts": ["VK_SAMPLE_COUNT_1_BIT", "VK_SAMPLE_COUNT_4_BIT"],
                                "maxColorAttachments": 8,
                                "sampledImageColorSampleCounts": ["VK_SAMPLE_COUNT_1_BIT", "VK_SAMPLE_COUNT_4_BIT"],
                                "sampledImageIntegerSampleCounts": ["VK_SAMPLE_COUNT_1_BIT", "VK_SAMPLE_COUNT_4_BIT"],
                                "sampledImageDepthSampleCounts": ["VK_SAMPLE_COUNT_1_BIT", "VK_SAMPLE_COUNT_4_BIT"],
                                "sampledImageStencilSampleCounts": ["VK_SAMPLE_COUNT_1_BIT", "VK_SAMPLE_COUNT_4_BIT"],
                                "storageImageSampleCounts": ["VK_SAMPLE_COUNT_1_BIT"],
                                "maxSampleMaskWords": 1,
                                "maxClipDistances": 0,
                                "maxCullDistances": 0,
                                "maxCombinedClipAndCullDistances": 0,
                                "discreteQueuePriorities": 2,
                                "pointSizeRange": [1.0, 256.0],
                                "lineWidthRange": [1.0, 1.0],
                                "pointSizeGranularity": 0.125,
                                "lineWidthGranularity": 0.5,
                                "nonCoherentAtomSize": 256,
                                "timestampComputeAndGraphics": true,
                                "standardSampleLocations": true
                            },
                            "sparseProperties": {
                                "residencyStandard2DBlockShape": false,
                                "residencyStandard2DMultisampleBlockShape": false,
                                "residencyStandard3DBlockShape": false,
                                "residencyNonResidentStrict": false
                            }
                        },
                        "VkPhysicalDeviceVulkan11Properties": {
                            "subgroupSize": 1,
                            "subgroupSupportedStages": ["VK_SHADER_STAGE_COMPUTE_BIT", "VK_SHADER_STAGE_FRAGMENT_BIT"],
                            "subgroupSupportedOperations": [
                                "VK_SUBGROUP_FEATURE_BASIC_BIT",
                                "VK_SUBGROUP_FEATURE_ROTATE_BIT",
                                "VK_SUBGROUP_FEATURE_ROTATE_CLUSTERED_BIT"
                            ],
                            "maxMultiviewViewCount": 6,
                            "maxMultiviewInstanceIndex": 134217727,
                            "maxPerSetDescriptors": 1024,
                            "maxMemoryAllocationSize": 1073741824
                        },
                        "VkPhysicalDeviceVulkan12Properties": {
                            "shaderSignedZeroInfNanPreserveFloat16": true,
                            "shaderSignedZeroInfNanPreserveFloat32": true,
                            "shaderSignedZeroInfNanPreserveFloat64": false,
                            "shaderDenormPreserveFloat16": false,
                            "shaderDenormPreserveFloat32": false,
                            "shaderDenormPreserveFloat64": false,
                            "shaderDenormFlushToZeroFloat16": false,
                            "shaderDenormFlushToZeroFloat32": false,
                            "shaderDenormFlushToZeroFloat64": false,
                            "shaderRoundingModeRTEFloat16": false,
                            "shaderRoundingModeRTEFloat32": false,
                            "shaderRoundingModeRTEFloat64": false,
                            "shaderRoundingModeRTZFloat16": false,
                            "shaderRoundingModeRTZFloat32": false,
                            "shaderRoundingModeRTZFloat64": false,
                            "maxUpdateAfterBindDescriptorsInAllPools": 0,
                            "shaderUniformBufferArrayNonUniformIndexingNative": false,
                            "shaderSampledImageArrayNonUniformIndexingNative": false,
                            "shaderStorageBufferArrayNonUniformIndexingNative": false,
                            "shaderStorageImageArrayNonUniformIndexingNative": false,
                            "shaderInputAttachmentArrayNonUniformIndexingNative": false,
                            "robustBufferAccessUpdateAfterBind": false,
                            "quadDivergentImplicitLod": false,
                            "maxPerStageDescriptorUpdateAfterBindSamplers": 0,
                            "maxPerStageDescriptorUpdateAfterBindUniformBuffers": 0,
                            "maxPerStageDescriptorUpdateAfterBindStorageBuffers": 0,
                            "maxPerStageDescriptorUpdateAfterBindSampledImages": 0,
                            "maxPerStageDescriptorUpdateAfterBindStorageImages": 0,
                            "maxPerStageDescriptorUpdateAfterBindInputAttachments": 0,
                            "maxPerStageUpdateAfterBindResources": 0,
                            "maxDescriptorSetUpdateAfterBindSamplers": 0,
                            "maxDescriptorSetUpdateAfterBindUniformBuffers": 0,
                            "maxDescriptorSetUpdateAfterBindUniformBuffersDynamic": 0,
                            "maxDescriptorSetUpdateAfterBindStorageBuffers": 0,
                            "maxDescriptorSetUpdateAfterBindStorageBuffersDynamic": 0,
                            "maxDescriptorSetUpdateAfterBindSampledImages": 0,
                            "maxDescriptorSetUpdateAfterBindStorageImages": 0,
                            "maxDescriptorSetUpdateAfterBindInputAttachments": 0,
                            "supportedDepthResolveModes": [],
                            "supportedStencilResolveModes": [],
                            "independentResolveNone": false,
                            "independentResolve": false,
                            "filterMinmaxSingleComponentFormats": false,
                            "filterMinmaxImageComponentMapping": false,
                            "maxTimelineSemaphoreValueDifference": 2147483647,
                            "framebufferIntegerColorSampleCounts": ["VK_SAMPLE_COUNT_1_BIT"]
                        },
                        "VkPhysicalDeviceVulkan13Properties": {
                            "maxComputeWorkgroupSubgroups": 0,
                            "requiredSubgroupSizeStages": [],
                            "maxInlineUniformBlockSize": 256,
                            "maxPerStageDescriptorInlineUniformBlocks": 4,
                            "maxPerStageDescriptorUpdateAfterBindInlineUniformBlocks": 4,
                            "maxDescriptorSetInlineUniformBlocks": 4,
                            "maxDescriptorSetUpdateAfterBindInlineUniformBlocks": 4,
                            "maxInlineUniformTotalSize": 256,
                            "integerDotProduct8BitUnsignedAccelerated": false,
                            "integerDotProduct8BitSignedAccelerated": false,
                            "integerDotProduct8BitMixedSignednessAccelerated": false,
                            "integerDotProduct4x8BitPackedUnsignedAccelerated": false,
                            "integerDotProduct4x8BitPackedSignedAccelerated": false,
                            "integerDotProduct4x8BitPackedMixedSignednessAccelerated": false,
                            "integerDotProduct16BitUnsignedAccelerated": false,
                            "integerDotProduct16BitSignedAccelerated": false,
                            "integerDotProduct16BitMixedSignednessAccelerated": false,
                            "integerDotProduct32BitUnsignedAccelerated": false,
                            "integerDotProduct32BitSignedAccelerated": false,
                            "integerDotProduct32BitMixedSignednessAccelerated": false,
                            "integerDotProduct64BitUnsignedAccelerated": false,
                            "integerDotProduct64BitSignedAccelerated": false,
                            "integerDotProduct64BitMixedSignednessAccelerated": false,
                            "integerDotProductAccumulatingSaturating8BitUnsignedAccelerated": false,
                            "integerDotProductAccumulatingSaturating8BitSignedAccelerated": false,
                            "integerDotProductAccumulatingSaturating8BitMixedSignednessAccelerated": false,
                            "integerDotProductAccumulatingSaturating4x8BitPackedUnsignedAccelerated": false,
                            "integerDotProductAccumulatingSaturating4x8BitPackedSignedAccelerated": false,
                            "integerDotProductAccumulatingSaturating4x8BitPackedMixedSignednessAccelerated": false,
                            "integerDotProductAccumulatingSaturating16BitUnsignedAccelerated": false,
                            "integerDotProductAccumulatingSaturating16BitSignedAccelerated": false,
                            "integerDotProductAccumulatingSaturating16BitMixedSignednessAccelerated": false,
                            "integerDotProductAccumulatingSaturating32BitUnsignedAccelerated": false,
                            "integerDotProductAccumulatingSaturating32BitSignedAccelerated": false,
                            "integerDotProductAccumulatingSaturating32BitMixedSignednessAccelerated": false,
                            "integerDotProductAccumulatingSaturating64BitUnsignedAccelerated": false,
                            "integerDotProductAccumulatingSaturating64BitSignedAccelerated": false,
                            "integerDotProductAccumulatingSaturating64BitMixedSignednessAccelerated": false,
                            "maxBufferSize": 1073741824
                        },
                        "VkPhysicalDeviceVulkan14Properties": {
                            "maxPushDescriptors": 32
                        }
                    }
                }
            }
        }"""

        json_files_dict = {"test_profile.json": json.loads(original_json_text)}
        pull_required_capabilities_profiles_files(self.vk, json_files_dict)
        self.assertProfileDataEqual(json_files_dict["test_profile.json"], json.loads(expected_json_text))


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument(
        '--registry', '-r', action='store', required=False,
        help='Use specified registry file instead of vk.xml.'
    )

    args, unparsed = parser.parse_known_args()
    TestConvertPullRequiredCapabilitiesCore.registry_path = args.registry

    unittest.main(argv=[sys.argv[0]] + unparsed)