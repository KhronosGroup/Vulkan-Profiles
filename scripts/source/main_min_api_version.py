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

import json
import logging
import os
import re
import tempfile
import urllib.request
from enum import Enum
from pathlib import Path

from source.profiles_json_utils import (
    load_profiles_jsons,
    save_profiles_jsons,
    validate_profiles_json_data,
    get_profile_and_file_data,
    collect_profile_capabilities,
    OutputFormatType
)
from source.vulkan_object_version import VK_VERSION
from source.vulkan_object_expression_parsing import evalExpression
from source.vulkan_object_utils import (
    initVulkanObject,
    getStructCoreVersion,
    getStructDefiningExtensions,
    VulkanObject
)

SCHEMA_GITHUB_RAW_URL = "https://raw.githubusercontent.com/KhronosGroup/Khronos-Schemas/main/vulkan/"
SCHEMA_GITHUB_API_URL = "https://api.github.com/repos/KhronosGroup/Khronos-Schemas/contents/vulkan"


class MinApiVersionMode(str, Enum):
    SHOW = 'display'        # Displays the profiles "api-version" and schema URI read from input JSON.
    EVALUATE = 'evaluate'   # Evaluates schemas and vk.xml to determine min Vulkan version and updates JSON if output path is supplied.


def parse_schema_filename(filename: str) -> tuple[tuple[int, ...], int] | None:
    """Extracts (schema_version_tuple, header_version) from schema filename."""
    match = re.search(r"profiles-([0-9.]+)-(\d+)\.json$", filename)
    if match:
        try:
            ver_tuple = tuple(map(int, match.group(1).split('.')))
            header_ver = int(match.group(2))
            return ver_tuple, header_ver
        except ValueError:
            return None
    return None


def get_schema_api_version(schema_data: dict) -> str | None:
    """Extracts Vulkan API version string (e.g., '1.3.204') from schema title."""
    if not schema_data or not isinstance(schema_data, dict):
        return None
    title = schema_data.get("title", "")
    match = re.search(r"Vulkan\s+([0-9]+\.[0-9]+\.[0-9]+)", title)
    if match:
        return match.group(1)
    return None


def get_extension_min_core_version(vk: VulkanObject, ext_name: str, visited: set = None) -> VK_VERSION:
    """Evaluates the 'depends' attribute of an extension to determine its minimum required Vulkan core version."""
    if visited is None:
        visited = set()
    if ext_name in visited:
        return VK_VERSION.V1_0
    visited.add(ext_name)

    if not vk or not hasattr(vk, 'extensions') or ext_name not in vk.extensions:
        return VK_VERSION.V1_0

    ext_obj = vk.extensions[ext_name]
    depends_expr = getattr(ext_obj, 'depends', None)
    if not depends_expr or not depends_expr.strip():
        return VK_VERSION.V1_0

    for candidate_ver in VK_VERSION.versions():
        def is_symbol_enabled(symbol: str) -> bool:
            symbol = symbol.strip()
            if symbol.startswith("VK_VERSION_") or symbol.startswith("VK_API_VERSION_"):
                sym_ver = VK_VERSION.from_string(symbol)
                if sym_ver != VK_VERSION.NONE:
                    return candidate_ver >= sym_ver
            if hasattr(vk, 'extensions') and symbol in vk.extensions:
                req_ver = get_extension_min_core_version(vk, symbol, visited.copy())
                return candidate_ver >= req_ver
            return True

        if evalExpression(depends_expr, is_symbol_enabled):
            return candidate_ver

    return VK_VERSION.versions()[-1] if VK_VERSION.versions() else VK_VERSION.V1_4


def get_struct_min_core_version(vk: VulkanObject, struct_name: str) -> VK_VERSION:
    """Determines the minimum Vulkan core version required for a structure."""
    core_ver = getStructCoreVersion(vk, struct_name)
    if core_ver != VK_VERSION.NONE:
        return core_ver

    if not vk:
        return VK_VERSION.V1_0

    def_exts = getStructDefiningExtensions(vk, struct_name)
    if def_exts:
        min_vers = [get_extension_min_core_version(vk, ext) for ext in def_exts]
        return min(min_vers) if min_vers else VK_VERSION.V1_0

    return VK_VERSION.V1_0


