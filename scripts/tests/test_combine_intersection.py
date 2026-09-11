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

import os
import sys
import json
import argparse
import unittest
import tempfile
from pathlib import Path

scripts_dir = Path(__file__).resolve().parent.parent
if str(scripts_dir) not in sys.path:
    sys.path.insert(0, str(scripts_dir))

from source.main_combine import main_combine, CombineMode


REGISTRY_PATH = None


class TestCombineIntersection(unittest.TestCase):

    def setUp(self):
        if not REGISTRY_PATH or not os.path.exists(REGISTRY_PATH):
            self.skipTest(f"Vulkan registry XML (vk.xml) not found at: {REGISTRY_PATH}")

    def create_test_profiles(self, input_dir: Path):
        """Creates two minimal test profiles with extension, feature, property, and format overlaps."""
        profile_a = {
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8-latest.json#",
            "capabilities": {
                "cap_a": {
                    "extensions": {
                        "VK_KHR_swapchain": 1,
                        "VK_KHR_16bit_storage": 1
                    },
                    "features": {
                        "VkPhysicalDeviceFeatures": {
                            "robustBufferAccess": True,
                            "fullDrawIndexUint32": True
                        }
                    },
                    "properties": {
                        "VkPhysicalDeviceProperties": {
                            "limits": {
                                "maxImageDimension2D": 8192,
                                "maxPushConstantsSize": 128
                            }
                        }
                    },
                    "formats": {
                        "VK_FORMAT_R8G8B8A8_UNORM": {
                            "VkFormatProperties": {
                                "optimalTilingFeatures": [
                                    "VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT",
                                    "VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT"
                                ]
                            }
                        }
                    }
                }
            },
            "profiles": {
                "VP_TEST_A": {
                    "version": 1,
                    "api-version": "1.0.0",
                    "label": "Test Profile A",
                    "description": "Test profile A",
                    "capabilities": ["cap_a"]
                }
            }
        }
        profile_b = {
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8-latest.json#",
            "capabilities": {
                "cap_b": {
                    "extensions": {
                        "VK_KHR_swapchain": 1,
                        "VK_KHR_8bit_storage": 1
                    },
                    "features": {
                        "VkPhysicalDeviceFeatures": {
                            "robustBufferAccess": True,
                            "imageCubeArray": True
                        }
                    },
                    "properties": {
                        "VkPhysicalDeviceProperties": {
                            "limits": {
                                "maxImageDimension2D": 4096,
                                "maxPushConstantsSize": 128
                            }
                        }
                    },
                    "formats": {
                        "VK_FORMAT_R8G8B8A8_UNORM": {
                            "VkFormatProperties": {
                                "optimalTilingFeatures": [
                                    "VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT",
                                    "VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT"
                                ]
                            }
                        }
                    }
                }
            },
            "profiles": {
                "VP_TEST_B": {
                    "version": 1,
                    "api-version": "1.0.0",
                    "label": "Test Profile B",
                    "description": "Test profile B",
                    "capabilities": ["cap_b"]
                }
            }
        }

        with open(input_dir / "VP_TEST_A.json", "w", encoding="utf-8") as f:
            json.dump(profile_a, f, indent=4)
        with open(input_dir / "VP_TEST_B.json", "w", encoding="utf-8") as f:
            json.dump(profile_b, f, indent=4)

    def create_or_test_profiles(self, input_dir: Path):
        """Creates test profiles containing nested OR capability blocks (lists of capability names)."""
        profile_a = {
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8-latest.json#",
            "capabilities": {
                "cap_base_a": {
                    "features": {
                        "VkPhysicalDeviceFeatures": {
                            "robustBufferAccess": True
                        }
                    }
                },
                "cap_variant1": {
                    "features": {
                        "VkPhysicalDeviceFeatures": {
                            "multiDrawIndirect": True
                        }
                    }
                },
                "cap_variant2": {
                    "features": {
                        "VkPhysicalDeviceFeatures": {
                            "drawIndirectFirstInstance": True
                        }
                    }
                }
            },
            "profiles": {
                "VP_TEST_OR_A": {
                    "version": 1,
                    "api-version": "1.0.0",
                    "label": "Test OR Profile A",
                    "description": "Test OR profile A",
                    "capabilities": [
                        "cap_base_a",
                        ["cap_variant1", "cap_variant2"]
                    ]
                }
            }
        }
        profile_b = {
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8-latest.json#",
            "capabilities": {
                "cap_base_b": {
                    "features": {
                        "VkPhysicalDeviceFeatures": {
                            "robustBufferAccess": True
                        }
                    }
                },
                "cap_variant1": {
                    "features": {
                        "VkPhysicalDeviceFeatures": {
                            "multiDrawIndirect": True
                        }
                    }
                },
                "cap_variant3": {
                    "features": {
                        "VkPhysicalDeviceFeatures": {
                            "depthClamp": True
                        }
                    }
                }
            },
            "profiles": {
                "VP_TEST_OR_B": {
                    "version": 1,
                    "api-version": "1.0.0",
                    "label": "Test OR Profile B",
                    "description": "Test OR profile B",
                    "capabilities": [
                        "cap_base_b",
                        ["cap_variant1", "cap_variant3"]
                    ]
                }
            }
        }

        with open(input_dir / "VP_TEST_OR_A.json", "w", encoding="utf-8") as f:
            json.dump(profile_a, f, indent=4)
        with open(input_dir / "VP_TEST_OR_B.json", "w", encoding="utf-8") as f:
            json.dump(profile_b, f, indent=4)

    def test_combine_intersection(self):
        """Tests CombineMode.INTERSECTION across extensions, features, properties, and formats."""
        with tempfile.TemporaryDirectory() as temp_dir:
            temp_path = Path(temp_dir)
            input_dir = temp_path / "input"
            input_dir.mkdir()
            output_file = temp_path / "combined_intersection.json"

            self.create_test_profiles(input_dir)

            args = argparse.Namespace(
                registry=REGISTRY_PATH,
                input=str(input_dir),
                output=str(output_file),
                mode=CombineMode.INTERSECTION,
                profile_name="VP_COMBINED_INTERSECTION",
                api="vulkan",
                format="pretty",
                transform=None,
                validate=None
            )
            main_combine(args)

            self.assertTrue(output_file.exists())
            with open(output_file, "r", encoding="utf-8") as f:
                res_data = json.load(f)

            combined_cap = res_data.get("capabilities", {}).get("VP_COMBINED_INTERSECTION_block", {})

            # 1. Extensions: keeps only common extension
            exts = combined_cap.get("extensions", {})
            self.assertIn("VK_KHR_swapchain", exts)
            self.assertNotIn("VK_KHR_16bit_storage", exts)
            self.assertNotIn("VK_KHR_8bit_storage", exts)

            # 2. Features: keeps only common feature
            vk_feats = combined_cap.get("features", {}).get("VkPhysicalDeviceFeatures", {})
            self.assertIn("robustBufferAccess", vk_feats)
            self.assertNotIn("fullDrawIndexUint32", vk_feats)
            self.assertNotIn("imageCubeArray", vk_feats)

            # 3. Properties: keeps minimum required limit value (4096 vs 8192 -> 4096)
            limits = combined_cap.get("properties", {}).get("VkPhysicalDeviceProperties", {}).get("limits", {})
            self.assertEqual(limits.get("maxImageDimension2D"), 4096)
            self.assertEqual(limits.get("maxPushConstantsSize"), 128)

            # 4. Formats: keeps only common format feature flags
            fmt_feats = combined_cap.get("formats", {}).get("VK_FORMAT_R8G8B8A8_UNORM", {}).get("VkFormatProperties", {}).get("optimalTilingFeatures", [])
            self.assertIn("VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT", fmt_feats)
            self.assertNotIn("VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT", fmt_feats)
            self.assertNotIn("VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT", fmt_feats)

    def test_combine_intersection_or_capabilities(self):
        """Tests CombineMode.INTERSECTION with nested OR capability block lists."""
        with tempfile.TemporaryDirectory() as temp_dir:
            temp_path = Path(temp_dir)
            input_dir = temp_path / "input"
            input_dir.mkdir()
            output_file = temp_path / "combined_intersection_or.json"

            self.create_or_test_profiles(input_dir)

            args = argparse.Namespace(
                registry=REGISTRY_PATH,
                input=str(input_dir),
                output=str(output_file),
                mode=CombineMode.INTERSECTION,
                profile_name="VP_COMBINED_INTERSECTION_OR",
                api="vulkan",
                format="pretty",
                transform=None,
                validate=None
            )
            main_combine(args)

            self.assertTrue(output_file.exists())
            with open(output_file, "r", encoding="utf-8") as f:
                res_data = json.load(f)

            combined_cap = res_data.get("capabilities", {}).get("VP_COMBINED_INTERSECTION_OR_block", {})
            vk_feats = combined_cap.get("features", {}).get("VkPhysicalDeviceFeatures", {})

            # RobustBufferAccess is in base blocks of both profiles; multiDrawIndirect is in cap_variant1 of both
            self.assertIn("robustBufferAccess", vk_feats)
            self.assertIn("multiDrawIndirect", vk_feats)

            # drawIndirectFirstInstance (cap_variant2) and depthClamp (cap_variant3) are not in both
            self.assertNotIn("drawIndirectFirstInstance", vk_feats)
            self.assertNotIn("depthClamp", vk_feats)


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("--registry", "-r", action="store", help="Path to vk.xml")
    args, unittest_args = parser.parse_known_args()

    if args.registry:
        REGISTRY_PATH = args.registry

    sys.argv = [sys.argv[0]] + unittest_args
    unittest.main()
    