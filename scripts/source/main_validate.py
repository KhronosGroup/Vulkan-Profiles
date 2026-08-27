import sys
import json
from pathlib import Path

import gen_profiles_solution
from vulkan_object import StructCapabilityAlias, ExtensionCapabilityAlias
from source.vulkan_object_version import VK_VERSION
from source.vulkan_object_utils import (
    initVulkanObject, 
    getExtensionPromotedTo, 
    getStructDefiningExtensions,
    getStructCoreVersion,
    gatherCapabilityAliases
)
from source.profiles_parsing import collect_block_names
from source.generate_profiles_schema import VulkanProfilesSchemaGenerator2


class VulkanProfilesDataValidation:
    """
    Performs static analysis checks on Vulkan profile JSON data against the Vulkan registry.
    """
    def __init__(self, vk):
        self.vk = vk

    def validate_data(self, json_files_dict: dict) -> list[str]:
        issues = []

        for file_key, data in json_files_dict.items():
            filename = Path(file_key).name if isinstance(file_key, (str, Path)) else str(file_key)
            if not isinstance(data, dict):
                continue

            profiles = data.get('profiles', {})
            capabilities = data.get('capabilities', {})

            for profile_name, profile_obj in profiles.items():
                api_version_str = profile_obj.get('api-version', '1.0.0')
                api_version = VK_VERSION.from_string(api_version_str)

                block_names = collect_block_names(profile_obj.get('capabilities', []))
                for block_name in block_names:
                    if block_name not in capabilities:
                        continue

                    block = capabilities[block_name]
                    block_exts = set(block.get('extensions', {}).keys()) if isinstance(block.get('extensions'), dict) else set()

                    for section in ('features', 'properties'):
                        section_dict = block.get(section, {})
                        if not isinstance(section_dict, dict):
                            continue

                        # Check for capability value mismatches across aliased structures
                        visited_caps = set()
                        for struct_name, struct_members in section_dict.items():
                            if not isinstance(struct_members, dict):
                                continue
                            for member_name, member_value in struct_members.items():
                                if (struct_name, member_name) in visited_caps:
                                    continue

                                query_alias = StructCapabilityAlias(struct_name, member_name)
                                aliases = [query_alias] + gatherCapabilityAliases(self.vk, query_alias)

                                for a in aliases:
                                    if isinstance(a, StructCapabilityAlias):
                                        visited_caps.add((a.struct, a.member))

                                entries = []
                                for a in aliases:
                                    if isinstance(a, StructCapabilityAlias):
                                        if a.struct in section_dict and isinstance(section_dict[a.struct], dict):
                                            if a.member in section_dict[a.struct]:
                                                entries.append((a.struct, a.member, section_dict[a.struct][a.member]))

                                if len(entries) > 1:
                                    first_val = entries[0][2]
                                    if any(val != first_val for _, _, val in entries):
                                        details = ", ".join(f"'{s}::{m}': {v}" for s, m, v in entries)
                                        issues.append(
                                            f"Analysis Issue in '{filename}' (Profile '{profile_name}', Block '{block_name}'): "
                                            f"Capability '{member_name}' in section '{section}' has mismatching values across aliased structures ({details})."
                                        )

                        for struct_name in section_dict.keys():
                            # Issue 1: Capabilities block lists extension structure without declaring the extension
                            def_exts = getStructDefiningExtensions(self.vk, struct_name)
                            for ext_name in def_exts:
                                promoted_targets = getExtensionPromotedTo(self.vk, ext_name)
                                promoted_to_core = False
                                for target in promoted_targets:
                                    p_ver = VK_VERSION.from_string(target)
                                    if p_ver != VK_VERSION.NONE and api_version != VK_VERSION.NONE and api_version >= p_ver:
                                        promoted_to_core = True
                                        break

                                if not promoted_to_core and ext_name not in block_exts:
                                    issues.append(
                                        f"Analysis Issue in '{filename}' (Profile '{profile_name}', Block '{block_name}'): "
                                        f"Structure '{struct_name}' belongs to extension '{ext_name}', but '{ext_name}' is not declared in 'extensions'."
                                    )

                            # Issue 2: Vulkan core structure used with an older Vulkan version
                            struct_core_ver = getStructCoreVersion(self.vk, struct_name)
                            if struct_core_ver != VK_VERSION.NONE and api_version != VK_VERSION.NONE and api_version < struct_core_ver:
                                ver_tuple = struct_core_ver.as_tuple()
                                ver_str = f"{ver_tuple[0]}.{ver_tuple[1]}"
                                issues.append(
                                    f"Analysis Issue in '{filename}' (Profile '{profile_name}', Block '{block_name}'): "
                                    f"Core structure '{struct_name}' requires Vulkan {ver_str}, but profile api-version is '{api_version_str}'."
                                )

        return issues

    def validate_files(self, json_files: list[Path]) -> bool:
        json_files_dict = {}
        for f in json_files:
            try:
                with open(f, 'r', encoding='utf-8') as file_obj:
                    json_files_dict[f] = json.load(file_obj)
            except Exception as e:
                gen_profiles_solution.Log.e(f"Failed to load profile file for analysis '{f.name}': {e}")
                return False

        issues = self.validate_data(json_files_dict)
        for issue in issues:
            gen_profiles_solution.Log.e(issue)

        return len(issues) == 0