def collect_profile_elements(file_data_dict: dict, file_data: dict, profile_name: str) -> tuple[set[str], set[str]]:
    """Scans capability sets for structure names (Vk*) and extension names (VK_*)."""
    struct_names = set()
    ext_names = set()

    p_obj, file_data_found = get_profile_and_file_data(file_data_dict, profile_name)
    if not p_obj:
        p_obj = file_data.get("profiles", {}).get(profile_name, {})
        file_data_found = file_data

    merged_caps = collect_profile_capabilities(file_data_dict, file_data_found, p_obj) if p_obj and file_data_found else file_data.get("capabilities", {})

    def scan_dict(d):
        if not isinstance(d, dict):
            return
        for k, v in d.items():
            if isinstance(k, str):
                if k.startswith("VK_"):
                    ext_names.add(k)
                elif k.startswith("Vk"):
                    struct_names.add(k)
            if isinstance(v, dict):
                scan_dict(v)
            elif isinstance(v, list):
                for item in v:
                    if isinstance(item, dict):
                        scan_dict(item)

    scan_dict(merged_caps)
    return struct_names, ext_names


def calculate_profile_min_core_version(vk: VulkanObject, file_data_dict: dict, file_data: dict, profile_name: str) -> VK_VERSION:
    """Returns the newest Vulkan core version required across all structures and extensions used by a profile."""
    struct_names, ext_names = collect_profile_elements(file_data_dict, file_data, profile_name)

    max_ver = VK_VERSION.V1_0

    for s_name in struct_names:
        s_ver = get_struct_min_core_version(vk, s_name)
        if s_ver > max_ver:
            max_ver = s_ver

    if vk:
        for e_name in ext_names:
            e_ver = get_extension_min_core_version(vk, e_name)
            if e_ver > max_ver:
                max_ver = e_ver

    return max_ver


def load_available_schemas(schemas_dir: str | Path = None) -> list[tuple[int, Path | str, dict]]:
    """Loads local or cached profile schema files."""
    candidate_dirs = []
    if schemas_dir:
        candidate_dirs.append(Path(schemas_dir))
    else:
        candidate_dirs.extend([
            Path("schema"),
            Path("schemas"),
            Path("external/Khronos-Schemas/vulkan"),
            Path("Khronos-Schemas/vulkan"),
            Path(tempfile.gettempdir()) / "vkprofiles_schemas"
        ])

    best_schemas_by_header: dict[int, tuple[tuple[int, ...], Path, dict]] = {}

    for s_dir in candidate_dirs:
        if s_dir.exists() and s_dir.is_dir():
            for file_path in s_dir.glob("profiles-*.json"):
                parsed = parse_schema_filename(file_path.name)
                if not parsed:
                    continue

                schema_ver_tuple, header_ver = parsed

                if header_ver not in best_schemas_by_header or schema_ver_tuple > best_schemas_by_header[header_ver][0]:
                    try:
                        with open(file_path, "r", encoding="utf-8") as f:
                            schema_data = json.load(f)
                            best_schemas_by_header[header_ver] = (schema_ver_tuple, file_path, schema_data)
                    except Exception as e:
                        logging.debug(f"Failed to load schema {file_path}: {e}")

            if best_schemas_by_header:
                break

    if best_schemas_by_header:
        schemas = [(header_ver, info[1], info[2]) for header_ver, info in best_schemas_by_header.items()]
        schemas.sort(key=lambda x: x[0], reverse=True)
        return schemas

    download_dir = Path(tempfile.gettempdir()) / "vkprofiles_schemas"
    download_dir.mkdir(parents=True, exist_ok=True)
    manifest_file = download_dir / "manifest.json"

    local_manifest = {}
    if manifest_file.exists():
        try:
            with open(manifest_file, "r", encoding="utf-8") as f:
                local_manifest = json.load(f)
        except Exception:
            local_manifest = {}

    remote_manifest = {}
    try:
        req = urllib.request.Request(SCHEMA_GITHUB_API_URL, headers={"User-Agent": "vkprofiles"})
        with urllib.request.urlopen(req, timeout=5) as resp:
            contents = json.loads(resp.read().decode("utf-8"))
            for item in contents:
                filename = item.get("name", "")
                if filename.startswith("profiles-") and filename.endswith(".json"):
                    remote_manifest[filename] = {
                        "sha": item.get("sha"),
                        "download_url": item.get("download_url") or (SCHEMA_GITHUB_RAW_URL + filename)
                    }
    except Exception as e:
        logging.debug(f"Could not query remote GitHub repository: {e}")

    if remote_manifest:
        files_to_download = []
        for fname, info in remote_manifest.items():
            local_file = download_dir / fname
            if not local_file.exists() or local_manifest.get(fname) != info["sha"]:
                files_to_download.append((fname, info))

        if files_to_download:
            logging.info(f"Downloading {len(files_to_download)} new or updated schema file(s) to '{download_dir.resolve()}'...")
            for fname, info in files_to_download:
                local_file = download_dir / fname
                try:
                    with urllib.request.urlopen(info["download_url"], timeout=10) as s_resp:
                        schema_bytes = s_resp.read()
                        with open(local_file, "wb") as f_out:
                            f_out.write(schema_bytes)
                        local_manifest[fname] = info["sha"]
                except Exception as e:
                    logging.warning(f"Failed downloading schema {fname}: {e}")

            try:
                with open(manifest_file, "w", encoding="utf-8") as f:
                    json.dump(local_manifest, f, indent=4)
            except Exception as e:
                logging.debug(f"Failed to update manifest.json: {e}")

    for file_path in download_dir.glob("profiles-*.json"):
        parsed = parse_schema_filename(file_path.name)
        if not parsed:
            continue
        schema_ver_tuple, header_ver = parsed
        if header_ver not in best_schemas_by_header or schema_ver_tuple > best_schemas_by_header[header_ver][0]:
            try:
                with open(file_path, "r", encoding="utf-8") as f:
                    schema_data = json.load(f)
                    best_schemas_by_header[header_ver] = (schema_ver_tuple, file_path, schema_data)
            except Exception as e:
                logging.debug(f"Failed to load schema {file_path}: {e}")

    schemas = [(header_ver, info[1], info[2]) for header_ver, info in best_schemas_by_header.items()]
    schemas.sort(key=lambda x: x[0], reverse=True)
    return schemas


