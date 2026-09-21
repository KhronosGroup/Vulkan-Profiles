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
import json
from pathlib import Path
import sys
import unittest

scripts_dir = Path(__file__).resolve().parent.parent
if str(scripts_dir) not in sys.path:
    sys.path.insert(0, str(scripts_dir))

from source.main_extract import extract_profile, ExtractMode


class TestExtract(unittest.TestCase):
    registry_path = None

    def assertProfileDataEqual(self, gen_data: dict, exp_data: dict):
        """Helper to assert profile equality with detailed structural mismatch diagnostics."""
        self.assertEqual(
            list(gen_data.keys()),
            list(exp_data.keys()),
            f"Top-level keys mismatch: {list(gen_data.keys())} vs {list(exp_data.keys())}"
        )

        if "profiles" in exp_data:
            self.assertEqual(
                list(gen_data.get("profiles", {}).keys()),
                list(exp_data["profiles"].keys()),
                "Profile name list mismatch"
            )
            for prof_name, exp_prof in exp_data["profiles"].items():
                gen_prof = gen_data["profiles"].get(prof_name, {})
                self.assertEqual(
                    gen_prof.get("capabilities"),
                    exp_prof.get("capabilities"),
                    f"Capabilities block reference list mismatch in profile '{prof_name}'"
                )
                self.assertEqual(
                    gen_prof.get("profiles"),
                    exp_prof.get("profiles"),
                    f"Required parent profiles list mismatch in profile '{prof_name}'"
                )

        if "capabilities" in exp_data:
            self.assertEqual(
                list(gen_data.get("capabilities", {}).keys()),
                list(exp_data["capabilities"].keys()),
                "Capability block name list mismatch"
            )

            for cap_name, exp_block in exp_data["capabilities"].items():
                gen_block = gen_data["capabilities"].get(cap_name, {})

                self.assertEqual(
                    list(gen_block.keys()),
                    list(exp_block.keys()),
                    f"Section mismatch in capability block '{cap_name}' section"
                )

                for section in ("features", "properties"):
                    if section in exp_block:
                        exp_section = exp_block[section]
                        gen_section = gen_block.get(section, {})

                        self.assertEqual(
                            list(gen_section.keys()),
                            list(exp_section.keys()),
                            f"Structure name list mismatch in capability block '{cap_name}', section '{section}'"
                        )

                        for struct_name, exp_struct in exp_section.items():
                            gen_struct = gen_section.get(struct_name, {})
                            if isinstance(exp_struct, dict):
                                self.assertEqual(
                                    list(gen_struct.keys()),
                                    list(exp_struct.keys()),
                                    f"Member key list mismatch in '{cap_name}.{section}.{struct_name}'"
                                )
                                for member_name, exp_val in exp_struct.items():
                                    gen_val = gen_struct.get(member_name)
                                    self.assertEqual(
                                        gen_val,
                                        exp_val,
                                        f"Value mismatch in '{cap_name}.{section}.{struct_name}.{member_name}'"
                                    )
                            else:
                                self.assertEqual(
                                    gen_struct,
                                    exp_struct,
                                    f"Value mismatch in '{cap_name}.{section}.{struct_name}'"
                                )

        self.assertEqual(gen_data, exp_data)

    def test_extract_profile_reference_mode(self):
        """
        Verifies that extracting a profile in 'reference' mode retains external
        parent profile references and prefixes capability blocks with the profile name.
        """
        original_json_text = """{
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8.2-latest.json#",
            "profiles": {
                "VP_TEST_parent": {
                    "version": 1,
                    "api-version": "1.0.68",
                    "capabilities": ["parent_block"]
                },
                "VP_TEST_target": {
                    "version": 1,
                    "api-version": "1.1.0",
                    "profiles": ["VP_TEST_parent"],
                    "capabilities": ["target_block"]
                }
            },
            "capabilities": {
                "parent_block": {
                    "features": {
                        "VkPhysicalDeviceFeatures": {
                            "robustBufferAccess": true
                        }
                    }
                },
                "target_block": {
                    "features": {
                        "VkPhysicalDeviceFeatures": {
                            "fullDrawIndexUint32": true
                        }
                    }
                }
            }
        }"""

        expected_json_text = """{
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8.2-latest.json#",
            "profiles": {
                "VP_TEST_target": {
                    "version": 1,
                    "api-version": "1.1.0",
                    "profiles": ["VP_TEST_parent"],
                    "capabilities": ["VP_TEST_target_target_block"]
                }
            },
            "capabilities": {
                "VP_TEST_target_target_block": {
                    "features": {
                        "VkPhysicalDeviceFeatures": {
                            "fullDrawIndexUint32": true
                        }
                    }
                }
            }
        }"""

        input_data = json.loads(original_json_text)
        json_files_dict = {"input.json": input_data}
        result_data = extract_profile(json_files_dict, ["VP_TEST_target"], ExtractMode.REFERENCE)

        self.assertProfileDataEqual(result_data, json.loads(expected_json_text))

    def test_extract_profile_pull_mode(self):
        """
        Verifies that extracting a profile in 'pull' mode incorporates required
        parent profiles and capability blocks directly into the target output.
        """
        original_json_text = """{
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8.2-latest.json#",
            "profiles": {
                "VP_TEST_parent": {
                    "version": 1,
                    "api-version": "1.0.68",
                    "capabilities": ["parent_block"]
                },
                "VP_TEST_target": {
                    "version": 1,
                    "api-version": "1.1.0",
                    "profiles": ["VP_TEST_parent"],
                    "capabilities": ["target_block"]
                }
            },
            "capabilities": {
                "parent_block": {
                    "features": {
                        "VkPhysicalDeviceFeatures": {
                            "robustBufferAccess": true
                        }
                    }
                },
                "target_block": {
                    "features": {
                        "VkPhysicalDeviceFeatures": {
                            "fullDrawIndexUint32": true
                        }
                    }
                }
            }
        }"""

        expected_json_text = """{
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8.2-latest.json#",
            "profiles": {
                "VP_TEST_target": {
                    "version": 1,
                    "api-version": "1.1.0",
                    "profiles": [],
                    "capabilities": ["VP_TEST_target_block"]
                }
            },
            "capabilities": {
                "VP_TEST_target_block": {
                    "features": {
                        "VkPhysicalDeviceFeatures": {
                            "robustBufferAccess": true,
                            "fullDrawIndexUint32": true
                        }
                    }
                }
            }
        }"""

        input_data = json.loads(original_json_text)
        json_files_dict = {"input.json": input_data}
        result_data = extract_profile(json_files_dict, ["VP_TEST_target"], ExtractMode.PULL)

        self.assertProfileDataEqual(result_data, json.loads(expected_json_text))

    def test_extract_multiple_profiles(self):
        """
        Verifies that given an input dataset with 3 profiles, specifying 2 profiles
        to extract produces a single JSON file containing only those 2 profiles and
        their associated namespaced capability blocks.
        """
        original_json_text = """{
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8.2-latest.json#",
            "profiles": {
                "VP_TEST_profile1": {
                    "version": 1,
                    "api-version": "1.0.68",
                    "capabilities": ["block1"]
                },
                "VP_TEST_profile2": {
                    "version": 1,
                    "api-version": "1.1.0",
                    "capabilities": ["block2"]
                },
                "VP_TEST_profile3": {
                    "version": 1,
                    "api-version": "1.2.0",
                    "capabilities": ["block3"]
                }
            },
            "capabilities": {
                "block1": {
                    "features": {
                        "VkPhysicalDeviceFeatures": {
                            "robustBufferAccess": true
                        }
                    }
                },
                "block2": {
                    "features": {
                        "VkPhysicalDeviceFeatures": {
                            "fullDrawIndexUint32": true
                        }
                    }
                },
                "block3": {
                    "features": {
                        "VkPhysicalDeviceFeatures": {
                            "samplerAnisotropy": true
                        }
                    }
                }
            }
        }"""

        expected_json_text = """{
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8.2-latest.json#",
            "profiles": {
                "VP_TEST_profile1": {
                    "version": 1,
                    "api-version": "1.0.68",
                    "capabilities": ["VP_TEST_profile1_block1"]
                },
                "VP_TEST_profile2": {
                    "version": 1,
                    "api-version": "1.1.0",
                    "capabilities": ["VP_TEST_profile2_block2"]
                }
            },
            "capabilities": {
                "VP_TEST_profile1_block1": {
                    "features": {
                        "VkPhysicalDeviceFeatures": {
                            "robustBufferAccess": true
                        }
                    }
                },
                "VP_TEST_profile2_block2": {
                    "features": {
                        "VkPhysicalDeviceFeatures": {
                            "fullDrawIndexUint32": true
                        }
                    }
                }
            }
        }"""

        input_data = json.loads(original_json_text)
        json_files_dict = {"input.json": input_data}
        result_data = extract_profile(
            json_files_dict,
            ["VP_TEST_profile1", "VP_TEST_profile2"],
            ExtractMode.REFERENCE
        )

        self.assertProfileDataEqual(result_data, json.loads(expected_json_text))

    def test_extract_multiple_files_newest_schema(self):
        """
        Verifies that when extracting profiles defined across multiple input files
        with different schema versions, the output JSON uses the schema with the newest
        Vulkan Header version.
        """
        file1_json = """{
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8.2-303.json#",
            "profiles": {
                "VP_TEST_profile1": {
                    "version": 1,
                    "api-version": "1.0.68",
                    "capabilities": ["block1"]
                }
            },
            "capabilities": {
                "block1": {
                    "features": {
                        "VkPhysicalDeviceFeatures": {
                            "robustBufferAccess": true
                        }
                    }
                }
            }
        }"""

        file2_json = """{
            "$schema": "https://schema.khronos.org/vulkan/profiles-0.8.2-363.json#",
            "profiles": {
                "VP_TEST_profile2": {
                    "version": 1,
                    "api-version": "1.1.0",
                    "capabilities": ["block2"]
                }
            },
            "capabilities": {
                "block2": {
                    "features": {
                        "VkPhysicalDeviceFeatures": {
                            "fullDrawIndexUint32": true
                        }
                    }
                }
            }
        }"""

        json_files_dict = {
            "file1.json": json.loads(file1_json),
            "file2.json": json.loads(file2_json)
        }

        result_data = extract_profile(
            json_files_dict,
            ["VP_TEST_profile1", "VP_TEST_profile2"],
            ExtractMode.REFERENCE
        )

        self.assertEqual(result_data["$schema"], "https://schema.khronos.org/vulkan/profiles-0.8.2-363.json#")


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument(
        '--registry', '-r', action='store', required=False,
        help='Use specified registry file instead of vk.xml.'
    )

    args, unparsed = parser.parse_known_args()
    TestExtract.registry_path = args.registry

    unittest.main(argv=[sys.argv[0]] + unparsed)