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
import re
import sys
from pathlib import Path

from source.profiles_json_utils import load_profiles_jsons


def parse_header_version_from_schema(schema_str: str) -> str:
    """Extracts the Vulkan header revision number or tag from the $schema URI string."""
    if not schema_str:
        return "N/A"
    match = re.search(r'profiles-\d+\.\d+(?:\.\d+)?-(?:(\d+)|(latest))\.json', schema_str)
    if match:
        if match.group(1):
            return match.group(1)
        elif match.group(2):
            return "latest"
    nums = re.findall(r'\d+', schema_str)
    if nums:
        return nums[-1]
    return "N/A"


def main_summary(args):
    input_path = Path(args.input)
    raw_profiles = getattr(args, 'input_profiles', None)
    target_profiles = [p.strip() for p in raw_profiles.split(',') if p.strip()] if raw_profiles else None
    output_path = getattr(args, 'output', None)

    json_files_dict = load_profiles_jsons(input_path)
    if not json_files_dict:
        logging.error(f"No profile files loaded from {input_path}")
        sys.exit(1)

    summary_entries = []

    for file_key, file_data in json_files_dict.items():
        if not isinstance(file_data, dict):
            continue

        schema_str = file_data.get("$schema", "")
        header_ver = parse_header_version_from_schema(schema_str)
        profiles_dict = file_data.get("profiles", {})

        if not isinstance(profiles_dict, dict):
            continue

        filename = Path(file_key).name

        for p_name, p_val in profiles_dict.items():
            if target_profiles and p_name not in target_profiles:
                continue

            p_version = str(p_val.get("version", "N/A"))
            p_api_version = str(p_val.get("api-version", "N/A"))

            summary_entries.append({
                "name": p_name,
                "version": p_version,
                "api_version": p_api_version,
                "header_version": header_ver,
                "filename": filename
            })

    if not summary_entries:
        logging.info("No matching profiles found.")
        return

    summary_entries.sort(key=lambda x: x["name"])

    headers = ["Profile Name", "Profile Version", "API Version", "Header Version", "Filename"]
    col_widths = [
        max(len(h), max(len(e[k]) for e in summary_entries))
        for h, k in zip(headers, ["name", "version", "api_version", "header_version", "filename"])
    ]

    row_fmt = "  ".join(f"{{:<{w}}}" for w in col_widths)
    output_lines = [
        row_fmt.format(*headers),
        "  ".join("-" * w for w in col_widths)
    ]
    for entry in summary_entries:
        output_lines.append(row_fmt.format(
            entry["name"], 
            entry["version"], 
            entry["api_version"], 
            entry["header_version"], 
            entry["filename"]
        ))

    report = "\n".join(output_lines)

    if output_path:
        out_file = Path(output_path)
        out_file.parent.mkdir(parents=True, exist_ok=True)
        with open(out_file, "w", encoding="utf-8") as f:
            f.write(report + "\n")
        logging.info(f"Summary report written to {out_file}")
    else:
        print(report)