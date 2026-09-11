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


class TestCombineDifference(unittest.TestCase):

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

    def test_combine_difference(self):
        """Tests CombineMode.DIFFERENCE across extensions, features, properties, and formats."""
        with tempfile.TemporaryDirectory() as temp_dir:
            temp_path = Path(temp_dir)
            input_dir = temp_path / "input"
            input_dir.mkdir()
            output_dir = temp_path / "output_diff"

            self.create_test_profiles(input_dir)

            args = argparse.Namespace(
                registry=REGISTRY_PATH,
                input=str(input_dir),
                output=str(output_dir),
                mode=CombineMode.DIFFERENCE,
                api="vulkan",
                format="pretty",
                transform=None,
                validate=None
            )
            main_combine(args)

            self.assertTrue(output_dir.is_dir())
            out_a = output_dir / "VP_TEST_A.json"
            out_b = output_dir / "VP_TEST_B.json"

            self.assertTrue(out_a.exists())
            self.assertTrue(out_b.exists())

            with open(out_a, "r", encoding="utf-8") as f:
                data_a = json.load(f)
            with open(out_b, "r", encoding="utf-8") as f:
                data_b = json.load(f)

            cap_a = data_a.get("capabilities", {}).get("cap_a", {})
            cap_b = data_b.get("capabilities", {}).get("cap_b", {})

            # 1. Extensions: common extension (VK_KHR_swapchain) is removed
            exts_a = cap_a.get("extensions", {})
            exts_b = cap_b.get("extensions", {})
            self.assertNotIn("VK_KHR_swapchain", exts_a)
            self.assertIn("VK_KHR_16bit_storage", exts_a)
            self.assertNotIn("VK_KHR_swapchain", exts_b)
            self.assertIn("VK_KHR_8bit_storage", exts_b)

            # 2. Features: common feature (robustBufferAccess) is removed
            feats_a = cap_a.get("features", {}).get("VkPhysicalDeviceFeatures", {})
            feats_b = cap_b.get("features", {}).get("VkPhysicalDeviceFeatures", {})
            self.assertNotIn("robustBufferAccess", feats_a)
            self.assertIn("fullDrawIndexUint32", feats_a)
            self.assertNotIn("robustBufferAccess", feats_b)
            self.assertIn("imageCubeArray", feats_b)

            # 3. Properties: common property (maxPushConstantsSize) is removed
            limits_a = cap_a.get("properties", {}).get("VkPhysicalDeviceProperties", {}).get("limits", {})
            limits_b = cap_b.get("properties", {}).get("VkPhysicalDeviceProperties", {}).get("limits", {})
            self.assertNotIn("maxPushConstantsSize", limits_a)
            self.assertEqual(limits_a.get("maxImageDimension2D"), 8192)
            self.assertNotIn("maxPushConstantsSize", limits_b)

            # 4. Formats: common format feature flag (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT) is removed
            fmts_a = cap_a.get("formats", {}).get("VK_FORMAT_R8G8B8A8_UNORM", {}).get("VkFormatProperties", {}).get("optimalTilingFeatures", [])
            fmts_b = cap_b.get("formats", {}).get("VK_FORMAT_R8G8B8A8_UNORM", {}).get("VkFormatProperties", {}).get("optimalTilingFeatures", [])
            self.assertNotIn("VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT", fmts_a)
            self.assertIn("VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT", fmts_a)
            self.assertNotIn("VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT", fmts_b)
            self.assertIn("VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT", fmts_b)

    def test_combine_difference_or_capabilities(self):
        """Tests CombineMode.DIFFERENCE with nested OR capability block lists."""
        with tempfile.TemporaryDirectory() as temp_dir:
            temp_path = Path(temp_dir)
            input_dir = temp_path / "input"
            input_dir.mkdir()
            output_dir = temp_path / "output_diff_or"

            self.create_or_test_profiles(input_dir)

            args = argparse.Namespace(
                registry=REGISTRY_PATH,
                input=str(input_dir),
                output=str(output_dir),
                mode=CombineMode.DIFFERENCE,
                api="vulkan",
                format="pretty",
                transform=None,
                validate=None
            )
            main_combine(args)

            self.assertTrue(output_dir.is_dir())
            out_a = output_dir / "VP_TEST_OR_A.json"
            out_b = output_dir / "VP_TEST_OR_B.json"

            self.assertTrue(out_a.exists())
            self.assertTrue(out_b.exists())

            with open(out_a, "r", encoding="utf-8") as f:
                data_a = json.load(f)
            with open(out_b, "r", encoding="utf-8") as f:
                data_b = json.load(f)

            caps_a = data_a.get("capabilities", {})
            caps_b = data_b.get("capabilities", {})

            # 1. Common features (robustBufferAccess in base blocks, multiDrawIndirect in cap_variant1) are stripped
            feats_base_a = caps_a.get("cap_base_a", {}).get("features", {}).get("VkPhysicalDeviceFeatures", {})
            feats_base_b = caps_b.get("cap_base_b", {}).get("features", {}).get("VkPhysicalDeviceFeatures", {})
            self.assertNotIn("robustBufferAccess", feats_base_a)
            self.assertNotIn("robustBufferAccess", feats_base_b)

            feats_var1_a = caps_a.get("cap_variant1", {}).get("features", {}).get("VkPhysicalDeviceFeatures", {})
            feats_var1_b = caps_b.get("cap_variant1", {}).get("features", {}).get("VkPhysicalDeviceFeatures", {})
            self.assertNotIn("multiDrawIndirect", feats_var1_a)
            self.assertNotIn("multiDrawIndirect", feats_var1_b)

            # 2. Unique OR variant capabilities remain
            feats_var2_a = caps_a.get("cap_variant2", {}).get("features", {}).get("VkPhysicalDeviceFeatures", {})
            feats_var3_b = caps_b.get("cap_variant3", {}).get("features", {}).get("VkPhysicalDeviceFeatures", {})
            self.assertIn("drawIndirectFirstInstance", feats_var2_a)
            self.assertIn("depthClamp", feats_var3_b)


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("--registry", "-r", action="store", help="Path to vk.xml")
    args, unittest_args = parser.parse_known_args()

    if args.registry:
        REGISTRY_PATH = args.registry

    sys.argv = [sys.argv[0]] + unittest_args
    unittest.main()
    