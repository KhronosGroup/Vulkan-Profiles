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
    pull_extension_dependencies_profiles_files,
    pull_required_capabilities_profiles_files,
    sort_profiles_files
)


class TestConvertPullRequiredCapabilitiesOR(unittest.TestCase):
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

    def test_pull_required_capabilities_or_extension_feature_requirement(self):
        """
        Case 1: An extension (e.g. VK_KHR_shader_float16_int8) specifies an OR feature requirement
        in vk.xml (e.g., field="shaderFloat16, shaderInt8").
        When neither feature option is pre-enabled in context, pull-required-capabilities
        must NOT auto-pull all OR features into the profile, leaving feature over-specification
        avoided and emitting a diagnostic warning instead.
        """
        original_json_text = """{
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8.0-106.json#",
            "profiles": {
                "VP_TEST_profile_or_ext": {
                    "version": 1,
                    "api-version": "1.0.68",
                    "capabilities": ["baseline"]
                }
            },
            "capabilities": {
                "baseline": {
                    "extensions": {
                        "VK_KHR_shader_float16_int8": 1
                    }
                }
            }
        }"""

        expected_json_text = """{
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8.0-106.json#",
            "profiles": {
                "VP_TEST_profile_or_ext": {
                    "version": 1,
                    "api-version": "1.0.68",
                    "capabilities": ["baseline"]
                }
            },
            "capabilities": {
                "baseline": {
                    "extensions": {
                        "VK_KHR_shader_float16_int8": 1,
                        "VK_KHR_get_physical_device_properties2": 1
                    },
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
        pull_extension_dependencies_profiles_files(self.vk, True, json_files_dict)
        pull_required_capabilities_profiles_files(self.vk, json_files_dict)
        sort_profiles_files(self.vk, json_files_dict)

        gen_baseline = json_files_dict["test_profile.json"]["capabilities"]["baseline"]
        self.assertNotIn("VkPhysicalDeviceShaderFloat16Int8FeaturesKHR", gen_baseline.get("features", {}))
        self.assertProfileDataEqual(json_files_dict["test_profile.json"], json.loads(expected_json_text))

    def test_pull_required_capabilities_or_variant_capability_blocks(self):
        """
        Case 2: Profile includes an array of OR-variant capability blocks:
        "capabilities": [ ["primitivesGeneratedQuery", "pipelineStatisticsQuery"] ]
        Verifies that capabilities conversion handles variant array structures correctly,
        pulling extension dependencies for each block while leaving the variant array intact.
        """
        original_json_text = """{
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8.0-106.json#",
            "profiles": {
                "VP_TEST_profile_variant_blocks": {
                    "version": 1,
                    "api-version": "1.0.68",
                    "capabilities": [
                        ["primitivesGeneratedQuery", "pipelineStatisticsQuery"]
                    ]
                }
            },
            "capabilities": {
                "primitivesGeneratedQuery": {
                    "extensions": {
                        "VK_EXT_primitives_generated_query": 1
                    },
                    "features": {
                        "VkPhysicalDevicePrimitivesGeneratedQueryFeaturesEXT": {
                            "primitivesGeneratedQuery": true
                        }
                    }
                },
                "pipelineStatisticsQuery": {
                    "features": {
                        "VkPhysicalDeviceFeatures": {
                            "pipelineStatisticsQuery": true
                        }
                    }
                }
            }
        }"""

        expected_json_text = """{
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8.0-106.json#",
            "profiles": {
                "VP_TEST_profile_variant_blocks": {
                    "version": 1,
                    "api-version": "1.0.68",
                    "capabilities": [
                        ["primitivesGeneratedQuery", "pipelineStatisticsQuery"]
                    ]
                }
            },
            "capabilities": {
                "primitivesGeneratedQuery": {
                    "extensions": {
                        "VK_EXT_primitives_generated_query": 1,
                        "VK_KHR_get_physical_device_properties2": 1,
                        "VK_EXT_transform_feedback": 1
                    },
                    "features": {
                        "VkPhysicalDeviceFeatures": {
                            "robustBufferAccess": true
                        },
                        "VkPhysicalDevicePrimitivesGeneratedQueryFeaturesEXT": {
                            "primitivesGeneratedQuery": true
                        },
                        "VkPhysicalDeviceTransformFeedbackFeaturesEXT": {
                            "transformFeedback": true
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
                },
                "pipelineStatisticsQuery": {
                    "features": {
                        "VkPhysicalDeviceFeatures": {
                            "pipelineStatisticsQuery": true
                        }
                    }
                }
            }
        }"""

        json_files_dict = {"test_profile.json": json.loads(original_json_text)}
        pull_extension_dependencies_profiles_files(self.vk, True, json_files_dict)
        pull_required_capabilities_profiles_files(self.vk, json_files_dict)
        sort_profiles_files(self.vk, json_files_dict)

        self.assertProfileDataEqual(json_files_dict["test_profile.json"], json.loads(expected_json_text))


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument(
        '--registry', '-r', action='store', required=False,
        help='Use specified registry file instead of vk.xml.'
    )

    args, unparsed = parser.parse_known_args()
    TestConvertPullRequiredCapabilitiesOR.registry_path = args.registry

    unittest.main(argv=[sys.argv[0]] + unparsed)
    