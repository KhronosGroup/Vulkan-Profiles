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
from source.transform_strip_duplication import strip_duplication_profiles_files


class TestConvertStripFormat(unittest.TestCase):
    registry_path = None

    def setUp(self):
        self.vk: VulkanObject = initVulkanObject('vulkan', self.registry_path)

    def test_strip_formats_duplication_vulkan11(self):
        """
        Verifies format feature list duplication stripping for a Vulkan 1.1 profile
        using VkFormatProperties. Duplicate list entries are stripped while unique entries remain.
        """
        original_json_text = """{
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8.0-106.json#",
            "profiles": {
                "VP_TEST_v11_formats_profile": {
                    "version": 1,
                    "api-version": "1.1.0",
                    "capabilities": [
                        "caps1",
                        "caps2"
                    ]
                }
            },
            "capabilities": {
                "caps1": {
                    "formats": {
                        "VK_FORMAT_R8G8B8A8_UNORM": {
                            "VkFormatProperties": {
                                "linearTilingFeatures": [
                                    "VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT"
                                ],
                                "optimalTilingFeatures": [
                                    "VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT",
                                    "VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT"
                                ]
                            }
                        }
                    }
                },
                "caps2": {
                    "formats": {
                        "VK_FORMAT_R8G8B8A8_UNORM": {
                            "VkFormatProperties": {
                                "linearTilingFeatures": [
                                    "VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT"
                                ],
                                "optimalTilingFeatures": [
                                    "VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT",
                                    "VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT",
                                    "VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT"
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
                "VP_TEST_v11_formats_profile": {
                    "version": 1,
                    "api-version": "1.1.0",
                    "capabilities": [
                        "caps1",
                        "caps2"
                    ]
                }
            },
            "capabilities": {
                "caps1": {
                    "formats": {
                        "VK_FORMAT_R8G8B8A8_UNORM": {
                            "VkFormatProperties": {
                                "linearTilingFeatures": [
                                    "VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT"
                                ],
                                "optimalTilingFeatures": [
                                    "VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT",
                                    "VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT"
                                ]
                            }
                        }
                    }
                },
                "caps2": {
                    "formats": {
                        "VK_FORMAT_R8G8B8A8_UNORM": {
                            "VkFormatProperties": {
                                "optimalTilingFeatures": [
                                    "VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT",
                                    "VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT",
                                    "VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT"
                                ]
                            }
                        }
                    }
                }
            }
        }"""

        json_files_dict = {"test_profile.json": json.loads(original_json_text)}
        strip_duplication_profiles_files(self.vk, json_files_dict)

        self.assertEqual(json_files_dict["test_profile.json"], json.loads(expected_json_text))

    def test_strip_formats_duplication_vulkan13(self):
        """
        Verifies format feature list duplication stripping for a Vulkan 1.3 profile
        using VkFormatProperties3. Completely identical format definitions across capability blocks are fully stripped.
        """
        original_json_text = """{
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8.0-106.json#",
            "profiles": {
                "VP_TEST_v13_formats_profile": {
                    "version": 1,
                    "api-version": "1.3.0",
                    "capabilities": [
                        "caps1",
                        "caps2"
                    ]
                }
            },
            "capabilities": {
                "caps1": {
                    "formats": {
                        "VK_FORMAT_R8G8B8A8_UNORM": {
                            "VkFormatProperties3": {
                                "linearTilingFeatures": [
                                    "VK_FORMAT_FEATURE_2_SAMPLED_IMAGE_BIT"
                                ]
                            }
                        }
                    }
                },
                "caps2": {
                    "formats": {
                        "VK_FORMAT_R8G8B8A8_UNORM": {
                            "VkFormatProperties3": {
                                "linearTilingFeatures": [
                                    "VK_FORMAT_FEATURE_2_SAMPLED_IMAGE_BIT"
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
                "VP_TEST_v13_formats_profile": {
                    "version": 1,
                    "api-version": "1.3.0",
                    "capabilities": [
                        "caps1",
                        "caps2"
                    ]
                }
            },
            "capabilities": {
                "caps1": {
                    "formats": {
                        "VK_FORMAT_R8G8B8A8_UNORM": {
                            "VkFormatProperties3": {
                                "linearTilingFeatures": [
                                    "VK_FORMAT_FEATURE_2_SAMPLED_IMAGE_BIT"
                                ]
                            }
                        }
                    }
                },
                "caps2": {
                }
            }
        }"""

        json_files_dict = {"test_profile.json": json.loads(original_json_text)}
        strip_duplication_profiles_files(self.vk, json_files_dict)

        self.assertEqual(json_files_dict["test_profile.json"], json.loads(expected_json_text))

    def test_strip_formats_duplication_mixed_properties(self):
        """
        Verifies format feature duplication stripping when a capability block contains
        both VkFormatProperties and VkFormatProperties3 for the same format. Duplicate 
        VkFormatProperties structures are stripped when fully matched in parent blocks, 
        leaving only updated VkFormatProperties3 structures.
        """
        original_json_text = """{
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8.0-106.json#",
            "profiles": {
                "VP_TEST_v13_mixed_formats_profile": {
                    "version": 1,
                    "api-version": "1.3.0",
                    "capabilities": [
                        "caps1",
                        "caps2"
                    ]
                }
            },
            "capabilities": {
                "caps1": {
                    "formats": {
                        "VK_FORMAT_R8G8B8A8_UNORM": {
                            "VkFormatProperties": {
                                "linearTilingFeatures": [
                                    "VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT"
                                ]
                            },
                            "VkFormatProperties3": {
                                "linearTilingFeatures": [
                                    "VK_FORMAT_FEATURE_2_SAMPLED_IMAGE_BIT"
                                ]
                            }
                        }
                    }
                },
                "caps2": {
                    "formats": {
                        "VK_FORMAT_R8G8B8A8_UNORM": {
                            "VkFormatProperties": {
                                "linearTilingFeatures": [
                                    "VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT"
                                ]
                            },
                            "VkFormatProperties3": {
                                "linearTilingFeatures": [
                                    "VK_FORMAT_FEATURE_2_SAMPLED_IMAGE_BIT",
                                    "VK_FORMAT_FEATURE_2_STORAGE_IMAGE_BIT"
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
                "VP_TEST_v13_mixed_formats_profile": {
                    "version": 1,
                    "api-version": "1.3.0",
                    "capabilities": [
                        "caps1",
                        "caps2"
                    ]
                }
            },
            "capabilities": {
                "caps1": {
                    "formats": {
                        "VK_FORMAT_R8G8B8A8_UNORM": {
                            "VkFormatProperties": {
                                "linearTilingFeatures": [
                                    "VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT"
                                ]
                            },
                            "VkFormatProperties3": {
                                "linearTilingFeatures": [
                                    "VK_FORMAT_FEATURE_2_SAMPLED_IMAGE_BIT"
                                ]
                            }
                        }
                    }
                },
                "caps2": {
                    "formats": {
                        "VK_FORMAT_R8G8B8A8_UNORM": {
                            "VkFormatProperties3": {
                                "linearTilingFeatures": [
                                    "VK_FORMAT_FEATURE_2_SAMPLED_IMAGE_BIT",
                                    "VK_FORMAT_FEATURE_2_STORAGE_IMAGE_BIT"
                                ]
                            }
                        }
                    }
                }
            }
        }"""

        json_files_dict = {"test_profile.json": json.loads(original_json_text)}
        strip_duplication_profiles_files(self.vk, json_files_dict)

        self.assertEqual(json_files_dict["test_profile.json"], json.loads(expected_json_text))

    def test_strip_intra_block_format_alias_duplication(self):
        """
        Verifies that when a single capability block contains duplicate format property structure
        aliases (e.g., VkFormatProperties3 and VkFormatProperties3KHR), the extension alias structure
        is stripped in favor of the canonical core structure.
        """
        original_json_text = """{
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8.0-204.json#",
            "profiles": {
                "VP_LUNARG_test": {
                    "version": 1,
                    "api-version": "1.3.0",
                    "capabilities": ["caps_format"]
                }
            },
            "capabilities": {
                "caps_format": {
                    "formats": {
                        "VK_FORMAT_R8G8B8A8_UNORM": {
                            "VkFormatProperties3": {
                                "linearTilingFeatures": ["VK_FORMAT_FEATURE_2_SAMPLED_IMAGE_BIT"]
                            },
                            "VkFormatProperties3KHR": {
                                "linearTilingFeatures": ["VK_FORMAT_FEATURE_2_SAMPLED_IMAGE_BIT"]
                            }
                        }
                    }
                }
            }
        }"""

        expected_json_text = """{
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8.0-204.json#",
            "profiles": {
                "VP_LUNARG_test": {
                    "version": 1,
                    "api-version": "1.3.0",
                    "capabilities": ["caps_format"]
                }
            },
            "capabilities": {
                "caps_format": {
                    "formats": {
                        "VK_FORMAT_R8G8B8A8_UNORM": {
                            "VkFormatProperties3": {
                                "linearTilingFeatures": ["VK_FORMAT_FEATURE_2_SAMPLED_IMAGE_BIT"]
                            }
                        }
                    }
                }
            }
        }"""

        json_files_dict = {"test_profile.json": json.loads(original_json_text)}
        strip_duplication_profiles_files(self.vk, json_files_dict)

        self.assertEqual(json_files_dict["test_profile.json"], json.loads(expected_json_text))


