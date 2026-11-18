#!/usr/bin/python3
#
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

import argparse
from pathlib import Path
import sys
import tempfile
import unittest

scripts_dir = Path(__file__).resolve().parent.parent
if str(scripts_dir) not in sys.path:
    sys.path.insert(0, str(scripts_dir))

from vulkan_object import VulkanObject
from source.vulkan_object_utils import initVulkanObject
from source.vulkan_object_expression_parsing import checkDependsExpressionSyntax, tokenizeDependsExpression
from source.profiles_json_utils import (
    get_block_depends,
    get_depends_implied_capabilities,
    collect_profile_capabilities,
    get_primary_capability_block
)
from source.transform_utils import PullBits
from source.transform_consolidate import consolidate_profiles_files
from source.transform_pull_required_capabilities import (
    pull_required_capabilities_profiles_files,
    pull_extension_dependencies_profiles_files
)
from source.main_validate import VulkanProfilesDataValidation
from source.generate_profiles_doc import generate_profiles_doc
from source.generate_profiles_combine import VulkanProfilesCombineGenerator
from source.main_combine import combine_dependent_capabilities


def make_profiles_file(capabilities: dict, profile_capabilities: list, api_version: str = "1.4.304") -> dict:
    return {
        "$schema": "https://schema.khronos.org/vulkan/profiles-0.8.3-304.json#",
        "capabilities": capabilities,
        "profiles": {
            "VP_LUNARG_test_depends": {
                "version": 1,
                "api-version": api_version,
                "label": "Test depends",
                "description": "Test of the capability block 'depends' condition",
                "capabilities": profile_capabilities
            }
        }
    }


class TestCapabilityBlockDependsSyntax(unittest.TestCase):

    def test_valid_expressions(self):
        for expression in (
            "VkPhysicalDeviceVulkan11Features::protectedMemory",
            "VK_KHR_fragment_shading_rate",
            "VK_VERSION_1_3",
            "VK_KHR_maintenance5+VkPhysicalDeviceFeatures::wideLines",
            "(VK_VERSION_1_2,VK_KHR_8bit_storage)+!VkPhysicalDeviceFeatures::largePoints",
            " VK_VERSION_1_1 , VK_KHR_maintenance1 ",
        ):
            self.assertIsNone(checkDependsExpressionSyntax(expression), expression)

    def test_invalid_expressions(self):
        for expression in (
            "",
            "+VK_VERSION_1_1",
            "VK_VERSION_1_1+",
            "VK_VERSION_1_1,,VK_KHR_maintenance1",
            "(VK_VERSION_1_1",
            "VK_VERSION_1_1)",
            "VK_VERSION_1_1 VK_KHR_maintenance1",
            "!(VK_VERSION_1_1)",
            "VkPhysicalDeviceFeatures::",
        ):
            self.assertIsNotNone(checkDependsExpressionSyntax(expression), expression)

    def test_tokenize(self):
        self.assertEqual(
            tokenizeDependsExpression("(A,B)+!C::d"),
            ["(", "A", ",", "B", ")", "+", "!C::d"])

    def test_get_block_depends(self):
        self.assertIsNone(get_block_depends({"features": {}}))
        self.assertIsNone(get_block_depends({"depends": "  "}))
        self.assertIsNone(get_block_depends(None))
        self.assertEqual(get_block_depends({"depends": " VK_VERSION_1_1 "}), "VK_VERSION_1_1")

    def test_implied_capabilities(self):
        exts, feats = get_depends_implied_capabilities(
            "VK_KHR_maintenance5+VkPhysicalDeviceVulkan11Features::protectedMemory+VK_VERSION_1_3+!VK_KHR_maintenance6+(VK_KHR_a,VK_KHR_b)")
        self.assertEqual(exts, {"VK_KHR_maintenance5"})
        self.assertEqual(feats, {("VkPhysicalDeviceVulkan11Features", "protectedMemory")})

        # A top-level OR doesn't imply any capability
        exts, feats = get_depends_implied_capabilities("VK_KHR_maintenance5,VkPhysicalDeviceFeatures::wideLines")
        self.assertEqual(exts, set())
        self.assertEqual(feats, set())


