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

import logging
import sys
from pathlib import Path

from source.vulkan_object_version import VK_VERSION, is_bundle_structure
from source.vulkan_object_utils import (
    VulkanObject,
    initVulkanObject,
    getStructCoreVersion,
    getUndeclaredStructExtension,
    gatherCapabilityAliases,
    gatherSatisfiedCoreRequiredFeaturesForVersion,
    gatherSatisfiedCoreRequiredPropertiesForVersion,
    StructCapabilityAlias,
)
from source.profiles_json_utils import (
    load_profiles_jsons,
    validate_profiles_jsons_data,
    load_schema_json,
    collect_block_names,
    collect_profile_capabilities,
    get_block_depends,
    get_profile_and_file_data,
    deep_merge_dict,
    is_property_satisfied,
)
from source.vulkan_object_expression_parsing import checkDependsExpressionSyntax, tokenizeDependsExpression
from source.transform_utils import AnalyseBits, get_parent_property_value
from source.transform_pull_required_capabilities import (
    find_contradicting_properties,
    get_profile_enabled_capabilities,
    get_unsatisfied_property_values
)
from source.generate_profiles_schema import VulkanProfilesSchemaGenerator2
from source.validate_workarounds import apply_workarounds


class ValidationIssue(str):
    """Represents a validation issue with an associated log severity level."""
    level: int

    def __new__(cls, message: str, level: int = logging.WARNING):
        instance = super().__new__(cls, message)
        instance.level = level
        return instance