def find_min_schema_for_profile(profile_file_data: dict, schemas: list[tuple[int, Path | str, dict]], profile_name: str = "") -> tuple[int | None, dict | None]:
    """Validates a profile against schemas from newest to oldest to identify the oldest passing schema header version."""
    last_passing_header_ver = None
    last_passing_schema = None

    for header_ver, schema_identifier, schema_data in schemas:
        schema_file_name = Path(str(schema_identifier)).name if schema_identifier else f"profiles-0.8.2-{header_ver}.json"
        prof_str = f" for profile '{profile_name}'" if profile_name else ""
        logging.info(f"Checking schema '{schema_file_name}'{prof_str}...")

        is_valid = validate_profiles_json_data(profile_file_data, schema_data)
        if is_valid:
            last_passing_header_ver = header_ver
            last_passing_schema = schema_data
        else:
            if last_passing_header_ver is not None:
                break

    if last_passing_header_ver is None and schemas:
        last_passing_header_ver = schemas[-1][0]
        last_passing_schema = schemas[-1][2]

    return last_passing_header_ver, last_passing_schema


def evaluate_min_api_version(vk_object=None, json_data: dict = None, profile_names: list[str] = None, mode: MinApiVersionMode = MinApiVersionMode.SHOW, schemas_dir: str | Path = None, registry_path: str = None) -> dict[str, str]:
    """Evaluates or displays the minimum required Vulkan API version for specified profile(s)."""
    if json_data is None:
        return {}

    if isinstance(json_data, dict) and "profiles" in json_data:
        json_files_dict = {"input.json": json_data}
    elif isinstance(json_data, dict):
        json_files_dict = json_data
    else:
        return {}

    if mode in (MinApiVersionMode.EVALUATE, 'evaluate') and vk_object is None:
        try:
            vk_object = initVulkanObject('vulkan', registry_path)
        except Exception as e:
            logging.debug(f"Could not initialize VulkanObject: {e}")
            vk_object = None

    target_profiles = set(profile_names) if profile_names else None
    results = {}

    if mode in (MinApiVersionMode.SHOW, 'display'):
        for file_key, file_data in json_files_dict.items():
            profiles = file_data.get("profiles", {})
            for pname, p_obj in profiles.items():
                if target_profiles and pname not in target_profiles:
                    continue
                results[pname] = p_obj.get("api-version", "unknown")

    elif mode in (MinApiVersionMode.EVALUATE, 'evaluate'):
        schemas = load_available_schemas(schemas_dir)
        if not schemas:
            logging.error("No valid Vulkan profile schemas available for validation detection.")
            return {}

        from source.main_extract import extract_profile, ExtractMode

        for file_key, file_data in json_files_dict.items():
            profiles = file_data.get("profiles", {})
            for pname in profiles.keys():
                if target_profiles and pname not in target_profiles:
                    continue

                extracted_data = extract_profile(json_files_dict, pname, mode=ExtractMode.PULL)
                if not extracted_data:
                    extracted_data = file_data

                min_header_ver, min_schema = find_min_schema_for_profile(extracted_data, schemas, profile_name=pname)

                min_core_ver = calculate_profile_min_core_version(vk_object, json_files_dict, extracted_data, pname)

                if min_header_ver:
                    results[pname] = f"{min_core_ver.major}.{min_core_ver.minor}.{min_header_ver}"
                else:
                    results[pname] = f"{min_core_ver.major}.{min_core_ver.minor}.0"

    return results


