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

import argparse
import io
import sys
import unittest
from contextlib import redirect_stderr, redirect_stdout
from pathlib import Path
from unittest.mock import patch

scripts_dir = Path(__file__).resolve().parent.parent
if str(scripts_dir) not in sys.path:
    sys.path.insert(0, str(scripts_dir))

import vkprofiles
from source.transform_utils import PullBits, StripBits, AnalyseBits
from source.profiles_json_utils import OutputFormatType
from source.main_combine import CombineMode
from source.main_extract import ExtractMode
from source.main_min_api_version import MinApiVersionMode


class TestVkProfilesExhaustiveArgs(unittest.TestCase):
    registry_path = None

    def _run_cli_capture_exit(self, args: list[str]):
        """Helper to execute vkprofiles.main with args, suppressing output and asserting SystemExit error."""
        stderr_buf = io.StringIO()
        stdout_buf = io.StringIO()
        with redirect_stderr(stderr_buf), redirect_stdout(stdout_buf):
            with self.assertRaises(SystemExit) as cm:
                vkprofiles.main(args)
        return cm.exception.code, stderr_buf.getvalue()

    # -------------------------------------------------------------------------
    # 1. Global & Logging Options
    # -------------------------------------------------------------------------
    def test_global_logging_flags(self):
        """Tests --quiet, --verbose, and --log type options."""
        test_cases = [
            (["summary", "-i", "dummy", "--quiet"], "quiet"),
            (["summary", "-i", "dummy", "--verbose"], "verbose"),
            (["summary", "-i", "dummy", "--log", "info", "warning"], "log_list"),
            (["summary", "-i", "dummy", "--log", "info,error"], "log_comma"),
        ]

        for args, name in test_cases:
            with self.subTest(flag=name):
                with patch("vkprofiles.main_summary") as mock_summary:
                    vkprofiles.main(args)
                    self.assertTrue(mock_summary.called)

    def test_invalid_logging_combinations(self):
        """Tests that invalid --log combinations ('none' or 'all' with others) trigger parser errors."""
        invalid_logs = [
            ["summary", "-i", "dummy", "--log", "none", "info"],
            ["summary", "-i", "dummy", "--log", "all", "error"],
            ["summary", "-i", "dummy", "--log", "invalid_level"]
        ]
        for args in invalid_logs:
            with self.subTest(args=args):
                exit_code, _ = self._run_cli_capture_exit(args)
                self.assertNotEqual(exit_code, 0)

    # -------------------------------------------------------------------------
    # 2. 'validate' Command & AnalyseBits
    # -------------------------------------------------------------------------
    def test_validate_args(self):
        """Exhaustively tests 'validate' CLI options and --analyse choices."""
        with patch("vkprofiles.main_validate") as mock_validate:
            # Default validate (schema validation only, analyse=None)
            vkprofiles.main(["validate", "-i", "test_path"])
            args = mock_validate.call_args[0][0]
            self.assertEqual(args.input, "test_path")
            self.assertIsNone(args.analyse)

            # Bare --analyse flag (populates all AnalyseBits via AnalyseAction)
            vkprofiles.main(["validate", "-i", "test_path", "--analyse"])
            args = mock_validate.call_args[0][0]
            self.assertEqual(args.analyse, list(AnalyseBits))

            # Specific individual bits
            vkprofiles.main(["validate", "-i", "test_path", "--analyse", "api-version", "unratified"])
            args = mock_validate.call_args[0][0]
            self.assertEqual(args.analyse, [AnalyseBits.API_VERSION, AnalyseBits.UNRATIFIED])

            # Comma-separated bits
            vkprofiles.main(["validate", "-i", "test_path", "--analyse", "extensions,aliases"])
            args = mock_validate.call_args[0][0]
            self.assertEqual(args.analyse, [AnalyseBits.EXTENSIONS, AnalyseBits.ALIASES])

    def test_validate_invalid_analyse_bit(self):
        """Verifies rejection of unknown analysis check choices."""
        exit_code, _ = self._run_cli_capture_exit(["validate", "-i", "test_path", "--analyse", "unknown_check"])
        self.assertNotEqual(exit_code, 0)

    # -------------------------------------------------------------------------
    # 3. 'transform' Command: PullBits, StripBits, Format, Flags
    # -------------------------------------------------------------------------
    def test_transform_all_pull_bits(self):
        """Tests each PullBits choice individually and in combination."""
        for bit in PullBits:
            with self.subTest(pull_bit=bit.value):
                with patch("vkprofiles.main_transform") as mock_trans:
                    vkprofiles.main(["transform", "-i", "in", "-o", "out", "--pull", bit.value])
                    args = mock_trans.call_args[0][0]
                    self.assertEqual(args.pull, [bit])

    def test_transform_all_strip_bits(self):
        """Tests each StripBits choice individually and in combination."""
        for bit in StripBits:
            with self.subTest(strip_bit=bit.value):
                with patch("vkprofiles.main_transform") as mock_trans:
                    vkprofiles.main(["transform", "-i", "in", "-o", "out", "--strip", bit.value])
                    args = mock_trans.call_args[0][0]
                    self.assertEqual(args.strip, [bit])

    def test_transform_boolean_and_format_flags(self):
        """Tests --consolidate, --sort, --input-profiles, and --format options."""
        for fmt in OutputFormatType:
            with self.subTest(format=fmt.value):
                with patch("vkprofiles.main_transform") as mock_trans:
                    vkprofiles.main([
                        "transform", "-i", "in_dir", "-o", "out_dir",
                        "--input-profiles", "VP_TEST_1,VP_TEST_2",
                        "--consolidate", "--sort", "--format", fmt.value
                    ])
                    args = mock_trans.call_args[0][0]
                    self.assertTrue(args.consolidate)
                    self.assertTrue(args.sort)
                    self.assertEqual(args.input_profiles, "VP_TEST_1,VP_TEST_2")
                    self.assertEqual(args.format, fmt)

    # -------------------------------------------------------------------------
    # 4. 'combine' Command: Modes, Stages, Metadata Overrides
    # -------------------------------------------------------------------------
    def test_combine_modes_and_stages(self):
        """Tests all combine modes, profile stages, and metadata override options."""
        for mode in CombineMode:
            for stage in ["ALPHA", "BETA", "STABLE"]:
                with self.subTest(mode=mode.value, stage=stage):
                    with patch("vkprofiles.main_combine") as mock_combine:
                        vkprofiles.main([
                            "combine", "-i", "in_dir", "-o", "out_file", "-r", "vk.xml",
                            "--mode", mode.value,
                            "--profile-stage", stage,
                            "--profile-name", "VP_COMBINED_test",
                            "--profile-version", "2",
                            "--profile-label", "Test Label",
                            "--profile-desc", "Test Description",
                            "--profile-date", "2026-10-12",
                            "--profile-api-version", "1.3.275",
                            "--profile-required-profiles", "VP_PARENT_1"
                        ])
                        args = mock_combine.call_args[0][0]
                        self.assertEqual(args.mode, mode.value)
                        self.assertEqual(args.profile_stage, stage)
                        self.assertEqual(args.profile_name, "VP_COMBINED_test")
                        self.assertEqual(args.profile_version, "2")
                        self.assertEqual(args.profile_label, "Test Label")

    # -------------------------------------------------------------------------
    # 5. 'extract' & 'min-api-version' Commands
    # -------------------------------------------------------------------------
    def test_extract_modes(self):
        """Tests all ExtractMode choices."""
        for mode in ExtractMode:
            with self.subTest(mode=mode.value):
                with patch("vkprofiles.main_extract") as mock_extract:
                    vkprofiles.main([
                        "extract", "-i", "in.json", "-o", "out.json",
                        "--input-profiles", "VP_TEST",
                        "--mode", mode.value
                    ])
                    args = mock_extract.call_args[0][0]
                    self.assertEqual(args.mode, mode)

    def test_min_api_version_modes(self):
        """Tests all MinApiVersionMode choices."""
        for mode in MinApiVersionMode:
            with self.subTest(mode=mode.value):
                with patch("vkprofiles.main_min_api_version") as mock_min_api:
                    vkprofiles.main([
                        "min-api-version", "-i", "in.json",
                        "--schemas", "schemas_dir",
                        "--mode", mode.value
                    ])
                    args = mock_min_api.call_args[0][0]
                    self.assertEqual(args.mode, mode)

    # -------------------------------------------------------------------------
    # 6. 'library', 'doc', 'layer', 'schema', 'tests' Commands
    # -------------------------------------------------------------------------
    def test_library_flags(self):
        """Tests library generation mode choices, debug flags, and build configurations."""
        modes_to_test = [["header-only"], ["header+source"], ["header-only", "header+source"]]
        for mode_list in modes_to_test:
            for config in ["release", "debug"]:
                with self.subTest(modes=mode_list, config=config):
                    with patch("vkprofiles.main_library") as mock_lib:
                        vkprofiles.main([
                            "library", "-i", "in_dir", "-r", "vk.xml",
                            "-o", "inc_dir", "--output-src", "src_dir",
                            "--output-filename", "test_profiles",
                            "--mode", *mode_list,
                            "--config", config,
                            "--include-header", "custom_header.h",
                            "--strip", "--debug"
                        ])
                        args = mock_lib.call_args[0][0]
                        self.assertEqual(args.mode, mode_list)
                        self.assertEqual(args.config, config)
                        self.assertTrue(args.strip)
                        self.assertTrue(args.debug)
                        self.assertFalse(args.legacy)

        with patch("vkprofiles.main_library") as mock_lib:
            vkprofiles.main(["library", "-i", "in_dir", "-r", "vk.xml", "--legacy"])
            self.assertTrue(mock_lib.call_args[0][0].legacy)

    def test_doc_layer_tests_schema_flags(self):
        """Tests parameter parsing for doc, layer, tests, and schema subcommands."""
        # doc
        with patch("vkprofiles.main_doc") as mock_doc:
            vkprofiles.main(["doc", "-i", "in_dir", "-r", "vk.xml", "-o", "PROFILES.md"])
            self.assertEqual(mock_doc.call_args[0][0].output, "PROFILES.md")

        # layer
        with patch("vkprofiles.main_layer") as mock_layer:
            vkprofiles.main(["layer", "-r", "vk.xml", "-o", "layer.cpp"])
            self.assertEqual(mock_layer.call_args[0][0].output, "layer.cpp")

        # tests
        with patch("vkprofiles.main_tests") as mock_tests:
            vkprofiles.main(["tests", "-r", "vk.xml", "--output-profile", "test.json", "--output-cpp", "test.cpp"])
            args = mock_tests.call_args[0][0]
            self.assertEqual(args.output_profile, "test.json")
            self.assertEqual(args.output_cpp, "test.cpp")

        # schema upgrade
        with patch("vkprofiles.main_schema") as mock_schema:
            vkprofiles.main(["schema", "-i", "in_path", "-o", "out_path", "--upgrade"])
            self.assertTrue(mock_schema.call_args[0][0].upgrade)

    # -------------------------------------------------------------------------
    # 7. Missing Required Arguments
    # -------------------------------------------------------------------------
    def test_missing_required_args_all_commands(self):
        """Verifies that invoking any subcommand without required flags triggers non-zero exit."""
        subcommand_required_missing = [
            ["graph"],                                # missing -i, -r
            ["validate"],                             # missing -i
            ["schema"],                               # missing -o
            ["transform"],                            # missing -i, -o
            ["combine"],                              # missing -i, -o, -r
            ["extract"],                              # missing -i, -o, --input-profiles
            ["min-api-version"],                      # missing -i
            ["library"],                              # missing -i, -r
            ["doc"],                                  # missing -i, -o, -r
            ["summary"],                              # missing -i
            ["tests"]                                 # missing -r, --output-profile
        ]

        for cmd_args in subcommand_required_missing:
            with self.subTest(subcommand=cmd_args[0]):
                exit_code, _ = self._run_cli_capture_exit(cmd_args)
                self.assertNotEqual(exit_code, 0)


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument(
        '--registry', '-r', action='store', required=False,
        help='Use specified registry file instead of vk.xml.'
    )

    args, unparsed = parser.parse_known_args()
    TestVkProfilesExhaustiveArgs.registry_path = args.registry

    unittest.main(argv=[sys.argv[0]] + unparsed)