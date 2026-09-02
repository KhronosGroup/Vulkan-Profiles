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
from source.main_convert import sort_profiles_files


class TestConvertSort(unittest.TestCase):
    registry_path = None

    def setUp(self):
        self.vk: VulkanObject = initVulkanObject('vulkan', self.registry_path)

    def test_sort_capability_structures(self):
        """
        Verifies that feature and property structures inside capability blocks are sorted in the order:
        Core Bundles (1.0 -> 1.4) -> Core Split Structures (1.0 -> 1.4, alphabetical per version) ->
        KHR structures (alphabetical) -> EXT structures (alphabetical) -> Vendor structures (alphabetical).
        """
        original_json_text = """{
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8.0-304.json#",
            "profiles": {
                "VP_TEST_sort_structs": {
                    "version": 1,
                    "api-version": "1.4.304",
                    "capabilities": ["caps_unsorted"]
                }
            },
            "capabilities": {
                "caps_unsorted": {
                    "extensions": {
                        "VK_NV_mesh_shader": 1,
                        "VK_NV_corner_sampled_image": 1,
                        "VK_EXT_transform_feedback": 1,
                        "VK_KHR_shader_subgroup_rotate": 1,
                        "VK_KHR_line_rasterization": 1
                    },
                    "features": {
                        "VkPhysicalDeviceCornerSampledImageFeaturesNV": {
                            "cornerSampledImage": true
                        },
                        "VkPhysicalDeviceTransformFeedbackFeaturesEXT": {
                            "transformFeedback": true
                        },
                        "VkPhysicalDeviceVulkan12Features": {
                            "timelineSemaphore": true
                        },
                        "VkPhysicalDeviceShaderSubgroupRotateFeaturesKHR": {
                            "shaderSubgroupRotate": true
                        },
                        "VkPhysicalDeviceFeatures": {
                            "robustBufferAccess": true
                        },
                        "VkPhysicalDeviceMultiviewFeatures": {
                            "multiview": true
                        },
                        "VkPhysicalDeviceVulkan14Features": {
                            "pushDescriptor": true
                        }
                    },
                    "properties": {
                        "VkPhysicalDeviceSubgroupProperties": {
                            "subgroupSize": 1
                        },
                        "VkPhysicalDeviceMeshShaderPropertiesNV": {
                            "maxTaskWorkGroupSize": [32, 1, 1]
                        },
                        "VkPhysicalDeviceVulkan11Properties": {
                            "maxMultiviewViewCount": 6
                        },
                        "VkPhysicalDeviceTransformFeedbackPropertiesEXT": {
                            "maxTransformFeedbackStreams": 4
                        },
                        "VkPhysicalDeviceMaintenance3Properties": {
                            "maxPerSetDescriptors": 1024
                        },
                        "VkPhysicalDeviceLineRasterizationPropertiesKHR": {
                            "lineSubPixelPrecisionBits": 8
                        },
                        "VkPhysicalDeviceProperties": {
                            "limits": {
                                "maxImageDimension2D": 4096
                            }
                        }
                    }
                }
            }
        }"""

        expected_json_text = """{
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8.0-304.json#",
            "profiles": {
                "VP_TEST_sort_structs": {
                    "version": 1,
                    "api-version": "1.4.304",
                    "capabilities": ["caps_unsorted"]
                }
            },
            "capabilities": {
                "caps_unsorted": {
                    "extensions": {
                        "VK_KHR_line_rasterization": 1,
                        "VK_KHR_shader_subgroup_rotate": 1,
                        "VK_EXT_transform_feedback": 1,
                        "VK_NV_corner_sampled_image": 1,
                        "VK_NV_mesh_shader": 1
                    },
                    "features": {
                        "VkPhysicalDeviceFeatures": {
                            "robustBufferAccess": true
                        },
                        "VkPhysicalDeviceVulkan12Features": {
                            "timelineSemaphore": true
                        },
                        "VkPhysicalDeviceVulkan14Features": {
                            "pushDescriptor": true
                        },
                        "VkPhysicalDeviceMultiviewFeatures": {
                            "multiview": true
                        },
                        "VkPhysicalDeviceShaderSubgroupRotateFeaturesKHR": {
                            "shaderSubgroupRotate": true
                        },
                        "VkPhysicalDeviceTransformFeedbackFeaturesEXT": {
                            "transformFeedback": true
                        },
                        "VkPhysicalDeviceCornerSampledImageFeaturesNV": {
                            "cornerSampledImage": true
                        }
                    },
                    "properties": {
                        "VkPhysicalDeviceProperties": {
                            "limits": {
                                "maxImageDimension2D": 4096
                            }
                        },
                        "VkPhysicalDeviceVulkan11Properties": {
                            "maxMultiviewViewCount": 6
                        },
                        "VkPhysicalDeviceMaintenance3Properties": {
                            "maxPerSetDescriptors": 1024
                        },
                        "VkPhysicalDeviceSubgroupProperties": {
                            "subgroupSize": 1
                        },
                        "VkPhysicalDeviceLineRasterizationPropertiesKHR": {
                            "lineSubPixelPrecisionBits": 8
                        },
                        "VkPhysicalDeviceTransformFeedbackPropertiesEXT": {
                            "maxTransformFeedbackStreams": 4
                        },
                        "VkPhysicalDeviceMeshShaderPropertiesNV": {
                            "maxTaskWorkGroupSize": [32, 1, 1]
                        }
                    }
                }
            }
        }"""

        json_files_dict = {"test_profile.json": json.loads(original_json_text)}
        sort_profiles_files(self.vk, json_files_dict)

        gen_block = json_files_dict["test_profile.json"]["capabilities"]["caps_unsorted"]
        exp_block = json.loads(expected_json_text)["capabilities"]["caps_unsorted"]

        self.assertEqual(list(gen_block["features"].keys()), list(exp_block["features"].keys()))
        self.assertEqual(list(gen_block["properties"].keys()), list(exp_block["properties"].keys()))
        self.assertEqual(json_files_dict["test_profile.json"], json.loads(expected_json_text))

    def test_sort_extensions_priority_and_dependencies(self):
        """
        Verifies that extensions inside capability blocks are sorted with topological dependencies respected
        (prerequisites listed before dependents) and independent extensions prioritized as KHR -> EXT -> Vendor (alphabetical).
        """
        original_json_text = """{
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8.0-304.json#",
            "profiles": {
                "VP_TEST_sort_exts": {
                    "version": 1,
                    "api-version": "1.0.68",
                    "capabilities": ["caps_exts"]
                }
            },
            "capabilities": {
                "caps_exts": {
                    "extensions": {
                        "VK_NV_mesh_shader": 1,
                        "VK_KHR_swapchain": 1,
                        "VK_EXT_transform_feedback": 1,
                        "VK_KHR_surface": 1,
                        "VK_KHR_dynamic_rendering": 1
                    }
                }
            }
        }"""

        expected_json_text = """{
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8.0-304.json#",
            "profiles": {
                "VP_TEST_sort_exts": {
                    "version": 1,
                    "api-version": "1.0.68",
                    "capabilities": ["caps_exts"]
                }
            },
            "capabilities": {
                "caps_exts": {
                    "extensions": {
                        "VK_KHR_dynamic_rendering": 1,
                        "VK_KHR_surface": 1,
                        "VK_KHR_swapchain": 1,
                        "VK_EXT_transform_feedback": 1,
                        "VK_NV_mesh_shader": 1
                    }
                }
            }
        }"""

        json_files_dict = {"test_profile.json": json.loads(original_json_text)}
        sort_profiles_files(self.vk, json_files_dict)

        gen_exts = list(json_files_dict["test_profile.json"]["capabilities"]["caps_exts"]["extensions"].keys())
        exp_exts = list(json.loads(expected_json_text)["capabilities"]["caps_exts"]["extensions"].keys())

        self.assertEqual(gen_exts, exp_exts)
        self.assertEqual(json_files_dict["test_profile.json"], json.loads(expected_json_text))

    def test_sort_format_properties_structures(self):
        """
        Verifies that format feature structures inside formats capability blocks are sorted according to
        structure sorting rules (Core -> KHR -> EXT) for a Vulkan 1.4 profile, including extended format flags extensions.
        """
        original_json_text = """{
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8.0-304.json#",
            "profiles": {
                "VP_TEST_sort_formats": {
                    "version": 1,
                    "api-version": "1.4.304",
                    "capabilities": ["caps_formats"]
                }
            },
            "capabilities": {
                "caps_formats": {
                    "extensions": {
                        "VK_EXT_image_drm_format_modifier": 2,
                        "VK_KHR_format_feature_flags2": 1
                    },
                    "formats": {
                        "VK_FORMAT_R8G8B8A8_UNORM": {
                            "VkDrmFormatModifierPropertiesListEXT": {
                                "drmFormatModifierCount": 1
                            },
                            "VkFormatProperties3KHR": {
                                "linearTilingFeatures": ["VK_FORMAT_FEATURE_2_SAMPLED_IMAGE_BIT"]
                            },
                            "VkFormatProperties3": {
                                "linearTilingFeatures": ["VK_FORMAT_FEATURE_2_SAMPLED_IMAGE_BIT"]
                            },
                            "VkFormatProperties": {
                                "linearTilingFeatures": ["VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT"]
                            }
                        }
                    }
                }
            }
        }"""

        expected_json_text = """{
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8.0-304.json#",
            "profiles": {
                "VP_TEST_sort_formats": {
                    "version": 1,
                    "api-version": "1.4.304",
                    "capabilities": ["caps_formats"]
                }
            },
            "capabilities": {
                "caps_formats": {
                    "extensions": {
                        "VK_KHR_format_feature_flags2": 1,
                        "VK_EXT_image_drm_format_modifier": 2
                    },
                    "formats": {
                        "VK_FORMAT_R8G8B8A8_UNORM": {
                            "VkFormatProperties": {
                                "linearTilingFeatures": ["VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT"]
                            },
                            "VkFormatProperties3": {
                                "linearTilingFeatures": ["VK_FORMAT_FEATURE_2_SAMPLED_IMAGE_BIT"]
                            },
                            "VkFormatProperties3KHR": {
                                "linearTilingFeatures": ["VK_FORMAT_FEATURE_2_SAMPLED_IMAGE_BIT"]
                            },
                            "VkDrmFormatModifierPropertiesListEXT": {
                                "drmFormatModifierCount": 1
                            }
                        }
                    }
                }
            }
        }"""

        json_files_dict = {"test_profile.json": json.loads(original_json_text)}
        sort_profiles_files(self.vk, json_files_dict)

        gen_fmt_structs = list(json_files_dict["test_profile.json"]["capabilities"]["caps_formats"]["formats"]["VK_FORMAT_R8G8B8A8_UNORM"].keys())
        exp_fmt_structs = list(json.loads(expected_json_text)["capabilities"]["caps_formats"]["formats"]["VK_FORMAT_R8G8B8A8_UNORM"].keys())

        self.assertEqual(gen_fmt_structs, exp_fmt_structs)
        self.assertEqual(json_files_dict["test_profile.json"], json.loads(expected_json_text))


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument(
        '--registry', '-r', action='store', required=False,
        help='Use specified registry file instead of vk.xml.'
    )

    args, unparsed = parser.parse_known_args()
    TestConvertSort.registry_path = args.registry

    unittest.main(argv=[sys.argv[0]] + unparsed)
    