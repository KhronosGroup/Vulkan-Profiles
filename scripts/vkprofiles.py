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
import time

from source.transform_utils import PullBits, StripBits
from source.main_transform import main_transform, OutputFormatType
from source.main_schema import main_schema
from source.main_validate import main_validate
from source.main_layer import main_layer
from source.main_tests import main_tests
from source.main_combine import main_combine, CombineMode
from source.main_library import main_library
from source.main_doc import main_doc
from source.main_extract import main_extract, ExtractMode
from source.main_min_api_version import main_min_api_version, MinApiVersionMode
from source.main_graph import main_graph
from source.main_version import main_version, get_version_string


class ValidateAction(argparse.Action):
    def __call__(self, parser, namespace, values, option_string=None):
        valid_modes = ['schema', 'analysis']
        if values is None or len(values) == 0:
            setattr(namespace, self.dest, valid_modes)
        else:
            res = []
            for v in values:
                for item in v.split(','):
                    item = item.strip()
                    if item:
                        if item not in valid_modes:
                            parser.error(f"argument {option_string}: invalid choice: '{item}' (choose from 'schema', 'analysis')")
                        if item not in res:
                            res.append(item)
            setattr(namespace, self.dest, res)


class LogTypeAction(argparse.Action):
    def __call__(self, parser, namespace, values, option_string=None):
        valid_types = ['none', 'all', 'info', 'warning', 'error', 'critical']
        res = []
        if values:
            for v in values:
                for item in v.split(','):
                    item = item.strip().lower()
                    if item:
                        if item not in valid_types:
                            parser.error(f"argument {option_string}: invalid choice: '{item}' (choose from {', '.join(valid_types)})")
                        if item not in res:
                            res.append(item)

        if 'none' in res and len(res) > 1:
            parser.error(f"argument {option_string}: 'none' cannot be combined with other log types")
        if 'all' in res and len(res) > 1:
            parser.error(f"argument {option_string}: 'all' cannot be combined with other log types")

        setattr(namespace, self.dest, res)


def add_transform_arguments(parser: argparse.ArgumentParser) -> None:
    """Adds standard transform options (--pull, --consolidate, --strip, --sort) to a subcommand parser."""
    parser.add_argument(
        '--pull',
        nargs='*',
        type=PullBits,
        choices=list(PullBits),
        default=[],
        metavar='OPTION',
        help='Pull capability options to apply (choices: ' + ', '.join([e.value for e in PullBits]) + ').'
    )
    parser.add_argument(
        '--consolidate',
        action='store_true',
        help='Consolidate capabilities.'
    )
    parser.add_argument(
        '--strip',
        nargs='*',
        type=StripBits,
        choices=list(StripBits),
        default=[],
        metavar='OPTION',
        help='Strip options to apply (choices: ' + ', '.join([e.value for e in StripBits]) + ').'
    )
    parser.add_argument(
        '--sort',
        action='store_true',
        help='Sort profile capabilities.'
    )


