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
import tempfile
import unittest

scripts_dir = Path(__file__).resolve().parent.parent
if str(scripts_dir) not in sys.path:
    sys.path.insert(0, str(scripts_dir))

from source.vulkan_object_utils import initVulkanObject
from source.transform_utils import WorkaroundBits
from source.validate_workarounds import apply_driver_id_workaround, apply_64_bit_strings_workaround, apply_flags_as_numbers_workaround, apply_enums_as_numbers_workaround, apply_undeclared_extension_structures_workaround, apply_mismatching_aliases_workaround, apply_unsupported_feature_properties_workaround, apply_workarounds


def make_report(vulkan12_driver_id, khr_driver_id, driver_name="KosmicKrisp") -> dict:
    return {
        "$schema": "https://schema.khronos.org/vulkan/profiles-0.8-latest.json#",
        "profiles": {
            "VP_GPUINFO_test": {"version": 1, "api-version": "1.3.0", "capabilities": ["device"]}
        },
        "capabilities": {
            "device": {
                "properties": {
                    "VkPhysicalDeviceVulkan12Properties": {"driverID": vulkan12_driver_id, "driverName": driver_name},
                    "VkPhysicalDeviceDriverPropertiesKHR": {"driverID": khr_driver_id, "driverName": driver_name}
                }
            }
        }
    }