class TestCapabilityBlockDependsProfiles(unittest.TestCase):
    registry_path = None

    def setUp(self):
        self.vk: VulkanObject = initVulkanObject('vulkan', self.registry_path)

    def make_protected_profile(self) -> dict:
        return make_profiles_file({
            "baseline": {
                "features": {
                    "VkPhysicalDeviceVulkan14Features": {
                        "pushDescriptor": True
                    }
                }
            },
            "protected": {
                "depends": "VkPhysicalDeviceVulkan11Features::protectedMemory",
                "features": {
                    "VkPhysicalDeviceVulkan14Features": {
                        "pipelineProtectedAccess": True
                    }
                }
            }
        }, ["protected", "baseline"])

    def test_collect_profile_capabilities_excludes_dependent_blocks(self):
        profile = self.make_protected_profile()
        json_files_dict = {"test.json": profile}
        caps = collect_profile_capabilities(json_files_dict, profile, profile["profiles"]["VP_LUNARG_test_depends"])
        self.assertEqual(caps["features"]["VkPhysicalDeviceVulkan14Features"], {"pushDescriptor": True})
        self.assertNotIn("depends", caps)

    def test_primary_capability_block_is_unconditional(self):
        profile = self.make_protected_profile()
        primary = get_primary_capability_block(profile["profiles"]["VP_LUNARG_test_depends"], profile["capabilities"])
        self.assertIs(primary, profile["capabilities"]["baseline"])

    def test_consolidate_keeps_dependent_blocks(self):
        profile = self.make_protected_profile()
        json_files_dict = {"test.json": profile}
        consolidate_profiles_files(json_files_dict)

        profile_obj = profile["profiles"]["VP_LUNARG_test_depends"]
        self.assertEqual(profile_obj["capabilities"], ["VP_LUNARG_test_depends_requirements", "protected"])
        self.assertEqual(
            profile["capabilities"]["VP_LUNARG_test_depends_requirements"],
            {"features": {"VkPhysicalDeviceVulkan14Features": {"pushDescriptor": True}}})
        self.assertEqual(profile["capabilities"]["protected"]["depends"], "VkPhysicalDeviceVulkan11Features::protectedMemory")

    def test_consolidate_keeps_required_profiles_dependent_blocks(self):
        required = self.make_protected_profile()
        required["profiles"]["VP_LUNARG_test_depends"]["api-version"] = "1.4.304"
        child = {
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8.3-304.json#",
            "capabilities": {
                "child": {"extensions": {"VK_KHR_maintenance7": 1}}
            },
            "profiles": {
                "VP_LUNARG_test_depends_child": {
                    "version": 1,
                    "api-version": "1.4.304",
                    "label": "Test depends child",
                    "description": "Test of the capability block 'depends' condition of required profiles",
                    "profiles": ["VP_LUNARG_test_depends"],
                    "capabilities": ["child"]
                }
            }
        }
        json_files_dict = {"required.json": required, "child.json": child}
        consolidate_profiles_files(json_files_dict)

        profile_obj = child["profiles"]["VP_LUNARG_test_depends_child"]
        self.assertEqual(profile_obj["capabilities"], ["VP_LUNARG_test_depends_child_requirements", "protected"])
        self.assertEqual(child["capabilities"]["protected"], required["capabilities"]["protected"])
        self.assertNotIn("pipelineProtectedAccess", child["capabilities"]["VP_LUNARG_test_depends_child_requirements"]["features"]["VkPhysicalDeviceVulkan14Features"])

    def test_pull_requirements_doesnt_leak_dependent_capabilities(self):
        # VK_KHR_8bit_storage depends on VK_KHR_storage_buffer_storage_class on Vulkan 1.0
        profile = make_profiles_file({
            "baseline": {
                "extensions": {"VK_KHR_maintenance1": 1}
            },
            "storage": {
                "depends": "VkPhysicalDeviceFeatures::shaderInt16",
                "extensions": {"VK_KHR_8bit_storage": 1}
            }
        }, ["baseline", "storage"], api_version="1.0.68")
        json_files_dict = {"test.json": profile}

        pull_extension_dependencies_profiles_files(self.vk, [PullBits.REQUIRED_CAPABILITIES, PullBits.IGNORE_EXTENSION_VERSIONS], json_files_dict)
        pull_required_capabilities_profiles_files(self.vk, [PullBits.REQUIRED_CAPABILITIES], json_files_dict)

        baseline = profile["capabilities"]["baseline"]
        storage = profile["capabilities"]["storage"]

        # The extension dependencies are pulled into the dependent block only
        self.assertEqual(storage["depends"], "VkPhysicalDeviceFeatures::shaderInt16")
        self.assertIn("VK_KHR_storage_buffer_storage_class", storage["extensions"])
        self.assertNotIn("VK_KHR_8bit_storage", baseline.get("extensions", {}))
        self.assertNotIn("VK_KHR_storage_buffer_storage_class", baseline.get("extensions", {}))

        # The features required by the dependent block extensions are not pulled into the unconditional block
        self.assertNotIn("VkPhysicalDevice8BitStorageFeatures", baseline.get("features", {}))
        self.assertNotIn("VkPhysicalDevice8BitStorageFeaturesKHR", baseline.get("features", {}))

    def pull_dependent_requirements(self, profile: dict, pull_bits: list) -> dict:
        json_files_dict = {"test.json": profile}
        pull_extension_dependencies_profiles_files(self.vk, pull_bits, json_files_dict)
        pull_required_capabilities_profiles_files(self.vk, pull_bits, json_files_dict)
        return profile

    def make_limits_profile(self, limits: dict, features: dict = None) -> dict:
        block = {"properties": {"VkPhysicalDeviceProperties": {"limits": limits}}}
        if features:
            block["features"] = {"VkPhysicalDeviceFeatures": features}
        return make_profiles_file({"baseline": block}, ["baseline"], api_version="1.4.304")

    def test_pull_extract_dependent_capabilities(self):
        profile = self.pull_dependent_requirements(self.make_limits_profile({
            "maxGeometryShaderInvocations": 32,
            "lineWidthGranularity": 0.5,
            "sparseAddressSpaceSize": 0
        }, {"shaderInt64": True}), [PullBits.REQUIRED_CAPABILITIES, PullBits.DEPENDS])
        capabilities = profile["capabilities"]
        profile_blocks = profile["profiles"]["VP_LUNARG_test_depends"]["capabilities"]
        limits = capabilities["baseline"]["properties"]["VkPhysicalDeviceProperties"]["limits"]

        # A limit contradicting the requirement of the devices not supporting a feature the profile doesn't require is
        # extracted into a block with a 'depends' condition, the unconditional limit is the value of these devices
        geometry = capabilities["VP_LUNARG_test_depends_depends_geometryShader"]
        self.assertIn("VP_LUNARG_test_depends_depends_geometryShader", profile_blocks)
        self.assertEqual(geometry["depends"], "VkPhysicalDeviceFeatures::geometryShader")
        self.assertEqual(geometry["properties"]["VkPhysicalDeviceProperties"]["limits"], {"maxGeometryShaderInvocations": 32})
        self.assertEqual(limits["maxGeometryShaderInvocations"], 0)

        wide_lines = capabilities["VP_LUNARG_test_depends_depends_wideLines"]
        self.assertEqual(wide_lines["properties"]["VkPhysicalDeviceProperties"]["limits"], {"lineWidthGranularity": 0.5})
        self.assertEqual(limits["lineWidthGranularity"], 1.0)

        # A limit set to the value of the devices not supporting the feature is unchanged
        self.assertNotIn("VP_LUNARG_test_depends_depends_sparseBinding", capabilities)
        self.assertEqual(limits["sparseAddressSpaceSize"], 0)

        # The features are unconditional requirements of the profile: they are not extracted
        self.assertTrue(capabilities["baseline"]["features"]["VkPhysicalDeviceFeatures"]["shaderInt64"])

        # The requirements the profile doesn't set are not added
        self.assertNotIn("VP_LUNARG_test_depends_depends_tessellationShader", capabilities)

    def test_pull_extract_dependent_capabilities_satisfied_condition(self):
        profile = self.pull_dependent_requirements(self.make_limits_profile(
            {"maxGeometryShaderInvocations": 32}, {"geometryShader": True}), [PullBits.REQUIRED_CAPABILITIES, PullBits.DEPENDS])

        # The limits of a feature required by the profile are unconditional
        self.assertNotIn("VP_LUNARG_test_depends_depends_geometryShader", profile["capabilities"])
        limits = profile["capabilities"]["baseline"]["properties"]["VkPhysicalDeviceProperties"]["limits"]
        self.assertEqual(limits["maxGeometryShaderInvocations"], 32)

    def test_pull_extract_dependent_capabilities_existing_block(self):
        profile = self.make_limits_profile({"lineWidthGranularity": 0.5})
        profile["capabilities"]["wide_lines"] = {
            "depends": "VkPhysicalDeviceFeatures::wideLines",
            "properties": {
                "VkPhysicalDeviceProperties": {
                    "limits": {"lineWidthRange": [1.0, 8.0]}
                }
            }
        }
        profile["profiles"]["VP_LUNARG_test_depends"]["capabilities"].append("wide_lines")
        profile = self.pull_dependent_requirements(profile, [PullBits.REQUIRED_CAPABILITIES, PullBits.DEPENDS])

        # The extracted limit is added to the block of the profile with the same condition
        self.assertNotIn("VP_LUNARG_test_depends_depends_wideLines", profile["capabilities"])
        limits = profile["capabilities"]["wide_lines"]["properties"]["VkPhysicalDeviceProperties"]["limits"]
        self.assertEqual(limits, {"lineWidthRange": [1.0, 8.0], "lineWidthGranularity": 0.5})

    def test_pull_without_extract_dependent_capabilities(self):
        profile = self.pull_dependent_requirements(self.make_limits_profile({"maxGeometryShaderInvocations": 32}), [PullBits.REQUIRED_CAPABILITIES])

        # Without PullBits.DEPENDS, the capabilities are not extracted
        self.assertFalse(any(get_block_depends(block) is not None for block in profile["capabilities"].values()))
        limits = profile["capabilities"]["baseline"]["properties"]["VkPhysicalDeviceProperties"]["limits"]
        self.assertEqual(limits["maxGeometryShaderInvocations"], 32)

    def test_combine_difference_depends(self):
        def make_file(profile_name: str, granularity: float) -> dict:
            profile = make_profiles_file({
                "baseline": {
                    "features": {"VkPhysicalDeviceFeatures": {"depthClamp": True}}
                },
                "wide_lines": {
                    "depends": "VkPhysicalDeviceFeatures::wideLines",
                    "properties": {
                        "VkPhysicalDeviceProperties": {
                            "limits": {"lineWidthRange": [1.0, 7.0], "lineWidthGranularity": granularity}
                        }
                    }
                }
            }, ["baseline", "wide_lines"])
            profile["profiles"] = {profile_name: profile["profiles"]["VP_LUNARG_test_depends"]}
            return profile

        generator = VulkanProfilesCombineGenerator(self.vk)
        jsons = [make_file("VP_LUNARG_test_a", 0.5), make_file("VP_LUNARG_test_b", 1.0)]
        common = combine_dependent_capabilities(generator, jsons, ["VP_LUNARG_test_a", "VP_LUNARG_test_b"])

        # The blocks with the same condition are compared: only their common requirements are common
        limits = common["VkPhysicalDeviceFeatures::wideLines"]["properties"]["VkPhysicalDeviceProperties"]["limits"]
        self.assertEqual(limits["lineWidthRange"], [1.0, 7.0])

        # A condition of a single profile has no common requirements
        jsons[1]["capabilities"]["wide_lines"]["depends"] = "VkPhysicalDeviceFeatures::largePoints"
        common = combine_dependent_capabilities(generator, jsons, ["VP_LUNARG_test_a", "VP_LUNARG_test_b"])
        self.assertEqual(common, {})

    def test_validate_depends(self):
        validator = VulkanProfilesDataValidation(self.vk)

        profile = self.make_protected_profile()
        issues = validator.validate_data({"test.json": profile}, [])
        self.assertEqual(issues, [])

        for depends, error in (
            ("VkPhysicalDeviceVulkan11Features::unknownFeature", "is not a feature of"),
            ("VkPhysicalDeviceVulkan11Properties::maxMultiviewViewCount", "is not a feature structure"),
            ("VkUnknownStructure::member", "unknown structure"),
            ("VK_KHR_unknown_extension", "unknown extension"),
            ("VK_VERSION_9_9", "unknown Vulkan version"),
            ("VK_VERSION_1_1+", "missing operand"),
        ):
            invalid_profile = self.make_protected_profile()
            invalid_profile["capabilities"]["protected"]["depends"] = depends
            issues = validator.validate_data({"test.json": invalid_profile}, [])
            self.assertEqual(len(issues), 1, depends)
            self.assertIn(error, issues[0], depends)

    def test_doc_depends(self):
        profile = self.make_protected_profile()
        profile["capabilities"]["protected"]["depends"] = "(VK_KHR_maintenance5,VK_VERSION_1_4)+!VkPhysicalDeviceFeatures::wideLines"

        with tempfile.TemporaryDirectory() as temp_dir:
            output = Path(temp_dir) / "PROFILES.md"
            generate_profiles_doc(self.vk, [profile], None, str(output))
            doc = output.read_text()

        # The capability block with a 'depends' condition is linked from the profiles list
        self.assertIn("| Conditional capability blocks | [protected](#protected) |", doc)

        # The capability block is listed at the end of the document with its condition and the comparison tables of its requirements
        other_sections, conditional_section = doc.split("## Vulkan Profiles Conditional Requirements", 1)
        self.assertNotIn("\n## ", conditional_section)
        self.assertIn("\n### protected\n", conditional_section)
        self.assertIn("* Profiles: VP_LUNARG_test_depends", conditional_section)
        self.assertIn("* Condition: (`VK_KHR_maintenance5` **or** `VK_VERSION_1_4`) **and** **not** `VkPhysicalDeviceFeatures::wideLines`", conditional_section)
        self.assertIn("#### Vulkan Profile Features", conditional_section)
        self.assertIn("| Capability block | protected |", conditional_section)
        self.assertIn("[pipelineProtectedAccess]", conditional_section)

        # Its requirements are not part of the requirements of every device supporting the profile
        self.assertNotIn("[pipelineProtectedAccess]", other_sections)
        self.assertIn("pushDescriptor", other_sections)

    def test_doc_without_depends(self):
        profile = make_profiles_file({
            "baseline": {
                "features": {
                    "VkPhysicalDeviceVulkan14Features": {
                        "pushDescriptor": True
                    }
                }
            }
        }, ["baseline"])

        with tempfile.TemporaryDirectory() as temp_dir:
            output = Path(temp_dir) / "PROFILES.md"
            generate_profiles_doc(self.vk, [profile], None, str(output))
            doc = output.read_text()

        # Without a capability block with a 'depends' condition, the documentation is unchanged
        self.assertNotIn("Conditional", doc)


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument(
        '--registry', '-r', action='store', required=False,
        help='Use specified registry file instead of vk.xml.'
    )

    args, unparsed = parser.parse_known_args()
    TestCapabilityBlockDependsProfiles.registry_path = args.registry

    unittest.main(argv=[sys.argv[0]] + unparsed)