def main(argv):
    log_parser = argparse.ArgumentParser(add_help=False)
    log_group = log_parser.add_mutually_exclusive_group()
    log_group.add_argument('--quiet', action='store_true', help='Suppress warning and informational messages (ERROR level only).')
    log_group.add_argument('--verbose', action='store_true', help='Enable verbose output (INFO, WARNING, and ERROR levels).')
    log_group.add_argument('--log', nargs='*', action=LogTypeAction, default=None, help='Filter output by specific message types (choices: none, all, info, warning, error, critical). Cannot be combined with --quiet or --verbose.')

    parser = argparse.ArgumentParser(
        description='Transform Vulkan profile JSON file',
        parents=[log_parser]
    )
    
    parser.add_argument('--version', '-v', action='version', version=get_version_string())

    subparsers = parser.add_subparsers(dest='command', required=True)

    # 1. Pipeline Orchestrator (Graph)
    graph_parser = subparsers.add_parser('graph', parents=[log_parser], help='Execute a pipeline graph of vkprofiles operations from a JSON configuration file.')
    graph_parser.add_argument('--input', '-i', action='store', required=True, help='Path to graph execution JSON configuration file.')
    graph_parser.add_argument('--registry', '-r', action='store', required=True, help='Use specified Vulkan registry file (vk.xml) across all graph steps.')

    # 2. Atomic Pipeline Operations
    validate_parser = subparsers.add_parser('validate', parents=[log_parser], help='Validate a profile file against a profile schema or perform static analysis.')
    validate_parser.add_argument('--api', action='store', default='vulkan', choices=['vulkan'], help="Target API")
    validate_parser.add_argument('--registry', '-r', action='store', help='Use a specific Vulkan registry file (vk.xml).')
    validate_parser.add_argument('--schema', '-s', action='store', help='Use a profile schema (profiles-*.json). By default, generate a profile schema vk.xml.')
    validate_parser.add_argument('--input', '-i', action='store', required=True, help='Path to the input profiles files.')
    validate_parser.add_argument('--mode', '-m', nargs='*', action=ValidateAction, default=['schema', 'analysis'], help="Validation mode(s) to execute (default: schema analysis).")

    schema_parser = subparsers.add_parser('schema', parents=[log_parser], help='Generate a profile json schema file or upgrade profile JSON file(s) to schema 0.8.2.')
    schema_parser.add_argument('--registry', '-r', action='store', help='Use a specific Vulkan registry file (vk.xml).')
    schema_parser.add_argument('--input', '-i', action='store', help='Path to input profiles JSON file or directory to upgrade.')
    schema_parser.add_argument('--output', '-o', action='store', required=True, help='Path to output profile schema file or upgraded profiles file/directory.')
    schema_parser.add_argument('--upgrade', action='store_true', help='Upgrade input profile JSON file(s) from schema 0.8.0 / 0.8.1 to 0.8.2.')
    schema_parser.add_argument('--format', type=OutputFormatType, choices=list(OutputFormatType), default=OutputFormatType.PRETTY, help='Formatting style for output files.')
    schema_parser.add_argument('--api', action='store', default='vulkan', choices=['vulkan'], help="Target API")

    transform_parser = subparsers.add_parser('transform', parents=[log_parser], help='Transform an implicit profile to an explicit profile by pulling Vulkan capabilities dependencies from vk.xml.')
    transform_parser.add_argument('--api', action='store', default='vulkan', choices=['vulkan'], help="Target API")
    transform_parser.add_argument('--registry', '-r', action='store', help='Use a specific Vulkan registry file (vk.xml).')
    transform_parser.add_argument('--input', '-i', action='store', required=True, help='Path to the input profiles files.')
    transform_parser.add_argument('--output', '-o', action='store', required=True, help='Path to the output profiles files.')
    transform_parser.add_argument('--format', type=OutputFormatType, choices=list(OutputFormatType), default=OutputFormatType.PRETTY, help='Formatting style for the profiles files (default: pretty).')
    add_transform_arguments(transform_parser)
    transform_parser.add_argument('--validate', nargs='*', action=ValidateAction, default=None, help='Validate profile files before transformation (choices: schema, analysis).')

    combine_parser = subparsers.add_parser('combine', parents=[log_parser], help='Generate combined Vulkan profile JSON files.')
    combine_parser.add_argument('--api', action='store', default='vulkan', choices=['vulkan'], help="Target API")
    combine_parser.add_argument('--registry', '-r', action='store', required=True, help='Use specified registry file instead of vk.xml.')
    combine_parser.add_argument('--input', '-i', action='store', required=True, help='Path to directory with profiles.')
    combine_parser.add_argument('--input-profiles', action='store', help='Comma separated list of profiles.')
    combine_parser.add_argument('--output', '-o', action='store', required=True, help='Path to output profile.')
    combine_parser.add_argument('--output-profile', action='store', help='Profile name of the output profile. Deprecated, replaced by `--profile-name`.')
    combine_parser.add_argument('--profile-name', action='store', help='Profile name of the output profile. If the argument is not set, the value is generated.')
    combine_parser.add_argument('--profile-version', action='store', help='Override the Profile version of the generated profile. If the argument is not set, the value is 1.')
    combine_parser.add_argument('--profile-label', action='store', help='Override the Label of the generated profile. If the argument is not set, the value is generated.')
    combine_parser.add_argument('--profile-desc', action='store', help='Override the Description of the generated profile. If the argument is not set, the value is generated.')
    combine_parser.add_argument('--profile-date', action='store', help='Override the release date of the generated profile. If the argument is not set, the value is generated.')
    combine_parser.add_argument('--profile-api-version', action='store', help='Override the Vulkan API version of the generated profile. If the argument is not set, the value is generated.')
    combine_parser.add_argument('--profile-stage', action='store', choices=['ALPHA', 'BETA', 'STABLE'], default='STABLE', help='Override the development stage of the generated profile.')
    combine_parser.add_argument('--profile-required-profiles', action='store', help='Comma separated list of required profiles by the generated profile.')
    combine_parser.add_argument('--mode', '-m', action='store', choices=list(CombineMode), default=CombineMode.INTERSECTION, help='Mode of profile combination.')
    combine_parser.add_argument('--format', type=OutputFormatType, choices=list(OutputFormatType), default=OutputFormatType.PRETTY, help='Formatting style for the profiles files (default: pretty).')
    add_transform_arguments(combine_parser)
    combine_parser.add_argument('--validate', nargs='*', action=ValidateAction, default=None, help='Validate profile files before combining (choices: schema, analysis).')

    extract_parser = subparsers.add_parser('extract', parents=[log_parser], help='Extract a profile from a profile JSON file into a single profile JSON file.')
    extract_parser.add_argument('--input', '-i', action='store', required=True, help='Path to input profiles file.')
    extract_parser.add_argument('--output', '-o', action='store', required=True, help='Path to output profile JSON file.')
    extract_parser.add_argument('--profile-name', '-p', action='store', required=True, help='Specific profile name to extract.')
    extract_parser.add_argument('--mode', '-m', type=ExtractMode, choices=list(ExtractMode), default=ExtractMode.REFERENCE, help='Extraction mode: "reference-required-profiles" keeps parent profile references external, "pull-required-profiles" includes required parent profiles and blocks.')
    extract_parser.add_argument('--format', type=OutputFormatType, choices=list(OutputFormatType), default=OutputFormatType.PRETTY, help='Formatting style for the output file.')

    min_api_parser = subparsers.add_parser('min-api-version', parents=[log_parser], help='Display or process the minimum Vulkan API version of profile(s).')
    min_api_parser.add_argument('--registry', '-r', action='store', help='Use specified Vulkan registry file (vk.xml).')
    min_api_parser.add_argument('--input', '-i', action='store', required=True, help='Path to input profiles file or directory.')
    min_api_parser.add_argument('--output', '-o', action='store', help='Path to output profiles file or directory.')
    min_api_parser.add_argument('--profile-names', action='store', help='Comma separated list of profile names to process.')
    min_api_parser.add_argument('--schemas', '-s', action='store', help='Path to directory containing Vulkan profile schemas (profiles-*.json).')
    min_api_parser.add_argument('--mode', '-m', type=MinApiVersionMode, choices=list(MinApiVersionMode), default=MinApiVersionMode.SHOW, help='Operation mode: "display" (display api-version/schema read from input), "evaluate" (evaluate schemas to determine min Vulkan header version and update JSONs if output path is provided). Default: display.')
    min_api_parser.add_argument('--format', type=OutputFormatType, choices=list(OutputFormatType), default=OutputFormatType.PRETTY, help='Formatting style for output JSON files.')

    library_parser = subparsers.add_parser('library', parents=[log_parser], help='Generate the Vulkan profiles C/C++ API library headers and source files.')
    library_parser.add_argument('--api', action='store', default='vulkan', choices=['vulkan'], help="Target API")
    library_parser.add_argument('--registry', '-r', action='store', required=True, help='Use specified registry file instead of vk.xml.')
    library_parser.add_argument('--input', '-i', action='store', required=True, help='Path to directory with profiles.')
    library_parser.add_argument('--input-filenames', action='store', help='Comma separated list of profile filenames.')
    library_parser.add_argument('--output', '-o', '--output-inc', action='store', help='Output include directory for profile library.')
    library_parser.add_argument('--output-src', action='store', help='Output source directory for profile library.')
    library_parser.add_argument('--output-filename', action='store', default='vulkan_profiles', help='Output filename for profile library, default "vulkan_profiles".')
    library_parser.add_argument('--mode', nargs='*', action='store', choices=['header-only', 'header+source'], default=['header-only', 'header+source'], help='Library output generation mode.')
    library_parser.add_argument('--validate', nargs='*', action=ValidateAction, default=None, help='Validate generated JSON profile schema and JSON profiles (choices: schema, analysis).')
    add_transform_arguments(library_parser)
    library_parser.add_argument('--intermediate', action='store', help='Directory path for intermediate transformed profiles (used when transformation options are provided).')
    library_parser.add_argument('--debug', '-d', action='store_true', help='Also generate library variant with debug messages.')
    library_parser.add_argument('--config', '-c', action='store', default='release', choices=['release', 'debug'], help='Select build configuration.')
    library_parser.add_argument('--include-header', action='store', help='Override the header file include directive in generated C++ source files.')

    doc_parser = subparsers.add_parser('doc', parents=[log_parser], help='Generate markdown documentation for Vulkan profiles.')
    doc_parser.add_argument('--api', action='store', default='vulkan', choices=['vulkan'], help="Target API")
    doc_parser.add_argument('--registry', '-r', action='store', required=True, help='Use specified registry file instead of vk.xml.')
    doc_parser.add_argument('--input', '-i', action='store', required=True, help='Path to directory with profiles.')
    doc_parser.add_argument('--input-filenames', action='store', help='Comma separated list of profile filenames.')
    doc_parser.add_argument('--output', '-o', action='store', required=True, help='Output markdown file for profiles documentation.')
    doc_parser.add_argument('--validate', nargs='*', action=ValidateAction, default=None, help='Validate profile files before generating documentation (choices: schema, analysis).')

    layer_parser = subparsers.add_parser('layer', parents=[log_parser], help='Generate the Vulkan profiles layer source file.')
    layer_parser.add_argument('--api', action='store', default='vulkan', choices=['vulkan'], help="Target API")
    layer_parser.add_argument('--registry', '-r', action='store', help='Use specified registry file instead of vk.xml.')
    layer_parser.add_argument('--output', '-o', '--out-layer', action='store', help='Output the layer source file.')

    tests_parser = subparsers.add_parser('tests', parents=[log_parser], help='Generate test profile and test C++ source file.')
    tests_parser.add_argument('--api', action='store', default='vulkan', choices=['vulkan'], help="Target API")
    tests_parser.add_argument('--registry', '-r', action='store', required=True, help='Use specified registry file instead of vk.xml.')
    tests_parser.add_argument('--output-profile', action='store', required=True, help='Output profile test file.')
    tests_parser.add_argument('--output-cpp', action='store', help='Output C++ tests file.')

    subparsers.add_parser('version', parents=[log_parser], help='Print vkprofiles version.')

    args = parser.parse_args(argv)

    if args.quiet:
        log_level = logging.ERROR
    elif args.verbose:
        log_level = logging.INFO
    elif args.log is not None:
        if 'none' in args.log:
            log_level = logging.CRITICAL + 100
        elif 'all' in args.log:
            log_level = logging.INFO
        else:
            level_map = {
                'info': logging.INFO,
                'warning': logging.WARNING,
                'error': logging.ERROR,
                'critical': logging.CRITICAL
            }
            log_level = min([level_map[t] for t in args.log if t in level_map], default=logging.WARNING)
    else:
        log_level = logging.WARNING

    logging.basicConfig(level=log_level, format='%(levelname)s: %(message)s', force=True)

    start_time = time.perf_counter()

    if args.command == 'graph':
        main_graph(args, main_dispatcher=main)
    elif args.command == 'transform':
        main_transform(args)
    elif args.command == 'extract':
        main_extract(args)
    elif args.command == 'min-api-version':
        main_min_api_version(args)
    elif args.command == 'validate':
        main_validate(args)
    elif args.command == 'schema':
        main_schema(args)
    elif args.command == 'combine':
        main_combine(args)
    elif args.command == 'library':
        main_library(args)
    elif args.command == 'doc':
        main_doc(args)
    elif args.command == 'layer':
        main_layer(args)
    elif args.command == 'tests':
        main_tests(args)
    elif args.command == 'version':
        main_version(args)
    else:
        parser.print_help()
        return

    elapsed_time = time.perf_counter() - start_time
    logging.info(f"Command '{args.command}' executed in {elapsed_time:.3f}s")


if __name__ == '__main__':
    sys.exit(main(sys.argv[1:]))
    