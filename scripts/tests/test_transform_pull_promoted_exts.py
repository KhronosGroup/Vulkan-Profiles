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
from source.transform_pull_promoted_extensions import pull_promoted_extensions_profiles_files
from source.transform_sort import sort_profiles_files


class TestConvertPullPromotedExtensions(unittest.TestCase):
    registry_path = None

    def setUp(self):
        self.vk: VulkanObject = initVulkanObject('vulkan', self.registry_path)

    def assertProfileDataEqual(self, gen_data: dict, exp_data: dict):
        self.assertEqual(
            list(gen_data.keys()),
            list(exp_data.keys()),
            f"Top-level keys mismatch: {list(gen_data.keys())} vs {list(exp_data.keys())}"
        )

        if "profiles" in exp_data:
            self.assertEqual(
                list(gen_data.get("profiles", {}).keys()),
                list(exp_data["profiles"].keys()),
                "Profile name list mismatch"
            )
            for prof_name, exp_prof in exp_data["profiles"].items():
                gen_prof = gen_data["profiles"].get(prof_name, {})
                self.assertEqual(
                    gen_prof.get("capabilities"),
                    exp_prof.get("capabilities"),
                    f"Capabilities block reference list mismatch in profile '{prof_name}'"
                )

        if "capabilities" in exp_data:
            self.assertEqual(
                list(gen_data.get("capabilities", {}).keys()),
                list(exp_data["capabilities"].keys()),
                "Capability block name list mismatch"
            )

            for cap_name, exp_block in exp_data["capabilities"].items():
                gen_block = gen_data["capabilities"].get(cap_name, {})

                self.assertEqual(
                    list(gen_block.keys()),
                    list(exp_block.keys()),
                    f"Section mismatch in capability block '{cap_name}'"
                )

                for section in ("extensions", "features", "properties"):
                    if section in exp_block:
                        exp_section = exp_block[section]
                        gen_section = gen_block.get(section, {})

                        if section == "extensions":
                            self.assertEqual(
                                gen_section,
                                exp_section,
                                f"Extension mismatch in capability block '{cap_name}'"
                            )
                        else:
                            self.assertEqual(
                                list(gen_section.keys()),
                                list(exp_section.keys()),
                                f"Structure name list mismatch in capability block '{cap_name}', section '{section}'"
                            )

                            for struct_name, exp_struct in exp_section.items():
                                gen_struct = gen_section.get(struct_name, {})
                                if isinstance(exp_struct, dict):
                                    self.assertEqual(
                                        list(gen_struct.keys()),
                                        list(exp_struct.keys()),
                                        f"Member key list mismatch in '{cap_name}.{section}.{struct_name}'"
                                    )
                                    for member_name, exp_val in exp_struct.items():
                                        gen_val = gen_struct.get(member_name)
                                        self.assertEqual(
                                            gen_val,
                                            exp_val,
                                            f"Value mismatch in '{cap_name}.{section}.{struct_name}.{member_name}'"
                                        )
                                else:
                                    self.assertEqual(
                                        gen_struct,
                                        exp_struct,
                                        f"Value mismatch in '{cap_name}.{section}.{struct_name}'"
                                    )

        self.assertEqual(gen_data, exp_data)

    # -------------------------------------------------------------------------
    # Vulkan 1.0 Tests (VK_KHR_variable_pointers, VK_KHR_multiview)
    # -------------------------------------------------------------------------

    def test_pull_promoted_extensions_block_unique_vulkan10(self):
        """
        Verifies that for Vulkan 1.0 profiles, pull_promoted_extensions_profiles_files pulls no core
        promoted extensions (since core promotions start at Vulkan 1.1+), but still resolves dependencies
        and required features/properties for extensions explicitly enabled in the primary capability block.
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
                        "VK_KHR_variable_pointers": 1,
                        "VK_KHR_multiview": 1
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
                        "VK_KHR_get_physical_device_properties2": 1,
                        "VK_KHR_multiview": 1,
                        "VK_KHR_storage_buffer_storage_class": 1,
                        "VK_KHR_variable_pointers": 1
                    },
                    "features": {
                        "VkPhysicalDeviceMultiviewFeaturesKHR": {
                            "multiview": true
                        },
                        "VkPhysicalDeviceVariablePointerFeaturesKHR": {
                            "variablePointersStorageBuffer": true
                        }
                    },
                    "properties": {
                        "VkPhysicalDeviceMultiviewPropertiesKHR": {
                            "maxMultiviewInstanceIndex": 134217727,
                            "maxMultiviewViewCount": 6
                        }
                    }
                }
            }
        }"""

        json_files_dict = {"test_profile.json": json.loads(original_json_text)}
        pull_promoted_extensions_profiles_files(
            self.vk, 
            [PullBits.PROMOTED_EXTENSIONS, PullBits.IGNORE_EXTENSION_VERSIONS, PullBits.IGNORE_UNSUPPORTED], 
            json_files_dict
        )
        sort_profiles_files(self.vk, json_files_dict)

        res_baseline = json_files_dict["test_profile.json"]["capabilities"]["baseline"]
        
        # Verify feature and property structures were pulled for enabled extension
        self.assertIn("VkPhysicalDeviceMultiviewFeaturesKHR", res_baseline.get("features", {}))
        self.assertIn("VkPhysicalDeviceMultiviewPropertiesKHR", res_baseline.get("properties", {}))

        self.assertProfileDataEqual(json_files_dict["test_profile.json"], json.loads(expected_json_text))


    def test_pull_promoted_extensions_block_inheritance_vulkan10(self):
        """
        Verifies that for Vulkan 1.0 profiles with block inheritance, promoted extension extraction
        only targets primary capability block_a, leaving secondary block_b untouched.
        """
        original_json_text = """{
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8.0-106.json#",
            "profiles": {
                "VP_TEST_profile": {
                    "version": 1,
                    "api-version": "1.0.68",
                    "capabilities": [ "block_a", "block_b" ]
                }
            },
            "capabilities": {
                "block_a": {
                    "extensions": {
                        "VK_KHR_get_physical_device_properties2": 2
                    }
                },
                "block_b": {
                    "extensions": {
                        "VK_KHR_variable_pointers": 1
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
                    "capabilities": [ "block_a", "block_b" ]
                }
            },
            "capabilities": {
                "block_a": {
                    "extensions": {
                        "VK_KHR_get_physical_device_properties2": 2
                    }
                },
                "block_b": {
                    "extensions": {
                        "VK_KHR_storage_buffer_storage_class": 1,
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

        json_files_dict = {"test_profile.json": json.loads(original_json_text)}
        pull_promoted_extensions_profiles_files(
            self.vk, 
            [PullBits.PROMOTED_EXTENSIONS, PullBits.IGNORE_UNSUPPORTED], 
            json_files_dict
        )
        sort_profiles_files(self.vk, json_files_dict)

        self.assertProfileDataEqual(json_files_dict["test_profile.json"], json.loads(expected_json_text))


    def test_pull_promoted_extensions_profile_inheritance_vulkan10(self):
        """
        Verifies that for Vulkan 1.0 profiles with profile inheritance, dependency pulling
        operates independently per profile primary block.
        """
        original_json_text = """{
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8.0-106.json#",
            "profiles": {
                "VP_TEST_profile_a": {
                    "version": 1,
                    "api-version": "1.0.68",
                    "capabilities": [ "block_a" ]
                },
                "VP_TEST_profile_b": {
                    "version": 1,
                    "api-version": "1.0.68",
                    "capabilities": [ "block_b" ],
                    "profiles": [ "VP_TEST_profile_a" ]
                }
            },
            "capabilities": {
                "block_a": {
                    "extensions": {
                        "VK_KHR_get_physical_device_properties2": 1
                    }
                },
                "block_b": {
                    "extensions": {
                        "VK_KHR_storage_buffer_storage_class": 1,
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
                "VP_TEST_profile_a": {
                    "version": 1,
                    "api-version": "1.0.68",
                    "capabilities": [ "block_a" ]
                },
                "VP_TEST_profile_b": {
                    "version": 1,
                    "api-version": "1.0.68",
                    "capabilities": [ "block_b" ],
                    "profiles": [ "VP_TEST_profile_a" ]
                }
            },
            "capabilities": {
                "block_a": {
                    "extensions": {
                        "VK_KHR_get_physical_device_properties2": 1
                    }
                },
                "block_b": {
                    "extensions": {
                        "VK_KHR_storage_buffer_storage_class": 1,
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

        json_files_dict = {"test_profile.json": json.loads(original_json_text)}
        pull_promoted_extensions_profiles_files(
            self.vk, 
            [PullBits.PROMOTED_EXTENSIONS, PullBits.IGNORE_EXTENSION_VERSIONS, PullBits.IGNORE_UNSUPPORTED], 
            json_files_dict
        )
        sort_profiles_files(self.vk, json_files_dict)

        self.assertProfileDataEqual(json_files_dict["test_profile.json"], json.loads(expected_json_text))

    # -------------------------------------------------------------------------
    # Vulkan 1.1 Tests (VK_KHR_variable_pointers)
    # -------------------------------------------------------------------------

    def test_pull_promoted_extensions_block_unique_vulkan11(self):
        """
        Verifies that all extensions promoted to Vulkan 1.1 core (e.g. VK_KHR_multiview, VK_KHR_maintenance3)
        are automatically injected into a Vulkan 1.1 profile's primary capability block along with ALL their
        required features and properties structures.
        """
        original_json_text = """{
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8.0-106.json#",
            "profiles": {
                "VP_TEST_profile": {
                    "version": 1,
                    "api-version": "1.1.106",
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
        }"""

        expected_json_text = """{
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8.0-106.json#",
            "profiles": {
                "VP_TEST_profile": {
                    "version": 1,
                    "api-version": "1.1.106",
                    "capabilities": ["baseline"]
                }
            },
            "capabilities": {
                "baseline": {
                    "extensions": {
                        "VK_KHR_16bit_storage": 1,
                        "VK_KHR_bind_memory2": 1,
                        "VK_KHR_dedicated_allocation": 1,
                        "VK_KHR_descriptor_update_template": 1,
                        "VK_KHR_device_group": 1,
                        "VK_KHR_device_group_creation": 1,
                        "VK_KHR_external_fence": 1,
                        "VK_KHR_external_fence_capabilities": 1,
                        "VK_KHR_external_memory": 1,
                        "VK_KHR_external_memory_capabilities": 1,
                        "VK_KHR_external_semaphore": 1,
                        "VK_KHR_external_semaphore_capabilities": 1,
                        "VK_KHR_get_memory_requirements2": 1,
                        "VK_KHR_get_physical_device_properties2": 1,
                        "VK_KHR_maintenance1": 1,
                        "VK_KHR_maintenance2": 1,
                        "VK_KHR_maintenance3": 1,
                        "VK_KHR_multiview": 1,
                        "VK_KHR_relaxed_block_layout": 1,
                        "VK_KHR_sampler_ycbcr_conversion": 1,
                        "VK_KHR_shader_draw_parameters": 1,
                        "VK_KHR_storage_buffer_storage_class": 1,
                        "VK_KHR_variable_pointers": 1
                    },
                    "features": {
                        "VkPhysicalDevice16BitStorageFeaturesKHR": {
                            "storageBuffer16BitAccess": true
                        },
                        "VkPhysicalDeviceMultiviewFeaturesKHR": {
                            "multiview": true
                        },
                        "VkPhysicalDeviceSamplerYcbcrConversionFeaturesKHR": {
                            "samplerYcbcrConversion": true
                        },
                        "VkPhysicalDeviceVariablePointerFeaturesKHR": {
                            "variablePointersStorageBuffer": true
                        }
                    },
                    "properties": {
                        "VkPhysicalDeviceMaintenance3PropertiesKHR": {
                            "maxMemoryAllocationSize": 1073741824,
                            "maxPerSetDescriptors": 1024
                        },
                        "VkPhysicalDeviceMultiviewPropertiesKHR": {
                            "maxMultiviewInstanceIndex": 134217727,
                            "maxMultiviewViewCount": 6
                        }
                    }
                }
            }
        }"""

        json_files_dict = {"test_profile.json": json.loads(original_json_text)}
        pull_promoted_extensions_profiles_files(
            self.vk, 
            [PullBits.PROMOTED_EXTENSIONS, PullBits.IGNORE_EXTENSION_VERSIONS, PullBits.IGNORE_UNSUPPORTED], 
            json_files_dict
        )
        sort_profiles_files(self.vk, json_files_dict)

        res_baseline = json_files_dict["test_profile.json"]["capabilities"]["baseline"]

        # Specifically verify features structures from promoted extensions
        features_dict = res_baseline.get("features", {})
        self.assertIn("VkPhysicalDevice16BitStorageFeaturesKHR", features_dict)
        self.assertIn("VkPhysicalDeviceMultiviewFeaturesKHR", features_dict)
        self.assertIn("VkPhysicalDeviceSamplerYcbcrConversionFeaturesKHR", features_dict)
        self.assertIn("VkPhysicalDeviceVariablePointerFeaturesKHR", features_dict)

        # Specifically verify properties structures from promoted extensions
        properties_dict = res_baseline.get("properties", {})
        self.assertIn("VkPhysicalDeviceMaintenance3PropertiesKHR", properties_dict)
        self.assertIn("VkPhysicalDeviceMultiviewPropertiesKHR", properties_dict)

        self.assertProfileDataEqual(json_files_dict["test_profile.json"], json.loads(expected_json_text))


    def test_pull_promoted_extensions_block_inheritance_vulkan11(self):
        """
        Verifies that Vulkan 1.1 promoted extensions are injected into the primary block (block_a),
        leaving non-primary blocks (block_b) untouched.
        """
        original_json_text = """{
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8.0-106.json#",
            "profiles": {
                "VP_TEST_profile": {
                    "version": 1,
                    "api-version": "1.1.106",
                    "capabilities": [ "block_a", "block_b" ]
                }
            },
            "capabilities": {
                "block_a": {
                    "extensions": {
                        "VK_KHR_get_physical_device_properties2": 2
                    }
                },
                "block_b": {
                    "extensions": {
                        "VK_KHR_variable_pointers": 1
                    }
                }
            }
        }"""

        expected_json_text = """{
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8.0-106.json#",
            "profiles": {
                "VP_TEST_profile": {
                    "version": 1,
                    "api-version": "1.1.106",
                    "capabilities": [ "block_a", "block_b" ]
                }
            },
            "capabilities": {
                "block_a": {
                    "extensions": {
                        "VK_KHR_16bit_storage": 1,
                        "VK_KHR_bind_memory2": 1,
                        "VK_KHR_dedicated_allocation": 1,
                        "VK_KHR_descriptor_update_template": 1,
                        "VK_KHR_device_group": 1,
                        "VK_KHR_device_group_creation": 1,
                        "VK_KHR_external_fence": 1,
                        "VK_KHR_external_fence_capabilities": 1,
                        "VK_KHR_external_memory": 1,
                        "VK_KHR_external_memory_capabilities": 1,
                        "VK_KHR_external_semaphore": 1,
                        "VK_KHR_external_semaphore_capabilities": 1,
                        "VK_KHR_get_memory_requirements2": 1,
                        "VK_KHR_get_physical_device_properties2": 1,
                        "VK_KHR_maintenance1": 1,
                        "VK_KHR_maintenance2": 1,
                        "VK_KHR_maintenance3": 1,
                        "VK_KHR_multiview": 1,
                        "VK_KHR_relaxed_block_layout": 1,
                        "VK_KHR_sampler_ycbcr_conversion": 1,
                        "VK_KHR_shader_draw_parameters": 1,
                        "VK_KHR_storage_buffer_storage_class": 1,
                        "VK_KHR_variable_pointers": 1
                    },
                    "features": {
                        "VkPhysicalDevice16BitStorageFeaturesKHR": {
                            "storageBuffer16BitAccess": true
                        },
                        "VkPhysicalDeviceMultiviewFeaturesKHR": {
                            "multiview": true
                        },
                        "VkPhysicalDeviceSamplerYcbcrConversionFeaturesKHR": {
                            "samplerYcbcrConversion": true
                        },
                        "VkPhysicalDeviceVariablePointerFeaturesKHR": {
                            "variablePointersStorageBuffer": true
                        }
                    },
                    "properties": {
                        "VkPhysicalDeviceMaintenance3PropertiesKHR": {
                            "maxMemoryAllocationSize": 1073741824,
                            "maxPerSetDescriptors": 1024
                        },
                        "VkPhysicalDeviceMultiviewPropertiesKHR": {
                            "maxMultiviewInstanceIndex": 134217727,
                            "maxMultiviewViewCount": 6
                        }
                    }
                },
                "block_b": {
                    "extensions": {
                        "VK_KHR_variable_pointers": 1
                    }
                }
            }
        }"""

        json_files_dict = {"test_profile.json": json.loads(original_json_text)}
        pull_promoted_extensions_profiles_files(
            self.vk, 
            [PullBits.PROMOTED_EXTENSIONS, PullBits.IGNORE_EXTENSION_VERSIONS, PullBits.IGNORE_UNSUPPORTED], 
            json_files_dict
        )
        sort_profiles_files(self.vk, json_files_dict)

        self.assertProfileDataEqual(json_files_dict["test_profile.json"], json.loads(expected_json_text))


    def test_pull_promoted_extensions_profile_inheritance_vulkan11(self):
        """
        Verifies that Vulkan 1.1 promoted extensions are injected into each profile's primary block across profile inheritance trees.
        """
        original_json_text = """{
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8.0-106.json#",
            "profiles": {
                "VP_TEST_profile_a": {
                    "version": 1,
                    "api-version": "1.1.106",
                    "capabilities": [ "block_a" ]
                },
                "VP_TEST_profile_b": {
                    "version": 1,
                    "api-version": "1.1.106",
                    "capabilities": [ "block_b" ],
                    "profiles": [ "VP_TEST_profile_a" ]
                }
            },
            "capabilities": {
                "block_a": {
                    "extensions": {
                        "VK_KHR_get_physical_device_properties2": 2
                    }
                },
                "block_b": {
                    "extensions": {
                        "VK_KHR_variable_pointers": 1
                    }
                }
            }
        }"""

        expected_json_text = """{
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8.0-106.json#",
            "profiles": {
                "VP_TEST_profile_a": {
                    "version": 1,
                    "api-version": "1.1.106",
                    "capabilities": [ "block_a" ]
                },
                "VP_TEST_profile_b": {
                    "version": 1,
                    "api-version": "1.1.106",
                    "capabilities": [ "block_b" ],
                    "profiles": [ "VP_TEST_profile_a" ]
                }
            },
            "capabilities": {
                "block_a": {
                    "extensions": {
                        "VK_KHR_16bit_storage": 1,
                        "VK_KHR_bind_memory2": 1,
                        "VK_KHR_dedicated_allocation": 1,
                        "VK_KHR_descriptor_update_template": 1,
                        "VK_KHR_device_group": 1,
                        "VK_KHR_device_group_creation": 1,
                        "VK_KHR_external_fence": 1,
                        "VK_KHR_external_fence_capabilities": 1,
                        "VK_KHR_external_memory": 1,
                        "VK_KHR_external_memory_capabilities": 1,
                        "VK_KHR_external_semaphore": 1,
                        "VK_KHR_external_semaphore_capabilities": 1,
                        "VK_KHR_get_memory_requirements2": 1,
                        "VK_KHR_get_physical_device_properties2": 1,
                        "VK_KHR_maintenance1": 1,
                        "VK_KHR_maintenance2": 1,
                        "VK_KHR_maintenance3": 1,
                        "VK_KHR_multiview": 1,
                        "VK_KHR_relaxed_block_layout": 1,
                        "VK_KHR_sampler_ycbcr_conversion": 1,
                        "VK_KHR_shader_draw_parameters": 1,
                        "VK_KHR_storage_buffer_storage_class": 1,
                        "VK_KHR_variable_pointers": 1
                    },
                    "features": {
                        "VkPhysicalDevice16BitStorageFeaturesKHR": {
                            "storageBuffer16BitAccess": true
                        },
                        "VkPhysicalDeviceMultiviewFeaturesKHR": {
                            "multiview": true
                        },
                        "VkPhysicalDeviceSamplerYcbcrConversionFeaturesKHR": {
                            "samplerYcbcrConversion": true
                        },
                        "VkPhysicalDeviceVariablePointerFeaturesKHR": {
                            "variablePointersStorageBuffer": true
                        }
                    },
                    "properties": {
                        "VkPhysicalDeviceMaintenance3PropertiesKHR": {
                            "maxMemoryAllocationSize": 1073741824,
                            "maxPerSetDescriptors": 1024
                        },
                        "VkPhysicalDeviceMultiviewPropertiesKHR": {
                            "maxMultiviewInstanceIndex": 134217727,
                            "maxMultiviewViewCount": 6
                        }
                    }
                },
                "block_b": {
                    "extensions": {
                        "VK_KHR_variable_pointers": 1
                    }
                }
            }
        }"""

        json_files_dict = {"test_profile.json": json.loads(original_json_text)}
        pull_promoted_extensions_profiles_files(
            self.vk, 
            [PullBits.PROMOTED_EXTENSIONS, PullBits.IGNORE_EXTENSION_VERSIONS, PullBits.IGNORE_UNSUPPORTED], 
            json_files_dict
        )
        sort_profiles_files(self.vk, json_files_dict)

        self.assertProfileDataEqual(json_files_dict["test_profile.json"], json.loads(expected_json_text))

    # -------------------------------------------------------------------------
    # Vulkan 1.1 Advanced Tests (VK_KHR_swapchain_mutable_format)
    # -------------------------------------------------------------------------

    def test_pull_promoted_extensions_block_unique_vulkan11_advanced(self):
        """
        Verifies that when pulling promoted extensions for a Vulkan 1.1 profile containing VK_KHR_swapchain_mutable_format,
        all 1.1 promoted extensions plus transitive dependencies (VK_KHR_swapchain, VK_KHR_surface, VK_KHR_image_format_list) are present.
        """
        original_json_text = """{
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8.0-106.json#",
            "profiles": {
                "VP_TEST_profile": {
                    "version": 1,
                    "api-version": "1.1.106",
                    "capabilities": ["baseline"]
                }
            },
            "capabilities": {
                "baseline": {
                    "extensions": {
                        "VK_KHR_swapchain_mutable_format": 1
                    }
                }
            }
        }"""

        expected_json_text = """{
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8.0-106.json#",
            "profiles": {
                "VP_TEST_profile": {
                    "version": 1,
                    "api-version": "1.1.106",
                    "capabilities": ["baseline"]
                }
            },
            "capabilities": {
                "baseline": {
                    "extensions": {
                        "VK_KHR_16bit_storage": 1,
                        "VK_KHR_bind_memory2": 1,
                        "VK_KHR_dedicated_allocation": 1,
                        "VK_KHR_descriptor_update_template": 1,
                        "VK_KHR_device_group": 1,
                        "VK_KHR_device_group_creation": 1,
                        "VK_KHR_external_fence": 1,
                        "VK_KHR_external_fence_capabilities": 1,
                        "VK_KHR_external_memory": 1,
                        "VK_KHR_external_memory_capabilities": 1,
                        "VK_KHR_external_semaphore": 1,
                        "VK_KHR_external_semaphore_capabilities": 1,
                        "VK_KHR_get_memory_requirements2": 1,
                        "VK_KHR_get_physical_device_properties2": 1,
                        "VK_KHR_image_format_list": 1,
                        "VK_KHR_maintenance1": 1,
                        "VK_KHR_maintenance2": 1,
                        "VK_KHR_maintenance3": 1,
                        "VK_KHR_multiview": 1,
                        "VK_KHR_relaxed_block_layout": 1,
                        "VK_KHR_sampler_ycbcr_conversion": 1,
                        "VK_KHR_shader_draw_parameters": 1,
                        "VK_KHR_storage_buffer_storage_class": 1,
                        "VK_KHR_surface": 1,
                        "VK_KHR_swapchain": 1,
                        "VK_KHR_swapchain_mutable_format": 1,
                        "VK_KHR_variable_pointers": 1
                    },
                    "features": {
                        "VkPhysicalDevice16BitStorageFeaturesKHR": {
                            "storageBuffer16BitAccess": true
                        },
                        "VkPhysicalDeviceMultiviewFeaturesKHR": {
                            "multiview": true
                        },
                        "VkPhysicalDeviceSamplerYcbcrConversionFeaturesKHR": {
                            "samplerYcbcrConversion": true
                        },
                        "VkPhysicalDeviceVariablePointerFeaturesKHR": {
                            "variablePointersStorageBuffer": true
                        }
                    },
                    "properties": {
                        "VkPhysicalDeviceMaintenance3PropertiesKHR": {
                            "maxMemoryAllocationSize": 1073741824,
                            "maxPerSetDescriptors": 1024
                        },
                        "VkPhysicalDeviceMultiviewPropertiesKHR": {
                            "maxMultiviewInstanceIndex": 134217727,
                            "maxMultiviewViewCount": 6
                        }
                    }
                }
            }
        }"""

        json_files_dict = {"test_profile.json": json.loads(original_json_text)}
        pull_promoted_extensions_profiles_files(
            self.vk, 
            [PullBits.PROMOTED_EXTENSIONS, PullBits.IGNORE_EXTENSION_VERSIONS, PullBits.IGNORE_UNSUPPORTED], 
            json_files_dict
        )
        sort_profiles_files(self.vk, json_files_dict)

        self.assertProfileDataEqual(json_files_dict["test_profile.json"], json.loads(expected_json_text))


    def test_pull_promoted_extensions_block_inheritance_vulkan11_advanced(self):
        """
        Verifies that when pulling promoted extensions for a Vulkan 1.1 profile with block inheritance,
        promoted extensions and their dependencies are populated into primary block_a, while block_b is untouched.
        """
        original_json_text = """{
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8.0-106.json#",
            "profiles": {
                "VP_TEST_profile": {
                    "version": 1,
                    "api-version": "1.1.106",
                    "capabilities": [ "block_a", "block_b" ]
                }
            },
            "capabilities": {
                "block_a": {
                    "extensions": {
                        "VK_KHR_swapchain": 70
                    }
                },
                "block_b": {
                    "extensions": {
                        "VK_KHR_swapchain_mutable_format": 1
                    }
                }
            }
        }"""

        expected_json_text = """{
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8.0-106.json#",
            "profiles": {
                "VP_TEST_profile": {
                    "version": 1,
                    "api-version": "1.1.106",
                    "capabilities": [ "block_a", "block_b" ]
                }
            },
            "capabilities": {
                "block_a": {
                    "extensions": {
                        "VK_KHR_16bit_storage": 1,
                        "VK_KHR_bind_memory2": 1,
                        "VK_KHR_dedicated_allocation": 1,
                        "VK_KHR_descriptor_update_template": 1,
                        "VK_KHR_device_group": 1,
                        "VK_KHR_device_group_creation": 1,
                        "VK_KHR_external_fence": 1,
                        "VK_KHR_external_fence_capabilities": 1,
                        "VK_KHR_external_memory": 1,
                        "VK_KHR_external_memory_capabilities": 1,
                        "VK_KHR_external_semaphore": 1,
                        "VK_KHR_external_semaphore_capabilities": 1,
                        "VK_KHR_get_memory_requirements2": 1,
                        "VK_KHR_get_physical_device_properties2": 1,
                        "VK_KHR_maintenance1": 1,
                        "VK_KHR_maintenance2": 1,
                        "VK_KHR_maintenance3": 1,
                        "VK_KHR_multiview": 1,
                        "VK_KHR_relaxed_block_layout": 1,
                        "VK_KHR_sampler_ycbcr_conversion": 1,
                        "VK_KHR_shader_draw_parameters": 1,
                        "VK_KHR_storage_buffer_storage_class": 1,
                        "VK_KHR_surface": 1,
                        "VK_KHR_swapchain": 1,
                        "VK_KHR_variable_pointers": 1
                    },
                    "features": {
                        "VkPhysicalDevice16BitStorageFeaturesKHR": {
                            "storageBuffer16BitAccess": true
                        },
                        "VkPhysicalDeviceMultiviewFeaturesKHR": {
                            "multiview": true
                        },
                        "VkPhysicalDeviceSamplerYcbcrConversionFeaturesKHR": {
                            "samplerYcbcrConversion": true
                        },
                        "VkPhysicalDeviceVariablePointerFeaturesKHR": {
                            "variablePointersStorageBuffer": true
                        }
                    },
                    "properties": {
                        "VkPhysicalDeviceMaintenance3PropertiesKHR": {
                            "maxMemoryAllocationSize": 1073741824,
                            "maxPerSetDescriptors": 1024
                        },
                        "VkPhysicalDeviceMultiviewPropertiesKHR": {
                            "maxMultiviewInstanceIndex": 134217727,
                            "maxMultiviewViewCount": 6
                        }
                    }
                },
                "block_b": {
                    "extensions": {
                        "VK_KHR_image_format_list": 1,
                        "VK_KHR_swapchain_mutable_format": 1
                    }
                }
            }
        }"""

        json_files_dict = {"test_profile.json": json.loads(original_json_text)}
        pull_promoted_extensions_profiles_files(
            self.vk, 
            [PullBits.PROMOTED_EXTENSIONS, PullBits.IGNORE_EXTENSION_VERSIONS, PullBits.IGNORE_UNSUPPORTED], 
            json_files_dict
        )
        sort_profiles_files(self.vk, json_files_dict)

        self.assertProfileDataEqual(json_files_dict["test_profile.json"], json.loads(expected_json_text))


    def test_pull_promoted_extensions_profile_inheritance_vulkan11_advanced(self):
        """
        Verifies that when pulling promoted extensions for Vulkan 1.1 profiles with profile inheritance,
        each profile's primary block receives promoted extensions independently.
        """
        original_json_text = """{
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8.0-106.json#",
            "profiles": {
                "VP_TEST_profile_a": {
                    "version": 1,
                    "api-version": "1.1.106",
                    "capabilities": [ "block_a" ]
                },
                "VP_TEST_profile_b": {
                    "version": 1,
                    "api-version": "1.1.106",
                    "capabilities": [ "block_b" ],
                    "profiles": [ "VP_TEST_profile_a" ]
                }
            },
            "capabilities": {
                "block_a": {
                    "extensions": {
                        "VK_KHR_swapchain": 70
                    }
                },
                "block_b": {
                    "extensions": {
                        "VK_KHR_swapchain_mutable_format": 1
                    }
                }
            }
        }"""

        expected_json_text = """{
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8.0-106.json#",
            "profiles": {
                "VP_TEST_profile_a": {
                    "version": 1,
                    "api-version": "1.1.106",
                    "capabilities": [ "block_a" ]
                },
                "VP_TEST_profile_b": {
                    "version": 1,
                    "api-version": "1.1.106",
                    "capabilities": [ "block_b" ],
                    "profiles": [ "VP_TEST_profile_a" ]
                }
            },
            "capabilities": {
                "block_a": {
                    "extensions": {
                        "VK_KHR_16bit_storage": 1,
                        "VK_KHR_bind_memory2": 1,
                        "VK_KHR_dedicated_allocation": 1,
                        "VK_KHR_descriptor_update_template": 1,
                        "VK_KHR_device_group": 1,
                        "VK_KHR_device_group_creation": 1,
                        "VK_KHR_external_fence": 1,
                        "VK_KHR_external_fence_capabilities": 1,
                        "VK_KHR_external_memory": 1,
                        "VK_KHR_external_memory_capabilities": 1,
                        "VK_KHR_external_semaphore": 1,
                        "VK_KHR_external_semaphore_capabilities": 1,
                        "VK_KHR_get_memory_requirements2": 1,
                        "VK_KHR_get_physical_device_properties2": 1,
                        "VK_KHR_maintenance1": 1,
                        "VK_KHR_maintenance2": 1,
                        "VK_KHR_maintenance3": 1,
                        "VK_KHR_multiview": 1,
                        "VK_KHR_relaxed_block_layout": 1,
                        "VK_KHR_sampler_ycbcr_conversion": 1,
                        "VK_KHR_shader_draw_parameters": 1,
                        "VK_KHR_storage_buffer_storage_class": 1,
                        "VK_KHR_surface": 1,
                        "VK_KHR_swapchain": 1,
                        "VK_KHR_variable_pointers": 1
                    },
                    "features": {
                        "VkPhysicalDevice16BitStorageFeaturesKHR": {
                            "storageBuffer16BitAccess": true
                        },
                        "VkPhysicalDeviceMultiviewFeaturesKHR": {
                            "multiview": true
                        },
                        "VkPhysicalDeviceSamplerYcbcrConversionFeaturesKHR": {
                            "samplerYcbcrConversion": true
                        },
                        "VkPhysicalDeviceVariablePointerFeaturesKHR": {
                            "variablePointersStorageBuffer": true
                        }
                    },
                    "properties": {
                        "VkPhysicalDeviceMaintenance3PropertiesKHR": {
                            "maxMemoryAllocationSize": 1073741824,
                            "maxPerSetDescriptors": 1024
                        },
                        "VkPhysicalDeviceMultiviewPropertiesKHR": {
                            "maxMultiviewInstanceIndex": 134217727,
                            "maxMultiviewViewCount": 6
                        }
                    }
                },
                "block_b": {
                    "extensions": {
                        "VK_KHR_image_format_list": 1,
                        "VK_KHR_swapchain_mutable_format": 1
                    }
                }
            }
        }"""

        json_files_dict = {"test_profile.json": json.loads(original_json_text)}
        pull_promoted_extensions_profiles_files(
            self.vk, 
            [PullBits.PROMOTED_EXTENSIONS, PullBits.IGNORE_EXTENSION_VERSIONS, PullBits.IGNORE_UNSUPPORTED], 
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
    TestConvertPullPromotedExtensions.registry_path = args.registry

    unittest.main(argv=[sys.argv[0]] + unparsed)