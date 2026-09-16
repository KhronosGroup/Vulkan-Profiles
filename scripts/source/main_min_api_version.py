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
from pathlib import Path

from source.main_extract import extract_profile, ExtractMode
from source.profiles_json_utils import (
    load_profiles_jsons,
    save_profiles_jsons,
    _validate_profiles_json_data,
    OutputFormatType
)

SCHEMA_GITHUB_RAW_URL = "https://raw.githubusercontent.com/KhronosGroup/Khronos-Schemas/main/vulkan/"
SCHEMA_GITHUB_API_URL = "https://api.github.com/repos/KhronosGroup/Khronos-Schemas/contents/vulkan"


def load_available_schemas(schemas_dir: str | Path = None) -> list[tuple[int, Path | str, dict]]:
    """
    Finds and loads profile schemas (profiles-*.json), extracting header version numbers.
    Returns list of tuples: (header_version, schema_path_or_url, schema_data) sorted by header_version descending.
    """
    schemas = []

    candidate_dirs = []
    if schemas_dir:
        candidate_dirs.append(Path(schemas_dir))
    candidate_dirs.extend([
        Path("external/Khronos-Schemas/vulkan"),
        Path("Khronos-Schemas/vulkan"),
        Path("schemas")
    ])

    for s_dir in candidate_dirs:
        if s_dir.exists() and s_dir.is_dir():
            for file_path in s_dir.glob("profiles-*.json"):
                match = re.search(r"profiles-.*-(\d+)\.json$", file_path.name)
                if match:
                    header_ver = int(match.group(1))
                    try:
                        with open(file_path, "r", encoding="utf-8") as f:
                            schema_data = json.load(f)
                            schemas.append((header_ver, file_path, schema_data))
                    except Exception as e:
                        logging.debug(f"Failed to load schema {file_path}: {e}")

            if schemas:
                break

    if not schemas:
        download_dir = Path(schemas_dir) if schemas_dir else (Path(tempfile.gettempdir()) / "vkprofiles_schemas")
        download_dir.mkdir(parents=True, exist_ok=True)

        logging.info(f"Downloading Khronos Vulkan profile schemas to '{download_dir.resolve()}'...")
        try:
            req = urllib.request.Request(SCHEMA_GITHUB_API_URL, headers={"User-Agent": "vkprofiles"})
            with urllib.request.urlopen(req) as resp:
                contents = json.loads(resp.read().decode("utf-8"))

            for item in contents:
                filename = item.get("name", "")
                if filename.startswith("profiles-") and filename.endswith(".json"):
                    match = re.search(r"profiles-.*-(\d+)\.json$", filename)
                    if match:
                        header_ver = int(match.group(1))
                        download_url = item.get("download_url") or (SCHEMA_GITHUB_RAW_URL + filename)
                        local_file_path = download_dir / filename
                        try:
                            with urllib.request.urlopen(download_url) as s_resp:
                                schema_bytes = s_resp.read()
                                schema_data = json.loads(schema_bytes.decode("utf-8"))
                                with open(local_file_path, "wb") as f_out:
                                    f_out.write(schema_bytes)
                                schemas.append((header_ver, local_file_path, schema_data))
                        except Exception as e:
                            logging.debug(f"Failed downloading schema {filename}: {e}")
        except Exception as e:
            logging.warning(f"Could not fetch schemas from GitHub: {e}")

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

        is_valid = _validate_profiles_json_data(profile_file_data, schema_data)
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
    mode = getattr(args, 'mode', 'print') or 'print'
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

    if mode == 'print':
        for file_key, file_data in json_files_dict.items():
            schema_url = file_data.get("$schema", "")
            schema_filename = schema_url.split('/')[-1].rstrip('#') if schema_url else "unknown"
            profiles = file_data.get("profiles", {})
            for pname, p_obj in profiles.items():
                if target_profile_names and pname not in target_profile_names:
                    continue
                api_ver = p_obj.get("api-version", "unknown")
                print(f"Profile '{pname}': api-version = {api_ver}, schema = {schema_filename}")

    elif mode in ('detect', 'search', 'update'):
        schemas = load_available_schemas(schemas_dir)
        if not schemas:
            logging.error("No valid Vulkan profile schemas available for validation detection.")
            return

        updated_files_dict = {}

        for file_key, file_data in json_files_dict.items():
            profiles = file_data.get("profiles", {})
            profile_min_headers = {}

            for pname in profiles.keys():
                if target_profile_names and pname not in target_profile_names:
                    continue

                extracted_data = extract_profile(json_files_dict, pname, mode=ExtractMode.PULL)
                if not extracted_data:
                    continue

                min_header_ver, _ = find_min_schema_for_profile(extracted_data, schemas, profile_name=pname)
                profile_min_headers[pname] = min_header_ver

                print(f"Profile '{pname}': min required schema header version = {min_header_ver}")

            if profile_min_headers:
                max_header_ver = max([h for h in profile_min_headers.values() if h is not None] or [0])
                print(f"File '{file_key}': overall file schema header version = {max_header_ver}")

                if mode == 'update':
                    new_file_data = json.loads(json.dumps(file_data))
                    new_file_data["$schema"] = f"https://schema.khronos.org/vulkan/profiles-0.8.2-{max_header_ver}.json#"
                    updated_files_dict[file_key] = new_file_data

        if mode == 'update':
            if not output_path:
                logging.error("--output path is required when mode is 'update'")
                return
            save_profiles_jsons(updated_files_dict, output_path, format_type)
            logging.info(f"Updated profiles file(s) saved to {output_path}")
            