class TestValidateWorkarounds(unittest.TestCase):
    registry_path = None

    @classmethod
    def setUpClass(cls):
        cls.vk = initVulkanObject('vulkan', cls.registry_path)

    def test_driver_id_workaround(self):
        """The null and '_KHR' driverID of a KosmicKrisp device report are set to VK_DRIVER_ID_MESA_KOSMICKRISP."""
        report = make_report(None, "_KHR")
        changes = apply_driver_id_workaround(self.vk, report)

        self.assertEqual(len(changes), 2)
        properties = report["capabilities"]["device"]["properties"]
        self.assertEqual(properties["VkPhysicalDeviceVulkan12Properties"]["driverID"], "VK_DRIVER_ID_MESA_KOSMICKRISP")
        self.assertEqual(properties["VkPhysicalDeviceDriverPropertiesKHR"]["driverID"], "VK_DRIVER_ID_MESA_KOSMICKRISP")

    def test_driver_id_workaround_unchanged(self):
        """A correct driverID and the drivers without a workaround are left unchanged."""
        report = make_report("VK_DRIVER_ID_MESA_KOSMICKRISP", "VK_DRIVER_ID_MESA_KOSMICKRISP")
        self.assertEqual(apply_driver_id_workaround(self.vk, report), [])

        report = make_report("VK_DRIVER_ID_MOLTENVK", "VK_DRIVER_ID_MOLTENVK", driver_name="MoltenVK")
        self.assertEqual(apply_driver_id_workaround(self.vk, report), [])

    def test_driver_id_workaround_khr_suffix(self):
        """The '_KHR' suffix of a driver ID without a '_KHR' alias is removed, and the existing '_KHR' aliases and the unknown driver IDs are left unchanged."""
        report = make_report("VK_DRIVER_ID_MOLTENVK", "VK_DRIVER_ID_MOLTENVK_KHR", driver_name="MoltenVK")
        changes = apply_driver_id_workaround(self.vk, report)

        self.assertEqual(len(changes), 1)
        self.assertEqual(report["capabilities"]["device"]["properties"]["VkPhysicalDeviceDriverPropertiesKHR"]["driverID"], "VK_DRIVER_ID_MOLTENVK")

        report = make_report("VK_DRIVER_ID_MESA_RADV", "VK_DRIVER_ID_MESA_RADV_KHR", driver_name="radv")
        self.assertEqual(apply_driver_id_workaround(self.vk, report), [])

        report = make_report("VK_DRIVER_ID_UNKNOWN_KHR", "VK_DRIVER_ID_UNKNOWN_KHR", driver_name="Unknown")
        with self.assertLogs(level="WARNING"):
            self.assertEqual(apply_driver_id_workaround(self.vk, report), [])

    def test_64_bit_strings_workaround(self):
        """The 64-bit integer values written as strings are converted to numbers, including in nested structures, and the other members are left unchanged."""
        report = {
            "capabilities": {
                "device": {
                    "properties": {
                        "VkPhysicalDeviceExternalMemoryHostPropertiesEXT": {"minImportedHostPointerAlignment": "4096"},
                        "VkPhysicalDeviceDrmPropertiesEXT": {"primaryMajor": "226", "primaryMinor": 0, "hasPrimary": True},
                        "VkPhysicalDeviceProperties": {"deviceName": "123", "limits": {"bufferImageGranularity": "1024", "maxImageDimension1D": 16384}}
                    }
                }
            }
        }
        changes = apply_64_bit_strings_workaround(self.vk, report)

        self.assertEqual(len(changes), 3)
        properties = report["capabilities"]["device"]["properties"]
        self.assertEqual(properties["VkPhysicalDeviceExternalMemoryHostPropertiesEXT"]["minImportedHostPointerAlignment"], 4096)
        self.assertEqual(properties["VkPhysicalDeviceDrmPropertiesEXT"]["primaryMajor"], 226)
        self.assertEqual(properties["VkPhysicalDeviceProperties"]["limits"]["bufferImageGranularity"], 1024)
        self.assertEqual(properties["VkPhysicalDeviceProperties"]["deviceName"], "123")

        self.assertEqual(apply_64_bit_strings_workaround(self.vk, report), [])

    def test_flags_as_numbers_workaround(self):
        """The flags values written as numbers or numeric strings are converted to lists of flag bit names, and the unknown bits and the other members are left unchanged."""
        report = {
            "capabilities": {
                "device": {
                    "properties": {
                        "VkPhysicalDeviceExtendedSparseAddressSpacePropertiesNV": {
                            "extendedSparseAddressSpaceSize": 139637976727552,
                            "extendedSparseBufferUsageFlags": "0",
                            "extendedSparseImageUsageFlags": 6
                        },
                        "VkPhysicalDeviceSubgroupProperties": {"subgroupSize": 32, "supportedStages": ["VK_SHADER_STAGE_COMPUTE_BIT"]},
                        "VkPhysicalDeviceProperties": {"limits": {"framebufferColorSampleCounts": "0x80000000"}}
                    }
                }
            }
        }
        changes = apply_flags_as_numbers_workaround(self.vk, report)

        self.assertEqual(len(changes), 2)
        properties = report["capabilities"]["device"]["properties"]
        sparse = properties["VkPhysicalDeviceExtendedSparseAddressSpacePropertiesNV"]
        self.assertEqual(sparse["extendedSparseBufferUsageFlags"], [])
        self.assertEqual(sparse["extendedSparseImageUsageFlags"], ["VK_IMAGE_USAGE_TRANSFER_DST_BIT", "VK_IMAGE_USAGE_SAMPLED_BIT"])
        self.assertEqual(sparse["extendedSparseAddressSpaceSize"], 139637976727552)
        self.assertEqual(properties["VkPhysicalDeviceSubgroupProperties"]["supportedStages"], ["VK_SHADER_STAGE_COMPUTE_BIT"])

        report["capabilities"]["device"]["properties"]["VkPhysicalDeviceSubgroupProperties"]["supportedStages"] = "2147483648"
        with self.assertLogs(level="WARNING"):
            self.assertEqual(apply_flags_as_numbers_workaround(self.vk, report), [])

    def test_enums_as_numbers_workaround(self):
        """The enum values written as numbers or numeric strings are replaced by their names, and the names and the unknown values are left unchanged."""
        report = {
            "capabilities": {
                "device": {
                    "properties": {
                        "VkPhysicalDeviceHostImageCopyPropertiesEXT": {
                            "copySrcLayoutCount": 3,
                            "pCopySrcLayouts": ["VK_IMAGE_LAYOUT_GENERAL", 1000117000, "1000314000"]
                        },
                        "VkPhysicalDeviceVulkan12Properties": {"driverID": 14}
                    }
                }
            }
        }
        changes = apply_enums_as_numbers_workaround(self.vk, report)

        self.assertEqual(len(changes), 2)
        properties = report["capabilities"]["device"]["properties"]
        self.assertEqual(properties["VkPhysicalDeviceHostImageCopyPropertiesEXT"]["pCopySrcLayouts"],
                         ["VK_IMAGE_LAYOUT_GENERAL", "VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_STENCIL_ATTACHMENT_OPTIMAL", "VK_IMAGE_LAYOUT_READ_ONLY_OPTIMAL"])
        self.assertEqual(properties["VkPhysicalDeviceVulkan12Properties"]["driverID"], "VK_DRIVER_ID_MOLTENVK")
        self.assertEqual(properties["VkPhysicalDeviceHostImageCopyPropertiesEXT"]["copySrcLayoutCount"], 3)

        properties["VkPhysicalDeviceHostImageCopyPropertiesEXT"]["pCopySrcLayouts"] = [999]
        with self.assertLogs(level="WARNING"):
            self.assertEqual(apply_enums_as_numbers_workaround(self.vk, report), [])

    def test_undeclared_extension_structures_workaround(self):
        """The extension structures whose extension the block doesn't declare are removed, and the declared, promoted and core structures are kept."""
        report = {
            "profiles": {
                "VP_GPUINFO_test": {"version": 1, "api-version": "1.3.0", "capabilities": ["device"]}
            },
            "capabilities": {
                "device": {
                    "extensions": {"VK_KHR_maintenance5": 1},
                    "features": {
                        "VkPhysicalDeviceMaintenance7FeaturesKHR": {"maintenance7": True},
                        "VkPhysicalDeviceMaintenance5FeaturesKHR": {"maintenance5": True}
                    },
                    "properties": {
                        "VkPhysicalDeviceMaintenance7PropertiesKHR": {"robustFragmentShadingRateAttachmentAccess": True},
                        "VkPhysicalDeviceHostImageCopyPropertiesEXT": {"copySrcLayoutCount": 0},
                        "VkPhysicalDeviceMaintenance5PropertiesKHR": {"depthStencilSwizzleOneSupport": True},
                        "VkPhysicalDeviceMaintenance4PropertiesKHR": {"maxBufferSize": 1024},
                        "VkPhysicalDeviceVulkan12Properties": {"driverID": "VK_DRIVER_ID_MESA_RADV"}
                    }
                }
            }
        }
        changes = apply_undeclared_extension_structures_workaround(self.vk, report)

        self.assertEqual(len(changes), 3)
        block = report["capabilities"]["device"]
        self.assertEqual(list(block["features"].keys()), ["VkPhysicalDeviceMaintenance5FeaturesKHR"])
        self.assertEqual(list(block["properties"].keys()), ["VkPhysicalDeviceMaintenance5PropertiesKHR", "VkPhysicalDeviceMaintenance4PropertiesKHR", "VkPhysicalDeviceVulkan12Properties"])

        # VK_EXT_host_image_copy is promoted to Vulkan 1.4
        report["profiles"]["VP_GPUINFO_test"]["api-version"] = "1.4.0"
        block["properties"]["VkPhysicalDeviceHostImageCopyPropertiesEXT"] = {"copySrcLayoutCount": 0}
        self.assertEqual(apply_undeclared_extension_structures_workaround(self.vk, report), [])

    def test_mismatching_aliases_workaround(self):
        """The aliased property members with mismatching values are set to their least capable value, and the 'exact' and 'noauto' members are ignored."""
        report = {
            "capabilities": {
                "device": {
                    "properties": {
                        "VkPhysicalDeviceVulkan13Properties": {
                            "integerDotProduct8BitUnsignedAccelerated": False,
                            "integerDotProduct8BitSignedAccelerated": True,
                            "storageTexelBufferOffsetAlignmentBytes": 16
                        },
                        "VkPhysicalDeviceShaderIntegerDotProductPropertiesKHR": {
                            "integerDotProduct8BitUnsignedAccelerated": True,
                            "integerDotProduct8BitSignedAccelerated": True
                        },
                        "VkPhysicalDeviceTexelBufferAlignmentPropertiesEXT": {"storageTexelBufferOffsetAlignmentBytes": 64},
                        "VkPhysicalDeviceVulkan11Properties": {"subgroupSupportedStages": ["VK_SHADER_STAGE_COMPUTE_BIT", "VK_SHADER_STAGE_FRAGMENT_BIT"]},
                        "VkPhysicalDeviceSubgroupProperties": {"supportedStages": ["VK_SHADER_STAGE_COMPUTE_BIT", "VK_SHADER_STAGE_ALL"]},
                        "VkPhysicalDeviceVulkan12Properties": {"denormBehaviorIndependence": "VK_SHADER_FLOAT_CONTROLS_INDEPENDENCE_NONE"},
                        "VkPhysicalDeviceFloatControlsPropertiesKHR": {"denormBehaviorIndependence": "VK_SHADER_FLOAT_CONTROLS_INDEPENDENCE_ALL"}
                    }
                }
            }
        }
        changes = apply_mismatching_aliases_workaround(self.vk, report)

        self.assertEqual(len(changes), 3)
        properties = report["capabilities"]["device"]["properties"]
        # max limittype: the least capable value is false
        self.assertFalse(properties["VkPhysicalDeviceShaderIntegerDotProductPropertiesKHR"]["integerDotProduct8BitUnsignedAccelerated"])
        self.assertFalse(properties["VkPhysicalDeviceVulkan13Properties"]["integerDotProduct8BitUnsignedAccelerated"])
        self.assertTrue(properties["VkPhysicalDeviceShaderIntegerDotProductPropertiesKHR"]["integerDotProduct8BitSignedAccelerated"])
        # min limittype: the least capable value is the larger alignment
        self.assertEqual(properties["VkPhysicalDeviceVulkan13Properties"]["storageTexelBufferOffsetAlignmentBytes"], 64)
        # bitmask limittype: the bits set in both members, so the multi-bit flag VK_SHADER_STAGE_ALL, which isn't fully set
        # in the aliased member, is removed while the fragment bit it includes is kept
        self.assertEqual(properties["VkPhysicalDeviceSubgroupProperties"]["supportedStages"], ["VK_SHADER_STAGE_COMPUTE_BIT", "VK_SHADER_STAGE_FRAGMENT_BIT"])
        self.assertEqual(properties["VkPhysicalDeviceVulkan11Properties"]["subgroupSupportedStages"], ["VK_SHADER_STAGE_COMPUTE_BIT", "VK_SHADER_STAGE_FRAGMENT_BIT"])
        # The enum members without a least capable value are left unchanged
        self.assertEqual(properties["VkPhysicalDeviceFloatControlsPropertiesKHR"]["denormBehaviorIndependence"], "VK_SHADER_FLOAT_CONTROLS_INDEPENDENCE_ALL")

        self.assertEqual(apply_mismatching_aliases_workaround(self.vk, report), [])

    def test_mismatching_aliases_extra_workaround(self):
        """The aliased exact booleans with mismatching values are set to false only with the 'mismatching-aliases-extra' workaround, which leaves the limit members unchanged."""
        def make_properties() -> dict:
            return {
                "capabilities": {
                    "device": {
                        "properties": {
                            "VkPhysicalDeviceVulkan13Properties": {
                                "uniformTexelBufferOffsetSingleTexelAlignment": False,
                                "integerDotProduct8BitUnsignedAccelerated": False
                            },
                            "VkPhysicalDeviceTexelBufferAlignmentPropertiesEXT": {"uniformTexelBufferOffsetSingleTexelAlignment": True},
                            "VkPhysicalDeviceShaderIntegerDotProductPropertiesKHR": {"integerDotProduct8BitUnsignedAccelerated": True}
                        }
                    }
                }
            }

        report = make_properties()
        self.assertEqual(len(apply_mismatching_aliases_workaround(self.vk, report)), 1)
        self.assertTrue(report["capabilities"]["device"]["properties"]["VkPhysicalDeviceTexelBufferAlignmentPropertiesEXT"]["uniformTexelBufferOffsetSingleTexelAlignment"])

        report = make_properties()
        self.assertEqual(len(apply_mismatching_aliases_workaround(self.vk, report, limit_members=False, exact_booleans=True)), 1)
        properties = report["capabilities"]["device"]["properties"]
        self.assertFalse(properties["VkPhysicalDeviceTexelBufferAlignmentPropertiesEXT"]["uniformTexelBufferOffsetSingleTexelAlignment"])
        self.assertTrue(properties["VkPhysicalDeviceShaderIntegerDotProductPropertiesKHR"]["integerDotProduct8BitUnsignedAccelerated"])

    def test_unsupported_feature_properties_workaround(self):
        """The properties of an unsupported feature are set to the Vulkan core value of the devices not supporting it, and the properties without such a value, or of a supported feature, are left unchanged."""
        def make_properties(features: dict) -> dict:
            return {
                "profiles": {
                    "VP_GPUINFO_test": {"version": 1, "api-version": "1.4.304", "capabilities": ["device"]}
                },
                "capabilities": {
                    "device": {
                        "features": {"VkPhysicalDeviceFeatures": features},
                        "properties": {
                            "VkPhysicalDeviceProperties": {"limits": {"maxGeometryShaderInvocations": 32, "lineWidthGranularity": 0.5, "maxImageDimension2D": 16384}},
                            "VkPhysicalDeviceFloatControlsProperties": {"shaderSignedZeroInfNanPreserveFloat16": True}
                        }
                    }
                }
            }

        report = make_properties({"depthClamp": True})
        changes = apply_unsupported_feature_properties_workaround(self.vk, report)

        self.assertEqual(len(changes), 2)
        properties = report["capabilities"]["device"]["properties"]
        limits = properties["VkPhysicalDeviceProperties"]["limits"]
        self.assertEqual(limits["maxGeometryShaderInvocations"], 0)
        self.assertEqual(limits["lineWidthGranularity"], 1.0)
        self.assertEqual(limits["maxImageDimension2D"], 16384)
        # No Vulkan core value for the devices not supporting shaderFloat16
        self.assertTrue(properties["VkPhysicalDeviceFloatControlsProperties"]["shaderSignedZeroInfNanPreserveFloat16"])

        report = make_properties({"geometryShader": True, "wideLines": True})
        self.assertEqual(apply_unsupported_feature_properties_workaround(self.vk, report), [])

    def test_apply_workarounds_preserves_format(self):
        """Only the changed files are rewritten, preserving their CRLF line endings and the missing final line ending."""
        with tempfile.TemporaryDirectory() as temp_dir:
            changed_path = Path(temp_dir) / "changed.json"
            unchanged_path = Path(temp_dir) / "unchanged.json"
            unchanged_text = json.dumps(make_report("VK_DRIVER_ID_MOLTENVK", "VK_DRIVER_ID_MOLTENVK", "MoltenVK"), indent=2)
            with open(changed_path, "w", encoding="utf-8", newline="\r\n") as f:
                f.write(json.dumps(make_report(None, "_KHR"), indent=4))
            with open(unchanged_path, "w", encoding="utf-8", newline="") as f:
                f.write(unchanged_text)

            self.assertEqual(apply_workarounds(self.vk, Path(temp_dir), [WorkaroundBits.DRIVER_ID]), 1)

            with open(changed_path, "r", encoding="utf-8", newline="") as f:
                changed_text = f.read()
            self.assertEqual(changed_text, json.dumps(make_report("VK_DRIVER_ID_MESA_KOSMICKRISP", "VK_DRIVER_ID_MESA_KOSMICKRISP"), indent=4).replace("\n", "\r\n"))
            with open(unchanged_path, "r", encoding="utf-8", newline="") as f:
                self.assertEqual(f.read(), unchanged_text)

    def test_apply_workarounds_output(self):
        """With an output directory, the input files are left unchanged and every profiles file is written to the output directory."""
        with tempfile.TemporaryDirectory() as temp_dir:
            input_dir = Path(temp_dir) / "input"
            output_dir = Path(temp_dir) / "output"
            input_dir.mkdir()
            changed_text = json.dumps(make_report(None, "_KHR"), indent=4)
            unchanged_text = json.dumps(make_report("VK_DRIVER_ID_MOLTENVK", "VK_DRIVER_ID_MOLTENVK", "MoltenVK"), indent=2)
            (input_dir / "changed.json").write_text(changed_text, encoding="utf-8")
            (input_dir / "unchanged.json").write_text(unchanged_text, encoding="utf-8")

            self.assertEqual(apply_workarounds(self.vk, input_dir, [WorkaroundBits.DRIVER_ID], output_dir), 1)

            self.assertEqual((input_dir / "changed.json").read_text(encoding="utf-8"), changed_text)
            self.assertEqual((output_dir / "unchanged.json").read_text(encoding="utf-8"), unchanged_text)
            output_data = json.loads((output_dir / "changed.json").read_text(encoding="utf-8"))
            properties = output_data["capabilities"]["device"]["properties"]
            self.assertEqual(properties["VkPhysicalDeviceVulkan12Properties"]["driverID"], "VK_DRIVER_ID_MESA_KOSMICKRISP")
            self.assertEqual(properties["VkPhysicalDeviceDriverPropertiesKHR"]["driverID"], "VK_DRIVER_ID_MESA_KOSMICKRISP")


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument(
        '--registry', '-r', action='store', required=False,
        help='Use specified registry file instead of vk.xml.'
    )

    args, unparsed = parser.parse_known_args()
    TestValidateWorkarounds.registry_path = args.registry

    unittest.main(argv=[sys.argv[0]] + unparsed)
