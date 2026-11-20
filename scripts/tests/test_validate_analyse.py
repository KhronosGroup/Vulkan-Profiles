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
from source.transform_utils import AnalyseBits
from source.main_validate import VulkanProfilesDataValidation


class TestAnalyseBits(unittest.TestCase):
    registry_path = None

    def setUp(self):
        self.vk: VulkanObject = initVulkanObject('vulkan', self.registry_path)
        self.validator = VulkanProfilesDataValidation(self.vk)

    def test_analyse_api_version(self):
        """
        AnalyseBits.API_VERSION:
        Verifies detection when a child profile targets an older Vulkan version (1.0)
        than an inherited parent required profile (1.1).
        """
        json_text = """{
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8.0-106.json#",
            "profiles": {
                "VP_TEST_parent": {
                    "version": 1,
                    "api-version": "1.1.0",
                    "capabilities": ["caps_parent"]
                },
                "VP_TEST_child": {
                    "version": 1,
                    "api-version": "1.0.0",
                    "profiles": ["VP_TEST_parent"],
                    "capabilities": ["caps_child"]
                }
            },
            "capabilities": {
                "caps_parent": {},
                "caps_child": {}
            }
        }"""

        json_files_dict = {"test_profile.json": json.loads(json_text)}
        issues = self.validator.validate_data(json_files_dict, [AnalyseBits.API_VERSION])

        self.assertTrue(any("inherits profile 'VP_TEST_parent' which targets newer Vulkan" in issue for issue in issues))

    def test_analyse_extensions(self):
        """
        AnalyseBits.EXTENSIONS:
        Verifies detection when an extension structure (VkPhysicalDeviceImagelessFramebufferFeaturesKHR)
        is included without declaring its defining extension (VK_KHR_imageless_framebuffer).
        """
        json_text = """{
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
                        "VkPhysicalDeviceImagelessFramebufferFeaturesKHR": {
                            "imagelessFramebuffer": true
                        }
                    }
                }
            }
        }"""

        json_files_dict = {"test_profile.json": json.loads(json_text)}
        issues = self.validator.validate_data(json_files_dict, [AnalyseBits.EXTENSIONS])

        undeclared_issues = [issue for issue in issues if "belongs to extension 'VK_KHR_imageless_framebuffer'" in issue]
        self.assertEqual(len(undeclared_issues), 1)
        self.assertEqual(getattr(undeclared_issues[0], "level", None), logging.ERROR)

    def test_analyse_aliases(self):
        """
        AnalyseBits.ALIASES:
        Verifies detection of mismatching member values across aliased capability structures
        (e.g., VkPhysicalDeviceImagelessFramebufferFeaturesKHR vs VkPhysicalDeviceVulkan12Features).
        """
        json_text = """{
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8.0-106.json#",
            "profiles": {
                "VP_TEST_profile": {
                    "version": 1,
                    "api-version": "1.2.0",
                    "capabilities": ["baseline"]
                }
            },
            "capabilities": {
                "baseline": {
                    "features": {
                        "VkPhysicalDeviceImagelessFramebufferFeaturesKHR": {
                            "imagelessFramebuffer": true
                        },
                        "VkPhysicalDeviceVulkan12Features": {
                            "imagelessFramebuffer": false
                        }
                    }
                }
            }
        }"""

        json_files_dict = {"test_profile.json": json.loads(json_text)}
        issues = self.validator.validate_data(json_files_dict, [AnalyseBits.ALIASES])

        self.assertTrue(any("has mismatching value" in issue and "aliased structure" in issue for issue in issues))

    def test_analyse_aliases_enum_and_flags_values(self):
        """
        AnalyseBits.ALIASES:
        Verifies that the enum and flags values are compared by their integer values: an enum value name and its alias,
        and a flag bit name and its alias, are equal, while different enum values mismatch.
        """
        def make_profile(vulkan12_independence: str, khr_independence: str) -> dict:
            return {"test_profile.json": {
                "$schema": "https://schema.khronos.org/vulkan/profiles-0.8.3-304.json#",
                "profiles": {
                    "VP_TEST_profile": {"version": 1, "api-version": "1.2.0", "capabilities": ["baseline"]}
                },
                "capabilities": {
                    "baseline": {
                        "properties": {
                            "VkPhysicalDeviceVulkan12Properties": {"denormBehaviorIndependence": vulkan12_independence},
                            "VkPhysicalDeviceFloatControlsPropertiesKHR": {"denormBehaviorIndependence": khr_independence},
                            "VkPhysicalDeviceVulkan11Properties": {"subgroupSupportedStages": ["VK_SHADER_STAGE_COMPUTE_BIT", "VK_SHADER_STAGE_RAYGEN_BIT_KHR"]},
                            "VkPhysicalDeviceSubgroupProperties": {"supportedStages": ["VK_SHADER_STAGE_RAYGEN_BIT_NV", "VK_SHADER_STAGE_COMPUTE_BIT"]}
                        }
                    }
                }
            }}

        issues = self.validator.validate_data(make_profile("VK_SHADER_FLOAT_CONTROLS_INDEPENDENCE_NONE", "VK_SHADER_FLOAT_CONTROLS_INDEPENDENCE_NONE_KHR"), [AnalyseBits.ALIASES])
        self.assertEqual(issues, [])

        issues = self.validator.validate_data(make_profile("VK_SHADER_FLOAT_CONTROLS_INDEPENDENCE_NONE", "VK_SHADER_FLOAT_CONTROLS_INDEPENDENCE_ALL_KHR"), [AnalyseBits.ALIASES])
        self.assertEqual(len(issues), 1)
        self.assertIn("'denormBehaviorIndependence'", issues[0])

    def test_analyse_unratified(self):
        """
        AnalyseBits.UNRATIFIED:
        Verifies detection when an extension declared in the profile is unratified in vk.xml.
        """
        json_text = """{
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
                        "VK_ANDROID_external_memory_android_hardware_buffer": 1
                    }
                }
            }
        }"""

        json_files_dict = {"test_profile.json": json.loads(json_text)}
        issues = self.validator.validate_data(json_files_dict, [AnalyseBits.UNRATIFIED])

        self.assertTrue(any("is not ratified for Vulkan" in issue for issue in issues))

    def test_analyse_subcore_capabilities(self):
        """
        AnalyseBits.SUBCORE_CAPABILITIES:
        Verifies detection when a capability limit (maxImageDimension1D) is set below
        the Vulkan core specification requirement (4096), and a required core feature
        (robustBufferAccess) is explicitly set to false, reporting both with INFO severity level.
        """
        json_text = """{
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
                    "features": {
                        "VkPhysicalDeviceFeatures": {
                            "robustBufferAccess": false
                        }
                    },
                    "properties": {
                        "VkPhysicalDeviceProperties": {
                            "limits": {
                                "maxImageDimension1D": 2048
                            }
                        }
                    }
                }
            }
        }"""

        json_files_dict = {"test_profile.json": json.loads(json_text)}
        issues = self.validator.validate_data(json_files_dict, [AnalyseBits.SUBCORE_CAPABILITIES])

        # 1. Verify property limit check below Vulkan core baseline requirement
        prop_issue = next((issue for issue in issues if "maxImageDimension1D" in issue), None)
        self.assertIsNotNone(prop_issue)
        self.assertIn("below Vulkan core requirement", prop_issue)
        self.assertEqual(getattr(prop_issue, "level", None), logging.INFO)

        # 2. Verify core feature requirement explicitly set to false
        feat_issue = next((issue for issue in issues if "robustBufferAccess" in issue), None)
        self.assertIsNotNone(feat_issue)
        self.assertIn("is set to false in profile", feat_issue)
        self.assertEqual(getattr(feat_issue, "level", None), logging.INFO)

    def test_analyse_depends(self):
        """
        AnalyseBits.DEPENDS:
        Verifies detection when a capability block with a 'depends' condition requires an instance extension,
        while its device extensions and the instance extensions of unconditional blocks are not reported.
        """
        json_text = """{
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8.3-106.json#",
            "profiles": {
                "VP_TEST_profile": {
                    "version": 1,
                    "api-version": "1.1.0",
                    "capabilities": ["baseline", "conditional"]
                }
            },
            "capabilities": {
                "baseline": {
                    "extensions": {
                        "VK_KHR_surface": 1
                    }
                },
                "conditional": {
                    "depends": "VkPhysicalDeviceFeatures::depthClamp",
                    "extensions": {
                        "VK_KHR_get_surface_capabilities2": 1,
                        "VK_KHR_swapchain": 1
                    }
                }
            }
        }"""

        json_files_dict = {"test_profile.json": json.loads(json_text)}
        issues = self.validator.validate_data(json_files_dict, [AnalyseBits.DEPENDS])

        self.assertEqual(len(issues), 1)
        self.assertIn("'conditional'", issues[0])
        self.assertIn("instance extension 'VK_KHR_get_surface_capabilities2'", issues[0])
        self.assertEqual(getattr(issues[0], "level", None), logging.ERROR)

    def test_analyse_depends_contradicting_properties(self):
        """
        AnalyseBits.DEPENDS:
        Verifies the detection of the unconditional properties contradicting the value of the devices not supporting
        a feature the profile doesn't require, e.g. a maxGeometryShaderInvocations of 32 without geometryShader.
        """
        def make_profile(limits: dict, features: dict) -> dict:
            return {"test_profile.json": {
                "$schema": "https://schema.khronos.org/vulkan/profiles-0.8.3-304.json#",
                "profiles": {
                    "VP_TEST_profile": {"version": 1, "api-version": "1.4.304", "capabilities": ["baseline"]}
                },
                "capabilities": {
                    "baseline": {
                        "features": {"VkPhysicalDeviceFeatures": features},
                        "properties": {"VkPhysicalDeviceProperties": {"limits": limits}}
                    }
                }
            }}

        issues = self.validator.validate_data(make_profile({"maxGeometryShaderInvocations": 32}, {"depthClamp": True}), [AnalyseBits.DEPENDS])
        self.assertEqual(len(issues), 1)
        self.assertIn("'VkPhysicalDeviceLimits::maxGeometryShaderInvocations' (value: 32)", issues[0])
        self.assertIn("'VkPhysicalDeviceFeatures::geometryShader'", issues[0])
        self.assertEqual(getattr(issues[0], "level", None), logging.WARNING)

        # The value of the devices not supporting the feature, or a required feature, doesn't contradict anything
        self.assertEqual(self.validator.validate_data(make_profile({"maxGeometryShaderInvocations": 0}, {"depthClamp": True}), [AnalyseBits.DEPENDS]), [])
        self.assertEqual(self.validator.validate_data(make_profile({"maxGeometryShaderInvocations": 32}, {"geometryShader": True}), [AnalyseBits.DEPENDS]), [])

        # Without a Vulkan core value for the devices not supporting shaderFloat16, the value of an unsupported
        # property derived from the limittype isn't checked
        json_files_dict = make_profile({}, {"depthClamp": True})
        json_files_dict["test_profile.json"]["capabilities"]["baseline"]["properties"]["VkPhysicalDeviceFloatControlsProperties"] = {
            "shaderSignedZeroInfNanPreserveFloat16": True
        }
        self.assertEqual(self.validator.validate_data(json_files_dict, [AnalyseBits.DEPENDS]), [])


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument(
        '--registry', '-r', action='store', required=False,
        help='Use specified registry file instead of vk.xml.'
    )

    args, unparsed = parser.parse_known_args()
    TestAnalyseBits.registry_path = args.registry

    unittest.main(argv=[sys.argv[0]] + unparsed)