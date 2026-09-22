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
from source.transform_utils import PullBits
from source.transform_pull_required_capabilities import pull_required_capabilities_profiles_files
from source.transform_sort import sort_profiles_files


class TestConvertPullRequiredCapabilitiesCore14(unittest.TestCase):
    registry_path = None

    def setUp(self):
        self.vk: VulkanObject = initVulkanObject('vulkan', self.registry_path)

    def assertProfileDataEqual(self, gen_data: dict, exp_data: dict):
        diffs = []

        def compare_obj(gen, exp, path=""):
            if type(gen) is not type(exp):
                diffs.append(f"Type mismatch at '{path}': expected {type(exp).__name__}, got {type(gen).__name__}")
                return

            if isinstance(exp, dict):
                gen_keys = list(gen.keys())
                exp_keys = list(exp.keys())
                if gen_keys != exp_keys:
                    diffs.append(
                        f"Key list/order mismatch at '{path}':\n"
                        f"    Expected ({len(exp_keys)}): {exp_keys}\n"
                        f"    Got      ({len(gen_keys)}): {gen_keys}"
                    )

                all_keys = list(dict.fromkeys(exp_keys + gen_keys))
                for k in all_keys:
                    child_path = f"{path}.{k}" if path else k
                    if k not in gen:
                        diffs.append(f"Missing key at '{child_path}': expected in generated data")
                    elif k not in exp:
                        diffs.append(f"Extra key at '{child_path}': not present in expected data")
                    else:
                        compare_obj(gen[k], exp[k], child_path)

            elif isinstance(exp, list):
                if gen != exp:
                    diffs.append(
                        f"List value mismatch at '{path}':\n"
                        f"    Expected: {exp}\n"
                        f"    Got:      {gen}"
                    )
            else:
                if gen != exp:
                    diffs.append(f"Value mismatch at '{path}': expected {repr(exp)}, got {repr(gen)}")

        compare_obj(gen_data, exp_data, "root")

        if diffs:
            msg = f"\nFound {len(diffs)} difference(s) between generated profile data and expected JSON:\n" + "\n".join(f"  [{i+1}] {d}" for i, d in enumerate(diffs))
            self.fail(msg)


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
                            "largePoints": true,
                            "samplerAnisotropy": true,
                            "fragmentStoresAndAtomics": true,
                            "shaderImageGatherExtended": true,
                            "shaderStorageImageExtendedFormats": true,
                            "shaderUniformBufferArrayDynamicIndexing": true,
                            "shaderSampledImageArrayDynamicIndexing": true,
                            "shaderStorageBufferArrayDynamicIndexing": true,
                            "shaderStorageImageArrayDynamicIndexing": true,
                            "shaderInt16": true
                        },
                        "VkPhysicalDeviceVulkan11Features": {
                            "storageBuffer16BitAccess": true,
                            "multiview": true,
                            "variablePointersStorageBuffer": true,
                            "variablePointers": true,
                            "samplerYcbcrConversion": true
                        },
                        "VkPhysicalDeviceVulkan12Features": {
                            "samplerMirrorClampToEdge": true,
                            "storageBuffer8BitAccess": true,
                            "shaderInt8": true,
                            "shaderUniformTexelBufferArrayDynamicIndexing": true,
                            "shaderStorageTexelBufferArrayDynamicIndexing": true,
                            "scalarBlockLayout": true,
                            "imagelessFramebuffer": true,
                            "uniformBufferStandardLayout": true,
                            "shaderSubgroupExtendedTypes": true,
                            "separateDepthStencilLayouts": true,
                            "hostQueryReset": true,
                            "timelineSemaphore": true,
                            "bufferDeviceAddress": true,
                            "vulkanMemoryModel": true,
                            "vulkanMemoryModelDeviceScope": true,
                            "subgroupBroadcastDynamicId": true
                        },
                        "VkPhysicalDeviceVulkan13Features": {
                            "robustImageAccess": true,
                            "inlineUniformBlock": true,
                            "pipelineCreationCacheControl": true,
                            "privateData": true,
                            "shaderDemoteToHelperInvocation": true,
                            "shaderTerminateInvocation": true,
                            "subgroupSizeControl": true,
                            "computeFullSubgroups": true,
                            "synchronization2": true,
                            "shaderZeroInitializeWorkgroupMemory": true,
                            "dynamicRendering": true,
                            "shaderIntegerDotProduct": true,
                            "maintenance4": true
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
                            "dynamicRenderingLocalRead": true,
                            "maintenance5": true,
                            "maintenance6": true,
                            "pipelineRobustness": true,
                            "pushDescriptor": true
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
                                "maxFragmentInputComponents": 64,
                                "maxFragmentOutputAttachments": 4,
                                "maxFragmentCombinedOutputResources": 16,
                                "maxComputeSharedMemorySize": 16384,
                                "maxComputeWorkGroupCount": [65535, 65535, 65535],
                                "maxComputeWorkGroupInvocations": 256,
                                "maxComputeWorkGroupSize": [256, 256, 64],
                                "subPixelPrecisionBits": 4,
                                "subTexelPrecisionBits": 8,
                                "mipmapPrecisionBits": 6,
                                "maxSamplerLodBias": 14.0,
                                "maxViewportDimensions": [7680, 7680],
                                "viewportBoundsRange": [-15360.0, 15359.0],
                                "minMemoryMapAlignment": 64,
                                "minTexelBufferOffsetAlignment": 256,
                                "minUniformBufferOffsetAlignment": 256,
                                "minStorageBufferOffsetAlignment": 256,
                                "minTexelOffset": -8,
                                "maxTexelOffset": 7,
                                "maxFramebufferWidth": 7680,
                                "maxFramebufferHeight": 7680,
                                "maxFramebufferLayers": 256,
                                "framebufferColorSampleCounts": ["VK_SAMPLE_COUNT_1_BIT", "VK_SAMPLE_COUNT_4_BIT"],
                                "framebufferDepthSampleCounts": ["VK_SAMPLE_COUNT_1_BIT", "VK_SAMPLE_COUNT_4_BIT"],
                                "framebufferStencilSampleCounts": ["VK_SAMPLE_COUNT_1_BIT", "VK_SAMPLE_COUNT_4_BIT"],
                                "framebufferNoAttachmentsSampleCounts": ["VK_SAMPLE_COUNT_1_BIT", "VK_SAMPLE_COUNT_4_BIT"],
                                "maxColorAttachments": 8,
                                "sampledImageColorSampleCounts": ["VK_SAMPLE_COUNT_1_BIT", "VK_SAMPLE_COUNT_4_BIT"],
                                "sampledImageIntegerSampleCounts": ["VK_SAMPLE_COUNT_1_BIT"],
                                "sampledImageDepthSampleCounts": ["VK_SAMPLE_COUNT_1_BIT", "VK_SAMPLE_COUNT_4_BIT"],
                                "sampledImageStencilSampleCounts": ["VK_SAMPLE_COUNT_1_BIT", "VK_SAMPLE_COUNT_4_BIT"],
                                "maxSampleMaskWords": 1,
                                "discreteQueuePriorities": 2,
                                "nonCoherentAtomSize": 256,
                                "timestampComputeAndGraphics": true,
                                "pointSizeRange": [1.0, 256.0],
                                "pointSizeGranularity": 0.125,
                                "lineWidthGranularity": 0.5,
                                "standardSampleLocations": true
                            },
                            "sparseProperties": {
                                "residencyStandard2DBlockShape": false,
                                "residencyStandard2DMultisampleBlockShape": false,
                                "residencyStandard3DBlockShape": false,
                                "residencyAlignedMipSize": true,
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
                            "subgroupQuadOperationsInAllStages": false,
                            "maxMultiviewViewCount": 6,
                            "maxMultiviewInstanceIndex": 134217727,
                            "protectedNoFault": false,
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
                            "shaderUniformBufferArrayNonUniformIndexingNative": false,
                            "shaderSampledImageArrayNonUniformIndexingNative": false,
                            "shaderStorageBufferArrayNonUniformIndexingNative": false,
                            "shaderStorageImageArrayNonUniformIndexingNative": false,
                            "shaderInputAttachmentArrayNonUniformIndexingNative": false,
                            "robustBufferAccessUpdateAfterBind": false,
                            "quadDivergentImplicitLod": false,
                            "supportedDepthResolveModes": ["VK_RESOLVE_MODE_SAMPLE_ZERO_BIT"],
                            "supportedStencilResolveModes": ["VK_RESOLVE_MODE_SAMPLE_ZERO_BIT"],
                            "independentResolveNone": false,
                            "independentResolve": false,
                            "filterMinmaxImageComponentMapping": false,
                            "maxTimelineSemaphoreValueDifference": 2147483647,
                            "framebufferIntegerColorSampleCounts": ["VK_SAMPLE_COUNT_1_BIT"]
                        },
                        "VkPhysicalDeviceVulkan13Properties": {
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
                            "lineSubPixelPrecisionBits": 4,
                            "maxVertexAttribDivisor": 2147483647,
                            "supportsNonZeroFirstInstance": true,
                            "maxPushDescriptors": 32,
                            "dynamicRenderingLocalReadDepthStencilAttachments": false,
                            "dynamicRenderingLocalReadMultisampledAttachments": false,
                            "earlyFragmentMultisampleCoverageAfterSampleCounting": false,
                            "earlyFragmentSampleMaskTestBeforeSampleCounting": false,
                            "depthStencilSwizzleOneSupport": false,
                            "polygonModePointSize": false,
                            "nonStrictSinglePixelWideLinesUseParallelogram": false,
                            "nonStrictWideLinesUseParallelogram": false,
                            "blockTexelViewCompatibleMultipleLayers": false,
                            "maxCombinedImageSamplerDescriptorCount": 1,
                            "fragmentShadingRateClampCombinerInputs": false,
                            "copySrcLayoutCount": 0,
                            "pCopySrcLayouts": [],
                            "copyDstLayoutCount": 0,
                            "pCopyDstLayouts": [],
                            "identicalMemoryTypeRequirements": false
                        }
                    }
                }
            }
        }"""

        json_files_dict = {"test_profile.json": json.loads(original_json_text)}
        pull_required_capabilities_profiles_files(
            self.vk, 
            [PullBits.REQUIRED_CAPABILITIES, PullBits.IGNORE_UNSUPPORTED], 
            json_files_dict
        )
        sort_profiles_files(self.vk, json_files_dict)
        self.assertProfileDataEqual(json_files_dict["test_profile.json"], json.loads(expected_json_text))


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument(
        '--registry', '-r', action='store', required=False,
        help='Use specified registry file instead of vk.xml.'
    )

    args, unparsed = parser.parse_known_args()
    TestConvertPullRequiredCapabilitiesCore14.registry_path = args.registry

    unittest.main(argv=[sys.argv[0]] + unparsed)