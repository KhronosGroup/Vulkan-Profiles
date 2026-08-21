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
from source.main_convert import pull_aliases_profiles_files


class TestConvertPullAliases(unittest.TestCase):
    registry_path = None

    def setUp(self):
        self.vk: VulkanObject = initVulkanObject('vulkan', self.registry_path)

    def test_pull_structural_feature_aliases_vulkan_10(self):
        """
        Verifies that expanding feature aliases in a Vulkan 1.0 profile populates 
        structural aliases (e.g. VkPhysicalDeviceVariablePointerFeaturesKHR and 
        VkPhysicalDeviceVariablePointersFeaturesKHR).
        """
        original_json_text = """{
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8.0-106.json#",
            "profiles": {
                "VP_TEST_profile": {
                    "version": 1,
                    "api-version": "1.0.68",
                    "capabilities": ["baseline"]
                }
            },
            "capabilities": {
                "baseline": {
                    "extensions": {
                        "VK_KHR_variable_pointers": 1
                    },
                    "features": {
                        "VkPhysicalDeviceVariablePointerFeaturesKHR": {
                            "variablePointersStorageBuffer": true
                        }
                    }
                }
            }
        }"""

        expected_json_text = """{
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8.0-106.json#",
            "profiles": {
                "VP_TEST_profile": {
                    "version": 1,
                    "api-version": "1.0.68",
                    "capabilities": ["baseline"]
                }
            },
            "capabilities": {
                "baseline": {
                    "extensions": {
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
                }
            }
        }"""

        json_files_dict = {"test_profile.json": json.loads(original_json_text)}
        pull_aliases_profiles_files(self.vk, False, json_files_dict)

        self.assertEqual(json_files_dict["test_profile.json"], json.loads(expected_json_text))

    def test_pull_structural_feature_aliases_vulkan_11(self):
        """
        Verifies that expanding feature aliases in a Vulkan 1.1 profile populates 
        Core 1.1 split structures and extension aliases, but NOT VkPhysicalDeviceVulkan11Features
        (which requires Vulkan 1.2+).
        """
        original_json_text = """{
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8.0-106.json#",
            "profiles": {
                "VP_TEST_profile_v11": {
                    "version": 1,
                    "api-version": "1.1.0",
                    "capabilities": ["baseline"]
                }
            },
            "capabilities": {
                "baseline": {
                    "extensions": {
                        "VK_KHR_16bit_storage": 1,
                        "VK_KHR_multiview": 1
                    },
                    "features": {
                        "VkPhysicalDevice16BitStorageFeatures": {
                            "storageBuffer16BitAccess": true
                        },
                        "VkPhysicalDeviceMultiviewFeaturesKHR": {
                            "multiview": true
                        }
                    }
                }
            }
        }"""

        expected_json_text = """{
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8.0-106.json#",
            "profiles": {
                "VP_TEST_profile_v11": {
                    "version": 1,
                    "api-version": "1.1.0",
                    "capabilities": ["baseline"]
                }
            },
            "capabilities": {
                "baseline": {
                    "extensions": {
                        "VK_KHR_16bit_storage": 1,
                        "VK_KHR_multiview": 1
                    },
                    "features": {
                        "VkPhysicalDevice16BitStorageFeatures": {
                            "storageBuffer16BitAccess": true
                        },
                        "VkPhysicalDevice16BitStorageFeaturesKHR": {
                            "storageBuffer16BitAccess": true
                        },
                        "VkPhysicalDeviceMultiviewFeatures": {
                            "multiview": true
                        },
                        "VkPhysicalDeviceMultiviewFeaturesKHR": {
                            "multiview": true
                        }
                    }
                }
            }
        }"""

        json_files_dict = {"test_profile.json": json.loads(original_json_text)}
        pull_aliases_profiles_files(self.vk, False, json_files_dict)

        self.assertEqual(json_files_dict["test_profile.json"], json.loads(expected_json_text))

    def test_pull_structural_feature_aliases_vulkan_12(self):
        """
        Verifies that expanding feature aliases in a Vulkan 1.2 profile populates 
        Core 1.2 bundle structures (VkPhysicalDeviceVulkan12Features), Core 1.2 split structures,
        and extension aliases.
        """
        original_json_text = """{
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8.0-106.json#",
            "profiles": {
                "VP_TEST_profile_v12": {
                    "version": 1,
                    "api-version": "1.2.0",
                    "capabilities": ["baseline"]
                }
            },
            "capabilities": {
                "baseline": {
                    "extensions": {
                        "VK_KHR_8bit_storage": 1
                    },
                    "features": {
                        "VkPhysicalDevice8BitStorageFeaturesKHR": {
                            "storageBuffer8BitAccess": true
                        }
                    }
                }
            }
        }"""

        expected_json_text = """{
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8.0-106.json#",
            "profiles": {
                "VP_TEST_profile_v12": {
                    "version": 1,
                    "api-version": "1.2.0",
                    "capabilities": ["baseline"]
                }
            },
            "capabilities": {
                "baseline": {
                    "extensions": {
                        "VK_KHR_8bit_storage": 1
                    },
                    "features": {
                        "VkPhysicalDevice8BitStorageFeatures": {
                            "storageBuffer8BitAccess": true
                        },
                        "VkPhysicalDevice8BitStorageFeaturesKHR": {
                            "storageBuffer8BitAccess": true
                        },
                        "VkPhysicalDeviceVulkan12Features": {
                            "storageBuffer8BitAccess": true
                        }
                    }
                }
            }
        }"""

        json_files_dict = {"test_profile.json": json.loads(original_json_text)}
        pull_aliases_profiles_files(self.vk, False, json_files_dict)

        self.assertEqual(json_files_dict["test_profile.json"], json.loads(expected_json_text))


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument(
        '--registry', '-r', action='store', required=False,
        help='Use specified registry file instead of vk.xml.'
    )

    args, unparsed = parser.parse_known_args()
    TestConvertPullAliases.registry_path = args.registry

    unittest.main(argv=[sys.argv[0]] + unparsed)
    