def main_min_api_version(args):
    input_path = Path(args.input)
    mode = getattr(args, 'mode', MinApiVersionMode.SHOW) or MinApiVersionMode.SHOW
    output_path = Path(args.output) if getattr(args, 'output', None) else None
    schemas_dir = getattr(args, 'schemas', None)
    registry_path = getattr(args, 'registry', None)
    format_type = getattr(args, 'format', OutputFormatType.PRETTY)

    target_profile_names = None
    if getattr(args, 'profile_names', None):
        target_profile_names = {p.strip() for p in args.profile_names.split(',') if p.strip()}

    json_files_dict = load_profiles_jsons(input_path)
    if not json_files_dict:
        logging.error(f"No profile files loaded from {input_path}")
        return

    if mode == MinApiVersionMode.SHOW:
        for file_key, file_data in json_files_dict.items():
            schema_url = file_data.get("$schema", "")
            schema_filename = schema_url.split('/')[-1].rstrip('#') if schema_url else "unknown"
            profiles = file_data.get("profiles", {})
            for pname, p_obj in profiles.items():
                if target_profile_names and pname not in target_profile_names:
                    continue
                api_ver = p_obj.get("api-version", "unknown")
                logging.info(f"Profile '{pname}': api-version = {api_ver}, schema = {schema_filename}")

    elif mode == MinApiVersionMode.EVALUATE:
        schemas = load_available_schemas(schemas_dir)
        if not schemas:
            logging.error("No valid Vulkan profile schemas available for validation detection.")
            return

        vk_object = None
        try:
            vk_object = initVulkanObject('vulkan', registry_path)
        except Exception as e:
            logging.debug(f"Could not initialize VulkanObject ({e}); core minor version will be deduced from bundle structure names.")

        from source.main_extract import extract_profile, ExtractMode
        updated_files_dict = {}

        for file_key, file_data in json_files_dict.items():
            profiles = file_data.get("profiles", {})
            profile_min_headers = {}
            new_file_data = json.loads(json.dumps(file_data))

            for pname in profiles.keys():
                if target_profile_names and pname not in target_profile_names:
                    continue

                extracted_data = extract_profile(json_files_dict, pname, mode=ExtractMode.PULL)
                if not extracted_data:
                    extracted_data = file_data

                min_header_ver, min_schema = find_min_schema_for_profile(file_data, schemas, profile_name=pname)
                profile_min_headers[pname] = min_header_ver

                min_core_ver = calculate_profile_min_core_version(vk_object, json_files_dict, extracted_data, pname)

                eval_api_ver_str = f"{min_core_ver.major}.{min_core_ver.minor}.{min_header_ver}"

                logging.info(f"Profile '{pname}': min required schema header version = {min_header_ver} (api-version = {eval_api_ver_str})")
                if pname in new_file_data.get("profiles", {}):
                    new_file_data["profiles"][pname]["api-version"] = eval_api_ver_str

            if profile_min_headers:
                max_header_ver = max([h for h in profile_min_headers.values() if h is not None] or [0])
                logging.info(f"File '{file_key}': overall file schema header version = {max_header_ver}")

                new_file_data["$schema"] = f"https://schema.khronos.org/vulkan/profiles-0.8.2-{max_header_ver}.json#"
                updated_files_dict[file_key] = new_file_data

        if output_path:
            save_profiles_jsons(updated_files_dict, output_path, format_type)
            logging.info(f"Updated profiles file(s) saved to {output_path}")
            