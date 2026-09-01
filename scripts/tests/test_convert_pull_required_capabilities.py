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
from source.main_convert import (
    pull_required_capabilities_profiles_files
)


class TestConvertPullRequiredCapabilities(unittest.TestCase):
    registry_path = None

    def setUp(self):
        self.vk: VulkanObject = initVulkanObject('vulkan', self.registry_path)

    # -------------------------------------------------------------------------
    # Standalone Empty Core Profile Tests (Vulkan 1.0 to 1.4)
    # -------------------------------------------------------------------------

    def test_pull_required_capabilities_empty_vulkan10(self):
        """
        Verifies that an empty Vulkan 1.0 profile pulls required Vulkan 1.0 core features and properties.
        """
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
                                "maxComputeWorkGroupCount": [
                                    65535,
                                    65535,
                                    65535
                                ],
                                "maxComputeWorkGroupInvocations": 128,
                                "maxComputeWorkGroupSize": [
                                    128,
                                    128,
                                    64
                                ],
                                "subPixelPrecisionBits": 4,
                                "subTexelPrecisionBits": 4,
                                "mipmapPrecisionBits": 4,
                                "maxDrawIndexedIndexValue": 16777216,
                                "maxDrawIndirectCount": 1,
                                "maxSamplerLodBias": 2,
                                "maxSamplerAnisotropy": 1,
                                "maxViewports": 1,
                                "maxViewportDimensions": [
                                    4096,
                                    4096
                                ],
                                "viewportBoundsRange": [
                                    -8192,
                                    8192
                                ],
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
                                "framebufferColorSampleCounts": [
                                    "VK_SAMPLE_COUNT_1_BIT",
                                    "VK_SAMPLE_COUNT_4_BIT"
                                ],
                                "framebufferDepthSampleCounts": [
                                    "VK_SAMPLE_COUNT_1_BIT",
                                    "VK_SAMPLE_COUNT_4_BIT"
                                ],
                                "framebufferStencilSampleCounts": [
                                    "VK_SAMPLE_COUNT_1_BIT",
                                    "VK_SAMPLE_COUNT_4_BIT"
                                ],
                                "framebufferNoAttachmentsSampleCounts": [
                                    "VK_SAMPLE_COUNT_1_BIT",
                                    "VK_SAMPLE_COUNT_4_BIT"
                                ],
                                "maxColorAttachments": 4,
                                "sampledImageColorSampleCounts": [
                                    "VK_SAMPLE_COUNT_1_BIT",
                                    "VK_SAMPLE_COUNT_4_BIT"
                                ],
                                "sampledImageIntegerSampleCounts": [
                                    "VK_SAMPLE_COUNT_1_BIT",
                                    "VK_SAMPLE_COUNT_4_BIT"
                                ],
                                "sampledImageDepthSampleCounts": [
                                    "VK_SAMPLE_COUNT_1_BIT",
                                    "VK_SAMPLE_COUNT_4_BIT"
                                ],
                                "sampledImageStencilSampleCounts": [
                                    "VK_SAMPLE_COUNT_1_BIT",
                                    "VK_SAMPLE_COUNT_4_BIT"
                                ],
                                "storageImageSampleCounts": [
                                    "VK_SAMPLE_COUNT_1_BIT"
                                ],
                                "maxSampleMaskWords": 1,
                                "maxClipDistances": 0,
                                "maxCullDistances": 0,
                                "maxCombinedClipAndCullDistances": 0,
                                "discreteQueuePriorities": 2,
                                "pointSizeRange": [
                                    1.0,
                                    1.0
                                ],
                                "lineWidthRange": [
                                    1.0,
                                    1.0
                                ],
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

        try:
            original_data = json.loads(original_json_text)
            expected_data = json.loads(expected_json_text)
        except json.JSONDecodeError as e:
            print(f"JSON syntax is incorrect: {e.msg} at line {e.lineno}, column {e.colno}")

        json_files_dict = {"test_profile.json": original_data}
        pull_required_capabilities_profiles_files(self.vk, json_files_dict)

        self.assertEqual(json_files_dict["test_profile.json"], expected_data)

    def test_pull_required_capabilities_empty_vulkan11(self):
        """
        Verifies that an empty Vulkan 1.1 profile pulls required Vulkan 1.0 and 1.1 split core features and properties
        without bundle structures (VkPhysicalDeviceVulkan11Features/Properties).
        """
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
                                "maxComputeWorkGroupCount": [
                                    65535,
                                    65535,
                                    65535
                                ],
                                "maxComputeWorkGroupInvocations": 128,
                                "maxComputeWorkGroupSize": [
                                    128,
                                    128,
                                    64
                                ],
                                "subPixelPrecisionBits": 4,
                                "subTexelPrecisionBits": 4,
                                "mipmapPrecisionBits": 4,
                                "maxDrawIndexedIndexValue": 16777216,
                                "maxDrawIndirectCount": 1,
                                "maxSamplerLodBias": 2,
                                "maxSamplerAnisotropy": 1,
                                "maxViewports": 1,
                                "maxViewportDimensions": [
                                    4096,
                                    4096
                                ],
                                "viewportBoundsRange": [
                                    -8192,
                                    8192
                                ],
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
                                "framebufferColorSampleCounts": [
                                    "VK_SAMPLE_COUNT_1_BIT",
                                    "VK_SAMPLE_COUNT_4_BIT"
                                ],
                                "framebufferDepthSampleCounts": [
                                    "VK_SAMPLE_COUNT_1_BIT",
                                    "VK_SAMPLE_COUNT_4_BIT"
                                ],
                                "framebufferStencilSampleCounts": [
                                    "VK_SAMPLE_COUNT_1_BIT",
                                    "VK_SAMPLE_COUNT_4_BIT"
                                ],
                                "framebufferNoAttachmentsSampleCounts": [
                                    "VK_SAMPLE_COUNT_1_BIT",
                                    "VK_SAMPLE_COUNT_4_BIT"
                                ],
                                "maxColorAttachments": 4,
                                "sampledImageColorSampleCounts": [
                                    "VK_SAMPLE_COUNT_1_BIT",
                                    "VK_SAMPLE_COUNT_4_BIT"
                                ],
                                "sampledImageIntegerSampleCounts": [
                                    "VK_SAMPLE_COUNT_1_BIT",
                                    "VK_SAMPLE_COUNT_4_BIT"
                                ],
                                "sampledImageDepthSampleCounts": [
                                    "VK_SAMPLE_COUNT_1_BIT",
                                    "VK_SAMPLE_COUNT_4_BIT"
                                ],
                                "sampledImageStencilSampleCounts": [
                                    "VK_SAMPLE_COUNT_1_BIT",
                                    "VK_SAMPLE_COUNT_4_BIT"
                                ],
                                "storageImageSampleCounts": [
                                    "VK_SAMPLE_COUNT_1_BIT"
                                ],
                                "maxSampleMaskWords": 1,
                                "maxClipDistances": 0,
                                "maxCullDistances": 0,
                                "maxCombinedClipAndCullDistances": 0,
                                "discreteQueuePriorities": 2,
                                "pointSizeRange": [
                                    1.0,
                                    1.0
                                ],
                                "lineWidthRange": [
                                    1.0,
                                    1.0
                                ],
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
                            "supportedStages": [
                                "VK_SHADER_STAGE_COMPUTE_BIT"
                            ],
                            "supportedOperations": [
                                "VK_SUBGROUP_FEATURE_BASIC_BIT"
                            ]
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

        try:
            original_data = json.loads(original_json_text)
            expected_data = json.loads(expected_json_text)
        except json.JSONDecodeError as e:
            print(f"JSON syntax is incorrect: {e.msg} at line {e.lineno}, column {e.colno}")

        json_files_dict = {"test_profile.json": original_data}
        pull_required_capabilities_profiles_files(self.vk, json_files_dict)

        self.assertEqual(json_files_dict["test_profile.json"], expected_data)

    def test_pull_required_capabilities_empty_vulkan12(self):
        """
        Verifies that an empty Vulkan 1.2 profile pulls required Vulkan 1.0 through 1.2 core features and properties,
        including VkPhysicalDeviceVulkan11Properties and VkPhysicalDeviceVulkan12Properties.
        """
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
                                "maxComputeWorkGroupCount": [
                                    65535,
                                    65535,
                                    65535
                                ],
                                "maxComputeWorkGroupInvocations": 128,
                                "maxComputeWorkGroupSize": [
                                    128,
                                    128,
                                    64
                                ],
                                "subPixelPrecisionBits": 4,
                                "subTexelPrecisionBits": 4,
                                "mipmapPrecisionBits": 4,
                                "maxDrawIndexedIndexValue": 16777216,
                                "maxDrawIndirectCount": 1,
                                "maxSamplerLodBias": 2,
                                "maxSamplerAnisotropy": 1,
                                "maxViewports": 1,
                                "maxViewportDimensions": [
                                    4096,
                                    4096
                                ],
                                "viewportBoundsRange": [
                                    -8192,
                                    8192
                                ],
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
                                "framebufferColorSampleCounts": [
                                    "VK_SAMPLE_COUNT_1_BIT",
                                    "VK_SAMPLE_COUNT_4_BIT"
                                ],
                                "framebufferDepthSampleCounts": [
                                    "VK_SAMPLE_COUNT_1_BIT",
                                    "VK_SAMPLE_COUNT_4_BIT"
                                ],
                                "framebufferStencilSampleCounts": [
                                    "VK_SAMPLE_COUNT_1_BIT",
                                    "VK_SAMPLE_COUNT_4_BIT"
                                ],
                                "framebufferNoAttachmentsSampleCounts": [
                                    "VK_SAMPLE_COUNT_1_BIT",
                                    "VK_SAMPLE_COUNT_4_BIT"
                                ],
                                "maxColorAttachments": 4,
                                "sampledImageColorSampleCounts": [
                                    "VK_SAMPLE_COUNT_1_BIT",
                                    "VK_SAMPLE_COUNT_4_BIT"
                                ],
                                "sampledImageIntegerSampleCounts": [
                                    "VK_SAMPLE_COUNT_1_BIT",
                                    "VK_SAMPLE_COUNT_4_BIT"
                                ],
                                "sampledImageDepthSampleCounts": [
                                    "VK_SAMPLE_COUNT_1_BIT",
                                    "VK_SAMPLE_COUNT_4_BIT"
                                ],
                                "sampledImageStencilSampleCounts": [
                                    "VK_SAMPLE_COUNT_1_BIT",
                                    "VK_SAMPLE_COUNT_4_BIT"
                                ],
                                "storageImageSampleCounts": [
                                    "VK_SAMPLE_COUNT_1_BIT"
                                ],
                                "maxSampleMaskWords": 1,
                                "maxClipDistances": 0,
                                "maxCullDistances": 0,
                                "maxCombinedClipAndCullDistances": 0,
                                "discreteQueuePriorities": 2,
                                "pointSizeRange": [
                                    1.0,
                                    1.0
                                ],
                                "lineWidthRange": [
                                    1.0,
                                    1.0
                                ],
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
                            "subgroupSupportedStages": [
                                "VK_SHADER_STAGE_COMPUTE_BIT"
                            ],
                            "subgroupSupportedOperations": [
                                "VK_SUBGROUP_FEATURE_BASIC_BIT"
                            ],
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
                            "framebufferIntegerColorSampleCounts": [
                                "VK_SAMPLE_COUNT_1_BIT"
                            ]
                        }
                    }
                }
            }
        }"""

        try:
            original_data = json.loads(original_json_text)
            expected_data = json.loads(expected_json_text)
        except json.JSONDecodeError as e:
            print(f"JSON syntax is incorrect: {e.msg} at line {e.lineno}, column {e.colno}")

        json_files_dict = {"test_profile.json": original_data}
        pull_required_capabilities_profiles_files(self.vk, json_files_dict)

        self.assertEqual(json_files_dict["test_profile.json"], expected_data)

    def test_pull_required_capabilities_empty_vulkan13(self):
        """
        Verifies that an empty Vulkan 1.3 profile pulls required Vulkan 1.0 through 1.3 core features and properties.
        """
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
                                "maxComputeWorkGroupCount": [
                                    65535,
                                    65535,
                                    65535
                                ],
                                "maxComputeWorkGroupInvocations": 128,
                                "maxComputeWorkGroupSize": [
                                    128,
                                    128,
                                    64
                                ],
                                "subPixelPrecisionBits": 4,
                                "subTexelPrecisionBits": 4,
                                "mipmapPrecisionBits": 4,
                                "maxDrawIndexedIndexValue": 16777216,
                                "maxDrawIndirectCount": 1,
                                "maxSamplerLodBias": 2,
                                "maxSamplerAnisotropy": 1,
                                "maxViewports": 1,
                                "maxViewportDimensions": [
                                    4096,
                                    4096
                                ],
                                "viewportBoundsRange": [
                                    -8192,
                                    8192
                                ],
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
                                "framebufferColorSampleCounts": [
                                    "VK_SAMPLE_COUNT_1_BIT",
                                    "VK_SAMPLE_COUNT_4_BIT"
                                ],
                                "framebufferDepthSampleCounts": [
                                    "VK_SAMPLE_COUNT_1_BIT",
                                    "VK_SAMPLE_COUNT_4_BIT"
                                ],
                                "framebufferStencilSampleCounts": [
                                    "VK_SAMPLE_COUNT_1_BIT",
                                    "VK_SAMPLE_COUNT_4_BIT"
                                ],
                                "framebufferNoAttachmentsSampleCounts": [
                                    "VK_SAMPLE_COUNT_1_BIT",
                                    "VK_SAMPLE_COUNT_4_BIT"
                                ],
                                "maxColorAttachments": 4,
                                "sampledImageColorSampleCounts": [
                                    "VK_SAMPLE_COUNT_1_BIT",
                                    "VK_SAMPLE_COUNT_4_BIT"
                                ],
                                "sampledImageIntegerSampleCounts": [
                                    "VK_SAMPLE_COUNT_1_BIT",
                                    "VK_SAMPLE_COUNT_4_BIT"
                                ],
                                "sampledImageDepthSampleCounts": [
                                    "VK_SAMPLE_COUNT_1_BIT",
                                    "VK_SAMPLE_COUNT_4_BIT"
                                ],
                                "sampledImageStencilSampleCounts": [
                                    "VK_SAMPLE_COUNT_1_BIT",
                                    "VK_SAMPLE_COUNT_4_BIT"
                                ],
                                "storageImageSampleCounts": [
                                    "VK_SAMPLE_COUNT_1_BIT"
                                ],
                                "maxSampleMaskWords": 1,
                                "maxClipDistances": 0,
                                "maxCullDistances": 0,
                                "maxCombinedClipAndCullDistances": 0,
                                "discreteQueuePriorities": 2,
                                "pointSizeRange": [
                                    1.0,
                                    1.0
                                ],
                                "lineWidthRange": [
                                    1.0,
                                    1.0
                                ],
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
                            "subgroupSupportedStages": [
                                "VK_SHADER_STAGE_COMPUTE_BIT"
                            ],
                            "subgroupSupportedOperations": [
                                "VK_SUBGROUP_FEATURE_BASIC_BIT"
                            ],
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
                            "framebufferIntegerColorSampleCounts": [
                                "VK_SAMPLE_COUNT_1_BIT"
                            ]
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

        try:
            original_data = json.loads(original_json_text)
            expected_data = json.loads(expected_json_text)
        except json.JSONDecodeError as e:
            print(f"JSON syntax is incorrect: {e.msg} at line {e.lineno}, column {e.colno}")

        json_files_dict = {"test_profile.json": original_data}
        pull_required_capabilities_profiles_files(self.vk, json_files_dict)

        self.assertEqual(json_files_dict["test_profile.json"], expected_data)

    def test_pull_required_capabilities_empty_vulkan14(self):
        """
        Verifies that an empty Vulkan 1.4 profile pulls required Vulkan 1.0 through 1.4 core features and properties.
        """
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
                                "maxComputeWorkGroupCount": [
                                    65535,
                                    65535,
                                    65535
                                ],
                                "maxComputeWorkGroupInvocations": 256,
                                "maxComputeWorkGroupSize": [
                                    256,
                                    256,
                                    64
                                ],
                                "subPixelPrecisionBits": 4,
                                "subTexelPrecisionBits": 8,
                                "mipmapPrecisionBits": 6,
                                "maxDrawIndexedIndexValue": 16777216,
                                "maxDrawIndirectCount": 1,
                                "maxSamplerLodBias": 14,
                                "maxSamplerAnisotropy": 1,
                                "maxViewports": 1,
                                "maxViewportDimensions": [
                                    7680,
                                    7680
                                ],
                                "viewportBoundsRange": [
                                    -15360,
                                    15359
                                ],
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
                                "framebufferColorSampleCounts": [
                                    "VK_SAMPLE_COUNT_1_BIT",
                                    "VK_SAMPLE_COUNT_4_BIT"
                                ],
                                "framebufferDepthSampleCounts": [
                                    "VK_SAMPLE_COUNT_1_BIT",
                                    "VK_SAMPLE_COUNT_4_BIT"
                                ],
                                "framebufferStencilSampleCounts": [
                                    "VK_SAMPLE_COUNT_1_BIT",
                                    "VK_SAMPLE_COUNT_4_BIT"
                                ],
                                "framebufferNoAttachmentsSampleCounts": [
                                    "VK_SAMPLE_COUNT_1_BIT",
                                    "VK_SAMPLE_COUNT_4_BIT"
                                ],
                                "maxColorAttachments": 8,
                                "sampledImageColorSampleCounts": [
                                    "VK_SAMPLE_COUNT_1_BIT",
                                    "VK_SAMPLE_COUNT_4_BIT"
                                ],
                                "sampledImageIntegerSampleCounts": [
                                    "VK_SAMPLE_COUNT_1_BIT",
                                    "VK_SAMPLE_COUNT_4_BIT"
                                ],
                                "sampledImageDepthSampleCounts": [
                                    "VK_SAMPLE_COUNT_1_BIT",
                                    "VK_SAMPLE_COUNT_4_BIT"
                                ],
                                "sampledImageStencilSampleCounts": [
                                    "VK_SAMPLE_COUNT_1_BIT",
                                    "VK_SAMPLE_COUNT_4_BIT"
                                ],
                                "storageImageSampleCounts": [
                                    "VK_SAMPLE_COUNT_1_BIT"
                                ],
                                "maxSampleMaskWords": 1,
                                "maxClipDistances": 0,
                                "maxCullDistances": 0,
                                "maxCombinedClipAndCullDistances": 0,
                                "discreteQueuePriorities": 2,
                                "pointSizeRange": [
                                    1.0,
                                    256.0
                                ],
                                "lineWidthRange": [
                                    1.0,
                                    1.0
                                ],
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
                            "subgroupSupportedStages": [
                                "VK_SHADER_STAGE_COMPUTE_BIT",
                                "VK_SHADER_STAGE_FRAGMENT_BIT"
                            ],
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
                            "framebufferIntegerColorSampleCounts": [
                                "VK_SAMPLE_COUNT_1_BIT"
                            ]
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

        try:
            original_data = json.loads(original_json_text)
            expected_data = json.loads(expected_json_text)
        except json.JSONDecodeError as e:
            print(f"JSON syntax is incorrect: {e.msg} at line {e.lineno}, column {e.colno}")

        json_files_dict = {"test_profile.json": original_data}
        pull_required_capabilities_profiles_files(self.vk, json_files_dict)

        self.assertEqual(json_files_dict["test_profile.json"], expected_data)


    def test_pull_required_capabilities_profile_inheritance_vulkan10_to_vulkan11(self):
        """
        Verifies core capability pulling when a Vulkan 1.1 child profile inherits from a Vulkan 1.0 parent profile.
        The Vulkan 1.0 parent profile pulls Vulkan 1.0 core capabilities, and a transition block
        ('VP_TEST_profile_v10_to_vulkan11') is dynamically generated for the Vulkan 1.1 child profile.
        """
        original_json_text = """{
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8.0-106.json#",
            "profiles": {
                "VP_TEST_profile_v10": {
                    "version": 1,
                    "api-version": "1.0.68",
                    "capabilities": ["baseline_v10"]
                },
                "VP_TEST_profile_v11": {
                    "version": 1,
                    "api-version": "1.1.108",
                    "profiles": ["VP_TEST_profile_v10"],
                    "capabilities": ["baseline_v11"]
                }
            },
            "capabilities": {
                "baseline_v10": {},
                "baseline_v11": {}
            }
        }"""

        expected_json_text = """{
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8.0-106.json#",
            "profiles": {
                "VP_TEST_profile_v10": {
                    "version": 1,
                    "api-version": "1.0.68",
                    "capabilities": ["baseline_v10"]
                },
                "VP_TEST_profile_v11": {
                    "version": 1,
                    "api-version": "1.1.108",
                    "profiles": ["VP_TEST_profile_v10"],
                    "capabilities": [
                        "VP_TEST_profile_v10_to_vulkan11",
                        "baseline_v11"
                    ]
                }
            },
            "capabilities": {
                "baseline_v10": {
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
                                "maxComputeWorkGroupCount": [
                                    65535,
                                    65535,
                                    65535
                                ],
                                "maxComputeWorkGroupInvocations": 128,
                                "maxComputeWorkGroupSize": [
                                    128,
                                    128,
                                    64
                                ],
                                "subPixelPrecisionBits": 4,
                                "subTexelPrecisionBits": 4,
                                "mipmapPrecisionBits": 4,
                                "maxDrawIndexedIndexValue": 16777216,
                                "maxDrawIndirectCount": 1,
                                "maxSamplerLodBias": 2,
                                "maxSamplerAnisotropy": 1,
                                "maxViewports": 1,
                                "maxViewportDimensions": [
                                    4096,
                                    4096
                                ],
                                "viewportBoundsRange": [
                                    -8192,
                                    8192
                                ],
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
                                "framebufferColorSampleCounts": [
                                    "VK_SAMPLE_COUNT_1_BIT",
                                    "VK_SAMPLE_COUNT_4_BIT"
                                ],
                                "framebufferDepthSampleCounts": [
                                    "VK_SAMPLE_COUNT_1_BIT",
                                    "VK_SAMPLE_COUNT_4_BIT"
                                ],
                                "framebufferStencilSampleCounts": [
                                    "VK_SAMPLE_COUNT_1_BIT",
                                    "VK_SAMPLE_COUNT_4_BIT"
                                ],
                                "framebufferNoAttachmentsSampleCounts": [
                                    "VK_SAMPLE_COUNT_1_BIT",
                                    "VK_SAMPLE_COUNT_4_BIT"
                                ],
                                "maxColorAttachments": 4,
                                "sampledImageColorSampleCounts": [
                                    "VK_SAMPLE_COUNT_1_BIT",
                                    "VK_SAMPLE_COUNT_4_BIT"
                                ],
                                "sampledImageIntegerSampleCounts": [
                                    "VK_SAMPLE_COUNT_1_BIT",
                                    "VK_SAMPLE_COUNT_4_BIT"
                                ],
                                "sampledImageDepthSampleCounts": [
                                    "VK_SAMPLE_COUNT_1_BIT",
                                    "VK_SAMPLE_COUNT_4_BIT"
                                ],
                                "sampledImageStencilSampleCounts": [
                                    "VK_SAMPLE_COUNT_1_BIT",
                                    "VK_SAMPLE_COUNT_4_BIT"
                                ],
                                "storageImageSampleCounts": [
                                    "VK_SAMPLE_COUNT_1_BIT"
                                ],
                                "maxSampleMaskWords": 1,
                                "maxClipDistances": 0,
                                "maxCullDistances": 0,
                                "maxCombinedClipAndCullDistances": 0,
                                "discreteQueuePriorities": 2,
                                "pointSizeRange": [
                                    1.0,
                                    1.0
                                ],
                                "lineWidthRange": [
                                    1.0,
                                    1.0
                                ],
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
                },
                "VP_TEST_profile_v10_to_vulkan11": {
                    "features": {
                        "VkPhysicalDeviceMultiviewFeatures": {
                            "multiview": true
                        }
                    },
                    "properties": {
                        "VkPhysicalDeviceSubgroupProperties": {
                            "subgroupSize": 1,
                            "supportedStages": [
                                "VK_SHADER_STAGE_COMPUTE_BIT"
                            ],
                            "supportedOperations": [
                                "VK_SUBGROUP_FEATURE_BASIC_BIT"
                            ]
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
                },
                "baseline_v11": {}
            }
        }"""

        try:
            original_data = json.loads(original_json_text)
            expected_data = json.loads(expected_json_text)
        except json.JSONDecodeError as e:
            print(f"JSON syntax is incorrect: {e.msg} at line {e.lineno}, column {e.colno}")

        json_files_dict = {"test_profile.json": original_data}
        pull_required_capabilities_profiles_files(self.vk, json_files_dict)

        self.assertEqual(json_files_dict["test_profile.json"], expected_data)


    def test_pull_required_capabilities_profile_inheritance_vulkan11_to_vulkan12_empty(self):
        """
        Verifies core capability pulling when a Vulkan 1.2 child profile inherits from a Vulkan 1.1 parent profile.
        A transition block ('VP_TEST_profile_v11_to_vulkan12') is dynamically generated for the Vulkan 1.2 child profile,
        pulling Vulkan 1.2 core features and Vulkan 1.2 core properties (VkPhysicalDeviceVulkan11Properties is omitted
        as its values are already specified via split structures in Vulkan 1.1).
        """
        original_json_text = """{
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8.0-131.json#",
            "profiles": {
                "VP_TEST_profile_v11": {
                    "version": 1,
                    "api-version": "1.1.108",
                    "capabilities": ["baseline_v11"]
                },
                "VP_TEST_profile_v12": {
                    "version": 1,
                    "api-version": "1.2.131",
                    "profiles": ["VP_TEST_profile_v11"],
                    "capabilities": ["baseline_v12"]
                }
            },
            "capabilities": {
                "baseline_v11": {},
                "baseline_v12": {}
            }
        }"""

        expected_json_text = """{
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8.0-131.json#",
            "profiles": {
                "VP_TEST_profile_v11": {
                    "version": 1,
                    "api-version": "1.1.108",
                    "capabilities": ["baseline_v11"]
                },
                "VP_TEST_profile_v12": {
                    "version": 1,
                    "api-version": "1.2.131",
                    "profiles": ["VP_TEST_profile_v11"],
                    "capabilities": [
                        "VP_TEST_profile_v11_to_vulkan12",
                        "baseline_v12"
                    ]
                }
            },
            "capabilities": {
                "baseline_v11": {
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
                                "maxComputeWorkGroupCount": [
                                    65535,
                                    65535,
                                    65535
                                ],
                                "maxComputeWorkGroupInvocations": 128,
                                "maxComputeWorkGroupSize": [
                                    128,
                                    128,
                                    64
                                ],
                                "subPixelPrecisionBits": 4,
                                "subTexelPrecisionBits": 4,
                                "mipmapPrecisionBits": 4,
                                "maxDrawIndexedIndexValue": 16777216,
                                "maxDrawIndirectCount": 1,
                                "maxSamplerLodBias": 2,
                                "maxSamplerAnisotropy": 1,
                                "maxViewports": 1,
                                "maxViewportDimensions": [
                                    4096,
                                    4096
                                ],
                                "viewportBoundsRange": [
                                    -8192,
                                    8192
                                ],
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
                                "framebufferColorSampleCounts": [
                                    "VK_SAMPLE_COUNT_1_BIT",
                                    "VK_SAMPLE_COUNT_4_BIT"
                                ],
                                "framebufferDepthSampleCounts": [
                                    "VK_SAMPLE_COUNT_1_BIT",
                                    "VK_SAMPLE_COUNT_4_BIT"
                                ],
                                "framebufferStencilSampleCounts": [
                                    "VK_SAMPLE_COUNT_1_BIT",
                                    "VK_SAMPLE_COUNT_4_BIT"
                                ],
                                "framebufferNoAttachmentsSampleCounts": [
                                    "VK_SAMPLE_COUNT_1_BIT",
                                    "VK_SAMPLE_COUNT_4_BIT"
                                ],
                                "maxColorAttachments": 4,
                                "sampledImageColorSampleCounts": [
                                    "VK_SAMPLE_COUNT_1_BIT",
                                    "VK_SAMPLE_COUNT_4_BIT"
                                ],
                                "sampledImageIntegerSampleCounts": [
                                    "VK_SAMPLE_COUNT_1_BIT",
                                    "VK_SAMPLE_COUNT_4_BIT"
                                ],
                                "sampledImageDepthSampleCounts": [
                                    "VK_SAMPLE_COUNT_1_BIT",
                                    "VK_SAMPLE_COUNT_4_BIT"
                                ],
                                "sampledImageStencilSampleCounts": [
                                    "VK_SAMPLE_COUNT_1_BIT",
                                    "VK_SAMPLE_COUNT_4_BIT"
                                ],
                                "storageImageSampleCounts": [
                                    "VK_SAMPLE_COUNT_1_BIT"
                                ],
                                "maxSampleMaskWords": 1,
                                "maxClipDistances": 0,
                                "maxCullDistances": 0,
                                "maxCombinedClipAndCullDistances": 0,
                                "discreteQueuePriorities": 2,
                                "pointSizeRange": [
                                    1.0,
                                    1.0
                                ],
                                "lineWidthRange": [
                                    1.0,
                                    1.0
                                ],
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
                            "supportedStages": [
                                "VK_SHADER_STAGE_COMPUTE_BIT"
                            ],
                            "supportedOperations": [
                                "VK_SUBGROUP_FEATURE_BASIC_BIT"
                            ]
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
                },
                "VP_TEST_profile_v11_to_vulkan12": {
                    "features": {
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
                            "framebufferIntegerColorSampleCounts": [
                                "VK_SAMPLE_COUNT_1_BIT"
                            ]
                        }
                    }
                },
                "baseline_v12": {}
            }
        }"""

        try:
            original_data = json.loads(original_json_text)
            expected_data = json.loads(expected_json_text)
        except json.JSONDecodeError as e:
            print(f"JSON syntax is incorrect: {e.msg} at line {e.lineno}, column {e.colno}")

        json_files_dict = {"test_profile.json": original_data}
        pull_required_capabilities_profiles_files(self.vk, json_files_dict)

        self.assertEqual(json_files_dict["test_profile.json"], expected_data)

if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument(
        '--registry', '-r', action='store', required=False,
        help='Use specified registry file instead of vk.xml.'
    )

    args, unparsed = parser.parse_known_args()
    TestConvertPullRequiredCapabilities.registry_path = args.registry

    unittest.main(argv=[sys.argv[0]] + unparsed)
    