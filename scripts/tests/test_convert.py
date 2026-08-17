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
import copy
from pathlib import Path
import sys
import unittest

scripts_dir = Path(__file__).resolve().parent.parent
if str(scripts_dir) not in sys.path:
    sys.path.insert(0, str(scripts_dir))

from vulkan_object import VulkanObject
from source.vulkan_object_utils import initVulkanObject, VK_VERSION
from source.main_convert import (
    pull_promoted_extensions_profiles_files,
    pull_required_features_profiles_files,
    pull_aliases_profiles_files,
    make_unique_profiles_files,
    cleanup_and_sort_pulled_blocks
)


class TestMainConvertPipeline(unittest.TestCase):
    registry_path = None

    def testExtensionFeaturePullingAndAliasExpansionOrder(self):
        """
        Verifies that feature requirements defined by enabled extensions 
        (e.g., VK_KHR_variable_pointers) are pulled directly into the capability 
        block defining the extension, and subsequent alias expansion populates 
        both structure aliases (VkPhysicalDeviceVariablePointerFeaturesKHR and 
        VkPhysicalDeviceVariablePointersFeaturesKHR) in that same block.
        """
        vk: VulkanObject = initVulkanObject('vulkan', self.registry_path)

        json_file_data = {
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
                    }
                }
            }
        }
        json_files_dict = {"test_profile.json": json_file_data}

        # 1. Pull required features into capability blocks
        pull_required_features_profiles_files(vk, json_files_dict)

        # Verify variablePointersStorageBuffer was pulled into 'baseline' block, NOT a core block
        baseline_features = json_file_data["capabilities"]["baseline"].get("features", {})
        self.assertIn("VkPhysicalDeviceVariablePointerFeaturesKHR", baseline_features)
        self.assertTrue(baseline_features["VkPhysicalDeviceVariablePointerFeaturesKHR"]["variablePointersStorageBuffer"])
        self.assertNotIn("vulkan10pulledrequirements", json_file_data["capabilities"])

        # 2. Expand capability aliases across capability blocks
        pull_aliases_profiles_files(vk, False, json_files_dict)

        # Verify alias expansion created BOTH struct aliases within 'baseline'
        updated_features = json_file_data["capabilities"]["baseline"]["features"]
        self.assertIn("VkPhysicalDeviceVariablePointerFeaturesKHR", updated_features)
        self.assertIn("VkPhysicalDeviceVariablePointersFeaturesKHR", updated_features)
        self.assertTrue(updated_features["VkPhysicalDeviceVariablePointerFeaturesKHR"]["variablePointersStorageBuffer"])
        self.assertTrue(updated_features["VkPhysicalDeviceVariablePointersFeaturesKHR"]["variablePointersStorageBuffer"])

    def testUniqueFeatureStripping(self):
        """
        Verifies that when make_unique_profiles_files is executed (UNIQUE mode), 
        extension structure aliases (e.g., VkPhysicalDevice16BitStorageFeaturesKHR) 
        are stripped when the core split structure (e.g., VkPhysicalDevice16BitStorageFeatures)
        is defined.
        """
        vk: VulkanObject = initVulkanObject('vulkan', self.registry_path)

        json_file_data = {
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
                            "storageBuffer16BitAccess": True,
                            "uniformAndStorageBuffer16BitAccess": True
                        },
                        "VkPhysicalDevice16BitStorageFeaturesKHR": {
                            "storageBuffer16BitAccess": True,
                            "uniformAndStorageBuffer16BitAccess": True
                        }
                    }
                }
            }
        }
        json_files_dict = {"test_profile.json": json_file_data}

        # Execute UNIQUE phase
        make_unique_profiles_files(vk, json_files_dict)

        # Extension structure alias must be stripped, leaving only the Core 1.1 structure
        features = json_file_data["capabilities"]["baseline"]["features"]
        self.assertIn("VkPhysicalDevice16BitStorageFeatures", features)
        self.assertNotIn("VkPhysicalDevice16BitStorageFeaturesKHR", features)

    def testUniqueHierarchyPruningCoreOverVendor(self):
        """Verifies that Core Base structures prune KHR, EXT, and Vendor aliases."""
        vk: VulkanObject = initVulkanObject('vulkan', self.registry_path)

        json_file_data = {
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8.0-106.json#",
            "profiles": {
                "VP_TEST_profile": {"version": 1, "api-version": "1.1.0", "capabilities": ["baseline"]}
            },
            "capabilities": {
                "baseline": {
                    "features": {
                        "VkPhysicalDevice16BitStorageFeatures": {
                            "storageBuffer16BitAccess": True
                        },
                        "VkPhysicalDevice16BitStorageFeaturesKHR": {
                            "storageBuffer16BitAccess": True
                        }
                    }
                }
            }
        }
        json_files_dict = {"test_profile.json": json_file_data}

        make_unique_profiles_files(vk, json_files_dict)

        features = json_file_data["capabilities"]["baseline"]["features"]
        self.assertIn("VkPhysicalDevice16BitStorageFeatures", features)
        self.assertNotIn("VkPhysicalDevice16BitStorageFeaturesKHR", features)

    def testUniquePartialMemberStripping(self):
        """Verifies that non-overlapping members in lower-priority structures are retained."""
        vk: VulkanObject = initVulkanObject('vulkan', self.registry_path)

        json_file_data = {
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8.0-106.json#",
            "profiles": {
                "VP_TEST_profile": {"version": 1, "api-version": "1.1.0", "capabilities": ["baseline"]}
            },
            "capabilities": {
                "baseline": {
                    "features": {
                        "VkPhysicalDevice16BitStorageFeatures": {
                            "storageBuffer16BitAccess": True
                        },
                        "VkPhysicalDevice16BitStorageFeaturesKHR": {
                            "storageBuffer16BitAccess": True,
                            "uniformAndStorageBuffer16BitAccess": True
                        }
                    }
                }
            }
        }
        json_files_dict = {"test_profile.json": json_file_data}

        make_unique_profiles_files(vk, json_files_dict)

        features = json_file_data["capabilities"]["baseline"]["features"]
        self.assertIn("VkPhysicalDevice16BitStorageFeatures", features)
        self.assertIn("VkPhysicalDevice16BitStorageFeaturesKHR", features)
        self.assertNotIn("storageBuffer16BitAccess", features["VkPhysicalDevice16BitStorageFeaturesKHR"])
        self.assertTrue(features["VkPhysicalDevice16BitStorageFeaturesKHR"]["uniformAndStorageBuffer16BitAccess"])

    def testUniqueValueMismatchProtection(self):
        """Verifies that lower-priority structure members are preserved if values do not match."""
        vk: VulkanObject = initVulkanObject('vulkan', self.registry_path)

        json_file_data = {
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8.0-106.json#",
            "profiles": {
                "VP_TEST_profile": {"version": 1, "api-version": "1.1.0", "capabilities": ["baseline"]}
            },
            "capabilities": {
                "baseline": {
                    "features": {
                        "VkPhysicalDevice16BitStorageFeatures": {
                            "storageBuffer16BitAccess": True
                        },
                        "VkPhysicalDevice16BitStorageFeaturesKHR": {
                            "storageBuffer16BitAccess": False
                        }
                    }
                }
            }
        }
        json_files_dict = {"test_profile.json": json_file_data}

        make_unique_profiles_files(vk, json_files_dict)

        features = json_file_data["capabilities"]["baseline"]["features"]
        self.assertIn("VkPhysicalDevice16BitStorageFeaturesKHR", features)
        self.assertFalse(features["VkPhysicalDevice16BitStorageFeaturesKHR"]["storageBuffer16BitAccess"])

    def testUniquePropertiesDeduplication(self):
        """Verifies that UNIQUE mode handles property structures across the priority hierarchy."""
        vk: VulkanObject = initVulkanObject('vulkan', self.registry_path)

        json_file_data = {
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8.0-106.json#",
            "profiles": {
                "VP_TEST_profile": {"version": 1, "api-version": "1.1.0", "capabilities": ["baseline"]}
            },
            "capabilities": {
                "baseline": {
                    "properties": {
                        "VkPhysicalDeviceVulkan11Properties": {
                            "maxMultiviewViewCount": 6
                        },
                        "VkPhysicalDeviceMultiviewPropertiesKHR": {
                            "maxMultiviewViewCount": 6
                        }
                    }
                }
            }
        }
        json_files_dict = {"test_profile.json": json_file_data}

        make_unique_profiles_files(vk, json_files_dict)

        properties = json_file_data["capabilities"]["baseline"]["properties"]
        self.assertIn("VkPhysicalDeviceVulkan11Properties", properties)
        self.assertNotIn("VkPhysicalDeviceMultiviewPropertiesKHR", properties)

    def testPullPromotedExtensions(self):
        """
        Verifies that pull_promoted_extensions_profiles_files collects extensions 
        promoted to the exact core API version and puts them into version-specific 
        vulkan1Xpulledrequirements blocks.
        """
        vk: VulkanObject = initVulkanObject('vulkan', self.registry_path)

        json_file_data = {
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8.0-106.json#",
            "profiles": {
                "VP_TEST_profile_v11": {
                    "version": 1,
                    "api-version": "1.1.0",
                    "capabilities": ["vulkan10requirements"]
                }
            },
            "capabilities": {
                "vulkan10requirements": {
                    "features": {
                        "VkPhysicalDeviceFeatures": {
                            "robustBufferAccess": True
                        }
                    }
                }
            }
        }
        json_files_dict = {"test_profile.json": json_file_data}

        pull_promoted_extensions_profiles_files(vk, True, json_files_dict)

        # vulkan11pulledrequirements block should be created for Vulkan 1.1 promoted extensions
        self.assertIn("vulkan11pulledrequirements", json_file_data["capabilities"])
        v11_exts = json_file_data["capabilities"]["vulkan11pulledrequirements"].get("extensions", {})
        self.assertIn("VK_KHR_multiview", v11_exts)

    def testCleanupAndSortPulledBlocksOrdering(self):
        """
        Verifies that cleanup_and_sort_pulled_blocks cleans up empty pulled blocks 
        and ensures authored capability blocks always precede pulled requirement blocks 
        in the profile's capabilities list.
        """
        json_file_data = {
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8.0-106.json#",
            "profiles": {
                "VP_TEST_profile": {
                    "version": 1,
                    "api-version": "1.4.0",
                    "capabilities": [
                        "vulkan14pulledrequirements",
                        "vulkan10requirements",
                        "vulkan11requirements",
                        "vulkan11pulledrequirements"
                    ]
                }
            },
            "capabilities": {
                "vulkan10requirements": {
                    "features": {"VkPhysicalDeviceFeatures": {"robustBufferAccess": True}}
                },
                "vulkan11requirements": {
                    "features": {"VkPhysicalDeviceVulkan11Features": {"multiview": True}}
                },
                "vulkan14pulledrequirements": {
                    "features": {"VkPhysicalDeviceVulkan14Features": {"pushDescriptor": True}}
                },
                "vulkan11pulledrequirements": {}  # Empty pulled block
            }
        }

        cleanup_and_sort_pulled_blocks(json_file_data)

        # Empty vulkan11pulledrequirements block should be removed
        self.assertNotIn("vulkan11pulledrequirements", json_file_data["capabilities"])

        # Authored blocks must precede pulled blocks
        caps = json_file_data["profiles"]["VP_TEST_profile"]["capabilities"]
        self.assertEqual(caps, ["vulkan10requirements", "vulkan11requirements", "vulkan14pulledrequirements"])

    def testStalePulledBlockCleanup(self):
        """
        Verifies that previously generated or stale core pulled requirements blocks 
        (e.g., vulkan10pulledrequirements) present in the input JSON are properly 
        cleaned up and re-evaluated.
        """
        vk: VulkanObject = initVulkanObject('vulkan', self.registry_path)

        json_file_data = {
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8.0-106.json#",
            "profiles": {
                "VP_TEST_profile": {
                    "version": 1,
                    "api-version": "1.0.68",
                    "capabilities": ["vulkan10pulledrequirements", "baseline"]
                }
            },
            "capabilities": {
                "baseline": {
                    "extensions": {
                        "VK_KHR_variable_pointers": 1
                    }
                },
                "vulkan10pulledrequirements": {}
            }
        }
        json_files_dict = {"test_profile.json": json_file_data}

        # Run feature pulling phase
        pull_required_features_profiles_files(vk, json_files_dict)

        # Empty vulkan10pulledrequirements should be deleted
        self.assertNotIn("vulkan10pulledrequirements", json_file_data["capabilities"])

    def testCoreFeaturePullingTargetApiVersionBoundary(self):
        """
        Verifies that core version features (such as Vulkan 1.1/1.2 features) are NOT 
        pulled into a core pulled requirements block if the target profile API version is Vulkan 1.0.
        """
        vk: VulkanObject = initVulkanObject('vulkan', self.registry_path)

        json_file_data_v10 = {
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8.0-106.json#",
            "profiles": {
                "VP_TEST_v10": {
                    "version": 1,
                    "api-version": "1.0.68",
                    "capabilities": ["baseline"]
                }
            },
            "capabilities": {
                "baseline": {
                    "features": {
                        "VkPhysicalDeviceFeatures": {
                            "robustBufferAccess": True
                        }
                    }
                }
            }
        }
        json_files_dict = {"test_profile_v10.json": json_file_data_v10}

        pull_required_features_profiles_files(vk, json_files_dict)

        # No core 1.0 features with dependencies should generate a pulled block for 1.0.68
        self.assertNotIn("vulkan10pulledrequirements", json_file_data_v10["capabilities"])


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument(
        '--registry', '-r', action='store', required=False,
        help='Use specified registry file instead of vk.xml.'
    )

    args, unparsed = parser.parse_known_args()
    TestMainConvertPipeline.registry_path = args.registry

    unittest.main(argv=[sys.argv[0]] + unparsed)
    