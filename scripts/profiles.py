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
import argparse
import sys

from source.main_convert import main_convert, ConvertBits, OutputFormatType
from source.main_schema import main_schema
from source.main_validate import main_validate
from source.main_layer import main_layer
from source.main_tests import main_tests
from source.main_merge import main_merge
from source.main_library import main_library
from source.main_doc import main_doc


def main(argv):
    logging.basicConfig(level=logging.DEBUG, format='%(levelname)s: %(message)s')
    
    parser = argparse.ArgumentParser(description='Convert Vulkan profile JSON file')
    subparsers = parser.add_subparsers(dest='command', required=True)
    
    convert_parser = subparsers.add_parser('convert', help='Convert an implicit profile to an explicit profile by pulling Vulkan capabilities dependencies from vk.xml.')
    convert_parser.add_argument('--api', action='store', default='vulkan', choices=['vulkan'], help="Target API")
    convert_parser.add_argument('--registry', '-r', action='store', help='Use a specific Vulkan registry file (vk.xml).')
    convert_parser.add_argument('--input', '-i', action='store', required=True, help='Path to the input profiles files.')
    convert_parser.add_argument('--output', '-o', action='store', required=True, help='Path to the output profiles files.')
    convert_parser.add_argument('--format', action='store', choices=list(OutputFormatType), default=OutputFormatType.FLATTEN, help='Formatting style for the profiles files (default: flatten).')
    convert_parser.add_argument('--mode', nargs='*', action='store', choices=list(ConvertBits), default=list(ConvertBits), help='List of conversion capabilities')
    convert_parser.add_argument('--validate', action='store_true', help='Validate profile files before conversion against profile schema.')

    validate_parser = subparsers.add_parser('validate', help='Validate a profile file against a profile schema.')
    validate_parser.add_argument('--api', action='store', default='vulkan', choices=['vulkan'], help="Target API")
    validate_parser.add_argument('--registry', '-r', action='store', help='Use a specific Vulkan registry file (vk.xml).')
    validate_parser.add_argument('--schema', '-s', action='store', help='Use a profile schema (profiles-*.json). By default, generate a profile schema vk.xml.')
    validate_parser.add_argument('--input', '-i', action='store', required=True, help='Path to the input profiles files.')

    schema_parser = subparsers.add_parser('schema', help='Generate a profile json schema file.')
    schema_parser.add_argument('--registry', '-r', action='store', help='Use a specific Vulkan registry file (vk.xml).')
    schema_parser.add_argument('--output', '-o', action='store', required=True, help='Path to the output profile schema file.')
    schema_parser.add_argument('--api', action='store', default='vulkan', choices=['vulkan'], help="Target API")

    solution_parser = subparsers.add_parser('merge', help='Generate merged Vulkan profile JSON files.')
    solution_parser.add_argument('--registry', '-r', action='store', required=True, help='Use specified registry file instead of vk.xml.')
    solution_parser.add_argument('--config', '-c', action='store', help='Use specified a JSON merge config file path instead of using individual arguments.')
    solution_parser.add_argument('--input', '-i', action='store', help='Path to directory with profiles.')
    solution_parser.add_argument('--input-profiles', action='store', help='Comma separated list of profiles.')
    solution_parser.add_argument('--output', '--output-path', '-o', action='store', required=True, help='Path to output profile.')
    solution_parser.add_argument('--output-profile', action='store', help='Profile name of the output profile. Deprecated, replaced by `--profile-name`.')
    solution_parser.add_argument('--profile-name', action='store', help='Profile name of the output profile. If the argument is not set, the value is generated.')
    solution_parser.add_argument('--profile-version', action='store', help='Override the Profile version of the generated profile. If the argument is not set, the value is 1.')
    solution_parser.add_argument('--profile-label', action='store', help='Override the Label of the generated profile. If the argument is not set, the value is generated.')
    solution_parser.add_argument('--profile-desc', action='store', help='Override the Description of the generated profile. If the argument is not set, the value is generated.')
    solution_parser.add_argument('--profile-date', action='store', help='Override the release date of the generated profile. If the argument is not set, the value is generated.')
    solution_parser.add_argument('--profile-api-version', action='store', help='Override the Vulkan API version of the generated profile. If the argument is not set, the value is generated.')
    solution_parser.add_argument('--profile-stage', action='store', choices=['ALPHA', 'BETA', 'STABLE'], default='STABLE', help='Override the development stage of the generated profile.')
    solution_parser.add_argument('--profile-required-profiles', action='store', help='Comma separated list of required profiles by the generated profile.')
    solution_parser.add_argument('--mode', '-m', action='store', choices=['union', 'intersection'], default='intersection', help='Mode of profile combination.')
    solution_parser.add_argument('--strip-duplicate-structs', action='store_true', help='Strip the duplicated structures in the generated profiles file.')

    library_parser = subparsers.add_parser('library', help='Generate the Vulkan profiles C/C++ API library headers and source files.')
    library_parser.add_argument('--api', action='store', default='vulkan', choices=['vulkan'], help="Target API")
    library_parser.add_argument('--registry', '-r', action='store', required=True, help='Use specified registry file instead of vk.xml.')
    library_parser.add_argument('--input', '-i', action='store', required=True, help='Path to directory with profiles.')
    library_parser.add_argument('--input-filenames', action='store', help='Comma separated list of profile filenames.')
    library_parser.add_argument('--output-inc', '--output-library-inc', action='store', help='Output include directory for profile library.')
    library_parser.add_argument('--output-src', '--output-library-src', action='store', help='Output source directory for profile library.')
    library_parser.add_argument('--output-filename', '--output-library-filename', action='store', default='vulkan_profiles', help='Output filename for profile library, default "vulkan_profiles".')
    library_parser.add_argument('--output-schema', action='store', help='Output file for JSON profile schema.')
    library_parser.add_argument('--validate', '-v', action='store_true', help='Validate generated JSON profile schema and JSON profiles against the schema.')
    library_parser.add_argument('--convert', nargs='*', action='store', choices=list(ConvertBits), help='List of conversion capabilities to apply before generating the library.')
    library_parser.add_argument('--intermediate', action='store', help='Directory path for intermediate converted profiles (used when --convert is provided).')
    library_parser.add_argument('--debug', '-d', action='store_true', help='Also generate library variant with debug messages.')
    library_parser.add_argument('--config', '-c', action='store', default='release', choices=['release', 'debug'], help='Select build configuration.')

    doc_parser = subparsers.add_parser('doc', help='Generate markdown documentation for Vulkan profiles.')
    doc_parser.add_argument('--api', action='store', default='vulkan', choices=['vulkan'], help="Target API")
    doc_parser.add_argument('--registry', '-r', action='store', required=True, help='Use specified registry file instead of vk.xml.')
    doc_parser.add_argument('--input', '-i', action='store', required=True, help='Path to directory with profiles.')
    doc_parser.add_argument('--input-filenames', action='store', help='Comma separated list of profile filenames.')
    doc_parser.add_argument('--output', '-o', '--output-doc', action='store', required=True, help='Output markdown file for profiles documentation.')

    layer_parser = subparsers.add_parser('layer', help='Generate the Vulkan profiles layer source file.')
    layer_parser.add_argument('--api', action='store', default='vulkan', choices=['vulkan'], help="Target API")
    layer_parser.add_argument('--registry', '-r', action='store', help='Use specified registry file instead of vk.xml.')
    layer_parser.add_argument('--output', '-o', '--out-layer', action='store', help='Output the layer source file.')

    tests_parser = subparsers.add_parser('tests', help='Generate test profile and test C++ source file.')
    tests_parser.add_argument('--api', action='store', default='vulkan', choices=['vulkan'], help="Target API")
    tests_parser.add_argument('--registry', '-r', action='store', required=True, help='Use specified registry file instead of vk.xml.')
    tests_parser.add_argument('--output-profile', action='store', required=True, help='Output profile test file.')
    tests_parser.add_argument('--output-cpp', '-o', '--out-tests', action='store', help='Output C++ tests file.')

    args = parser.parse_args(argv)

    if args.command == 'convert':
        main_convert(args)
    elif args.command == 'validate':
        main_validate(args)
    elif args.command == 'schema':
        main_schema(args)
    elif args.command == 'merge':
        main_merge(args)
    elif args.command == 'library':
        main_library(args)
    elif args.command == 'doc':
        main_doc(args)
    elif args.command == 'layer':
        main_layer(args)
    elif args.command == 'tests':
        main_tests(args)
    else:
        parser.print_help()


if __name__ == '__main__':
    print(sys.executable)
    
    sys.exit(main(sys.argv[1:]))
    