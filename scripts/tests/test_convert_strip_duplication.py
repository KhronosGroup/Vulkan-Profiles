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
from source.main_convert import strip_duplication_profiles_files


class TestConvertStripDuplication(unittest.TestCase):
    registry_path = None

    def setUp(self):
        self.vk: VulkanObject = initVulkanObject('vulkan', self.registry_path)

    def test_strip_features_duplication_combined_vulkan12(self):
        """
        Verifies stripping behavior when a Vulkan 1.2 capability block contains duplicate split
        and bundle feature structures. Redundant split structures are stripped when covered by
        active Vulkan 1.1/1.2 bundle structures (VkPhysicalDeviceVulkan11Features and VkPhysicalDeviceVulkan12Features).
        """
        original_json_text = """{
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8.0-204.json#",
            "profiles": {
                "VP_LUNARG_profile": {
                    "version": 1,
                    "api-version": "1.2.0",
                    "capabilities": [
                        "caps_combined"
                    ]
                }
            },
            "capabilities": {
                "caps_combined": {
                    "extensions": {
                        "VK_KHR_16bit_storage": 1,
                        "VK_KHR_imageless_framebuffer": 1
                    },
                    "features": {
                        "VkPhysicalDevice16BitStorageFeatures": {
                            "storageBuffer16BitAccess": true,
                            "uniformAndStorageBuffer16BitAccess": true
                        },
                        "VkPhysicalDevice16BitStorageFeaturesKHR": {
                            "storageBuffer16BitAccess": true,
                            "uniformAndStorageBuffer16BitAccess": true
                        },
                        "VkPhysicalDeviceVulkan11Features": {
                            "storageBuffer16BitAccess": true,
                            "uniformAndStorageBuffer16BitAccess": true
                        },
                        "VkPhysicalDeviceVulkan12Features": {
                            "imagelessFramebuffer": true
                        },
                        "VkPhysicalDeviceImagelessFramebufferFeaturesKHR": {
                            "imagelessFramebuffer": true
                        },
                        "VkPhysicalDeviceImagelessFramebufferFeatures": {
                            "imagelessFramebuffer": true
                        }
                    }
                }
            }
        }"""

        expected_json_text = """{
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8.0-204.json#",
            "profiles": {
                "VP_LUNARG_profile": {
                    "version": 1,
                    "api-version": "1.2.0",
                    "capabilities": [
                        "caps_combined"
                    ]
                }
            },
            "capabilities": {
                "caps_combined": {
                    "extensions": {
                        "VK_KHR_16bit_storage": 1,
                        "VK_KHR_imageless_framebuffer": 1
                    },
                    "features": {
                        "VkPhysicalDeviceVulkan11Features": {
                            "storageBuffer16BitAccess": true,
                            "uniformAndStorageBuffer16BitAccess": true
                        },
                        "VkPhysicalDeviceVulkan12Features": {
                            "imagelessFramebuffer": true
                        }
                    }
                }
            }
        }"""

        json_files_dict = {"test_profile.json": json.loads(original_json_text)}
        strip_duplication_profiles_files(self.vk, json_files_dict)

        self.assertEqual(json_files_dict["test_profile.json"], json.loads(expected_json_text))

    def test_strip_features_duplication_combined_vulkan11(self):
        """
        Verifies stripping behavior for a Vulkan 1.1 profile containing duplicate canonical
        and extension alias structures. Because Vulkan 1.2 bundle structures are not active in Vulkan 1.1,
        extension alias structures (VkPhysicalDevice16BitStorageFeaturesKHR) are stripped in favor
        of the canonical Vulkan 1.1 core structure (VkPhysicalDevice16BitStorageFeatures).
        """
        original_json_text = """{
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8.0-204.json#",
            "profiles": {
                "VP_LUNARG_profile": {
                    "version": 1,
                    "api-version": "1.1.0",
                    "capabilities": [
                        "caps_combined"
                    ]
                }
            },
            "capabilities": {
                "caps_combined": {
                    "extensions": {
                        "VK_KHR_16bit_storage": 1,
                        "VK_KHR_imageless_framebuffer": 1
                    },
                    "features": {
                        "VkPhysicalDevice16BitStorageFeatures": {
                            "storageBuffer16BitAccess": true,
                            "uniformAndStorageBuffer16BitAccess": true
                        },
                        "VkPhysicalDevice16BitStorageFeaturesKHR": {
                            "storageBuffer16BitAccess": true,
                            "uniformAndStorageBuffer16BitAccess": true
                        },
                        "VkPhysicalDeviceImagelessFramebufferFeaturesKHR": {
                            "imagelessFramebuffer": true
                        },
                        "VkPhysicalDeviceImagelessFramebufferFeatures": {
                            "imagelessFramebuffer": true
                        }
                    }
                }
            }
        }"""

        expected_json_text = """{
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8.0-204.json#",
            "profiles": {
                "VP_LUNARG_profile": {
                    "version": 1,
                    "api-version": "1.1.0",
                    "capabilities": [
                        "caps_combined"
                    ]
                }
            },
            "capabilities": {
                "caps_combined": {
                    "extensions": {
                        "VK_KHR_16bit_storage": 1,
                        "VK_KHR_imageless_framebuffer": 1
                    },
                    "features": {
                        "VkPhysicalDevice16BitStorageFeatures": {
                            "storageBuffer16BitAccess": true,
                            "uniformAndStorageBuffer16BitAccess": true
                        },
                        "VkPhysicalDeviceImagelessFramebufferFeaturesKHR": {
                            "imagelessFramebuffer": true
                        }
                    }
                }
            }
        }"""

        json_files_dict = {"test_profile.json": json.loads(original_json_text)}
        strip_duplication_profiles_files(self.vk, json_files_dict)

        self.assertEqual(json_files_dict["test_profile.json"], json.loads(expected_json_text))

    def test_strip_features_duplication_from_inherited_parent_blocks(self):
        """
        Verifies that features and extensions already present in parent capability blocks
        are stripped from subsequent capability blocks unless a structure remaining in that block
        requires the extension. VK_KHR_imageless_framebuffer steps are shifted by caps + 1 relative
        to VK_KHR_16bit_storage.
        """
        original_json_text = """{
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8.0-106.json#",
            "profiles": {
                "VP_LUNARG_profile": {
                    "version": 1,
                    "api-version": "1.1.0",
                    "capabilities": [
                        "caps0",
                        "caps1",
                        "caps2",
                        "caps3",
                        "caps4"
                    ]
                }
            },
            "capabilities": {
                "caps0": {
                    "extensions": {
                        "VK_KHR_16bit_storage": 1
                    }
                },
                "caps1": {
                    "extensions": {
                        "VK_KHR_16bit_storage": 1,
                        "VK_KHR_imageless_framebuffer": 1
                    },
                    "features": {
                        "VkPhysicalDevice16BitStorageFeatures": {
                            "storageBuffer16BitAccess": true
                        }
                    }
                },
                "caps2": {
                    "extensions": {
                        "VK_KHR_16bit_storage": 1,
                        "VK_KHR_imageless_framebuffer": 1
                    },
                    "features": {
                        "VkPhysicalDevice16BitStorageFeatures": {
                            "storageBuffer16BitAccess": true,
                            "uniformAndStorageBuffer16BitAccess": true
                        },
                        "VkPhysicalDeviceImagelessFramebufferFeaturesKHR": {
                            "imagelessFramebuffer": true
                        }
                    }
                },
                "caps3": {
                    "extensions": {
                        "VK_KHR_16bit_storage": 1,
                        "VK_KHR_imageless_framebuffer": 1
                    },
                    "features": {
                        "VkPhysicalDevice16BitStorageFeatures": {
                            "storageBuffer16BitAccess": true,
                            "uniformAndStorageBuffer16BitAccess": true
                        },
                        "VkPhysicalDeviceImagelessFramebufferFeaturesKHR": {
                            "imagelessFramebuffer": true
                        }
                    }
                },
                "caps4": {
                    "extensions": {
                        "VK_KHR_16bit_storage": 1,
                        "VK_KHR_imageless_framebuffer": 1
                    }
                }
            }
        }"""

        expected_json_text = """{
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8.0-106.json#",
            "profiles": {
                "VP_LUNARG_profile": {
                    "version": 1,
                    "api-version": "1.1.0",
                    "capabilities": [
                        "caps0",
                        "caps1",
                        "caps2",
                        "caps3",
                        "caps4"
                    ]
                }
            },
            "capabilities": {
                "caps0": {
                    "extensions": {
                        "VK_KHR_16bit_storage": 1
                    }
                },
                "caps1": {
                    "extensions": {
                        "VK_KHR_imageless_framebuffer": 1
                    },
                    "features": {
                        "VkPhysicalDevice16BitStorageFeatures": {
                            "storageBuffer16BitAccess": true
                        }
                    }
                },
                "caps2": {
                    "extensions": {
                        "VK_KHR_imageless_framebuffer": 1
                    },
                    "features": {
                        "VkPhysicalDevice16BitStorageFeatures": {
                            "uniformAndStorageBuffer16BitAccess": true
                        },
                        "VkPhysicalDeviceImagelessFramebufferFeaturesKHR": {
                            "imagelessFramebuffer": true
                        }
                    }
                },
                "caps3": {
                },
                "caps4": {
                }
            }
        }"""

        json_files_dict = {"test_profile.json": json.loads(original_json_text)}
        strip_duplication_profiles_files(self.vk, json_files_dict)

        generated_data = json_files_dict["test_profile.json"]
        expected_data = json.loads(expected_json_text)

        gen_caps = generated_data.get("capabilities", {})
        exp_caps = expected_data.get("capabilities", {})

        for block_name in ["caps0", "caps1", "caps2", "caps3", "caps4"]:
            self.assertEqual(gen_caps.get(block_name), exp_caps.get(block_name), f"Mismatch in {block_name}")

        self.assertEqual(generated_data, expected_data)

    def test_strip_features_duplication_from_inherited_parent_profile(self):
        """
        Verifies that features and extensions already present in a required parent profile
        are stripped from the child profile. VK_KHR_imageless_framebuffer is declared in the parent
        and its feature introduced in the child profile (caps + 1).
        """
        original_json_text = """{
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8.0-106.json#",
            "profiles": {
                "VP_PARENT_profile": {
                    "version": 1,
                    "api-version": "1.1.0",
                    "capabilities": [
                        "caps1"
                    ]
                },
                "VP_CHILD_profile": {
                    "version": 1,
                    "api-version": "1.1.0",
                    "profiles": ["VP_PARENT_profile"],
                    "capabilities": [
                        "caps2"
                    ]
                }
            },
            "capabilities": {
                "caps1": {
                    "extensions": {
                        "VK_KHR_16bit_storage": 1,
                        "VK_KHR_imageless_framebuffer": 1
                    },
                    "features": {
                        "VkPhysicalDevice16BitStorageFeatures": {
                            "storageBuffer16BitAccess": true
                        }
                    }
                },
                "caps2": {
                    "extensions": {
                        "VK_KHR_16bit_storage": 1,
                        "VK_KHR_imageless_framebuffer": 1
                    },
                    "features": {
                        "VkPhysicalDevice16BitStorageFeatures": {
                            "storageBuffer16BitAccess": true,
                            "uniformAndStorageBuffer16BitAccess": true
                        },
                        "VkPhysicalDeviceImagelessFramebufferFeaturesKHR": {
                            "imagelessFramebuffer": true
                        }
                    }
                }
            }
        }"""

        expected_json_text = """{
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8.0-106.json#",
            "profiles": {
                "VP_PARENT_profile": {
                    "version": 1,
                    "api-version": "1.1.0",
                    "capabilities": [
                        "caps1"
                    ]
                },
                "VP_CHILD_profile": {
                    "version": 1,
                    "api-version": "1.1.0",
                    "profiles": ["VP_PARENT_profile"],
                    "capabilities": [
                        "caps2"
                    ]
                }
            },
            "capabilities": {
                "caps1": {
                    "extensions": {
                        "VK_KHR_16bit_storage": 1,
                        "VK_KHR_imageless_framebuffer": 1
                    },
                    "features": {
                        "VkPhysicalDevice16BitStorageFeatures": {
                            "storageBuffer16BitAccess": true
                        }
                    }
                },
                "caps2": {
                    "extensions": {
                        "VK_KHR_imageless_framebuffer": 1
                    },
                    "features": {
                        "VkPhysicalDevice16BitStorageFeatures": {
                            "uniformAndStorageBuffer16BitAccess": true
                        },
                        "VkPhysicalDeviceImagelessFramebufferFeaturesKHR": {
                            "imagelessFramebuffer": true
                        }
                    }
                }
            }
        }"""

        json_files_dict = {"test_profile.json": json.loads(original_json_text)}
        strip_duplication_profiles_files(self.vk, json_files_dict)

        self.assertEqual(json_files_dict["test_profile.json"], json.loads(expected_json_text))

    def test_strip_features_duplication_with_vulkan12_bundle_structure(self):
        """
        Verifies that duplicate feature entries inside Vulkan 1.2 bundle structures 
        (e.g., VkPhysicalDeviceVulkan11Features and VkPhysicalDeviceVulkan12Features)
        are properly stripped across inherited capability blocks.
        """
        original_json_text = """{
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8.0-106.json#",
            "profiles": {
                "VP_TEST_v12_profile": {
                    "version": 1,
                    "api-version": "1.2.0",
                    "capabilities": [
                        "caps1",
                        "caps2"
                    ]
                }
            },
            "capabilities": {
                "caps1": {
                    "features": {
                        "VkPhysicalDeviceVulkan11Features": {
                            "storageBuffer16BitAccess": true
                        },
                        "VkPhysicalDeviceVulkan12Features": {
                            "drawIndirectCount": true
                        }
                    }
                },
                "caps2": {
                    "features": {
                        "VkPhysicalDeviceVulkan11Features": {
                            "storageBuffer16BitAccess": true,
                            "uniformAndStorageBuffer16BitAccess": true
                        },
                        "VkPhysicalDeviceVulkan12Features": {
                            "drawIndirectCount": true,
                            "samplerMirrorClampToEdge": true
                        }
                    }
                }
            }
        }"""

        expected_json_text = """{
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8.0-106.json#",
            "profiles": {
                "VP_TEST_v12_profile": {
                    "version": 1,
                    "api-version": "1.2.0",
                    "capabilities": [
                        "caps1",
                        "caps2"
                    ]
                }
            },
            "capabilities": {
                "caps1": {
                    "features": {
                        "VkPhysicalDeviceVulkan11Features": {
                            "storageBuffer16BitAccess": true
                        },
                        "VkPhysicalDeviceVulkan12Features": {
                            "drawIndirectCount": true
                        }
                    }
                },
                "caps2": {
                    "features": {
                        "VkPhysicalDeviceVulkan11Features": {
                            "uniformAndStorageBuffer16BitAccess": true
                        },
                        "VkPhysicalDeviceVulkan12Features": {
                            "samplerMirrorClampToEdge": true
                        }
                    }
                }
            }
        }"""

        json_files_dict = {"test_profile.json": json.loads(original_json_text)}
        strip_duplication_profiles_files(self.vk, json_files_dict)

        self.assertEqual(json_files_dict["test_profile.json"], json.loads(expected_json_text))

    def test_strip_properties_duplication_vulkan11(self):
        """
        Verifies property duplication stripping for a Vulkan 1.1 profile using valid Vulkan 1.1
        property structures (VkPhysicalDeviceSubgroupProperties and VkPhysicalDeviceMultiviewProperties).
        """
        original_json_text = """{
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8.0-106.json#",
            "profiles": {
                "VP_TEST_v11_properties_profile": {
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
                    "properties": {
                        "VkPhysicalDeviceSubgroupProperties": {
                            "subgroupSize": 32,
                            "quadOperationsInAllStages": true
                        },
                        "VkPhysicalDeviceMultiviewProperties": {
                            "maxMultiviewViewCount": 6
                        }
                    }
                },
                "caps2": {
                    "properties": {
                        "VkPhysicalDeviceSubgroupProperties": {
                            "subgroupSize": 32,
                            "quadOperationsInAllStages": false
                        },
                        "VkPhysicalDeviceMultiviewProperties": {
                            "maxMultiviewViewCount": 6
                        }
                    }
                }
            }
        }"""

        expected_json_text = """{
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8.0-106.json#",
            "profiles": {
                "VP_TEST_v11_properties_profile": {
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
                    "properties": {
                        "VkPhysicalDeviceSubgroupProperties": {
                            "subgroupSize": 32,
                            "quadOperationsInAllStages": true
                        },
                        "VkPhysicalDeviceMultiviewProperties": {
                            "maxMultiviewViewCount": 6
                        }
                    }
                },
                "caps2": {
                    "properties": {
                        "VkPhysicalDeviceSubgroupProperties": {
                            "quadOperationsInAllStages": false
                        }
                    }
                }
            }
        }"""

        json_files_dict = {"test_profile.json": json.loads(original_json_text)}
        strip_duplication_profiles_files(self.vk, json_files_dict)

        self.assertEqual(json_files_dict["test_profile.json"], json.loads(expected_json_text))

    def test_strip_properties_duplication_vulkan12(self):
        """
        Verifies property duplication stripping for a Vulkan 1.2 profile using Vulkan 1.2 core 
        bundle structures (VkPhysicalDeviceVulkan11Properties).
        """
        original_json_text = """{
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8.0-106.json#",
            "profiles": {
                "VP_TEST_v12_properties_profile": {
                    "version": 1,
                    "api-version": "1.2.0",
                    "capabilities": [
                        "caps1",
                        "caps2"
                    ]
                }
            },
            "capabilities": {
                "caps1": {
                    "properties": {
                        "VkPhysicalDeviceVulkan11Properties": {
                            "maxSubgroupSize": 64,
                            "subgroupSize": 32,
                            "maxMultiviewViewCount": 12
                        }
                    }
                },
                "caps2": {
                    "properties": {
                        "VkPhysicalDeviceVulkan11Properties": {
                            "maxSubgroupSize": 64,
                            "subgroupSize": 64,
                            "maxMultiviewViewCount": 6
                        }
                    }
                }
            }
        }"""

        expected_json_text = """{
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8.0-106.json#",
            "profiles": {
                "VP_TEST_v12_properties_profile": {
                    "version": 1,
                    "api-version": "1.2.0",
                    "capabilities": [
                        "caps1",
                        "caps2"
                    ]
                }
            },
            "capabilities": {
                "caps1": {
                    "properties": {
                        "VkPhysicalDeviceVulkan11Properties": {
                            "maxSubgroupSize": 64,
                            "subgroupSize": 32,
                            "maxMultiviewViewCount": 12
                        }
                    }
                },
                "caps2": {
                    "properties": {
                        "VkPhysicalDeviceVulkan11Properties": {
                            "subgroupSize": 64,
                            "maxMultiviewViewCount": 6
                        }
                    }
                }
            }
        }"""

        json_files_dict = {"test_profile.json": json.loads(original_json_text)}
        strip_duplication_profiles_files(self.vk, json_files_dict)

        self.assertEqual(json_files_dict["test_profile.json"], json.loads(expected_json_text))

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


    def test_strip_intra_block_property_alias_duplication(self):
        """
        Verifies that when a single capability block contains duplicate property aliases
        (e.g., VkPhysicalDeviceMultiviewProperties and VkPhysicalDeviceMultiviewPropertiesKHR),
        the extension alias structure is stripped in favor of the canonical core structure.
        """
        original_json_text = """{
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8.0-204.json#",
            "profiles": {
                "VP_LUNARG_test": {
                    "version": 1,
                    "api-version": "1.1.0",
                    "capabilities": ["caps_multiview"]
                }
            },
            "capabilities": {
                "caps_multiview": {
                    "properties": {
                        "VkPhysicalDeviceMultiviewProperties": {
                            "maxMultiviewViewCount": 6
                        },
                        "VkPhysicalDeviceMultiviewPropertiesKHR": {
                            "maxMultiviewViewCount": 6
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
                    "api-version": "1.1.0",
                    "capabilities": ["caps_multiview"]
                }
            },
            "capabilities": {
                "caps_multiview": {
                    "properties": {
                        "VkPhysicalDeviceMultiviewProperties": {
                            "maxMultiviewViewCount": 6
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

    def test_strip_robustness2_extension_alias_duplication(self):
        """
        Verifies stripping behavior when a capability block contains duplicate robustness2
        features and extension aliases (VK_EXT_robustness2 and VK_KHR_robustness2).
        The EXT extension and structure are stripped in favor of the KHR variants.
        """
        original_json_text = """{
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8.0-204.json#",
            "profiles": {
                "VP_LUNARG_test_robustness2": {
                    "version": 1,
                    "api-version": "1.1.0",
                    "capabilities": [
                        "caps_robustness2"
                    ]
                }
            },
            "capabilities": {
                "caps_robustness2": {
                    "extensions": {
                        "VK_EXT_robustness2": 1,
                        "VK_KHR_robustness2": 1
                    },
                    "features": {
                        "VkPhysicalDeviceRobustness2FeaturesEXT": {
                            "robustBufferAccess2": true,
                            "robustImageAccess2": true,
                            "nullDescriptor": true
                        },
                        "VkPhysicalDeviceRobustness2FeaturesKHR": {
                            "robustBufferAccess2": true,
                            "robustImageAccess2": true,
                            "nullDescriptor": true
                        }
                    }
                }
            }
        }"""

        expected_json_text = """{
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8.0-204.json#",
            "profiles": {
                "VP_LUNARG_test_robustness2": {
                    "version": 1,
                    "api-version": "1.1.0",
                    "capabilities": [
                        "caps_robustness2"
                    ]
                }
            },
            "capabilities": {
                "caps_robustness2": {
                    "extensions": {
                        "VK_EXT_robustness2": 1,
                        "VK_KHR_robustness2": 1
                    },
                    "features": {
                        "VkPhysicalDeviceRobustness2FeaturesKHR": {
                            "robustBufferAccess2": true,
                            "robustImageAccess2": true,
                            "nullDescriptor": true
                        }
                    }
                }
            }
        }"""

        json_files_dict = {"test_profile.json": json.loads(original_json_text)}
        strip_duplication_profiles_files(self.vk, json_files_dict)

        self.assertEqual(json_files_dict["test_profile.json"], json.loads(expected_json_text))

    def test_strip_multiple_aliases_single_block_version11(self):
        """
        Verifies that when a single capability block contains duplicate feature aliases
        for shader draw parameters (VkPhysicalDeviceShaderDrawParameterFeatures vs
        VkPhysicalDeviceShaderDrawParametersFeatures) and variable pointers
        (VkPhysicalDeviceVariablePointerFeatures vs VkPhysicalDeviceVariablePointersFeatures),
        the redundant/non-canonical alias structures are stripped.
        """
        original_json_text = """{
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8.0-204.json#",
            "profiles": {
                "VP_LUNARG_test_draw_and_pointers": {
                    "version": 1,
                    "api-version": "1.1.0",
                    "capabilities": [
                        "caps_draw_and_pointers"
                    ]
                }
            },
            "capabilities": {
                "caps_draw_and_pointers": {
                    "extensions": {
                        "VK_KHR_shader_draw_parameters": 1,
                        "VK_KHR_variable_pointers": 1
                    },
                    "features": {
                        "VkPhysicalDeviceShaderDrawParameterFeatures": {
                            "shaderDrawParameters": true
                        },
                        "VkPhysicalDeviceShaderDrawParametersFeatures": {
                            "shaderDrawParameters": true
                        },
                        "VkPhysicalDeviceVariablePointerFeatures": {
                            "variablePointersStorageBuffer": true
                        },
                        "VkPhysicalDeviceVariablePointerFeaturesKHR": {
                            "variablePointersStorageBuffer": true
                        },
                        "VkPhysicalDeviceVariablePointersFeatures": {
                            "variablePointersStorageBuffer": true
                        },
                        "VkPhysicalDeviceVariablePointersFeaturesKHR": {
                            "variablePointersStorageBuffer": true
                        }
                    }
                }
            }
        }"""

        expected_json_text = """{
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8.0-204.json#",
            "profiles": {
                "VP_LUNARG_test_draw_and_pointers": {
                    "version": 1,
                    "api-version": "1.1.0",
                    "capabilities": [
                        "caps_draw_and_pointers"
                    ]
                }
            },
            "capabilities": {
                "caps_draw_and_pointers": {
                    "extensions": {
                        "VK_KHR_shader_draw_parameters": 1,
                        "VK_KHR_variable_pointers": 1
                    },
                    "features": {
                        "VkPhysicalDeviceShaderDrawParametersFeatures": {
                            "shaderDrawParameters": true
                        },
                        "VkPhysicalDeviceVariablePointersFeatures": {
                            "variablePointersStorageBuffer": true
                        }
                    }
                }
            }
        }"""

        json_files_dict = {"test_profile.json": json.loads(original_json_text)}
        strip_duplication_profiles_files(self.vk, json_files_dict)

        self.assertEqual(json_files_dict["test_profile.json"], json.loads(expected_json_text))

    def test_strip_multiple_aliases_single_block_version12(self):
        """
        Verifies that when a Vulkan 1.2 capability block contains duplicate feature aliases
        for shader draw parameters and variable pointers alongside the core bundle structure
        (VkPhysicalDeviceVulkan11Features), all redundant split and extension alias structures
        are stripped in favor of the active bundle structure.
        """
        original_json_text = """{
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8.0-204.json#",
            "profiles": {
                "VP_LUNARG_test_draw_and_pointers_v12": {
                    "version": 1,
                    "api-version": "1.2.0",
                    "capabilities": [
                        "caps_draw_and_pointers"
                    ]
                }
            },
            "capabilities": {
                "caps_draw_and_pointers": {
                    "extensions": {
                        "VK_KHR_shader_draw_parameters": 1,
                        "VK_KHR_variable_pointers": 1
                    },
                    "features": {
                        "VkPhysicalDeviceVulkan11Features": {
                            "shaderDrawParameters": true,
                            "variablePointersStorageBuffer": true
                        },
                        "VkPhysicalDeviceShaderDrawParameterFeatures": {
                            "shaderDrawParameters": true
                        },
                        "VkPhysicalDeviceVariablePointerFeaturesKHR": {
                            "variablePointersStorageBuffer": true
                        },
                        "VkPhysicalDeviceVariablePointersFeaturesKHR": {
                            "variablePointersStorageBuffer": true
                        }
                    }
                }
            }
        }"""

        expected_json_text = """{
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8.0-204.json#",
            "profiles": {
                "VP_LUNARG_test_draw_and_pointers_v12": {
                    "version": 1,
                    "api-version": "1.2.0",
                    "capabilities": [
                        "caps_draw_and_pointers"
                    ]
                }
            },
            "capabilities": {
                "caps_draw_and_pointers": {
                    "extensions": {
                        "VK_KHR_shader_draw_parameters": 1,
                        "VK_KHR_variable_pointers": 1
                    },
                    "features": {
                        "VkPhysicalDeviceVulkan11Features": {
                            "shaderDrawParameters": true,
                            "variablePointersStorageBuffer": true
                        }
                    }
                }
            }
        }"""

        json_files_dict = {"test_profile.json": json.loads(original_json_text)}
        strip_duplication_profiles_files(self.vk, json_files_dict)

        self.assertEqual(json_files_dict["test_profile.json"], json.loads(expected_json_text))

    def test_strip_multiple_aliases_inherited_profile_version12(self):
        """
        Verifies that when a child Vulkan 1.2 profile inherits from a parent profile containing
        the core bundle structure (VkPhysicalDeviceVulkan11Features with shaderDrawParameters and 
        variablePointersStorageBuffer), all duplicate split and extension alias feature structures 
        in the child profile are stripped against the parent profile.
        """
        original_json_text = """{
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8.0-204.json#",
            "profiles": {
                "VP_PARENT_profile": {
                    "version": 1,
                    "api-version": "1.1.0",
                    "capabilities": [
                        "caps_parent"
                    ]
                },
                "VP_CHILD_profile": {
                    "version": 1,
                    "api-version": "1.2.0",
                    "profiles": [
                        "VP_PARENT_profile"
                    ],
                    "capabilities": [
                        "caps_child"
                    ]
                }
            },
            "capabilities": {
                "caps_parent": {
                    "extensions": {
                        "VK_KHR_shader_draw_parameters": 1,
                        "VK_KHR_variable_pointers": 1
                    },
                    "features": {
                        "VkPhysicalDeviceVariablePointerFeaturesKHR": {
                            "variablePointersStorageBuffer": true
                        },
                        "VkPhysicalDeviceVariablePointersFeaturesKHR": {
                            "variablePointersStorageBuffer": true
                        }
                    }
                },
                "caps_child": {
                    "features": {
                        "VkPhysicalDeviceVulkan12Features": {
                            "drawIndirectCount": true
                        },
                        "VkPhysicalDeviceShaderDrawParameterFeatures": {
                            "shaderDrawParameters": true
                        },
                        "VkPhysicalDeviceShaderDrawParametersFeatures": {
                            "shaderDrawParameters": true
                        },
                        "VkPhysicalDeviceVariablePointerFeatures": {
                            "variablePointersStorageBuffer": true
                        },
                        "VkPhysicalDeviceVariablePointersFeatures": {
                            "variablePointersStorageBuffer": true
                        }
                    }
                }
            }
        }"""

        expected_json_text = """{
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8.0-204.json#",
            "profiles": {
                "VP_PARENT_profile": {
                    "version": 1,
                    "api-version": "1.1.0",
                    "capabilities": [
                        "caps_parent"
                    ]
                },
                "VP_CHILD_profile": {
                    "version": 1,
                    "api-version": "1.2.0",
                    "profiles": [
                        "VP_PARENT_profile"
                    ],
                    "capabilities": [
                        "caps_child"
                    ]
                }
            },
            "capabilities": {
                "caps_parent": {
                    "extensions": {
                        "VK_KHR_shader_draw_parameters": 1,
                        "VK_KHR_variable_pointers": 1
                    },
                    "features": {
                        "VkPhysicalDeviceVariablePointerFeaturesKHR": {
                            "variablePointersStorageBuffer": true
                        }
                    }
                },
                "caps_child": {
                    "features": {
                        "VkPhysicalDeviceVulkan12Features": {
                            "drawIndirectCount": true
                        },
                        "VkPhysicalDeviceShaderDrawParametersFeatures": {
                            "shaderDrawParameters": true
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
    TestConvertStripDuplication.registry_path = args.registry

    unittest.main(argv=[sys.argv[0]] + unparsed)
    