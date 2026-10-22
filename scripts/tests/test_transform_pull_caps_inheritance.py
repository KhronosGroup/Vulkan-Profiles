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


class TestConvertPullRequiredCapabilitiesCoreInheriance(unittest.TestCase):
    registry_path = None

    def setUp(self):
        self.vk: VulkanObject = initVulkanObject('vulkan', self.registry_path)

    def test_pull_required_capabilities_profile_inheritance_vulkan10_to_vulkan11(self):
        """
        Verifies core capability pulling when a Vulkan 1.1 child profile inherits from a Vulkan 1.0 parent profile.
        The Vulkan 1.0 parent profile pulls Vulkan 1.0 core capabilities, and a transition block
        ('VP_TEST_profile_v10_to_vulkan11') is dynamically generated for the Vulkan 1.1 child profile.
        """
        original_json_text = """{
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8.0-106.json#",
            "profiles": {
                "VP_TEST_profile_v10": {
                    "version": 1,
                    "api-version": "1.0.68",
                    "capabilities": ["baseline_v10"]
                },
                "VP_TEST_profile_v11": {
                    "version": 1,
                    "api-version": "1.1.108",
                    "profiles": ["VP_TEST_profile_v10"],
                    "capabilities": ["baseline_v11"]
                }
            },
            "capabilities": {
                "baseline_v10": {},
                "baseline_v11": {}
            }
        }"""

        json_files_dict = {"test_profile.json": json.loads(original_json_text)}
        pull_required_capabilities_profiles_files(self.vk, [PullBits.REQUIRED_CAPABILITIES], json_files_dict)
        sort_profiles_files(self.vk, json_files_dict)

        result_data = json_files_dict["test_profile.json"]
        profiles_dict = result_data["profiles"]
        capabilities_dict = result_data["capabilities"]

        # 1. Verify parent profile v10 capabilities list
        self.assertEqual(profiles_dict["VP_TEST_profile_v10"].get("capabilities"), ["baseline_v10"])

        # 2. Verify child profile v11 received generated transition block
        self.assertEqual(
            profiles_dict["VP_TEST_profile_v11"].get("capabilities"),
            ["VP_TEST_profile_v10_to_vulkan11", "baseline_v11"]
        )

        # 3. Verify baseline_v10 contains Vulkan 1.0 core features & properties
        self.assertTrue(capabilities_dict["baseline_v10"]["features"]["VkPhysicalDeviceFeatures"].get("robustBufferAccess"))
        self.assertIn("VkPhysicalDeviceProperties", capabilities_dict["baseline_v10"].get("properties", {}))

        # 4. Verify transition block VP_TEST_profile_v10_to_vulkan11
        trans_v11 = capabilities_dict.get("VP_TEST_profile_v10_to_vulkan11", {})
        self.assertTrue(trans_v11["features"]["VkPhysicalDeviceMultiviewFeatures"].get("multiview"))
        self.assertIn("VkPhysicalDeviceMaintenance3Properties", trans_v11.get("properties", {}))
        self.assertIn("VkPhysicalDeviceMultiviewProperties", trans_v11.get("properties", {}))
        self.assertIn("VkPhysicalDeviceSubgroupProperties", trans_v11.get("properties", {}))

    def test_pull_required_capabilities_profile_inheritance_vulkan11_to_vulkan12(self):
        """
        Verifies core capability pulling when a Vulkan 1.2 child profile inherits from a Vulkan 1.1 parent profile.
        A transition block ('VP_TEST_profile_v11_to_vulkan12') is dynamically generated for the Vulkan 1.2 child profile,
        pulling Vulkan 1.2 core features and Vulkan 1.2 core properties (VkPhysicalDeviceVulkan11Properties is omitted
        as its values are already specified via split structures in Vulkan 1.1).
        """
        original_json_text = """{
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8.0-131.json#",
            "profiles": {
                "VP_TEST_profile_v11": {
                    "version": 1,
                    "api-version": "1.1.108",
                    "capabilities": ["baseline_v11"]
                },
                "VP_TEST_profile_v12": {
                    "version": 1,
                    "api-version": "1.2.131",
                    "profiles": ["VP_TEST_profile_v11"],
                    "capabilities": ["baseline_v12"]
                }
            },
            "capabilities": {
                "baseline_v11": {},
                "baseline_v12": {}
            }
        }"""

        json_files_dict = {"test_profile.json": json.loads(original_json_text)}
        pull_required_capabilities_profiles_files(self.vk, [PullBits.REQUIRED_CAPABILITIES], json_files_dict)
        sort_profiles_files(self.vk, json_files_dict)

        result_data = json_files_dict["test_profile.json"]
        profiles_dict = result_data["profiles"]
        capabilities_dict = result_data["capabilities"]

        # 1. Verify parent profile v11 capabilities list
        self.assertEqual(profiles_dict["VP_TEST_profile_v11"].get("capabilities"), ["baseline_v11"])

        # 2. Verify child profile v12 received generated transition block
        self.assertEqual(
            profiles_dict["VP_TEST_profile_v12"].get("capabilities"),
            ["VP_TEST_profile_v11_to_vulkan12", "baseline_v12"]
        )

        # 3. Verify parent baseline_v11 features and properties
        v11_features = capabilities_dict["baseline_v11"].get("features", {})
        self.assertTrue(v11_features["VkPhysicalDeviceFeatures"].get("robustBufferAccess"))
        self.assertTrue(v11_features["VkPhysicalDeviceMultiviewFeatures"].get("multiview"))

        v11_properties = capabilities_dict["baseline_v11"].get("properties", {})
        self.assertIn("VkPhysicalDeviceProperties", v11_properties)
        self.assertIn("VkPhysicalDeviceMaintenance3Properties", v11_properties)
        self.assertIn("VkPhysicalDeviceMultiviewProperties", v11_properties)
        self.assertIn("VkPhysicalDeviceSubgroupProperties", v11_properties)

        # 4. Verify transition block VP_TEST_profile_v11_to_vulkan12
        trans_block = capabilities_dict.get("VP_TEST_profile_v11_to_vulkan12", {})
        self.assertIn("VkPhysicalDeviceVulkan12Features", trans_block.get("features", {}))
        self.assertTrue(trans_block["features"]["VkPhysicalDeviceVulkan12Features"].get("imagelessFramebuffer"))
        self.assertTrue(trans_block["features"]["VkPhysicalDeviceVulkan12Features"].get("timelineSemaphore"))

        self.assertIn("VkPhysicalDeviceVulkan12Properties", trans_block.get("properties", {}))
        self.assertEqual(
            trans_block["properties"]["VkPhysicalDeviceVulkan12Properties"].get("maxTimelineSemaphoreValueDifference"),
            2147483647
        )

    def test_pull_required_capabilities_profile_inheritance_vulkan10_to_vulkan12(self):
        """
        Verifies core capability pulling across a 3-level profile chain (Vulkan 1.0 -> 1.1 -> 1.2).
        - VP_TEST_profile_v10 (V1.0) pulls Vulkan 1.0 core capabilities into baseline_v10.
        - VP_TEST_profile_v11 (V1.1) inherits VP_TEST_profile_v10 and generates transition block 'VP_TEST_profile_v10_to_vulkan11'.
        - VP_TEST_profile_v12 (V1.2) inherits VP_TEST_profile_v11 and generates transition block 'VP_TEST_profile_v11_to_vulkan12'.
        """
        original_json_text = """{
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8.0-131.json#",
            "profiles": {
                "VP_TEST_profile_v10": {
                    "version": 1,
                    "api-version": "1.0.68",
                    "capabilities": ["baseline_v10"]
                },
                "VP_TEST_profile_v11": {
                    "version": 1,
                    "api-version": "1.1.108",
                    "profiles": ["VP_TEST_profile_v10"],
                    "capabilities": ["baseline_v11"]
                },
                "VP_TEST_profile_v12": {
                    "version": 1,
                    "api-version": "1.2.131",
                    "profiles": ["VP_TEST_profile_v11"],
                    "capabilities": ["baseline_v12"]
                }
            },
            "capabilities": {
                "baseline_v10": {},
                "baseline_v11": {},
                "baseline_v12": {}
            }
        }"""

        json_files_dict = {"test_profile.json": json.loads(original_json_text)}
        pull_required_capabilities_profiles_files(self.vk, [PullBits.REQUIRED_CAPABILITIES], json_files_dict)
        sort_profiles_files(self.vk, json_files_dict)

        result_data = json_files_dict["test_profile.json"]
        profiles_dict = result_data["profiles"]
        capabilities_dict = result_data["capabilities"]

        # 1. Verify profile capabilities references
        self.assertEqual(profiles_dict["VP_TEST_profile_v10"].get("capabilities"), ["baseline_v10"])
        self.assertEqual(
            profiles_dict["VP_TEST_profile_v11"].get("capabilities"),
            ["VP_TEST_profile_v10_to_vulkan11", "baseline_v11"]
        )
        self.assertEqual(
            profiles_dict["VP_TEST_profile_v12"].get("capabilities"),
            ["VP_TEST_profile_v11_to_vulkan12", "baseline_v12"]
        )

        # 2. Verify baseline_v10 contains Vulkan 1.0 core features & properties
        self.assertTrue(capabilities_dict["baseline_v10"]["features"]["VkPhysicalDeviceFeatures"].get("robustBufferAccess"))
        self.assertIn("VkPhysicalDeviceProperties", capabilities_dict["baseline_v10"].get("properties", {}))

        # 3. Verify transition block VP_TEST_profile_v10_to_vulkan11
        trans_v11 = capabilities_dict.get("VP_TEST_profile_v10_to_vulkan11", {})
        self.assertTrue(trans_v11["features"]["VkPhysicalDeviceMultiviewFeatures"].get("multiview"))
        self.assertIn("VkPhysicalDeviceMaintenance3Properties", trans_v11.get("properties", {}))
        self.assertIn("VkPhysicalDeviceMultiviewProperties", trans_v11.get("properties", {}))
        self.assertIn("VkPhysicalDeviceSubgroupProperties", trans_v11.get("properties", {}))

        # 4. Verify transition block VP_TEST_profile_v11_to_vulkan12
        trans_v12 = capabilities_dict.get("VP_TEST_profile_v11_to_vulkan12", {})
        self.assertTrue(trans_v12["features"]["VkPhysicalDeviceVulkan12Features"].get("imagelessFramebuffer"))
        self.assertIn("VkPhysicalDeviceVulkan12Properties", trans_v12.get("properties", {}))


    def test_pull_required_capabilities_inheritance_populates_profile_block_defaults(self):
        """
        Verifies that a profile with a required profile gets the defaults of the structures defined
        by its own capability blocks, as when the same structures are pulled into a transition block.
        """
        original_json_text = """{
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8.0-108.json#",
            "profiles": {
                "VP_TEST_base_profile": {
                    "version": 1,
                    "api-version": "1.0.68",
                    "capabilities": ["baseline"]
                },
                "VP_TEST_derived_profile": {
                    "version": 1,
                    "api-version": "1.1.108",
                    "profiles": ["VP_TEST_base_profile"],
                    "capabilities": ["derived"]
                }
            },
            "capabilities": {
                "baseline": {},
                "derived": {
                    "features": {
                        "VkPhysicalDeviceMultiviewFeatures": {
                            "multiview": true
                        }
                    }
                }
            }
        }"""

        json_files_dict = {"test_profile.json": json.loads(original_json_text)}
        pull_required_capabilities_profiles_files(self.vk, [PullBits.REQUIRED_CAPABILITIES], json_files_dict)

        derived_features = json_files_dict["test_profile.json"]["capabilities"]["derived"]["features"]
        multiview_features = derived_features["VkPhysicalDeviceMultiviewFeatures"]

        # 1. The profile defined value MUST be kept
        self.assertTrue(multiview_features["multiview"])

        # 2. The unlisted members of the profile defined structure MUST get their default value
        self.assertIn("multiviewGeometryShader", multiview_features)
        self.assertFalse(multiview_features["multiviewGeometryShader"])
        self.assertIn("multiviewTessellationShader", multiview_features)
        self.assertFalse(multiview_features["multiviewTessellationShader"])

    def test_pull_required_capabilities_inheritance_skips_defaults_defined_by_aliases(self):
        """
        Verifies that pulling Vulkan 1.4 requirements into a VkPhysicalDeviceVulkan11Features structure
        does not populate the multiview default value when the required profile already defines it
        through VkPhysicalDeviceMultiviewFeatures::multiview.
        """
        original_json_text = """{
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8.0-304.json#",
            "profiles": {
                "VP_TEST_base_profile": {
                    "version": 1,
                    "api-version": "1.1.108",
                    "capabilities": ["baseline"]
                },
                "VP_TEST_derived_profile": {
                    "version": 1,
                    "api-version": "1.4.304",
                    "profiles": ["VP_TEST_base_profile"],
                    "capabilities": ["derived"]
                }
            },
            "capabilities": {
                "baseline": {
                    "features": {
                        "VkPhysicalDeviceMultiviewFeatures": {
                            "multiview": true
                        }
                    }
                },
                "derived": {}
            }
        }"""

        json_files_dict = {"test_profile.json": json.loads(original_json_text)}
        pull_required_capabilities_profiles_files(self.vk, [PullBits.REQUIRED_CAPABILITIES], json_files_dict)

        capabilities = json_files_dict["test_profile.json"]["capabilities"]
        transition_features = capabilities["VP_TEST_base_profile_to_vulkan14"]["features"]
        vulkan11_features = transition_features["VkPhysicalDeviceVulkan11Features"]

        # 1. Vulkan 1.4 required VkPhysicalDeviceVulkan11Features members MUST be pulled
        self.assertTrue(vulkan11_features["storageBuffer16BitAccess"])
        self.assertTrue(vulkan11_features["variablePointers"])

        # 2. Unlisted members without alias in the required profile MUST get their default value
        self.assertIn("protectedMemory", vulkan11_features)
        self.assertFalse(vulkan11_features["protectedMemory"])

        # 3. Members defined by the required profile through an alias MUST NOT get a default value
        self.assertNotIn("multiview", vulkan11_features)
        self.assertTrue(capabilities["baseline"]["features"]["VkPhysicalDeviceMultiviewFeatures"]["multiview"])

    def test_pull_required_capabilities_inheritance_required_profile_does_not_override_requirements(self):
        """
        Verifies that the capabilities of a required profile targeting a lower Vulkan version don't
        prevent pulling the requirements of the profile Vulkan version: a Vulkan 1.2 required profile
        with bufferDeviceAddress = false and maxImageDimension1D = 4096 doesn't override the Vulkan 1.3
        bufferDeviceAddress and the Vulkan 1.4 maxImageDimension1D requirements, while requirements
        already satisfied by the required profile are not duplicated.
        """
        original_json_text = """{
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8.0-304.json#",
            "profiles": {
                "VP_TEST_base_profile": {
                    "version": 1,
                    "api-version": "1.2.131",
                    "capabilities": ["baseline"]
                },
                "VP_TEST_derived_profile": {
                    "version": 1,
                    "api-version": "1.4.304",
                    "profiles": ["VP_TEST_base_profile"],
                    "capabilities": ["derived"]
                }
            },
            "capabilities": {
                "baseline": {
                    "features": {
                        "VkPhysicalDeviceVulkan12Features": {
                            "bufferDeviceAddress": false,
                            "timelineSemaphore": true
                        }
                    },
                    "properties": {
                        "VkPhysicalDeviceProperties": {
                            "limits": {
                                "maxImageDimension1D": 4096,
                                "maxImageDimension2D": 16384
                            }
                        }
                    }
                },
                "derived": {}
            }
        }"""

        json_files_dict = {"test_profile.json": json.loads(original_json_text)}
        pull_required_capabilities_profiles_files(self.vk, [PullBits.REQUIRED_CAPABILITIES], json_files_dict)

        capabilities = json_files_dict["test_profile.json"]["capabilities"]
        transition_block = capabilities["VP_TEST_base_profile_to_vulkan14"]
        vulkan12_features = transition_block["features"]["VkPhysicalDeviceVulkan12Features"]
        limits = transition_block["properties"]["VkPhysicalDeviceProperties"]["limits"]

        # 1. Requirements not satisfied by the required profile MUST be pulled
        self.assertTrue(vulkan12_features["bufferDeviceAddress"])
        self.assertEqual(limits["maxImageDimension1D"], 8192)

        # 2. Requirements already satisfied by the required profile MUST NOT be duplicated
        self.assertNotIn("timelineSemaphore", vulkan12_features)
        self.assertNotIn("maxImageDimension2D", limits)

        # 3. The required profile capabilities MUST be kept
        self.assertFalse(capabilities["baseline"]["features"]["VkPhysicalDeviceVulkan12Features"]["bufferDeviceAddress"])

    def test_pull_required_capabilities_inheritance_profile_values_override_requirements(self):
        """
        Verifies that, without PullBits.IGNORE_SUBCORE, the capabilities defined by the profile itself
        still take precedence over the requirements of its Vulkan version.
        """
        original_json_text = """{
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8.0-204.json#",
            "profiles": {
                "VP_TEST_base_profile": {
                    "version": 1,
                    "api-version": "1.2.131",
                    "capabilities": ["baseline"]
                },
                "VP_TEST_derived_profile": {
                    "version": 1,
                    "api-version": "1.3.204",
                    "profiles": ["VP_TEST_base_profile"],
                    "capabilities": ["derived"]
                }
            },
            "capabilities": {
                "baseline": {},
                "derived": {
                    "features": {
                        "VkPhysicalDeviceVulkan12Features": {
                            "bufferDeviceAddress": false
                        }
                    }
                }
            }
        }"""

        json_files_dict = {"test_profile.json": json.loads(original_json_text)}
        pull_required_capabilities_profiles_files(self.vk, [PullBits.REQUIRED_CAPABILITIES], json_files_dict)

        capabilities = json_files_dict["test_profile.json"]["capabilities"]
        transition_features = capabilities["VP_TEST_base_profile_to_vulkan13"]["features"]

        # 1. The profile defined value MUST NOT be overridden by the pulled requirement
        self.assertFalse(capabilities["derived"]["features"]["VkPhysicalDeviceVulkan12Features"]["bufferDeviceAddress"])
        self.assertNotIn("bufferDeviceAddress", transition_features.get("VkPhysicalDeviceVulkan12Features", {}))

        # 2. Other requirements MUST be pulled
        self.assertTrue(transition_features["VkPhysicalDeviceVulkan12Features"]["vulkanMemoryModel"])

    def test_pull_required_capabilities_inheritance_unsupported_defaults_not_pulled(self):
        """
        Verifies that a Vulkan version requirement equal to the unsupported default value (e.g.
        VkPhysicalDeviceSubgroupProperties::quadOperationsInAllStages = false, a 'max' limit type) is
        not pulled into the transition block when promoting a profile to a higher Vulkan version, but
        populated as a default value in the structure defined by the profile.
        """
        original_json_text = """{
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8.0-108.json#",
            "profiles": {
                "VP_TEST_base_profile": {
                    "version": 1,
                    "api-version": "1.0.68",
                    "capabilities": ["baseline"]
                },
                "VP_TEST_derived_profile": {
                    "version": 1,
                    "api-version": "1.1.108",
                    "profiles": ["VP_TEST_base_profile"],
                    "capabilities": ["derived"]
                }
            },
            "capabilities": {
                "baseline": {},
                "derived": {
                    "features": {
                        "VkPhysicalDeviceMultiviewFeatures": {
                            "multiview": true
                        }
                    },
                    "properties": {
                        "VkPhysicalDeviceSubgroupProperties": {
                            "subgroupSize": 1,
                            "supportedStages": ["VK_SHADER_STAGE_COMPUTE_BIT"],
                            "supportedOperations": ["VK_SUBGROUP_FEATURE_BASIC_BIT"]
                        },
                        "VkPhysicalDeviceMultiviewProperties": {
                            "maxMultiviewViewCount": 6,
                            "maxMultiviewInstanceIndex": 134217727
                        },
                        "VkPhysicalDeviceMaintenance3Properties": {
                            "maxPerSetDescriptors": 1024,
                            "maxMemoryAllocationSize": 1073741824
                        }
                    }
                }
            }
        }"""

        json_files_dict = {"test_profile.json": json.loads(original_json_text)}
        pull_required_capabilities_profiles_files(self.vk, [PullBits.REQUIRED_CAPABILITIES], json_files_dict)

        capabilities = json_files_dict["test_profile.json"]["capabilities"]
        subgroup_props = capabilities["derived"]["properties"]["VkPhysicalDeviceSubgroupProperties"]

        # 1. The unsupported default value MUST be populated in the structure defined by the profile
        self.assertIn("quadOperationsInAllStages", subgroup_props)
        self.assertFalse(subgroup_props["quadOperationsInAllStages"])

        # 2. The unsupported default value MUST NOT be pulled as a Vulkan 1.1 requirement
        transition_block = capabilities.get("VP_TEST_base_profile_to_vulkan11", {})
        self.assertNotIn("VkPhysicalDeviceSubgroupProperties", transition_block.get("properties", {}))

    def test_pull_required_capabilities_inheritance_earlier_versions_feature_dependent_requirements(self):
        """
        Verifies that promoting a profile to a higher Vulkan version pulls the requirements of the
        earlier Vulkan versions which depend on features required by the higher Vulkan version: Vulkan
        1.4 requires samplerAnisotropy and fullDrawIndexUint32, so the Vulkan 1.0 maxSamplerAnisotropy = 16
        and maxDrawIndexedIndexValue = 4294967295 requirements replace the Vulkan 1.0 required profile
        values of the unsupported features.
        """
        original_json_text = """{
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8.0-304.json#",
            "profiles": {
                "VP_TEST_base_profile": {
                    "version": 1,
                    "api-version": "1.0.68",
                    "capabilities": ["baseline"]
                },
                "VP_TEST_derived_profile": {
                    "version": 1,
                    "api-version": "1.4.304",
                    "profiles": ["VP_TEST_base_profile"],
                    "capabilities": ["derived"]
                }
            },
            "capabilities": {
                "baseline": {},
                "derived": {}
            }
        }"""

        json_files_dict = {"test_profile.json": json.loads(original_json_text)}
        pull_required_capabilities_profiles_files(self.vk, [PullBits.REQUIRED_CAPABILITIES], json_files_dict)

        capabilities = json_files_dict["test_profile.json"]["capabilities"]
        base_limits = capabilities["baseline"]["properties"]["VkPhysicalDeviceProperties"]["limits"]
        transition_block = capabilities["VP_TEST_base_profile_to_vulkan14"]
        transition_features = transition_block["features"]["VkPhysicalDeviceFeatures"]
        transition_limits = transition_block["properties"]["VkPhysicalDeviceProperties"]["limits"]

        # 1. The Vulkan 1.0 required profile MUST keep the values of the unsupported features
        self.assertEqual(base_limits["maxSamplerAnisotropy"], 1.0)
        self.assertEqual(base_limits["maxDrawIndexedIndexValue"], 16777215)

        # 2. The Vulkan 1.4 required features MUST be pulled
        self.assertTrue(transition_features["samplerAnisotropy"])
        self.assertTrue(transition_features["fullDrawIndexUint32"])

        # 3. The Vulkan 1.0 requirements depending on these features MUST be pulled
        self.assertEqual(transition_limits["maxSamplerAnisotropy"], 16.0)
        self.assertEqual(transition_limits["maxDrawIndexedIndexValue"], 4294967295)

        # 4. The Vulkan 1.0 requirements already satisfied by the required profile MUST NOT be duplicated
        self.assertNotIn("maxStorageBufferRange", transition_limits)

if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument(
        '--registry', '-r', action='store', required=False,
        help='Use specified registry file instead of vk.xml.'
    )

    args, unparsed = parser.parse_known_args()
    TestConvertPullRequiredCapabilitiesCoreInheriance.registry_path = args.registry

    unittest.main(argv=[sys.argv[0]] + unparsed)