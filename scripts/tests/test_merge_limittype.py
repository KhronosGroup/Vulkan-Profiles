#!/usr/bin/python3
#
# Copyright (c) 2026-2026 Google, Inc.
# Copyright (C) 2026-2026 Valve Corporation
# Copyright (c) 2026-2026 LunarG, Inc.
#
# Licensed under the Apache License, Version 2.0 (the "License")
# xxxx

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
from source.generate_profiles_merge import VulkanProfilesMergeGenerator
from source.json_config import JsonConfig


class TestMergeLimitType(unittest.TestCase):
    registry_path = None

    @classmethod
    def setUpClass(cls):
        cls.vk: VulkanObject = initVulkanObject('vulkan', cls.registry_path)

    def _run_merge(self, json_text_a: str, json_text_b: str, mode: str = "intersection") -> dict:
        json_a = json.loads(json_text_a)
        json_b = json.loads(json_text_b)

        p_config = JsonConfig.__new__(JsonConfig)
        p_config.name = "VP_TEST_merged"
        p_config.version = 1
        p_config.label = "Test merged profile"
        p_config.description = "Test profile"
        p_config.stage = "STABLE"
        p_config.api_version = ["1", "2", "0"]
        p_config.required_profiles = []
        p_config.input_jsons = [json_a, json_b]
        p_config.input_profile_names = ["VP_TEST_a", "VP_TEST_b"]
        p_config.input_profile_values = [json_a["profiles"]["VP_TEST_a"], json_b["profiles"]["VP_TEST_b"]]
        p_config.merge_mode = mode

        profile_file = {
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8-latest.json#",
            "capabilities": {},
            "profiles": {},
            "contributors": {},
            "history": []
        }

        merger = VulkanProfilesMergeGenerator(self.vk)
        merger.merge(p_config, profile_file, mode=mode)

        return profile_file

    def test_merge_limittype_max_and_bits(self):
        """
        Tests 'max' and 'bits' limit types (e.g. maxImageDimension2D, subPixelPrecisionBits).
        - Intersection mode selects min(A, B).
        - Union mode selects max(A, B).
        """
        input_json_a = """{
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8-latest.json#",
            "profiles": {
                "VP_TEST_a": {
                    "version": 1,
                    "api-version": "1.2.0",
                    "capabilities": ["cap_a"]
                }
            },
            "capabilities": {
                "cap_a": {
                    "properties": {
                        "VkPhysicalDeviceProperties": {
                            "limits": {
                                "maxImageDimension2D": 16384,
                                "subPixelPrecisionBits": 8
                            }
                        }
                    }
                }
            }
        }"""

        input_json_b = """{
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8-latest.json#",
            "profiles": {
                "VP_TEST_b": {
                    "version": 1,
                    "api-version": "1.2.0",
                    "capabilities": ["cap_b"]
                }
            },
            "capabilities": {
                "cap_b": {
                    "properties": {
                        "VkPhysicalDeviceProperties": {
                            "limits": {
                                "maxImageDimension2D": 8192,
                                "subPixelPrecisionBits": 4
                            }
                        }
                    }
                }
            }
        }"""

        expected_json_intersection = """{
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8-latest.json#",
            "capabilities": {
                "VP_TEST_merged_block": {
                    "properties": {
                        "VkPhysicalDeviceProperties": {
                            "limits": {
                                "maxImageDimension2D": 8192,
                                "subPixelPrecisionBits": 4
                            }
                        }
                    }
                }
            },
            "profiles": {
                "VP_TEST_merged": {
                    "version": 1,
                    "api-version": "1.2.0",
                    "label": "Test merged profile",
                    "description": "Test profile",
                    "capabilities": ["VP_TEST_merged_block"]
                }
            },
            "contributors": {},
            "history": []
        }"""

        expected_json_union = """{
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8-latest.json#",
            "capabilities": {
                "VP_TEST_merged_block": {
                    "properties": {
                        "VkPhysicalDeviceProperties": {
                            "limits": {
                                "maxImageDimension2D": 16384,
                                "subPixelPrecisionBits": 8
                            }
                        }
                    }
                }
            },
            "profiles": {
                "VP_TEST_merged": {
                    "version": 1,
                    "api-version": "1.2.0",
                    "label": "Test merged profile",
                    "description": "Test profile",
                    "capabilities": ["VP_TEST_merged_block"]
                }
            },
            "contributors": {},
            "history": []
        }"""

        merged_intersection = self._run_merge(input_json_a, input_json_b, mode="intersection")
        expected_intersection = json.loads(expected_json_intersection)
        self.assertEqual(merged_intersection, expected_intersection)

        merged_union = self._run_merge(input_json_a, input_json_b, mode="union")
        expected_union = json.loads(expected_json_union)
        self.assertEqual(merged_union, expected_union)

    def test_merge_limittype_max_array(self):
        """
        Tests multi-element fixed-size array 'max' limit types (e.g. maxComputeWorkGroupCount).
        - Intersection mode calculates element-wise min: min([65535, 4096, 4096], [4096, 65535, 2048]) -> [4096, 4096, 2048].
        - Union mode calculates element-wise max: max([65535, 4096, 4096], [4096, 65535, 2048]) -> [65535, 65535, 4096].
        """
        input_json_a = """{
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8-latest.json#",
            "profiles": {
                "VP_TEST_a": {
                    "version": 1,
                    "api-version": "1.2.0",
                    "capabilities": ["cap_a"]
                }
            },
            "capabilities": {
                "cap_a": {
                    "properties": {
                        "VkPhysicalDeviceProperties": {
                            "limits": {
                                "maxComputeWorkGroupCount": [65535, 4096, 4096]
                            }
                        }
                    }
                }
            }
        }"""

        input_json_b = """{
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8-latest.json#",
            "profiles": {
                "VP_TEST_b": {
                    "version": 1,
                    "api-version": "1.2.0",
                    "capabilities": ["cap_b"]
                }
            },
            "capabilities": {
                "cap_b": {
                    "properties": {
                        "VkPhysicalDeviceProperties": {
                            "limits": {
                                "maxComputeWorkGroupCount": [4096, 65535, 2048]
                            }
                        }
                    }
                }
            }
        }"""

        expected_json_intersection = """{
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8-latest.json#",
            "capabilities": {
                "VP_TEST_merged_block": {
                    "properties": {
                        "VkPhysicalDeviceProperties": {
                            "limits": {
                                "maxComputeWorkGroupCount": [4096, 4096, 2048]
                            }
                        }
                    }
                }
            },
            "profiles": {
                "VP_TEST_merged": {
                    "version": 1,
                    "api-version": "1.2.0",
                    "label": "Test merged profile",
                    "description": "Test profile",
                    "capabilities": ["VP_TEST_merged_block"]
                }
            },
            "contributors": {},
            "history": []
        }"""

        expected_json_union = """{
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8-latest.json#",
            "capabilities": {
                "VP_TEST_merged_block": {
                    "properties": {
                        "VkPhysicalDeviceProperties": {
                            "limits": {
                                "maxComputeWorkGroupCount": [65535, 65535, 4096]
                            }
                        }
                    }
                }
            },
            "profiles": {
                "VP_TEST_merged": {
                    "version": 1,
                    "api-version": "1.2.0",
                    "label": "Test merged profile",
                    "description": "Test profile",
                    "capabilities": ["VP_TEST_merged_block"]
                }
            },
            "contributors": {},
            "history": []
        }"""

        merged_intersection = self._run_merge(input_json_a, input_json_b, mode="intersection")
        expected_intersection = json.loads(expected_json_intersection)
        self.assertEqual(merged_intersection, expected_intersection)

        merged_union = self._run_merge(input_json_a, input_json_b, mode="union")
        expected_union = json.loads(expected_json_union)
        self.assertEqual(merged_union, expected_union)

    def test_merge_limittype_min_pot(self):
        """
        Tests 'min,pot' compound limit types (e.g. minUniformBufferOffsetAlignment).
        - Intersection mode calculates max(A, B) if both are power-of-two.
        - Union mode calculates min(A, B) if both are power-of-two.
        """
        input_json_a = """{
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8-latest.json#",
            "profiles": {
                "VP_TEST_a": {
                    "version": 1,
                    "api-version": "1.2.0",
                    "capabilities": ["cap_a"]
                }
            },
            "capabilities": {
                "cap_a": {
                    "properties": {
                        "VkPhysicalDeviceProperties": {
                            "limits": {
                                "minUniformBufferOffsetAlignment": 64
                            }
                        }
                    }
                }
            }
        }"""

        input_json_b = """{
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8-latest.json#",
            "profiles": {
                "VP_TEST_b": {
                    "version": 1,
                    "api-version": "1.2.0",
                    "capabilities": ["cap_b"]
                }
            },
            "capabilities": {
                "cap_b": {
                    "properties": {
                        "VkPhysicalDeviceProperties": {
                            "limits": {
                                "minUniformBufferOffsetAlignment": 256
                            }
                        }
                    }
                }
            }
        }"""

        expected_json_intersection = """{
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8-latest.json#",
            "capabilities": {
                "VP_TEST_merged_block": {
                    "properties": {
                        "VkPhysicalDeviceProperties": {
                            "limits": {
                                "minUniformBufferOffsetAlignment": 256
                            }
                        }
                    }
                }
            },
            "profiles": {
                "VP_TEST_merged": {
                    "version": 1,
                    "api-version": "1.2.0",
                    "label": "Test merged profile",
                    "description": "Test profile",
                    "capabilities": ["VP_TEST_merged_block"]
                }
            },
            "contributors": {},
            "history": []
        }"""

        expected_json_union = """{
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8-latest.json#",
            "capabilities": {
                "VP_TEST_merged_block": {
                    "properties": {
                        "VkPhysicalDeviceProperties": {
                            "limits": {
                                "minUniformBufferOffsetAlignment": 64
                            }
                        }
                    }
                }
            },
            "profiles": {
                "VP_TEST_merged": {
                    "version": 1,
                    "api-version": "1.2.0",
                    "label": "Test merged profile",
                    "description": "Test profile",
                    "capabilities": ["VP_TEST_merged_block"]
                }
            },
            "contributors": {},
            "history": []
        }"""

        merged_intersection = self._run_merge(input_json_a, input_json_b, mode="intersection")
        expected_intersection = json.loads(expected_json_intersection)
        self.assertEqual(merged_intersection, expected_intersection)

        merged_union = self._run_merge(input_json_a, input_json_b, mode="union")
        expected_union = json.loads(expected_json_union)
        self.assertEqual(merged_union, expected_union)

    def test_merge_limittype_min_mul(self):
        """
        Tests 'min,mul' compound limit types (e.g. bufferImageGranularity, timestampPeriod).
        - Finding a value: Integer LCM (12 vs 16 -> 48) in intersection; GCD (12 vs 16 -> 4) in union.
        - Not finding a value: Incompatible floating-point periods (1.0 vs 83.3333) omit the member.
        """
        input_json_a = """{
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8-latest.json#",
            "profiles": {
                "VP_TEST_a": {
                    "version": 1,
                    "api-version": "1.2.0",
                    "capabilities": ["cap_a"]
                }
            },
            "capabilities": {
                "cap_a": {
                    "properties": {
                        "VkPhysicalDeviceProperties": {
                            "limits": {
                                "bufferImageGranularity": 12,
                                "timestampPeriod": 1.0
                            }
                        }
                    }
                }
            }
        }"""

        input_json_b = """{
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8-latest.json#",
            "profiles": {
                "VP_TEST_b": {
                    "version": 1,
                    "api-version": "1.2.0",
                    "capabilities": ["cap_b"]
                }
            },
            "capabilities": {
                "cap_b": {
                    "properties": {
                        "VkPhysicalDeviceProperties": {
                            "limits": {
                                "bufferImageGranularity": 16,
                                "timestampPeriod": 83.3333
                            }
                        }
                    }
                }
            }
        }"""

        expected_json_intersection = """{
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8-latest.json#",
            "capabilities": {
                "VP_TEST_merged_block": {
                    "properties": {
                        "VkPhysicalDeviceProperties": {
                            "limits": {
                                "bufferImageGranularity": 48
                            }
                        }
                    }
                }
            },
            "profiles": {
                "VP_TEST_merged": {
                    "version": 1,
                    "api-version": "1.2.0",
                    "label": "Test merged profile",
                    "description": "Test profile",
                    "capabilities": ["VP_TEST_merged_block"]
                }
            },
            "contributors": {},
            "history": []
        }"""

        expected_json_union = """{
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8-latest.json#",
            "capabilities": {
                "VP_TEST_merged_block": {
                    "properties": {
                        "VkPhysicalDeviceProperties": {
                            "limits": {
                                "bufferImageGranularity": 4
                            }
                        }
                    }
                }
            },
            "profiles": {
                "VP_TEST_merged": {
                    "version": 1,
                    "api-version": "1.2.0",
                    "label": "Test merged profile",
                    "description": "Test profile",
                    "capabilities": ["VP_TEST_merged_block"]
                }
            },
            "contributors": {},
            "history": []
        }"""

        merged_intersection = self._run_merge(input_json_a, input_json_b, mode="intersection")
        expected_intersection = json.loads(expected_json_intersection)
        self.assertEqual(merged_intersection, expected_intersection)

        merged_union = self._run_merge(input_json_a, input_json_b, mode="union")
        expected_union = json.loads(expected_json_union)
        self.assertEqual(merged_union, expected_union)

    def test_merge_limittype_min_mul_compatible_floats(self):
        """
        Tests 'min,mul' float limit types when values are integer multiples.
        - Intersection mode selects 5.0 for (2.5, 5.0).
        - Union mode selects 2.5 for (2.5, 5.0).
        """
        input_json_a = """{
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8-latest.json#",
            "profiles": {
                "VP_TEST_a": {
                    "version": 1,
                    "api-version": "1.2.0",
                    "capabilities": ["cap_a"]
                }
            },
            "capabilities": {
                "cap_a": {
                    "properties": {
                        "VkPhysicalDeviceProperties": {
                            "limits": {
                                "timestampPeriod": 2.5
                            }
                        }
                    }
                }
            }
        }"""

        input_json_b = """{
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8-latest.json#",
            "profiles": {
                "VP_TEST_b": {
                    "version": 1,
                    "api-version": "1.2.0",
                    "capabilities": ["cap_b"]
                }
            },
            "capabilities": {
                "cap_b": {
                    "properties": {
                        "VkPhysicalDeviceProperties": {
                            "limits": {
                                "timestampPeriod": 5.0
                            }
                        }
                    }
                }
            }
        }"""

        expected_json_intersection = """{
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8-latest.json#",
            "capabilities": {
                "VP_TEST_merged_block": {
                    "properties": {
                        "VkPhysicalDeviceProperties": {
                            "limits": {
                                "timestampPeriod": 5.0
                            }
                        }
                    }
                }
            },
            "profiles": {
                "VP_TEST_merged": {
                    "version": 1,
                    "api-version": "1.2.0",
                    "label": "Test merged profile",
                    "description": "Test profile",
                    "capabilities": ["VP_TEST_merged_block"]
                }
            },
            "contributors": {},
            "history": []
        }"""

        expected_json_union = """{
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8-latest.json#",
            "capabilities": {
                "VP_TEST_merged_block": {
                    "properties": {
                        "VkPhysicalDeviceProperties": {
                            "limits": {
                                "timestampPeriod": 2.5
                            }
                        }
                    }
                }
            },
            "profiles": {
                "VP_TEST_merged": {
                    "version": 1,
                    "api-version": "1.2.0",
                    "label": "Test merged profile",
                    "description": "Test profile",
                    "capabilities": ["VP_TEST_merged_block"]
                }
            },
            "contributors": {},
            "history": []
        }"""

        merged_intersection = self._run_merge(input_json_a, input_json_b, mode="intersection")
        expected_intersection = json.loads(expected_json_intersection)
        self.assertEqual(merged_intersection, expected_intersection)

        merged_union = self._run_merge(input_json_a, input_json_b, mode="union")
        expected_union = json.loads(expected_json_union)
        self.assertEqual(merged_union, expected_union)

    def test_merge_host_image_copy_properties(self):
        """
        Tests merging of VkPhysicalDeviceHostImageCopyProperties (pCopySrcLayouts and pCopyDstLayouts).
        - Intersection mode calculates list intersection of VkImageLayout enum strings.
        - Synchronizes copySrcLayoutCount and copyDstLayoutCount to match list lengths.
        """
        input_json_a = """{
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8-latest.json#",
            "profiles": {
                "VP_TEST_a": {
                    "version": 1,
                    "api-version": "1.3.0",
                    "capabilities": ["cap_a"]
                }
            },
            "capabilities": {
                "cap_a": {
                    "properties": {
                        "VkPhysicalDeviceHostImageCopyProperties": {
                            "copySrcLayoutCount": 2,
                            "copyDstLayoutCount": 2,
                            "pCopySrcLayouts": [
                                "VK_IMAGE_LAYOUT_GENERAL",
                                "VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL"
                            ],
                            "pCopyDstLayouts": [
                                "VK_IMAGE_LAYOUT_GENERAL",
                                "VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL"
                            ]
                        }
                    }
                }
            }
        }"""

        input_json_b = """{
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8-latest.json#",
            "profiles": {
                "VP_TEST_b": {
                    "version": 1,
                    "api-version": "1.3.0",
                    "capabilities": ["cap_b"]
                }
            },
            "capabilities": {
                "cap_b": {
                    "properties": {
                        "VkPhysicalDeviceHostImageCopyProperties": {
                            "copySrcLayoutCount": 2,
                            "copyDstLayoutCount": 2,
                            "pCopySrcLayouts": [
                                "VK_IMAGE_LAYOUT_GENERAL",
                                "VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL"
                            ],
                            "pCopyDstLayouts": [
                                "VK_IMAGE_LAYOUT_GENERAL",
                                "VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL"
                            ]
                        }
                    }
                }
            }
        }"""

        expected_json_intersection = """{
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8-latest.json#",
            "capabilities": {
                "VP_TEST_merged_block": {
                    "properties": {
                        "VkPhysicalDeviceHostImageCopyProperties": {
                            "copySrcLayoutCount": 1,
                            "copyDstLayoutCount": 2,
                            "pCopySrcLayouts": [
                                "VK_IMAGE_LAYOUT_GENERAL"
                            ],
                            "pCopyDstLayouts": [
                                "VK_IMAGE_LAYOUT_GENERAL",
                                "VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL"
                            ]
                        }
                    }
                }
            },
            "profiles": {
                "VP_TEST_merged": {
                    "version": 1,
                    "api-version": "1.2.0",
                    "label": "Test merged profile",
                    "description": "Test profile",
                    "capabilities": ["VP_TEST_merged_block"]
                }
            },
            "contributors": {},
            "history": []
        }"""

        merged_intersection = self._run_merge(input_json_a, input_json_b, mode="intersection")
        expected_intersection = json.loads(expected_json_intersection)
        self.assertEqual(merged_intersection, expected_intersection)

    def test_merge_limittype_bitmask(self):
        """
        Tests 'bitmask' flag bit arrays (e.g. sampledImageColorSampleCounts).
        - Intersection mode takes common flags (set intersection).
        - Union mode takes all unique flags (set union).
        """
        input_json_a = """{
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8-latest.json#",
            "profiles": {
                "VP_TEST_a": {
                    "version": 1,
                    "api-version": "1.2.0",
                    "capabilities": ["cap_a"]
                }
            },
            "capabilities": {
                "cap_a": {
                    "properties": {
                        "VkPhysicalDeviceProperties": {
                            "limits": {
                                "sampledImageColorSampleCounts": [
                                    "VK_SAMPLE_COUNT_1_BIT",
                                    "VK_SAMPLE_COUNT_2_BIT",
                                    "VK_SAMPLE_COUNT_4_BIT"
                                ]
                            }
                        }
                    }
                }
            }
        }"""

        input_json_b = """{
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8-latest.json#",
            "profiles": {
                "VP_TEST_b": {
                    "version": 1,
                    "api-version": "1.2.0",
                    "capabilities": ["cap_b"]
                }
            },
            "capabilities": {
                "cap_b": {
                    "properties": {
                        "VkPhysicalDeviceProperties": {
                            "limits": {
                                "sampledImageColorSampleCounts": [
                                    "VK_SAMPLE_COUNT_1_BIT",
                                    "VK_SAMPLE_COUNT_4_BIT",
                                    "VK_SAMPLE_COUNT_8_BIT"
                                ]
                            }
                        }
                    }
                }
            }
        }"""

        expected_json_intersection = """{
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8-latest.json#",
            "capabilities": {
                "VP_TEST_merged_block": {
                    "properties": {
                        "VkPhysicalDeviceProperties": {
                            "limits": {
                                "sampledImageColorSampleCounts": [
                                    "VK_SAMPLE_COUNT_1_BIT",
                                    "VK_SAMPLE_COUNT_4_BIT"
                                ]
                            }
                        }
                    }
                }
            },
            "profiles": {
                "VP_TEST_merged": {
                    "version": 1,
                    "api-version": "1.2.0",
                    "label": "Test merged profile",
                    "description": "Test profile",
                    "capabilities": ["VP_TEST_merged_block"]
                }
            },
            "contributors": {},
            "history": []
        }"""

        expected_json_union = """{
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8-latest.json#",
            "capabilities": {
                "VP_TEST_merged_block": {
                    "properties": {
                        "VkPhysicalDeviceProperties": {
                            "limits": {
                                "sampledImageColorSampleCounts": [
                                    "VK_SAMPLE_COUNT_1_BIT",
                                    "VK_SAMPLE_COUNT_2_BIT",
                                    "VK_SAMPLE_COUNT_4_BIT",
                                    "VK_SAMPLE_COUNT_8_BIT"
                                ]
                            }
                        }
                    }
                }
            },
            "profiles": {
                "VP_TEST_merged": {
                    "version": 1,
                    "api-version": "1.2.0",
                    "label": "Test merged profile",
                    "description": "Test profile",
                    "capabilities": ["VP_TEST_merged_block"]
                }
            },
            "contributors": {},
            "history": []
        }"""

        merged_intersection = self._run_merge(input_json_a, input_json_b, mode="intersection")
        expected_intersection = json.loads(expected_json_intersection)
        self.assertEqual(merged_intersection, expected_intersection)

        merged_union = self._run_merge(input_json_a, input_json_b, mode="union")
        expected_union = json.loads(expected_json_union)
        self.assertEqual(merged_union, expected_union)

    def test_merge_limittype_bitmask_int(self):
        """
        Tests integer 'bitmask' limit types (e.g. integer bitwise sample count masks).
        - Intersection mode calculates bitwise AND (1 & 3 = 1).
        - Union mode calculates bitwise OR (1 | 3 = 3).
        """
        input_json_a = """{
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8-latest.json#",
            "profiles": {
                "VP_TEST_a": {
                    "version": 1,
                    "api-version": "1.2.0",
                    "capabilities": ["cap_a"]
                }
            },
            "capabilities": {
                "cap_a": {
                    "properties": {
                        "VkPhysicalDeviceProperties": {
                            "limits": {
                                "sampledImageColorSampleCounts": 1
                            }
                        }
                    }
                }
            }
        }"""

        input_json_b = """{
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8-latest.json#",
            "profiles": {
                "VP_TEST_b": {
                    "version": 1,
                    "api-version": "1.2.0",
                    "capabilities": ["cap_b"]
                }
            },
            "capabilities": {
                "cap_b": {
                    "properties": {
                        "VkPhysicalDeviceProperties": {
                            "limits": {
                                "sampledImageColorSampleCounts": 3
                            }
                        }
                    }
                }
            }
        }"""

        expected_json_intersection = """{
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8-latest.json#",
            "capabilities": {
                "VP_TEST_merged_block": {
                    "properties": {
                        "VkPhysicalDeviceProperties": {
                            "limits": {
                                "sampledImageColorSampleCounts": 1
                            }
                        }
                    }
                }
            },
            "profiles": {
                "VP_TEST_merged": {
                    "version": 1,
                    "api-version": "1.2.0",
                    "label": "Test merged profile",
                    "description": "Test profile",
                    "capabilities": ["VP_TEST_merged_block"]
                }
            },
            "contributors": {},
            "history": []
        }"""

        expected_json_union = """{
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8-latest.json#",
            "capabilities": {
                "VP_TEST_merged_block": {
                    "properties": {
                        "VkPhysicalDeviceProperties": {
                            "limits": {
                                "sampledImageColorSampleCounts": 3
                            }
                        }
                    }
                }
            },
            "profiles": {
                "VP_TEST_merged": {
                    "version": 1,
                    "api-version": "1.2.0",
                    "label": "Test merged profile",
                    "description": "Test profile",
                    "capabilities": ["VP_TEST_merged_block"]
                }
            },
            "contributors": {},
            "history": []
        }"""

        merged_intersection = self._run_merge(input_json_a, input_json_b, mode="intersection")
        expected_intersection = json.loads(expected_json_intersection)
        self.assertEqual(merged_intersection, expected_intersection)

        merged_union = self._run_merge(input_json_a, input_json_b, mode="union")
        expected_union = json.loads(expected_json_union)
        self.assertEqual(merged_union, expected_union)

    def test_merge_limittype_range(self):
        """
        Tests 'range' limit arrays [min, max] (e.g. pointSizeRange, lineWidthRange).
        - Intersection mode takes [max(A.min, B.min), min(A.max, B.max)].
        - Union mode takes [min(A.min, B.min), max(A.max, B.max)].
        """
        input_json_a = """{
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8-latest.json#",
            "profiles": {
                "VP_TEST_a": {
                    "version": 1,
                    "api-version": "1.2.0",
                    "capabilities": ["cap_a"]
                }
            },
            "capabilities": {
                "cap_a": {
                    "properties": {
                        "VkPhysicalDeviceProperties": {
                            "limits": {
                                "pointSizeRange": [1.0, 64.0]
                            }
                        }
                    }
                }
            }
        }"""

        input_json_b = """{
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8-latest.json#",
            "profiles": {
                "VP_TEST_b": {
                    "version": 1,
                    "api-version": "1.2.0",
                    "capabilities": ["cap_b"]
                }
            },
            "capabilities": {
                "cap_b": {
                    "properties": {
                        "VkPhysicalDeviceProperties": {
                            "limits": {
                                "pointSizeRange": [2.0, 128.0]
                            }
                        }
                    }
                }
            }
        }"""

        expected_json_intersection = """{
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8-latest.json#",
            "capabilities": {
                "VP_TEST_merged_block": {
                    "properties": {
                        "VkPhysicalDeviceProperties": {
                            "limits": {
                                "pointSizeRange": [2.0, 64.0]
                            }
                        }
                    }
                }
            },
            "profiles": {
                "VP_TEST_merged": {
                    "version": 1,
                    "api-version": "1.2.0",
                    "label": "Test merged profile",
                    "description": "Test profile",
                    "capabilities": ["VP_TEST_merged_block"]
                }
            },
            "contributors": {},
            "history": []
        }"""

        expected_json_union = """{
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8-latest.json#",
            "capabilities": {
                "VP_TEST_merged_block": {
                    "properties": {
                        "VkPhysicalDeviceProperties": {
                            "limits": {
                                "pointSizeRange": [1.0, 128.0]
                            }
                        }
                    }
                }
            },
            "profiles": {
                "VP_TEST_merged": {
                    "version": 1,
                    "api-version": "1.2.0",
                    "label": "Test merged profile",
                    "description": "Test profile",
                    "capabilities": ["VP_TEST_merged_block"]
                }
            },
            "contributors": {},
            "history": []
        }"""

        merged_intersection = self._run_merge(input_json_a, input_json_b, mode="intersection")
        expected_intersection = json.loads(expected_json_intersection)
        self.assertEqual(merged_intersection, expected_intersection)

        merged_union = self._run_merge(input_json_a, input_json_b, mode="union")
        expected_union = json.loads(expected_json_union)
        self.assertEqual(merged_union, expected_union)

    def test_merge_limittype_exact_and_noauto(self):
        """
        Tests 'exact' and 'noauto' limit types.
        - 'exact' limits (e.g. pointClippingBehavior in PointClippingProperties):
          preserved if values match, and dropped if they differ.
        - 'noauto' limits (e.g. vendorID/deviceID in Properties or driverID in DriverProperties) are unmergeable metadata and always excluded.
        """
        input_json_a = """{
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8-latest.json#",
            "profiles": {
                "VP_TEST_a": {
                    "version": 1,
                    "api-version": "1.2.0",
                    "capabilities": ["cap_a"]
                }
            },
            "capabilities": {
                "cap_a": {
                    "properties": {
                        "VkPhysicalDeviceProperties": {
                            "vendorID": 4098,
                            "deviceID": 26880,
                            "limits": {
                                "timestampComputeAndGraphics": true
                            }
                        },
                        "VkPhysicalDeviceDriverProperties": {
                            "driverID": "VK_DRIVER_ID_NVIDIA_PROPRIETARY"
                        },
                        "VkPhysicalDevicePointClippingProperties": {
                            "pointClippingBehavior": "VK_POINT_CLIPPING_BEHAVIOR_ALL_CLIP_PLANES"
                        }
                    }
                }
            }
        }"""

        input_json_b = """{
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8-latest.json#",
            "profiles": {
                "VP_TEST_b": {
                    "version": 1,
                    "api-version": "1.2.0",
                    "capabilities": ["cap_b"]
                }
            },
            "capabilities": {
                "cap_b": {
                    "properties": {
                        "VkPhysicalDeviceProperties": {
                            "vendorID": 4098,
                            "deviceID": 26881,
                            "limits": {
                                "timestampComputeAndGraphics": true
                            }
                        },
                        "VkPhysicalDeviceDriverProperties": {
                            "driverID": "VK_DRIVER_ID_AMD_PROPRIETARY"
                        },
                        "VkPhysicalDevicePointClippingProperties": {
                            "pointClippingBehavior": "VK_POINT_CLIPPING_BEHAVIOR_USER_CLIP_PLANES_ONLY"
                        }
                    }
                }
            }
        }"""

        expected_json_intersection = """{
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8-latest.json#",
            "capabilities": {
                "VP_TEST_merged_block": {
                    "properties": {
                        "VkPhysicalDeviceProperties": {
                            "limits": {
                                "timestampComputeAndGraphics": true
                            }
                        }
                    }
                }
            },
            "profiles": {
                "VP_TEST_merged": {
                    "version": 1,
                    "api-version": "1.2.0",
                    "label": "Test merged profile",
                    "description": "Test profile",
                    "capabilities": ["VP_TEST_merged_block"]
                }
            },
            "contributors": {},
            "history": []
        }"""

        merged_intersection = self._run_merge(input_json_a, input_json_b, mode="intersection")
        expected_intersection = json.loads(expected_json_intersection)
        self.assertEqual(merged_intersection, expected_intersection)

    def test_merge_limittype_struct(self):
        """
        Tests 'struct' limit types (e.g. VkPhysicalDeviceProperties::limits and VkPhysicalDeviceProperties::sparseProperties).
        Verifies recursive sub-structure parsing and member limit evaluations.
        """
        input_json_a = """{
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8-latest.json#",
            "profiles": {
                "VP_TEST_a": {
                    "version": 1,
                    "api-version": "1.2.0",
                    "capabilities": ["cap_a"]
                }
            },
            "capabilities": {
                "cap_a": {
                    "properties": {
                        "VkPhysicalDeviceProperties": {
                            "sparseProperties": {
                                "residencyStandard2DBlockShape": true,
                                "residencyAlignedMipSize": true
                            }
                        }
                    }
                }
            }
        }"""

        input_json_b = """{
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8-latest.json#",
            "profiles": {
                "VP_TEST_b": {
                    "version": 1,
                    "api-version": "1.2.0",
                    "capabilities": ["cap_b"]
                }
            },
            "capabilities": {
                "cap_b": {
                    "properties": {
                        "VkPhysicalDeviceProperties": {
                            "sparseProperties": {
                                "residencyStandard2DBlockShape": false,
                                "residencyAlignedMipSize": true
                            }
                        }
                    }
                }
            }
        }"""

        expected_json_intersection = """{
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8-latest.json#",
            "capabilities": {
                "VP_TEST_merged_block": {
                    "properties": {
                        "VkPhysicalDeviceProperties": {
                            "sparseProperties": {
                                "residencyAlignedMipSize": true,
                                "residencyStandard2DBlockShape": false
                            }
                        }
                    }
                }
            },
            "profiles": {
                "VP_TEST_merged": {
                    "version": 1,
                    "api-version": "1.2.0",
                    "label": "Test merged profile",
                    "description": "Test profile",
                    "capabilities": ["VP_TEST_merged_block"]
                }
            },
            "contributors": {},
            "history": []
        }"""

        merged_intersection = self._run_merge(input_json_a, input_json_b, mode="intersection")
        expected_intersection = json.loads(expected_json_intersection)
        self.assertEqual(merged_intersection, expected_intersection)


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument(
        '--registry', '-r', action='store', required=False,
        help='Use specified registry file instead of vk.xml.'
    )

    args, unparsed = parser.parse_known_args()
    TestMergeLimitType.registry_path = args.registry

    unittest.main(argv=[sys.argv[0]] + unparsed)
    