def test_strip_formats_duplication_extended_flags(self):
        """
        Verifies format feature list duplication stripping across capability blocks
        when declaring VK_KHR_extended_flags while using VkFormatProperties3 alongside
        an empty VkFormatProperties4KHR structure. Duplicate extension declarations,
        identical format feature flags, and redundant empty VkFormatProperties4KHR
        structures are stripped, preserving updated feature lists.
        """
        original_json_text = """{
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8.0-204.json#",
            "profiles": {
                "VP_TEST_extended_flags_profile": {
                    "version": 1,
                    "api-version": "1.3.0",
                    "capabilities": [
                        "caps1",
                        "caps2"
                    ]
                }
            },
            "capabilities": {
                "caps1": {
                    "extensions": {
                        "VK_KHR_extended_flags": 1
                    },
                    "formats": {
                        "VK_FORMAT_R8G8B8A8_UNORM": {
                            "VkFormatProperties3": {
                                "linearTilingFeatures": [
                                    "VK_FORMAT_FEATURE_2_SAMPLED_IMAGE_BIT"
                                ],
                                "optimalTilingFeatures": [
                                    "VK_FORMAT_FEATURE_2_SAMPLED_IMAGE_BIT",
                                    "VK_FORMAT_FEATURE_2_COLOR_ATTACHMENT_BIT"
                                ]
                            },
                            "VkFormatProperties4KHR": {}
                        }
                    }
                },
                "caps2": {
                    "extensions": {
                        "VK_KHR_extended_flags": 1
                    },
                    "formats": {
                        "VK_FORMAT_R8G8B8A8_UNORM": {
                            "VkFormatProperties3": {
                                "linearTilingFeatures": [
                                    "VK_FORMAT_FEATURE_2_SAMPLED_IMAGE_BIT"
                                ],
                                "optimalTilingFeatures": [
                                    "VK_FORMAT_FEATURE_2_SAMPLED_IMAGE_BIT",
                                    "VK_FORMAT_FEATURE_2_COLOR_ATTACHMENT_BIT",
                                    "VK_FORMAT_FEATURE_2_STORAGE_IMAGE_BIT"
                                ]
                            },
                            "VkFormatProperties4KHR": {}
                        }
                    }
                }
            }
        }"""

        expected_json_text = """{
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8.0-204.json#",
            "profiles": {
                "VP_TEST_extended_flags_profile": {
                    "version": 1,
                    "api-version": "1.3.0",
                    "capabilities": [
                        "caps1",
                        "caps2"
                    ]
                }
            },
            "capabilities": {
                "caps1": {
                    "extensions": {
                        "VK_KHR_extended_flags": 1
                    },
                    "formats": {
                        "VK_FORMAT_R8G8B8A8_UNORM": {
                            "VkFormatProperties3": {
                                "linearTilingFeatures": [
                                    "VK_FORMAT_FEATURE_2_SAMPLED_IMAGE_BIT"
                                ],
                                "optimalTilingFeatures": [
                                    "VK_FORMAT_FEATURE_2_SAMPLED_IMAGE_BIT",
                                    "VK_FORMAT_FEATURE_2_COLOR_ATTACHMENT_BIT"
                                ]
                            },
                            "VkFormatProperties4KHR": {}
                        }
                    }
                },
                "caps2": {
                    "formats": {
                        "VK_FORMAT_R8G8B8A8_UNORM": {
                            "VkFormatProperties3": {
                                "optimalTilingFeatures": [
                                    "VK_FORMAT_FEATURE_2_SAMPLED_IMAGE_BIT",
                                    "VK_FORMAT_FEATURE_2_COLOR_ATTACHMENT_BIT",
                                    "VK_FORMAT_FEATURE_2_STORAGE_IMAGE_BIT"
                                ]
                            }
                        }
                    }
                }
            }
        }"""

        json_files_dict = {"test_profile.json": json.loads(original_json_text)}
        strip_duplication_profiles_files(self.vk, json_files_dict)

        self.assertEqual(json_files_dict["test_profile.json"], json.loads(expected_json_text))
        

if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument(
        '--registry', '-r', action='store', required=False,
        help='Use specified registry file instead of vk.xml.'
    )

    args, unparsed = parser.parse_known_args()
    TestConvertStripFormat.registry_path = args.registry

    unittest.main(argv=[sys.argv[0]] + unparsed)