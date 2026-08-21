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
from source.main_convert import strip_profiles_files_capabilities_duplication


class TestConvertStripDuplication(unittest.TestCase):
    registry_path = None

    def setUp(self):
        self.vk: VulkanObject = initVulkanObject('vulkan', self.registry_path)

    def test_strip_duplication_from_inherited_parent_blocks(self):
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
        strip_profiles_files_capabilities_duplication(self.vk, json_files_dict)

        generated_data = json_files_dict["test_profile.json"]
        expected_data = json.loads(expected_json_text)

        gen_caps = generated_data.get("capabilities", {})
        exp_caps = expected_data.get("capabilities", {})

        for block_name in ["caps0", "caps1", "caps2", "caps3", "caps4"]:
            self.assertEqual(gen_caps.get(block_name), exp_caps.get(block_name), f"Mismatch in {block_name}")

        self.assertEqual(generated_data, expected_data)

        
    def test_strip_duplication_from_inherited_parent_profile(self):
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
        strip_profiles_files_capabilities_duplication(self.vk, json_files_dict)

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
    