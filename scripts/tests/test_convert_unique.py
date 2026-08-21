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
import logging
from pathlib import Path
import sys
import unittest

scripts_dir = Path(__file__).resolve().parent.parent
if str(scripts_dir) not in sys.path:
    sys.path.insert(0, str(scripts_dir))

from vulkan_object import VulkanObject
from source.vulkan_object_utils import initVulkanObject
from source.main_convert import make_unique_profiles_files, make_unique_capabilities_block


class TestConvertUnique(unittest.TestCase):
    registry_path = None

    def setUp(self):
        self.vk: VulkanObject = initVulkanObject('vulkan', self.registry_path)

    def test_promoted_to_core_version_exact_match(self):
        """
        Tests that an extension feature structure promoted to a Vulkan core version 
        is stripped when the core structure is present and feature values match.
        """
        original_json_text = """{
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8.0-106.json#",
            "profiles": {
                "VP_TEST_profile": {
                    "version": 1,
                    "api-version": "1.1.0",
                    "capabilities": ["baseline"]
                }
            },
            "capabilities": {
                "baseline": {
                    "features": {
                        "VkPhysicalDevice16BitStorageFeatures": {
                            "storageBuffer16BitAccess": true
                        },
                        "VkPhysicalDevice16BitStorageFeaturesKHR": {
                            "storageBuffer16BitAccess": true
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
                    "api-version": "1.1.0",
                    "capabilities": ["baseline"]
                }
            },
            "capabilities": {
                "baseline": {
                    "features": {
                        "VkPhysicalDevice16BitStorageFeatures": {
                            "storageBuffer16BitAccess": true
                        }
                    }
                }
            }
        }"""

        json_files_dict = {"test_profile.json": json.loads(original_json_text)}
        make_unique_profiles_files(self.vk, json_files_dict)

        self.assertEqual(json_files_dict["test_profile.json"], json.loads(expected_json_text))

    def test_promoted_to_core_version_value_mismatch_warning(self):
        """
        Tests that an extension feature structure promoted to a core version is stripped,
        and a warning is logged when a feature value in the extension struct differs from core.
        """
        original_json_text = """{
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8.0-106.json#",
            "profiles": {
                "VP_TEST_profile": {
                    "version": 1,
                    "api-version": "1.1.0",
                    "capabilities": ["baseline"]
                }
            },
            "capabilities": {
                "baseline": {
                    "features": {
                        "VkPhysicalDevice16BitStorageFeatures": {
                            "storageBuffer16BitAccess": false
                        },
                        "VkPhysicalDevice16BitStorageFeaturesKHR": {
                            "storageBuffer16BitAccess": true
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
                    "api-version": "1.1.0",
                    "capabilities": ["baseline"]
                }
            },
            "capabilities": {
                "baseline": {
                    "features": {
                        "VkPhysicalDevice16BitStorageFeatures": {
                            "storageBuffer16BitAccess": false
                        }
                    }
                }
            }
        }"""

        json_files_dict = {"test_profile.json": json.loads(original_json_text)}

        with self.assertLogs(level='WARNING') as cm:
            make_unique_profiles_files(self.vk, json_files_dict)

        self.assertEqual(json_files_dict["test_profile.json"], json.loads(expected_json_text))
        self.assertTrue(any("differs" in log for log in cm.output))

    def test_promoted_to_core_version_missing_feature_warning(self):
        """
        Tests that an extension feature structure promoted to core is stripped,
        and a warning is logged when a feature in the extension struct is missing in core.
        """
        original_json_text = """{
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8.0-106.json#",
            "profiles": {
                "VP_TEST_profile": {
                    "version": 1,
                    "api-version": "1.1.0",
                    "capabilities": ["baseline"]
                }
            },
            "capabilities": {
                "baseline": {
                    "features": {
                        "VkPhysicalDevice16BitStorageFeatures": {
                            "storageBuffer16BitAccess": true
                        },
                        "VkPhysicalDevice16BitStorageFeaturesKHR": {
                            "storageBuffer16BitAccess": true,
                            "nonExistentCoreFeature": true
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
                    "api-version": "1.1.0",
                    "capabilities": ["baseline"]
                }
            },
            "capabilities": {
                "baseline": {
                    "features": {
                        "VkPhysicalDevice16BitStorageFeatures": {
                            "storageBuffer16BitAccess": true
                        }
                    }
                }
            }
        }"""

        json_files_dict = {"test_profile.json": json.loads(original_json_text)}

        with self.assertLogs(level='WARNING') as cm:
            make_unique_profiles_files(self.vk, json_files_dict)

        self.assertEqual(json_files_dict["test_profile.json"], json.loads(expected_json_text))
        self.assertTrue(any("missing" in log for log in cm.output))

    def test_promoted_to_extension_with_aliases(self):
        """
        Tests that when extension A is promoted to extension B and their feature structures 
        are aliases, the feature structure of the "promotedto" extension is kept.
        """
        original_json_text = """{
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8.0-106.json#",
            "profiles": {
                "VP_TEST_profile": {
                    "version": 1,
                    "api-version": "1.0.0",
                    "capabilities": ["baseline"]
                }
            },
            "capabilities": {
                "baseline": {
                    "features": {
                        "VkPhysicalDeviceVariablePointerFeatures": {
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
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8.0-106.json#",
            "profiles": {
                "VP_TEST_profile": {
                    "version": 1,
                    "api-version": "1.0.0",
                    "capabilities": ["baseline"]
                }
            },
            "capabilities": {
                "baseline": {
                    "features": {
                        "VkPhysicalDeviceVariablePointersFeaturesKHR": {
                            "variablePointersStorageBuffer": true
                        }
                    }
                }
            }
        }"""

        json_files_dict = {"test_profile.json": json.loads(original_json_text)}
        make_unique_profiles_files(self.vk, json_files_dict)

        self.assertEqual(json_files_dict["test_profile.json"], json.loads(expected_json_text))

    def test_promoted_to_extension_without_aliases(self):
        """
        Tests that when feature structures are NOT aliases, both structures are preserved.
        """
        original_json_text = """{
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8.0-106.json#",
            "profiles": {
                "VP_TEST_profile": {
                    "version": 1,
                    "api-version": "1.0.0",
                    "capabilities": ["baseline"]
                }
            },
            "capabilities": {
                "baseline": {
                    "features": {
                        "VkPhysicalDeviceCustomFeatureStructA": {
                            "customFeature": true
                        },
                        "VkPhysicalDeviceCustomFeatureStructB": {
                            "customFeature": true
                        }
                    }
                }
            }
        }"""

        expected_json_text = copy.deepcopy(json.loads(original_json_text))

        json_files_dict = {"test_profile.json": json.loads(original_json_text)}
        make_unique_profiles_files(self.vk, json_files_dict)

        self.assertEqual(json_files_dict["test_profile.json"], expected_json_text)

    def test_unique_mode_applied_only_to_features(self):
        """
        Verifies that UNIQUE mode operates exclusively on 'features' 
        and leaves 'properties' and 'formats' completely untouched.
        """
        original_json_text = """{
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8.0-106.json#",
            "profiles": {
                "VP_TEST_profile": {
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
                        }
                    },
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
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8.0-106.json#",
            "profiles": {
                "VP_TEST_profile": {
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
                        }
                    },
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

        json_files_dict = {"test_profile.json": json.loads(original_json_text)}
        make_unique_profiles_files(self.vk, json_files_dict)

        self.assertEqual(json_files_dict["test_profile.json"], json.loads(expected_json_text))


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument(
        '--registry', '-r', action='store', required=False,
        help='Use specified registry file instead of vk.xml.'
    )

    args, unparsed = parser.parse_known_args()
    TestConvertUnique.registry_path = args.registry

    unittest.main(argv=[sys.argv[0]] + unparsed)
    