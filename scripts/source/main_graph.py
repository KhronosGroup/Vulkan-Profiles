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
import sys
import time
from collections import deque
from pathlib import Path

from source.graph_schema import GRAPH_JSON_SCHEMA
from source.profiles_json_utils import validate_profiles_json_data, save_profiles_jsons, OutputFormatType
from source.vulkan_object_utils import initVulkanObject
from source.generate_profiles_combine import VulkanProfilesCombineGenerator, CombineConfig


def resolve_variables(value, node_outputs: dict, global_vars: dict):
    """Recursively resolves $node_id.attr or ${var_name} references across strings, lists, and dicts."""
    if isinstance(value, str):
        def replace_match(match):
            ref = match.group(1) or match.group(2)
            parts = ref.split('.')
            if len(parts) == 2:
                node_id, attr = parts
                if node_id in node_outputs and attr in node_outputs[node_id]:
                    return str(node_outputs[node_id][attr])
            if ref in global_vars:
                return str(global_vars[ref])
            return match.group(0)

        pattern = re.compile(r'\$\{([^}]+)\}|\$([A-Za-z0-9_]+\.[A-Za-z0-9_]+)')
        return pattern.sub(replace_match, value)
    elif isinstance(value, list):
        return [resolve_variables(item, node_outputs, global_vars) for item in value]
    elif isinstance(value, dict):
        return {k: resolve_variables(v, node_outputs, global_vars) for k, v in value.items()}
    return value


def build_argv_from_args(command: str, args_dict: dict) -> list[str]:
    """Converts a dictionary of arguments into CLI argv list for argparse sub-command execution."""
    argv = [command]
    ignored_keys = {"profiles", "contributors", "history"}
    for key, val in args_dict.items():
        if val is None or val is False or key in ignored_keys:
            continue

        flag = f"--{key.replace('_', '-')}"

        if val is True:
            argv.append(flag)
        elif isinstance(val, list):
            argv.append(flag)
            for item in val:
                argv.append(str(item))
        else:
            argv.extend([flag, str(val)])

    return argv


def execute_batch_combine_node(node_id: str, resolved_args: dict, graph_file: Path):
    """Executes multi-profile combine configuration embedded directly inside a graph node."""
    api = resolved_args.get("api", "vulkan")
    registry = resolved_args["registry"]
    output_path = Path(resolved_args["output"])
    mode = resolved_args.get("mode", "intersection")
    format_type = OutputFormatType(resolved_args.get("format", OutputFormatType.PRETTY))

    vk = initVulkanObject(api, registry)

    combined_json = {
        "$schema": "https://schema.khronos.org/vulkan/profiles-0.8-latest.json#",
        "capabilities": {},
        "profiles": {},
        "contributors": resolved_args.get("contributors", {}),
        "history": resolved_args.get("history", [])
    }

    profile_configs = []
    profiles_dict = resolved_args.get("profiles", {})

    for p_name, p_val in profiles_dict.items():
        in_dir = p_val.get("input", "")
        p_config = CombineConfig(in_dir, [], p_val.get("api-version"), mode)
        p_config.apply_json_value(p_name, p_val)
        profile_configs.append(p_config)

    for cfg in profile_configs:
        combiner = VulkanProfilesCombineGenerator(vk)
        combiner.combine(cfg, combined_json, mode)

    save_profiles_jsons({output_path: combined_json}, output_path, format_type)


def main_graph(args, main_dispatcher=None):
    registry_path = getattr(args, 'registry', None)
    if not registry_path:
        logging.error("Executing a graph requires specifying --registry / -r")
        sys.exit(1)

    graph_file = Path(args.input)
    if not graph_file.exists():
        logging.error(f"Graph execution file '{graph_file}' does not exist.")
        sys.exit(1)

    try:
        with open(graph_file, "r", encoding="utf-8") as f:
            graph_data = json.load(f)
    except Exception as e:
        logging.error(f"Failed to load graph JSON file '{graph_file}': {e}")
        sys.exit(1)

    if not validate_profiles_json_data(graph_data, GRAPH_JSON_SCHEMA):
        logging.error(f"Graph execution file '{graph_file}' failed schema validation.")
        sys.exit(1)

    nodes = graph_data.get("nodes", [])
    global_vars = graph_data.get("variables", {})

    node_outputs = {}
    node_map = {}
    in_degree = {}
    adj = {}

    for idx, node in enumerate(nodes):
        node_id = node.get("id", f"node_{idx}")
        node["id"] = node_id
        node_map[node_id] = node
        in_degree[node_id] = 0
        adj[node_id] = []

    for node_id, node in node_map.items():
        deps = set(node.get("depends_on", []))
        args_str = json.dumps(node.get("args", {}))

        for other_id in node_map.keys():
            if other_id != node_id and f"${other_id}." in args_str:
                deps.add(other_id)

        for dep in deps:
            if dep in node_map:
                adj[dep].append(node_id)
                in_degree[node_id] += 1

    queue = deque([nid for nid, deg in in_degree.items() if deg == 0])
    execution_order = []

    while queue:
        curr = queue.popleft()
        execution_order.append(curr)
        for neighbor in adj[curr]:
            in_degree[neighbor] -= 1
            if in_degree[neighbor] == 0:
                queue.append(neighbor)

    total_steps = len(node_map)
    if len(execution_order) < total_steps:
        logging.error("Cyclic dependency detected in graph execution file!")
        sys.exit(1)

    graph_start_time = time.time()
    logging.info(f"Executing operation graph '{graph_file.name}' ({total_steps} steps)...")

    is_verbose = getattr(args, 'verbose', False)
    is_quiet = getattr(args, 'quiet', False)

    for step_idx, node_id in enumerate(execution_order, start=1):
        node = node_map[node_id]
        cmd = node.get("command")
        description = node.get("description", "")
        raw_args = node.get("args", {})
        resolved_args = resolve_variables(raw_args, node_outputs, global_vars)

        resolved_args["registry"] = registry_path
        if is_verbose:
            resolved_args["verbose"] = True
        elif is_quiet:
            resolved_args["quiet"] = True

        out_dest = resolved_args.get("output", "")
        out_info = f" -> '{out_dest}'" if out_dest else ""
        desc_info = f" ({description})" if description else ""

        logging.info(f"[{step_idx}/{total_steps}] Step '{node_id}': '{cmd}'{desc_info}{out_info}")
        step_start_time = time.time()

        if cmd == "combine" and "profiles" in resolved_args:
            execute_batch_combine_node(node_id, resolved_args, graph_file)
        elif main_dispatcher:
            sub_argv = build_argv_from_args(cmd, resolved_args)
            exit_code = main_dispatcher(sub_argv)
            if exit_code and exit_code != 0:
                logging.error(f"[{step_idx}/{total_steps}] Step '{node_id}' failed with exit code {exit_code}")
                sys.exit(exit_code)

        elapsed = time.time() - step_start_time
        logging.info(f"[{step_idx}/{total_steps}] Completed '{node_id}' in {elapsed:.2f}s")

        node_outputs[node_id] = {
            "output": out_dest,
            **resolved_args
        }

    total_elapsed = time.time() - graph_start_time
    logging.info(f"Graph execution '{graph_file.name}' finished successfully ({total_steps} steps) in {total_elapsed:.2f}s.")