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
    pull_required_capabilities_profiles_files,
    sort_profiles_files
)


class TestConvertPullRequiredCapabilitiesProf(unittest.TestCase):
    registry_path = None

    def setUp(self):
        self.vk: VulkanObject = initVulkanObject('vulkan', self.registry_path)

    def test_pull_required_capabilities_already_present_vulkan10(self):
        """
        Verifies that pulling core requirements onto an existing profile preserves profile capability requirements
        higher than Vulkan core requirements (e.g. maxImageDimension2D = 8192, framebufferColorSampleCounts with 8_BIT)
        while enforcing core baseline minimums (maxImageDimension1D = 4096, 1_BIT sample counts).
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
                "baseline": {
                    "extensions": {
                        "VK_KHR_shader_draw_parameters": 1,
                        "VK_KHR_16bit_storage": 1
                    },
                    "features": {
                        "VkPhysicalDeviceFeatures": {
                            "robustBufferAccess": true,
                            "fullDrawIndexUint32": true,
                            "imageCubeArray": false
                        },
                        "VkPhysicalDevice16BitStorageFeaturesKHR": {
                            "uniformAndStorageBuffer16BitAccess": true
                        }
                    },
                    "properties": {
                        "VkPhysicalDeviceProperties": {
                            "limits": {
                                "maxImageDimension1D": 2048,
                                "maxImageDimension2D": 8192,
                                "maxImageDimension3D": 256,
                                "framebufferColorSampleCounts": [
                                    "VK_SAMPLE_COUNT_4_BIT",
                                    "VK_SAMPLE_COUNT_8_BIT"
                                ]
                            }
                        }
                    }
                }
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
                    "extensions": {
                        "VK_KHR_shader_draw_parameters": 1,
                        "VK_KHR_16bit_storage": 1
                    },
                    "features": {
                        "VkPhysicalDeviceFeatures": {
                            "robustBufferAccess": true,
                            "fullDrawIndexUint32": true,
                            "imageCubeArray": false
                        },
                        "VkPhysicalDevice16BitStorageFeaturesKHR": {
                            "uniformAndStorageBuffer16BitAccess": true,
                            "storageBuffer16BitAccess": true
                        }
                    },
                    "properties": {
                        "VkPhysicalDeviceProperties": {
                            "limits": {
                                "maxImageDimension1D": 4096,
                                "maxImageDimension2D": 8192,
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
                                    "VK_SAMPLE_COUNT_4_BIT",
                                    "VK_SAMPLE_COUNT_8_BIT"
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

        gen_data = json_files_dict["test_profile.json"]
        exp_data = json.loads(expected_json_text)

        # Subsection assertions for granular diagnostics
        self.assertEqual(
            list(gen_data["capabilities"].keys()),
            list(exp_data["capabilities"].keys()),
            "Mismatch in capability block names or order"
        )
        for block_name, exp_block in exp_data["capabilities"].items():
            gen_block = gen_data["capabilities"].get(block_name, {})
            for section in ("features", "properties", "formats"):
                if section in exp_block:
                    self.assertIn(
                        section, 
                        gen_block, 
                        f"Section '{section}' missing in generated capability block '{block_name}'"
                    )
                    self.assertEqual(
                        list(gen_block[section].keys()),
                        list(exp_block[section].keys()),
                        f"Mismatch in structure keys/order for block '{block_name}' section '{section}'"
                    )

        self.assertEqual(gen_data, exp_data)

    def test_pull_required_capabilities_not_ext_vulkan10(self):
        """
        Verifies that when VK_KHR_portability_subset is set, robustness is not added.
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
                "baseline": {
                    "extensions": {
                        "VK_KHR_portability_subset": 1
                    },
                    "properties": {
                        "VkPhysicalDeviceProperties": {
                            "limits": {
                                "maxImageDimension1D": 4096,
                                "maxImageDimension2D": 4096,
                                "maxImageDimension3D": 256
                            }
                        }
                    }
                }
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
                    "extensions": {
                        "VK_KHR_portability_subset": 1
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

        gen_data = json_files_dict["test_profile.json"]
        exp_data = json.loads(expected_json_text)

        # Subsection assertions for granular diagnostics
        self.assertEqual(
            list(gen_data["capabilities"].keys()),
            list(exp_data["capabilities"].keys()),
            "Mismatch in capability block names or order"
        )
        for block_name, exp_block in exp_data["capabilities"].items():
            gen_block = gen_data["capabilities"].get(block_name, {})
            for section in ("features", "properties", "formats"):
                if section in exp_block:
                    self.assertIn(
                        section, 
                        gen_block, 
                        f"Section '{section}' missing in generated capability block '{block_name}'"
                    )
                    self.assertEqual(
                        list(gen_block[section].keys()),
                        list(exp_block[section].keys()),
                        f"Mismatch in structure keys/order for block '{block_name}' section '{section}'"
                    )

        self.assertEqual(gen_data, exp_data)


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument(
        '--registry', '-r', action='store', required=False,
        help='Use specified registry file instead of vk.xml.'
    )

    args, unparsed = parser.parse_known_args()
    TestConvertPullRequiredCapabilitiesProf.registry_path = args.registry

    unittest.main(argv=[sys.argv[0]] + unparsed)
    