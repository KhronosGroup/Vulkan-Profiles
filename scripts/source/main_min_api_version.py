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

from source.main_extract import extract_profile, ExtractMode
from source.profiles_json_utils import (
    load_profiles_jsons,
    save_profiles_jsons,
    validate_profiles_json_data,
    OutputFormatType
)

SCHEMA_GITHUB_RAW_URL = "https://raw.githubusercontent.com/KhronosGroup/Khronos-Schemas/main/vulkan/"
SCHEMA_GITHUB_API_URL = "https://api.github.com/repos/KhronosGroup/Khronos-Schemas/contents/vulkan"


class MinApiVersionMode(str, Enum):
    SHOW = 'display'        # Displays the profiles "api-version" and schema URI read from input JSON.
    PROCESS = 'evaluate'  # Evaluates schemas to determine min Vulkan Header version and updates JSON if output path is supplied.


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


def load_available_schemas(schemas_dir: str | Path = None) -> list[tuple[int, Path | str, dict]]:
    """
    Finds and loads profile schemas (profiles-*.json).
    Only selects the highest schema revision per Vulkan header version.
    Returns list of tuples: (header_version, schema_path_or_url, schema_data) sorted by header_version descending.
    """
    default_cache_dir = Path(tempfile.gettempdir()) / "vkprofiles_schemas"
    download_dir = Path(schemas_dir) if schemas_dir else default_cache_dir
    download_dir.mkdir(parents=True, exist_ok=True)
    manifest_file = download_dir / "manifest.json"

    # Load local manifest tracking cached file SHAs
    local_manifest = {}
    if manifest_file.exists():
        try:
            with open(manifest_file, "r", encoding="utf-8") as f:
                local_manifest = json.load(f)
        except Exception:
            local_manifest = {}

    # Query GitHub contents API to check remote file SHAs
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

    # Identify missing or updated files
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

    # Search local candidate directories for schema files
    candidate_dirs = []
    if schemas_dir:
        candidate_dirs.append(Path(schemas_dir))
    candidate_dirs.extend([
        default_cache_dir,
        Path("external/Khronos-Schemas/vulkan"),
        Path("Khronos-Schemas/vulkan"),
        Path("schemas")
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

    schemas = [(header_ver, info[1], info[2]) for header_ver, info in best_schemas_by_header.items()]
    schemas.sort(key=lambda x: x[0], reverse=True)
    return schemas


def find_min_schema_for_profile(profile_file_data: dict, schemas: list[tuple[int, Path | str, dict]], profile_name: str = "") -> tuple[int | None, dict | None]:
    """
    Validates a profile against schemas from newest to oldest.
    The first schema validation that fails indicates that the previous validated schema is the min-vulkan-api version.
    """
    last_passing_header_ver = None
    last_passing_schema = None

    for header_ver, schema_identifier, schema_data in schemas:
        schema_file_name = Path(str(schema_identifier)).name if schema_identifier else f"profiles-0.8.2-{header_ver}.json"
        prof_str = f" for profile '{profile_name}'" if profile_name else ""
        print(f"Checking schema '{schema_file_name}'{prof_str}...")

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


def main_min_api_version(args):
    input_path = Path(args.input)
    mode = getattr(args, 'mode', MinApiVersionMode.SHOW) or MinApiVersionMode.SHOW
    output_path = Path(args.output) if getattr(args, 'output', None) else None
    schemas_dir = getattr(args, 'schemas_dir', None)
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
                print(f"Profile '{pname}': api-version = {api_ver}, schema = {schema_filename}")

    elif mode == MinApiVersionMode.PROCESS:
        schemas = load_available_schemas(schemas_dir)
        if not schemas:
            logging.error("No valid Vulkan profile schemas available for validation detection.")
            return

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
                    continue

                min_header_ver, min_schema = find_min_schema_for_profile(extracted_data, schemas, profile_name=pname)
                profile_min_headers[pname] = min_header_ver
                api_ver_str = get_schema_api_version(min_schema)

                if api_ver_str:
                    print(f"Profile '{pname}': min required schema header version = {min_header_ver} (api-version = {api_ver_str})")
                    if pname in new_file_data.get("profiles", {}):
                        new_file_data["profiles"][pname]["api-version"] = api_ver_str
                else:
                    print(f"Profile '{pname}': min required schema header version = {min_header_ver}")

            if profile_min_headers:
                max_header_ver = max([h for h in profile_min_headers.values() if h is not None] or [0])
                print(f"File '{file_key}': overall file schema header version = {max_header_ver}")

                new_file_data["$schema"] = f"https://schema.khronos.org/vulkan/profiles-0.8.2-{max_header_ver}.json#"
                updated_files_dict[file_key] = new_file_data

        if output_path:
            save_profiles_jsons(updated_files_dict, output_path, format_type)
            logging.info(f"Updated profiles file(s) saved to {output_path}")