class VulkanProfilesDataValidation:
    def __init__(self, vk: VulkanObject):
        self.vk = vk
        # The structures, enums and flags by name, including their aliases
        self.structs_by_name = {name: struct for struct in vk.structs.values() for name in [struct.name] + struct.aliases}
        self.enums_by_name = {name: enum for enum in vk.enums.values() for name in [enum.name] + enum.aliases}
        self.flags_by_name = {name: flags for flags in vk.flags.values() for name in [flags.name] + flags.aliases}

    def get_member_integer_value(self, struct_name: str, member_name: str, value) -> int | None:
        """
        Returns the integer value of an enum value name, or of a list of flag bit names, of a structure member whose
        type is an enum or a flags type, including their aliases. Returns None for the other types and for the names
        which aren't values of the member type.
        """
        struct = self.structs_by_name.get(struct_name)
        member = next((member for member in struct.members if member.name == member_name), None) if struct else None
        if member is None:
            return None

        enum = self.enums_by_name.get(member.type)
        if enum is not None:
            if not isinstance(value, str):
                return None
            field = next((field for field in enum.fields if value == field.name or value in field.aliases), None)
            return field.value if field is not None else None

        flags = self.flags_by_name.get(member.type)
        if flags is not None and flags.bitmaskName:
            bitmask = self.vk.bitmasks.get(flags.bitmaskName)
            if bitmask is None or not isinstance(value, list):
                return None
            integer_value = 0
            for name in value:
                flag = next((flag for flag in bitmask.flags if name == flag.name or name in flag.aliases), None)
                if flag is None:
                    return None
                integer_value |= flag.value
            return integer_value

        return None

    def validate_required_profile_api_versions(
        self,
        json_files_dict: dict,
        profile_name: str,
        profile_obj: dict
    ) -> list[ValidationIssue]:
        """Analysis Case 1: Recursively checks that all inherited required profiles target an older or equal API version."""
        issues = []
        child_api_version = VK_VERSION.from_string(profile_obj.get("api-version", "1.0.0"))

        def check_parents_recursive(req_pnames: list[str], visited: set[str]):
            for req_pname in req_pnames:
                if req_pname in visited:
                    continue
                visited.add(req_pname)

                req_profile_obj, _ = get_profile_and_file_data(json_files_dict, req_pname)
                if not req_profile_obj:
                    continue

                req_api_version = VK_VERSION.from_string(req_profile_obj.get("api-version", "1.0.0"))
                if child_api_version < req_api_version:
                    issues.append(
                        ValidationIssue(
                            f"Profile '{profile_name}' targets Vulkan {child_api_version} "
                            f"but inherits profile '{req_pname}' which targets newer Vulkan {req_api_version}",
                            level=logging.WARNING
                        )
                    )

                nested_parents = req_profile_obj.get("profiles", [])
                if nested_parents:
                    check_parents_recursive(nested_parents, visited)

        req_profiles = profile_obj.get("profiles", [])
        if req_profiles:
            check_parents_recursive(req_profiles, set())

        return issues

    def validate_core_structures_and_extensions(
        self,
        profile_name: str,
        api_version: VK_VERSION,
        category_dict: dict,
        enabled_exts: set[str]
    ) -> list[ValidationIssue]:
        """Analysis Case 2: Checks that required core structures and extension structures match declared profile API version and extensions."""
        issues = []

        for struct_name in category_dict.keys():
            core_ver = getStructCoreVersion(self.vk, struct_name)
            if core_ver != VK_VERSION.NONE and api_version < core_ver:
                ver_tuple = core_ver.as_tuple()
                issues.append(
                    ValidationIssue(
                        f"Core structure '{struct_name}' requires Vulkan {ver_tuple[0]}.{ver_tuple[1]} "
                        f"but profile '{profile_name}' targets Vulkan {api_version}",
                        level=logging.WARNING
                    )
                )

            undeclared_ext = getUndeclaredStructExtension(self.vk, struct_name, api_version, enabled_exts)
            if undeclared_ext is not None:
                issues.append(
                    ValidationIssue(
                        f"Structure '{struct_name}' in profile '{profile_name}' "
                        f"belongs to extension '{undeclared_ext}' which is not declared in extensions",
                        level=logging.ERROR
                    )
                )

        return issues

    def validate_aliased_capabilities(
        self,
        profile_name: str,
        category_dict: dict,
        checked_aliases: set[tuple[str, str]]
    ) -> list[ValidationIssue]:
        """Analysis Case 3: Checks that aliased structure members have consistent values across capability definitions."""
        issues = []

        def are_values_equal(struct1: str, member1: str, val1, struct2: str, member2: str, val2) -> bool:
            # Compare the enum and flags values by their integer values, so that an enum value name and its alias,
            # e.g. 'VK_SHADER_FLOAT_CONTROLS_INDEPENDENCE_NONE' and 'VK_SHADER_FLOAT_CONTROLS_INDEPENDENCE_NONE_KHR', are equal
            int1 = self.get_member_integer_value(struct1, member1, val1)
            int2 = self.get_member_integer_value(struct2, member2, val2)
            if int1 is not None and int2 is not None:
                return int1 == int2
            if isinstance(val1, list) and isinstance(val2, list):
                return sorted(val1) == sorted(val2)
            return val1 == val2

        for struct_name, members in category_dict.items():
            if not isinstance(members, dict):
                continue

            for member_name, val in members.items():
                pair_key = (struct_name, member_name)
                if pair_key in checked_aliases:
                    continue

                checked_aliases.add(pair_key)
                query_id = StructCapabilityAlias(struct_name, member_name)
                aliases = gatherCapabilityAliases(self.vk, query_id)

                for alias in aliases:
                    if isinstance(alias, StructCapabilityAlias):
                        checked_aliases.add((alias.struct, alias.member))
                        if alias.struct in category_dict:
                            alias_members = category_dict[alias.struct]
                            if isinstance(alias_members, dict) and alias.member in alias_members:
                                alias_val = alias_members[alias.member]
                                if not are_values_equal(struct_name, member_name, val, alias.struct, alias.member, alias_val):
                                    issues.append(
                                        ValidationIssue(
                                            f"Member '{member_name}' in structure '{struct_name}' (value: {val}) "
                                            f"has mismatching value with member '{alias.member}' in aliased structure '{alias.struct}' (value: {alias_val}) "
                                            f"in profile '{profile_name}'",
                                            level=logging.WARNING
                                        )
                                    )

        return issues

    def validate_ratified_extensions(
        self,
        profile_name: str,
        enabled_exts: set[str]
    ) -> list[ValidationIssue]:
        """Analysis Case 4: Checks that all extensions enabled in the profile are ratified for Vulkan."""
        issues = []

        for ext_name in sorted(enabled_exts):
            if ext_name in self.vk.extensions:
                ext_obj = self.vk.extensions[ext_name]
                if not ext_obj.ratified:
                    issues.append(
                        ValidationIssue(
                            f"Extension '{ext_name}' in profile '{profile_name}' is not ratified for Vulkan",
                            level=logging.WARNING
                        )
                    )

        return issues

    def validate_subcore_capabilities(
        self,
        profile_name: str,
        api_version: VK_VERSION,
        profile_caps: dict
    ) -> list[ValidationIssue]:
        """Analysis Case 5: Checks for profile capability values (features/properties) below Vulkan core specification baseline."""
        issues = []
        enabled_exts = set(profile_caps.get("extensions", {}).keys())

        profile_features = profile_caps.get("features", {})
        profile_properties = profile_caps.get("properties", {})

        profile_feature_values: dict[tuple[str, str], bool] = {}
        if isinstance(profile_features, dict):
            for s_name, members in profile_features.items():
                if isinstance(members, dict):
                    for m_name, val in members.items():
                        profile_feature_values[(s_name, m_name)] = bool(val)

        core_features_req = {}
        core_properties_req = {}

        enabled_features_set = {(s, m) for (s, m), val in profile_feature_values.items() if val}

        for ver in VK_VERSION.versions():
            if ver != VK_VERSION.NONE and api_version != VK_VERSION.NONE and ver <= api_version:
                sat_feat, _ = gatherSatisfiedCoreRequiredFeaturesForVersion(
                    self.vk, ver, api_version, enabled_exts, enabled_features_set
                )
                if sat_feat:
                    deep_merge_dict(core_features_req, sat_feat)

                sat_prop = gatherSatisfiedCoreRequiredPropertiesForVersion(
                    self.vk, ver, api_version, enabled_exts, enabled_features_set, ignore_unsupported=True
                )
                if sat_prop:
                    deep_merge_dict(core_properties_req, sat_prop)

        # 1. Inspect required core features
        for struct_name, members in core_features_req.items():
            if not isinstance(members, dict):
                continue
            for member_name, req_val in members.items():
                if not req_val:
                    continue
                query_id = StructCapabilityAlias(struct_name, member_name)
                aliases = [query_id] + gatherCapabilityAliases(self.vk, query_id)

                val_in_profile = None
                for alias in aliases:
                    if isinstance(alias, StructCapabilityAlias):
                        if (alias.struct, alias.member) in profile_feature_values:
                            val_in_profile = profile_feature_values[(alias.struct, alias.member)]
                            break

                if val_in_profile is False:
                    issues.append(
                        ValidationIssue(
                            f"Feature '{member_name}' in structure '{struct_name}' is set to false in profile '{profile_name}', "
                            f"which is below Vulkan core requirement for API version {api_version}",
                            level=logging.INFO
                        )
                    )

        # 2. Inspect required core properties/limits
        for struct_name, prop_data in core_properties_req.items():
            if not isinstance(prop_data, dict):
                continue

            if struct_name == "VkPhysicalDeviceProperties":
                for sub_group, sub_dict in prop_data.items():
                    if isinstance(sub_dict, dict):
                        for prop_name, req_val in sub_dict.items():
                            found_in_ctx, prof_val = get_parent_property_value(
                                profile_properties, struct_name, prop_name, self.vk
                            )
                            if found_in_ctx and not is_property_satisfied(prof_val, req_val, prop_name):
                                issues.append(
                                    ValidationIssue(
                                        f"Property '{prop_name}' in structure '{struct_name}' (value: {prof_val}) in profile '{profile_name}' "
                                        f"is below Vulkan core requirement (required: {req_val}) for API version {api_version}",
                                        level=logging.INFO
                                    )
                                )
            else:
                for prop_name, req_val in prop_data.items():
                    found_in_ctx, prof_val = get_parent_property_value(
                        profile_properties, struct_name, prop_name, self.vk
                    )
                    if found_in_ctx and not is_property_satisfied(prof_val, req_val, prop_name):
                        issues.append(
                            ValidationIssue(
                                f"Property '{prop_name}' in structure '{struct_name}' (value: {prof_val}) in profile '{profile_name}' "
                                f"is below Vulkan core requirement (required: {req_val}) for API version {api_version}",
                                level=logging.INFO
                            )
                        )

        return issues

    def validate_depends_symbol(self, symbol: str) -> str | None:
        """Returns an error message when a 'depends' expression symbol isn't a Vulkan version, extension or feature."""
        if symbol.startswith("VK_VERSION_"):
            return None if symbol in self.vk.versions else f"unknown Vulkan version '{symbol}'"

        if "::" in symbol:
            struct_name, member_name = symbol.split("::", 1)
            struct_obj = self.vk.structs.get(struct_name)
            if struct_obj is None:
                return f"unknown structure '{struct_name}'"
            is_feature_struct = struct_name == "VkPhysicalDeviceFeatures" or "VkPhysicalDeviceFeatures2" in struct_obj.extends
            if not is_feature_struct:
                return f"structure '{struct_name}' is not a feature structure"
            member = next((m for m in struct_obj.members if m.name == member_name), None)
            if member is None or member.type != "VkBool32":
                return f"'{member_name}' is not a feature of '{struct_name}'"
            return None

        return None if symbol in self.vk.extensions else f"unknown extension '{symbol}'"

    def validate_capability_block_depends(
        self,
        profile_name: str,
        block_name: str,
        block: dict
    ) -> list[ValidationIssue]:
        """Checks the syntax and the symbols of the 'depends' condition of a capability block."""
        issues = []
        depends = get_block_depends(block)
        if depends is None:
            return issues

        syntax_error = checkDependsExpressionSyntax(depends)
        if syntax_error:
            issues.append(ValidationIssue(
                f"Capability block '{block_name}' of profile '{profile_name}' has an invalid 'depends' condition '{depends}': {syntax_error}",
                level=logging.ERROR
            ))
            return issues

        for token in tokenizeDependsExpression(depends):
            if token in ('(', ')', '+', ','):
                continue
            symbol = token[1:].strip() if token.startswith('!') else token
            symbol_error = self.validate_depends_symbol(symbol)
            if symbol_error:
                issues.append(ValidationIssue(
                    f"Capability block '{block_name}' of profile '{profile_name}' has an invalid 'depends' condition '{depends}': {symbol_error}",
                    level=logging.ERROR
                ))

        return issues

    def validate_capability_block_depends_instance_extensions(
        self,
        profile_name: str,
        block_name: str,
        block: dict
    ) -> list[ValidationIssue]:
        """Analysis Case 6: Checks that a capability block with a 'depends' condition doesn't require instance extensions.

        The 'depends' condition is evaluated against a physical device, after the instance creation: the Profiles API
        library and the Profiles layer ignore the instance extensions of the capability blocks with a 'depends' condition.
        """
        issues = []
        if not isinstance(block, dict) or get_block_depends(block) is None:
            return issues

        for ext_name in sorted(block.get("extensions", {}).keys()):
            ext_obj = self.vk.extensions.get(ext_name)
            if ext_obj is not None and ext_obj.instance:
                issues.append(ValidationIssue(
                    f"Capability block '{block_name}' of profile '{profile_name}' has a 'depends' condition and requires "
                    f"the instance extension '{ext_name}': the instance extensions of a capability block with a 'depends' "
                    f"condition are ignored, as the condition is evaluated on a physical device",
                    level=logging.ERROR
                ))

        return issues

    def validate_contradicting_properties(
        self,
        json_files_dict: dict,
        json_file_data: dict,
        profile_name: str,
        profile_obj: dict
    ) -> list[ValidationIssue]:
        """
        Analysis Case 7: Checks that the unconditional properties of a profile don't contradict the value of the devices
        not supporting a capability the profile doesn't require, e.g. a maxTessellationGenerationLevel of 64 in a profile
        not requiring tessellationShader, which only applies to the devices supporting tessellationShader.
        """
        issues = []
        api_version = VK_VERSION.from_string(profile_obj.get("api-version", "1.0.0"))
        enabled_exts, enabled_features = get_profile_enabled_capabilities(json_files_dict, json_file_data, profile_obj)
        unsatisfied_values = get_unsatisfied_property_values(self.vk, api_version, enabled_exts, enabled_features)
        if not unsatisfied_values:
            return issues

        block_names = collect_block_names(profile_obj.get("capabilities", []))
        for contradiction in find_contradicting_properties(self.vk, api_version, json_file_data.get("capabilities", {}), block_names, unsatisfied_values):
            issues.append(ValidationIssue(
                f"Property '{contradiction['struct']}::{contradiction['member']}' (value: {contradiction['value']}) of capability block "
                f"'{contradiction['block']}' of profile '{profile_name}' only applies to the devices satisfying '{contradiction['depends']}', "
                f"which the profile doesn't require: the devices not satisfying it have the value {contradiction['unsatisfied_value']}. "
                f"'vkprofiles transform --pull requirements depends' moves it into a capability block with a 'depends' condition",
                level=logging.WARNING
            ))
        return issues

    def validate_data(self, json_files_dict: dict, analyse_bits: list[AnalyseBits] = None) -> list[ValidationIssue]:
        """Executes selected static analysis validation cases across loaded profile files."""
        if analyse_bits is None:
            analyse_bits = list(AnalyseBits)

        issues: list[ValidationIssue] = []

        for file_path, json_file_data in json_files_dict.items():
            if not isinstance(json_file_data, dict) or "profiles" not in json_file_data:
                continue

            for profile_name, profile_obj in json_file_data["profiles"].items():
                api_version = VK_VERSION.from_string(profile_obj.get("api-version", "1.0.0"))
                profile_caps = collect_profile_capabilities(json_files_dict, json_file_data, profile_obj)
                enabled_exts = set(profile_caps.get("extensions", {}).keys())

                # The 'depends' conditions of the capability blocks are always checked
                capabilities_dict = json_file_data.get("capabilities", {})
                for block_name in collect_block_names(profile_obj.get("capabilities", [])):
                    issues.extend(
                        self.validate_capability_block_depends(
                            profile_name, block_name, capabilities_dict.get(block_name)
                        )
                    )

                    # Analysis Case 6: Instance extensions of the capability blocks with a 'depends' condition
                    if AnalyseBits.DEPENDS in analyse_bits:
                        issues.extend(
                            self.validate_capability_block_depends_instance_extensions(
                                profile_name, block_name, capabilities_dict.get(block_name)
                            )
                        )

                # Analysis Case 7: Unconditional properties contradicting the value of the devices not supporting a capability
                if AnalyseBits.DEPENDS in analyse_bits:
                    issues.extend(
                        self.validate_contradicting_properties(
                            json_files_dict, json_file_data, profile_name, profile_obj
                        )
                    )

                # Analysis Case 1: Required Profile API Version Inheritance Checks
                if AnalyseBits.API_VERSION in analyse_bits:
                    issues.extend(
                        self.validate_required_profile_api_versions(
                            json_files_dict, profile_name, profile_obj
                        )
                    )

                # Analysis Case 4: Extension Ratification Checks
                if AnalyseBits.UNRATIFIED in analyse_bits:
                    issues.extend(
                        self.validate_ratified_extensions(
                            profile_name, enabled_exts
                        )
                    )

                # Analysis Case 5: Core Specification Minimum Capability Baseline Checks
                if AnalyseBits.SUBCORE_CAPABILITIES in analyse_bits:
                    issues.extend(
                        self.validate_subcore_capabilities(
                            profile_name, api_version, profile_caps
                        )
                    )

                for category in ("features", "properties"):
                    category_dict = profile_caps.get(category, {})
                    if not isinstance(category_dict, dict):
                        continue

                    # Analysis Case 2: Core Structure & Extension Declaration Checks
                    if AnalyseBits.EXTENSIONS in analyse_bits:
                        issues.extend(
                            self.validate_core_structures_and_extensions(
                                profile_name, api_version, category_dict, enabled_exts
                            )
                        )

                    # Analysis Case 3: Aliased Capability Value Mismatch Checks
                    if AnalyseBits.ALIASES in analyse_bits:
                        checked_aliases = set()
                        issues.extend(
                            self.validate_aliased_capabilities(
                                profile_name, category_dict, checked_aliases
                            )
                        )

        return issues