def main_validate(args):
    modes = getattr(args, 'mode', None) or ['schema']

    registry = getattr(args, 'registry', None)
    api = getattr(args, 'api', 'vulkan') or 'vulkan'

    input_path = Path(args.input)
    if input_path.is_file():
        json_files = [input_path]
    elif input_path.is_dir():
        json_files = list(input_path.glob('*.json'))
    else:
        gen_profiles_solution.Log.e(f"Input path '{args.input}' does not exist.")
        sys.exit(1)

    vk = None
    if 'schema' in modes or 'analysis' in modes:
        vk = initVulkanObject(api, registry, video=True)

    # 1. Schema Validation Mode
    if 'schema' in modes:
        try:
            import jsonschema
        except ModuleNotFoundError:
            gen_profiles_solution.Log.w("`jsonschema` module is not installed, skipping schema validation.")
        else:
            if getattr(args, 'schema', None):
                try:
                    with open(args.schema, 'r') as f:
                        schema = json.load(f)
                except json.JSONDecodeError as e:
                    gen_profiles_solution.Log.e(f"JSON Parse Error in schema file '{args.schema}':\n  {e}")
                    sys.exit(1)
                except Exception as e:
                    gen_profiles_solution.Log.e(f"Failed to load schema file '{args.schema}': {e}")
                    sys.exit(1)
            else:
                schema_gen = VulkanProfilesSchemaGenerator2(vk)
                schema = schema_gen.schema

            for json_file in json_files:
                try:
                    with open(json_file, 'r') as f:
                        data = json.load(f)
                    jsonschema.validate(instance=data, schema=schema)
                    gen_profiles_solution.Log.i(f"Schema Validation successful: '{json_file.name}'")
                except json.JSONDecodeError as e:
                    gen_profiles_solution.Log.e(f"JSON Parse Error in '{json_file.name}':\n  {e}")
                    sys.exit(1)
                except jsonschema.exceptions.ValidationError as e:
                    location = " -> ".join(str(p) for p in e.path) if e.path else "root"
                    gen_profiles_solution.Log.e(
                        f"Schema Validation Error in '{json_file.name}':\n"
                        f"  Location : {location}\n"
                        f"  Message  : {e.message}"
                    )
                    sys.exit(1)
                except jsonschema.exceptions.SchemaError as e:
                    gen_profiles_solution.Log.e(f"Schema Error:\n  {e.message}")
                    sys.exit(1)

    # 2. Static Analysis Mode
    if 'analysis' in modes:
        analyzer = VulkanProfilesDataValidation(vk)
        success = analyzer.validate_files(json_files)
        if not success:
            sys.exit(1)
            