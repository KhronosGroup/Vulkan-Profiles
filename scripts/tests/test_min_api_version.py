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
from source.main_min_api_version import evaluate_min_api_version, MinApiVersionMode


class TestMinApiVersion(unittest.TestCase):
    registry_path = None
    schemas_dir = Path(__file__).resolve().parent / "schemas"

    def setUp(self):
        self.vk: VulkanObject = initVulkanObject('vulkan', self.registry_path)

    def test_show_min_api_version(self):
        """
        Verifies that reading the minimum API version in 'display' mode returns 
        the api-version field stated directly in the profile JSON.
        """
        original_json_text = """{
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8.2-latest.json#",
            "profiles": {
                "VP_TEST_min_api_show": {
                    "label": "Test Show Profile",
                    "description": "Profile to test display mode",
                    "version": 1,
                    "api-version": "1.2.148",
                    "capabilities": ["baseline"]
                }
            },
            "capabilities": {
                "baseline": {
                    "features": {
                        "VkPhysicalDeviceFeatures": {
                            "robustBufferAccess": true
                        }
                    }
                }
            }
        }"""

        json_data = json.loads(original_json_text)
        evaluated_versions = evaluate_min_api_version(
            self.vk,
            json_data,
            profile_names=["VP_TEST_min_api_show"],
            mode=MinApiVersionMode.SHOW,
            schemas_dir=self.schemas_dir
        )

        self.assertEqual(evaluated_versions.get("VP_TEST_min_api_show"), "1.2.148")

    def test_evaluate_min_api_version_v10_baseline(self):
        """
        Verifies that a basic Vulkan 1.0 profile containing only VkPhysicalDeviceFeatures
        evaluates to '1.0.106'.
        """
        original_json_text = """{
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8.2-latest.json#",
            "profiles": {
                "VP_TEST_v10_baseline": {
                    "label": "Test 1.0 Baseline Profile",
                    "description": "Profile to test Vulkan 1.0 baseline evaluation",
                    "version": 1,
                    "api-version": "1.0.0",
                    "capabilities": ["v10_block"]
                }
            },
            "capabilities": {
                "v10_block": {
                    "features": {
                        "VkPhysicalDeviceFeatures": {
                            "robustBufferAccess": true
                        }
                    }
                }
            }
        }"""

        json_data = json.loads(original_json_text)
        evaluated_versions = evaluate_min_api_version(
            self.vk,
            json_data,
            profile_names=["VP_TEST_v10_baseline"],
            mode=MinApiVersionMode.EVALUATE,
            schemas_dir=self.schemas_dir
        )

        self.assertEqual(evaluated_versions.get("VP_TEST_v10_baseline"), "1.0.106")

    def test_evaluate_min_api_version_properties_block(self):
        """
        Verifies that structure names declared inside the 'properties' block
        (VkPhysicalDeviceVulkan12Properties) correctly drive Vulkan core version detection.
        """
        original_json_text = """{
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8.2-latest.json#",
            "profiles": {
                "VP_TEST_properties_eval": {
                    "label": "Test Properties Eval Profile",
                    "description": "Profile to test structure detection in properties block",
                    "version": 1,
                    "api-version": "1.0.0",
                    "capabilities": ["properties_block"]
                }
            },
            "capabilities": {
                "properties_block": {
                    "properties": {
                        "VkPhysicalDeviceVulkan12Properties": {
                            "maxTimelineSemaphoreValueDifference": 2147483647
                        }
                    }
                }
            }
        }"""

        json_data = json.loads(original_json_text)
        evaluated_versions = evaluate_min_api_version(
            self.vk,
            json_data,
            profile_names=["VP_TEST_properties_eval"],
            mode=MinApiVersionMode.EVALUATE,
            schemas_dir=self.schemas_dir
        )

        self.assertEqual(evaluated_versions.get("VP_TEST_properties_eval"), "1.2.251")

    def test_evaluate_min_api_version(self):
        """
        Verifies that evaluating required capability structures against Vulkan header
        definitions correctly calculates the minimum required Vulkan API version.
        """
        original_json_text = """{
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8.2-latest.json#",
            "profiles": {
                "VP_TEST_min_api_eval": {
                    "label": "Test Eval Profile",
                    "description": "Profile to test evaluation mode",
                    "version": 1,
                    "api-version": "1.0.0",
                    "capabilities": ["vulkan12_block"]
                }
            },
            "capabilities": {
                "vulkan12_block": {
                    "features": {
                        "VkPhysicalDeviceVulkan12Features": {
                            "timelineSemaphore": true
                        }
                    }
                }
            }
        }"""

        json_data = json.loads(original_json_text)
        evaluated_versions = evaluate_min_api_version(
            self.vk,
            json_data,
            profile_names=["VP_TEST_min_api_eval"],
            mode=MinApiVersionMode.EVALUATE,
            schemas_dir=self.schemas_dir
        )

        self.assertEqual(evaluated_versions.get("VP_TEST_min_api_eval"), "1.2.251")

    def test_evaluate_min_api_version_v11_header251(self):
        """
        Verifies that evaluating a profile requiring Vulkan 1.1 core features 
        alongside an extension introduced in schema 251 (VK_KHR_dynamic_rendering)
        evaluates to '1.1.251'.
        """
        original_json_text = """{
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8.2-latest.json#",
            "profiles": {
                "VP_TEST_min_api_eval_11_251": {
                    "label": "Test Eval 1.1.251 Profile",
                    "description": "Profile to test Vulkan 1.1 core with header 251 extension",
                    "version": 1,
                    "api-version": "1.0.0",
                    "capabilities": ["v11_ext251_block"]
                }
            },
            "capabilities": {
                "v11_ext251_block": {
                    "extensions": {
                        "VK_KHR_dynamic_rendering": 1
                    },
                    "features": {
                        "VkPhysicalDevice16BitStorageFeatures": {
                            "storageBuffer16BitAccess": true
                        },
                        "VkPhysicalDeviceDynamicRenderingFeaturesKHR": {
                            "dynamicRendering": true
                        }
                    }
                }
            }
        }"""

        json_data = json.loads(original_json_text)
        evaluated_versions = evaluate_min_api_version(
            self.vk,
            json_data,
            profile_names=["VP_TEST_min_api_eval_11_251"],
            mode=MinApiVersionMode.EVALUATE,
            schemas_dir=self.schemas_dir
        )

        self.assertEqual(evaluated_versions.get("VP_TEST_min_api_eval_11_251"), "1.1.251")

    def test_evaluate_min_api_version_three_level_inheritance(self):
        """
        Verifies minimum API version evaluation across a 3-level profile hierarchy
        (Grandparent -> Parent -> Child):
        - Grandparent defines Vulkan 1.0 features (robustBufferAccess).
        - Parent inherits Grandparent and adds Vulkan 1.1 features (VkPhysicalDevice16BitStorageFeatures).
        - Child inherits Parent and adds Vulkan 1.2 features (VkPhysicalDeviceVulkan12Features).
        """
        original_json_text = """{
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8.2-latest.json#",
            "profiles": {
                "VP_TEST_grandparent": {
                    "label": "Grandparent Profile",
                    "description": "Grandparent profile description",
                    "version": 1,
                    "api-version": "1.0.0",
                    "capabilities": ["grandparent_block"]
                },
                "VP_TEST_parent": {
                    "label": "Parent Profile",
                    "description": "Parent profile description",
                    "version": 1,
                    "api-version": "1.0.0",
                    "profiles": ["VP_TEST_grandparent"],
                    "capabilities": ["parent_block"]
                },
                "VP_TEST_child": {
                    "label": "Child Profile",
                    "description": "Child profile description",
                    "version": 1,
                    "api-version": "1.0.0",
                    "profiles": ["VP_TEST_parent"],
                    "capabilities": ["child_block"]
                }
            },
            "capabilities": {
                "grandparent_block": {
                    "features": {
                        "VkPhysicalDeviceFeatures": {
                            "robustBufferAccess": true
                        }
                    }
                },
                "parent_block": {
                    "features": {
                        "VkPhysicalDevice16BitStorageFeatures": {
                            "storageBuffer16BitAccess": true
                        }
                    }
                },
                "child_block": {
                    "features": {
                        "VkPhysicalDeviceVulkan12Features": {
                            "timelineSemaphore": true
                        }
                    }
                }
            }
        }"""

        json_data = json.loads(original_json_text)
        evaluated_versions = evaluate_min_api_version(
            self.vk,
            json_data,
            profile_names=["VP_TEST_grandparent", "VP_TEST_parent", "VP_TEST_child"],
            mode=MinApiVersionMode.EVALUATE,
            schemas_dir=self.schemas_dir
        )

        self.assertEqual(evaluated_versions.get("VP_TEST_grandparent"), "1.0.106")
        self.assertEqual(evaluated_versions.get("VP_TEST_parent"), "1.1.106")
        self.assertEqual(evaluated_versions.get("VP_TEST_child"), "1.2.251")

    def test_evaluate_min_api_version_v14_features(self):
        """
        Verifies that a profile specifying VkPhysicalDeviceVulkan14Features evaluates
        to Vulkan 1.4 core and matches schema header 340 or 361.
        """
        original_json_text = """{
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8.2-latest.json#",
            "profiles": {
                "VP_TEST_v14_eval": {
                    "label": "Test 1.4 Profile",
                    "description": "Profile to test Vulkan 1.4 core features",
                    "version": 1,
                    "api-version": "1.0.0",
                    "capabilities": ["v14_block"]
                }
            },
            "capabilities": {
                "v14_block": {
                    "features": {
                        "VkPhysicalDeviceVulkan14Features": {
                            "pushDescriptor": true
                        }
                    }
                }
            }
        }"""

        json_data = json.loads(original_json_text)
        evaluated_versions = evaluate_min_api_version(
            self.vk,
            json_data,
            profile_names=["VP_TEST_v14_eval"],
            mode=MinApiVersionMode.EVALUATE,
            schemas_dir=self.schemas_dir
        )

        self.assertEqual(evaluated_versions.get("VP_TEST_v14_eval"), "1.4.340")

    def test_evaluate_min_api_version_extension_core_dependency(self):
        """
        Verifies that an extension strictly depending on Vulkan 1.1 core in vk.xml 
        (VK_KHR_shader_subgroup_extended_types) elevates the detected core version to 1.1.
        """
        original_json_text = """{
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8.2-latest.json#",
            "profiles": {
                "VP_TEST_ext_dep_eval": {
                    "label": "Test Extension Depends Profile",
                    "description": "Profile to test extension with vk.xml core dependency",
                    "version": 1,
                    "api-version": "1.0.0",
                    "capabilities": ["ext_dep_block"]
                }
            },
            "capabilities": {
                "ext_dep_block": {
                    "extensions": {
                        "VK_KHR_shader_subgroup_extended_types": 1
                    },
                    "features": {
                        "VkPhysicalDeviceShaderSubgroupExtendedTypesFeaturesKHR": {
                            "shaderSubgroupExtendedTypes": true
                        }
                    }
                }
            }
        }"""

        json_data = json.loads(original_json_text)
        evaluated_versions = evaluate_min_api_version(
            self.vk,
            json_data,
            profile_names=["VP_TEST_ext_dep_eval"],
            mode=MinApiVersionMode.EVALUATE,
            schemas_dir=self.schemas_dir
        )

        self.assertEqual(evaluated_versions.get("VP_TEST_ext_dep_eval"), "1.1.251")

    def test_evaluate_min_api_version_extension_or_dependency(self):
        """
        Verifies that an extension depending on (VK_KHR_get_physical_device_properties2 OR VK_VERSION_1_1)
        evaluates to Vulkan 1.0 when VK_KHR_get_physical_device_properties2 is explicitly enabled in the profile.
        """
        original_json_text = """{
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8.2-latest.json#",
            "profiles": {
                "VP_TEST_ext_or_dep_eval": {
                    "label": "Test Extension OR Dependency Profile",
                    "description": "Profile testing extension dependency satisfied via extension rather than core 1.1",
                    "version": 1,
                    "api-version": "1.0.0",
                    "capabilities": ["ext_or_dep_block"]
                }
            },
            "capabilities": {
                "ext_or_dep_block": {
                    "extensions": {
                        "VK_KHR_get_physical_device_properties2": 1,
                        "VK_KHR_shader_float_controls": 1
                    },
                    "properties": {
                        "VkPhysicalDeviceFloatControlsPropertiesKHR": {
                            "shaderSignedZeroInfNanPreserveFloat32": true
                        }
                    }
                }
            }
        }"""

        json_data = json.loads(original_json_text)
        evaluated_versions = evaluate_min_api_version(
            self.vk,
            json_data,
            profile_names=["VP_TEST_ext_or_dep_eval"],
            mode=MinApiVersionMode.EVALUATE,
            schemas_dir=self.schemas_dir
        )

        self.assertEqual(evaluated_versions.get("VP_TEST_ext_or_dep_eval"), "1.0.106")

    def test_evaluate_min_api_version_extract_pull_integration(self):
        """
        Verifies that evaluate_min_api_version internally invokes extract_profile in PULL mode,
        evaluating both parent and child profiles correctly across inherited capabilities:
        - VP_TEST_extract_parent uses Vulkan 1.2 features ('1.2.251').
        - VP_TEST_extract_child inherits VP_TEST_extract_parent and adds Vulkan 1.3 features ('1.3.251').
        """
        original_json_text = """{
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8.2-latest.json#",
            "profiles": {
                "VP_TEST_extract_parent": {
                    "label": "Parent Profile with 1.2 Features",
                    "description": "Parent profile containing Vulkan 1.2 features",
                    "version": 1,
                    "api-version": "1.0.0",
                    "capabilities": ["parent_v12_block"]
                },
                "VP_TEST_extract_child": {
                    "label": "Child Profile Inheriting Parent and Adding 1.3 Features",
                    "description": "Child profile inheriting from parent and adding Vulkan 1.3 features",
                    "version": 1,
                    "api-version": "1.0.0",
                    "profiles": ["VP_TEST_extract_parent"],
                    "capabilities": ["child_v13_block"]
                }
            },
            "capabilities": {
                "parent_v12_block": {
                    "features": {
                        "VkPhysicalDeviceVulkan12Features": {
                            "timelineSemaphore": true
                        }
                    }
                },
                "child_v13_block": {
                    "features": {
                        "VkPhysicalDeviceVulkan13Features": {
                            "dynamicRendering": true
                        }
                    }
                }
            }
        }"""

        json_data = json.loads(original_json_text)
        evaluated_versions = evaluate_min_api_version(
            self.vk,
            json_data,
            profile_names=["VP_TEST_extract_parent", "VP_TEST_extract_child"],
            mode=MinApiVersionMode.EVALUATE,
            schemas_dir=self.schemas_dir
        )

        self.assertEqual(evaluated_versions.get("VP_TEST_extract_parent"), "1.2.251")
        self.assertEqual(evaluated_versions.get("VP_TEST_extract_child"), "1.3.251")

    def test_evaluate_min_api_version_extract_pull_parent_v13_child_v12(self):
        """
        Verifies that when a child profile uses Vulkan 1.2 features but inherits from a parent profile
        that uses Vulkan 1.3 features, extracting in PULL mode results in Vulkan 1.3 minimum API version
        for the child as well:
        - VP_TEST_extract_parent_v13 uses Vulkan 1.3 features ('1.3.251').
        - VP_TEST_extract_child_v12 inherits VP_TEST_extract_parent_v13 and adds Vulkan 1.2 features,
          evaluating to ('1.3.251').
        """
        original_json_text = """{
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8.2-latest.json#",
            "profiles": {
                "VP_TEST_extract_parent_v13": {
                    "label": "Parent Profile with 1.3 Features",
                    "description": "Parent profile containing Vulkan 1.3 features",
                    "version": 1,
                    "api-version": "1.0.0",
                    "capabilities": ["parent_v13_block"]
                },
                "VP_TEST_extract_child_v12": {
                    "label": "Child Profile Inheriting Parent and Adding 1.2 Features",
                    "description": "Child profile inheriting from parent with 1.3 features and adding 1.2 features",
                    "version": 1,
                    "api-version": "1.0.0",
                    "profiles": ["VP_TEST_extract_parent_v13"],
                    "capabilities": ["child_v12_block"]
                }
            },
            "capabilities": {
                "parent_v13_block": {
                    "features": {
                        "VkPhysicalDeviceVulkan13Features": {
                            "dynamicRendering": true
                        }
                    }
                },
                "child_v12_block": {
                    "features": {
                        "VkPhysicalDeviceVulkan12Features": {
                            "timelineSemaphore": true
                        }
                    }
                }
            }
        }"""

        json_data = json.loads(original_json_text)
        evaluated_versions = evaluate_min_api_version(
            self.vk,
            json_data,
            profile_names=["VP_TEST_extract_parent_v13", "VP_TEST_extract_child_v12"],
            mode=MinApiVersionMode.EVALUATE,
            schemas_dir=self.schemas_dir
        )

        self.assertEqual(evaluated_versions.get("VP_TEST_extract_parent_v13"), "1.3.251")
        self.assertEqual(evaluated_versions.get("VP_TEST_extract_child_v12"), "1.3.251")


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument(
        '--registry', '-r', action='store', required=False,
        help='Use specified registry file instead of vk.xml.'
    )

    args, unparsed = parser.parse_known_args()
    TestMinApiVersion.registry_path = args.registry

    unittest.main(argv=[sys.argv[0]] + unparsed)