def main_validate(args):
    input_path = Path(args.input)
    schema_path = Path(args.schema) if getattr(args, 'schema', None) else None
    analyse_bits = getattr(args, 'analyse', None)
    workaround_bits = getattr(args, 'apply_workarounds', None) or []
    output_dir = Path(args.output) if getattr(args, 'output', None) else None

    vk = initVulkanObject(getattr(args, 'api', 'vulkan') or 'vulkan', getattr(args, 'registry', None))

    if output_dir is not None:
        # Write the input profiles files with the workarounds applied to the output directory, leaving the input
        # profiles files unchanged, and validate and analyse the written profiles files
        if output_dir.resolve() == (input_path.parent if input_path.is_file() else input_path).resolve():
            logging.error(f"The output directory '{output_dir}' is the input directory: omit --output to apply the workarounds in place")
            sys.exit(1)
        changed_files = apply_workarounds(vk, input_path, workaround_bits, output_dir)
        logging.info(f"Applied workarounds to {changed_files} file(s) written to '{output_dir}'.")
        failed = validate_and_analyse(vk, output_dir / input_path.name if input_path.is_file() else output_dir, schema_path, analyse_bits)
    else:
        failed = validate_and_analyse(vk, input_path, schema_path, analyse_bits)

        # Apply the workarounds in place to the input profiles files once their issues are reported, whether they are valid or not
        if workaround_bits:
            changed_files = apply_workarounds(vk, input_path, workaround_bits)
            logging.info(f"Applied workarounds to {changed_files} file(s).")

    if failed:
        sys.exit(1)


