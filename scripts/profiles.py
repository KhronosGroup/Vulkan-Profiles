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
 
def main(argv):
    logging.basicConfig(level=logging.DEBUG, format='%(levelname)s: %(message)s')
    
    parser = argparse.ArgumentParser(description='Convert Vulkan profile JSON file')
    subparsers = parser.add_subparsers(dest='command', required=True)
    
    convert_parser = subparsers.add_parser('convert', help='Convert an implicit profile to an explicit profile by pulling Vulkan capabilities dependencies from vk.xml.')
    convert_parser.add_argument('--registry', '-r', action='store', help='Use a specific Vulkan registry file (vk.xml).')
    convert_parser.add_argument('--input', '-i', action='store', required=True, help='Path to the input profiles files.')
    convert_parser.add_argument('--output', '-o', action='store', required=True, help='Path to the output profiles files.')
    convert_parser.add_argument('--format', action='store', choices=list(OutputFormatType), default=OutputFormatType.FLATTEN, help='Formatting style for the profiles files (default: flatten).')
    convert_parser.add_argument('--mode', nargs='*',action='store', choices=list(ConvertBits), default=list(ConvertBits), help='List of conversion capabilities')
    
    validate_parser = subparsers.add_parser('validate', help='Validate a profile file against a profile schema.')
    validate_parser.add_argument('--registry', '-r', action='store', help='Use a specific Vulkan registry file (vk.xml).')
    validate_parser.add_argument('--schema', '-s', action='store', help='Use a profile schema (profiles-*.json). By default, generate a profile schema vk.xml.')
    validate_parser.add_argument('--input', '-i', action='store', required=True, help='Path to the input profiles files.')

    schema_parser = subparsers.add_parser('schema', help='Generate a profile json schema file.')
    schema_parser.add_argument('--registry', '-r', action='store', help='Use a specific Vulkan registry file (vk.xml).')
    schema_parser.add_argument('--output', '-o', action='store', required=True, help='Path to the output profile schema file.')
    schema_parser.add_argument('--api', action='store', default='vulkan', choices=['vulkan'], help="Target API")

    args = parser.parse_args(argv)

    if args.command == 'convert':
        main_convert(args)
    elif args.command == 'validate':
        main_validate(args)
    elif args.command == 'schema':
        main_schema(args)
    else:
        parser.print_help()


if __name__ == '__main__':
    print(sys.executable)
    
    sys.exit(main(sys.argv[1:]))

