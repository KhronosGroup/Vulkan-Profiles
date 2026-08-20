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

import os
import sys
import tempfile
import argparse
import gen_profiles_solution
from source.vulkan_object_utils import initVulkanObject
from source.generate_profiles_schema import VulkanProfilesSchemaGenerator2
from source.main_convert import main_convert, OutputFormatType


def main_library(args):
    if not args.registry or not args.input:
        gen_profiles_solution.Log.e("Generating the profile library requires specifying --registry and --input")
        sys.exit(1)

    api = getattr(args, 'api', 'vulkan') or 'vulkan'
    registry = gen_profiles_solution.VulkanRegistry(args.registry, api)

    output_schema = getattr(args, 'output_schema', None)
    validate = getattr(args, 'validate', False)
    out_inc = getattr(args, 'output_inc', None) or getattr(args, 'output', None)
    out_src = getattr(args, 'output_src', None)

    mode = getattr(args, 'mode', ['header+source'])
    if isinstance(mode, str):
        mode = [mode]

    if 'header-only' in mode:
        out_src = None
    elif not out_src:
        out_src = out_inc

    if not out_inc and not validate and not output_schema:
        gen_profiles_solution.Log.e("At least one action (--output, --output-inc, --output-schema, or --validate) must be provided")
        sys.exit(1)

    schema = None
    if output_schema or validate:
        vk = initVulkanObject(api, args.registry or None, video=True)
        schema_gen = VulkanProfilesSchemaGenerator2(vk)
        if output_schema:
            schema_gen.generate(output_schema)
        if validate:
            schema_gen.validate()
            schema = schema_gen.schema

    if out_inc or out_src:
        input_dir = args.input

        temp_dir_obj = None
        convert_mode = getattr(args, 'convert', None)

        if convert_mode is not None:
            intermediate_arg = getattr(args, 'intermediate', None)
            if intermediate_arg:
                input_dir = intermediate_arg
                os.makedirs(input_dir, exist_ok=True)
            else:
                temp_dir_obj = tempfile.TemporaryDirectory()
                input_dir = temp_dir_obj.name

            convert_args = argparse.Namespace(
                registry=args.registry,
                input=args.input,
                output=input_dir,
                mode=convert_mode,
                format=OutputFormatType.FLATTEN,
                api=api
            )
            main_convert(convert_args)

        profiles_filenames = []
        input_filenames = getattr(args, 'input_filenames', None)
        if input_filenames:
            profiles_filenames = input_filenames.split(',')

        input_profiles_files = gen_profiles_solution.VulkanProfilesFiles(
            registry, input_dir, profiles_filenames, validate, schema
        )

        debug = getattr(args, 'debug', False) or (getattr(args, 'config', 'release').lower() == 'debug')
        output_filename = getattr(args, 'output_filename', None) or 'vulkan_profiles'

        generator = gen_profiles_solution.VulkanProfilesLibraryGenerator(
            registry, input_profiles_files, output_filename, debug
        )
        generator.generate(out_inc, out_src)

        if temp_dir_obj is not None:
            temp_dir_obj.cleanup()