def validate_and_analyse(vk, input_path: Path, schema_path: Path | None, analyse_bits) -> bool:
    """Validates the profiles files against the schema and analyses the valid profiles files. Returns True on failure."""
    # 1. Systematically perform JSON schema validation first, failing the command when a profiles file is invalid
    invalid_files: list[Path] = []
    if schema_path is None:
        schema_gen = VulkanProfilesSchemaGenerator2(vk)
        schema_data = schema_gen.schema
        schema_name = "generated schema"
        count = validate_profiles_jsons_data(input_path, schema_data, logging.ERROR, invalid_files)
    else:
        schema_data = load_schema_json(schema_path)
        if schema_data is None:
            logging.error(f"Invalid profiles schema file: {schema_path}")
            sys.exit(1)
        schema_name = f"schema {schema_path}"
        count = validate_profiles_jsons_data(input_path, schema_data, logging.ERROR, invalid_files)

    logging.info(f"Validated {count} file(s) against {schema_name}.")
    if invalid_files:
        logging.error(f"Schema validation failed for {len(invalid_files)} file(s) against {schema_name}: {', '.join(str(f) for f in invalid_files)}")
        return True

    # 2. Perform static analysis if --analyse was specified
    if analyse_bits:
        json_files_dict = load_profiles_jsons(input_path)
        validator = VulkanProfilesDataValidation(vk)
        issues = validator.validate_data(json_files_dict, analyse_bits)
        if issues:
            for issue in issues:
                level = getattr(issue, "level", logging.WARNING)
                if level == logging.INFO:
                    logging.info(issue)
                elif level == logging.ERROR:
                    logging.error(issue)
                else:
                    logging.warning(issue)
            if any(getattr(issue, "level", logging.WARNING) == logging.ERROR for issue in issues):
                return True

    return False