# `vkprofiles` Command-Line Tool

This repository exposes the command-line frontend in [scripts/vkprofiles.py](vkprofiles.py). When installed as a package or console script, the entry point is typically named `vkprofiles`; from the repository root, the same CLI can be run directly as:

```bash
python3 scripts/vkprofiles.py <command> [options]
python3 scripts/vkprofiles.py [--help | -h]
python3 scripts/vkprofiles.py [--version | -v]
```

The runtime behavior is the same regardless of how the tool is launched, and the dispatcher is implemented in [scripts/vkprofiles.py](vkprofiles.py) with graph execution handled by [scripts/source/main_graph.py](source/main_graph.py).

Global options:

* `--help`, `-h`: Print help message and exit (available at top-level and for all subcommands).
* `--version`, `-v`: Print `vkprofiles` version.
* `--quiet`: Suppress warning and informational messages (ERROR level only).
* `--verbose`: Enable INFO, WARNING, and ERROR output.
* `--log [TYPES ...]`: Filter output by message type: `none`, `all`, `info`, `warning`, `error`, or `critical`. Values may be comma-separated or space-separated. `none` and `all` cannot be combined with other types, and this option is mutually exclusive with `--quiet` and `--verbose`.

The top-level CLI registers commands in this order: `graph`, `validate`, `schema`, `transform`, `combine`, `extract`, `min-api-version`, `library`, `doc`, `layer`, `tests`, and `version`.

Within a graph file, `nodes[].command` is limited to the pipeline operations supported by the executor (`schema`, `transform`, `combine`, `validate`, `library`, `doc`, `extract`, `min-api-version`, `layer`, and `tests`). Top-level `graph` and `version` are CLI entry points, not valid node commands.

---

## Subcommands

### 1. `graph`

Executes a dependency-driven pipeline graph of `vkprofiles` commands from a JSON file. This is useful for automating profile generation, validation, document generation, and library generation in one workflow.

```bash
vkprofiles graph --registry vk.xml --input path/to/graph.json
```

* `--input`, `-i`: *(Required)* Path to the graph execution JSON configuration file.
* `--registry`, `-r`: *(Required for execution)* Shared Vulkan registry file (`vk.xml`) applied to every step in the graph.

#### Graph JSON Schema

The graph schema is defined in [scripts/source/graph_schema.py](source/graph_schema.py). It enforces the following structure:

```json
{
  "$schema": "http://json-schema.org/draft-07/schema#",
  "$id": "https://schema.khronos.org/vulkan/profiles-graph.json#",
  "title": "Vulkan Profiles Operation Graph Schema",
  "description": "Schema for vkprofiles execution graph files.",
  "type": "object",
  "required": ["nodes"],
  "properties": {
    "$schema": { "type": "string" },
    "variables": {
      "type": "object",
      "additionalProperties": { "type": "string" }
    },
    "nodes": {
      "type": "array",
      "items": {
        "type": "object",
        "required": ["id", "command", "args"],
        "properties": {
          "id": { "type": "string" },
          "description": { "type": "string" },
          "depends_on": {
            "type": "array",
            "items": { "type": "string" }
          },
          "command": {
            "type": "string",
            "enum": [
              "schema",
              "transform",
              "combine",
              "validate",
              "library",
              "doc",
              "extract",
              "min-api-version",
              "layer",
              "tests"
            ]
          },
          "args": { "type": "object" }
        },
        "additionalProperties": false
      }
    }
  },
  "additionalProperties": false
}
```

#### Graph Schema Element Description

| Element | Required | Description |
| --- | --- | --- |
| `$schema` | No | Optional schema identifier for the graph file itself. |
| `variables` | No | Global variable map. The current schema expects string values, which are resolved with `${var_name}` and `$node_id.attribute` substitution. |
| `nodes` | Yes | Required array with at least one execution node. |
| `nodes[].id` | Yes | Unique node identifier. |
| `nodes[].description` | No | Human-readable description for logging/output. |
| `nodes[].command` | Yes | Pipeline operation to execute. Valid values are `validate`, `schema`, `transform`, `combine`, `extract`, `min-api-version`, `library`, `doc`, `layer`, and `tests`. |
| `nodes[].depends_on` | No | Array of upstream node ids that must complete before this node runs. |
| `nodes[].args` | Yes | Object of CLI arguments for the underlying command. Keys follow the CLI flag names, using underscores instead of hyphens. |

The graph executor supports variable substitution in strings, lists, and nested objects. For example, `"output": "${profile_root}/VP_LUNARG_desktop_baseline.json"` resolves using the global `variables` map, and `$node_id.output` can be used to pass the output from a previous step into a later node.

In addition, a graph node can embed a batch combine configuration directly in `args` using `profiles`, `contributors`, and `history`, which the executor handles before delegating to the normal combine logic. This is especially useful for multi-profile generation pipelines.

#### Example: Generate the Desktop Baseline profile, validate it, build a library, and produce documentation

Create a graph file such as `profiles/desktop_baseline_pipeline.json`:

```json
{
  "$schema": "https://schema.khronos.org/vulkan/profiles-graph.json#",
  "nodes": [
    {
      "id": "generate_schema",
      "description": "Generate the Vulkan Profiles schema",
      "command": "schema",
      "args": {
        "output": "schema/profiles-0.8-latest.json",
        "api": "vulkan"
      }
    },
    {
      "id": "desktop_baseline",
      "description": "Generate VP_LUNARG_desktop_baseline profile",
      "depends_on": ["generate_schema"],
      "command": "combine",
      "args": {
        "output": "profiles/LunarG/VP_LUNARG_desktop_baseline.json",
        "format": "flatten",
        "pull": ["aliases"],
        "strip": ["helper-values"],
        "consolidate": true,
        "contributors": {
          "Christophe Riccio": {
            "company": "LunarG",
            "email": "christophe@lunarg.com",
            "contact": true
          }
        },
        "history": [
          {
            "revision": 4,
            "date": "2026-09-16",
            "author": "Christophe Riccio",
            "comment": "Updated Desktop Baseline profiles with missing structures"
          }
        ],
        "profiles": {
          "VP_LUNARG_desktop_baseline_2022": {
            "version": 3,
            "input": "profiles/LunarG/VP_LUNARG_desktop_baseline_2022",
            "label": "LunarG Vulkan Desktop Baseline 2022 profile",
            "description": "Desktop baseline profile for 2022-era systems.",
            "date": "2026-09-16",
            "stage": "STABLE",
            "api-version": "1.1.139",
            "required-profiles": ""
          },
          "VP_LUNARG_desktop_baseline_2023": {
            "version": 3,
            "input": "profiles/LunarG/VP_LUNARG_desktop_baseline_2023",
            "label": "LunarG Vulkan Desktop Baseline 2023 profile",
            "description": "Desktop baseline profile for 2023-era systems.",
            "date": "2026-09-16",
            "stage": "STABLE",
            "api-version": "1.2.148",
            "required-profiles": ""
          },
          "VP_LUNARG_desktop_baseline_2024": {
            "version": 2,
            "input": "profiles/LunarG/VP_LUNARG_desktop_baseline_2024",
            "label": "LunarG Vulkan Desktop Baseline 2024 profile",
            "description": "Desktop baseline profile for 2024-era systems.",
            "date": "2026-09-16",
            "stage": "STABLE",
            "api-version": "1.2.204",
            "required-profiles": ""
          },
          "VP_LUNARG_desktop_baseline_2026": {
            "version": 2,
            "input": "profiles/LunarG/VP_LUNARG_desktop_baseline_2026",
            "label": "LunarG Vulkan Desktop Baseline 2026 profile",
            "description": "Desktop baseline profile for 2026-era systems.",
            "date": "2026-09-16",
            "stage": "STABLE",
            "api-version": "1.4.303",
            "required-profiles": ""
          }
        }
      }
    },
    {
      "id": "validate_profiles",
      "description": "Validate the generated profile",
      "depends_on": ["desktop_baseline"],
      "command": "validate",
      "args": {
        "input": "profiles/LunarG",
        "mode": ["schema", "analysis"]
      }
    },
    {
      "id": "library_profiles",
      "description": "Generate the Vulkan API library for the baseline profiles",
      "depends_on": ["validate_profiles"],
      "command": "library",
      "args": {
        "input": "profiles",
        "input_filenames": "VP_KHR_roadmap.json,VP_LUNARG_desktop_baseline.json",
        "output": "library/include/vulkan",
        "output_src": "library/source",
        "output_filename": "vulkan_profiles",
        "mode": ["header+source"],
        "strip": true,
        "config": "release"
      }
    },
    {
      "id": "doc_profiles",
      "description": "Generate markdown documentation for the baselines",
      "depends_on": ["validate_profiles"],
      "command": "doc",
      "args": {
        "input": "profiles",
        "input_filenames": "VP_KHR_roadmap.json,VP_LUNARG_desktop_baseline.json",
        "output": "PROFILES_ALL.md"
      }
    }
  ]
}
```

Run it with:

```bash
vkprofiles graph \
    --registry external/Debug/64/Vulkan-Headers/registry/vk.xml \
    --input profiles/desktop_baseline_pipeline.json
```

This graph will:

1. Generate the schema file at `schema/profiles-0.8-latest.json`.
2. Build the `VP_LUNARG_desktop_baseline.json` file from the Desktop Baseline subprofiles.
3. Validate the generated baseline profile against the schema and `vk.xml` analysis rules.
4. Generate the Vulkan API C/C++ library for the selected profiles.
5. Generate the Markdown profile documentation in `PROFILES_ALL.md`.

---

### 2. `validate`

Validates one or more profile JSON files against the Vulkan Profiles JSON schema and performs static analysis against `vk.xml`.

By default, schema validation dynamically generates a schema from `vk.xml`. To validate profile JSON files against an explicit or externally published profile schema file (`profiles-*.json`), developers can supply the `--schema` (`-s`) parameter.

```bash
vkprofiles validate --input path/to/profiles [options]
```

* `--input`, `-i`: *(Required)* Path to profile JSON file or directory to validate.
* `--registry`, `-r`: Path to `vk.xml` (used to generate schema if `--schema` is omitted and required for `analysis` mode).
* `--schema`, `-s`: Path to an explicit JSON schema file (`profiles-*.json`). By default, generates schema from `vk.xml`.
* `--mode`, `-m`: Validation mode(s) to execute (`schema`, `analysis`). Default: `schema analysis`.
* `--api`: Target API variant (`vulkan`). Default: `vulkan`.

#### Validation Modes (`--mode`)

| Mode Value | Description |
| --- | --- |
| `schema` | **Structural Validation:** Validates the JSON document layout and syntax against the Vulkan Profiles JSON Schema. It checks top-level key hierarchy, confirms required fields are present, verifies value data types (booleans, integers, arrays), and enforces value constraints and enumeration choices. |
| `analysis` | **Static Semantic Analysis:** Performs deep semantic checks directly against the Vulkan Registry (`vk.xml`). It verifies that all referenced extension names, physical device feature/property structures, and format flags exist in `vk.xml`, checks that all capabilities are compatible with the target Vulkan `api-version`, and flags unrecognized struct members or unfulfilled specification dependencies. |

**Example (Using explicit external schema and running both modes):**

```bash
vkprofiles validate \
    --api vulkan \
    --registry vk.xml \
    --schema schema/profiles-0.8-latest.json \
    --input profiles/LunarG \
    --mode schema analysis
```

---

### 3. `schema`

Generates a Vulkan Profiles JSON schema file from `vk.xml`, or upgrades existing profile JSON files to schema 0.8.2.

```bash
vkprofiles schema --registry vk.xml --output path/to/output [options]
```

* `--registry`, `-r`: Path to `vk.xml`.
* `--input`, `-i`: Path to input profile JSON file or directory to upgrade.
* `--output`, `-o`: *(Required)* Output path for the generated schema file or upgraded output directory/file.
* `--upgrade`: Upgrade input profile JSON file(s) from schema 0.8.0 / 0.8.1 to 0.8.2.
* `--format`: Output formatting style (`flatten` or `pretty`). Default: `pretty`.
* `--api`: Target API variant (`vulkan`). Default: `vulkan`.

**Example (generate a schema):**

```bash
vkprofiles schema \
    --api vulkan \
    --registry vk.xml \
    --output schema/profiles-0.8-latest.json
```

**Example (upgrade a profile directory):**

```bash
vkprofiles schema \
    --api vulkan \
    --registry vk.xml \
    --input profiles/LunarG \
    --output profiles/LunarG/upgraded \
    --upgrade \
    --format pretty
```

---

### 4. `transform`

Transforms implicit profile JSON files to explicit profile JSON files by pulling Vulkan capabilities, extension dependencies, and feature aliases directly from `vk.xml`.

```bash
vkprofiles transform --registry vk.xml --input path/to/input_dir --output path/to/output_dir [options]
```

* `--input`, `-i`: *(Required)* Path to input profile JSON file or directory.
* `--output`, `-o`: *(Required)* Path to output directory or file.
* `--registry`, `-r`: Path to `vk.xml`.
* `--api`: Target API variant (`vulkan`). Default: `vulkan`.
* `--format`: Output formatting style (`flatten` or `pretty`). Default: `pretty`.
* `--pull [OPTIONS ...]`: Pull capability options: `required-capabilities`, `promoted-extensions`, `ignore-extension-versions`, `ignore-unsupported`, `override-with-core-capabilities`, or `aliases`.
* `--consolidate`: Consolidate capabilities.
* `--strip [OPTIONS ...]`: Strip options: `helper-values`, `duplication`, or `promoted-extensions`.
* `--sort`: Sort profile capabilities.
* `--validate [MODES ...]`: Validate profile files before transformation (`schema`, `analysis`). If specified without modes, both validation modes are used.

#### Transformation Options

Conversion flags are processed in a deterministic internal pipeline order regardless of the order specified on the command line. This multi-phase sequence ensures that all extension dependencies and core promotions are populated first, structural feature/property/format aliases are subsequently expanded across all required structures, redundant inherited definitions are stripped, capability blocks are consolidated, and promoted extensions are cleaned up.

| Option value | Description |
| --- | --- |
| `--pull required-capabilities` | Evaluates extension dependencies and pulls satisfied core/extension feature and property requirements into capability blocks. |
| `--pull promoted-extensions` | Requires all extensions promoted to core up to the profile's target Vulkan version. |
| `--pull ignore-extension-versions` | Sets all required extension versions to 1, overriding specific extension spec versions. |
| `--pull ignore-unsupported` | Skips pulling default values for feature-dependent properties when the corresponding feature is unsupported. |
| `--pull override-with-core-capabilities` | Allows required-capabilities pulling to override profile capability values below Vulkan core requirements. |
| `--pull aliases` | Resolves and populates equivalent capability aliases across core structures and extensions. |
| `--consolidate` | Combines capability blocks into a consolidated requirements block per profile. |
| `--strip helper-values` | Removes non-bit-position bitmask helper values from capability blocks. |
| `--strip duplication` | Removes redundant duplicate features, properties, and extension requirements. |
| `--strip promoted-extensions` | Removes extensions already promoted to the profile's target core Vulkan version. |
| `--sort` | Sorts profile capabilities into canonical Vulkan order. |

**Example:**

```bash
vkprofiles transform \
    --registry vk.xml \
    --input profiles/LunarG \
    --output profiles/generated \
    --pull required-capabilities aliases \
    --strip duplication promoted-extensions \
    --consolidate \
    --validate
```

---

### 5. `combine`

Combines multiple profile JSON files into a single output profile JSON file using one of the supported combination modes: `intersection`, `union`, or `difference`.

```bash
vkprofiles combine --registry vk.xml --input path/to/profiles --output path/to/combined.json [options]
```

* `--registry`, `-r`: *(Required)* Path to `vk.xml`.
* `--output`, `-o`: *(Required)* Output JSON file path.
* `--input`, `-i`: Directory path containing profiles to combine.
* `--input-profiles`: Comma-separated list of profiles to combine.
* `--config`, `-c`: Path to JSON combine config file.
* `--mode`, `-m`: Combination mode (`intersection`, `union`, or `difference`). Default: `intersection`.
* `--format`: Output formatting style (`flatten` or `pretty`). Default: `pretty`.
* `--pull [OPTIONS ...]`: Pull capability options: `required-capabilities`, `promoted-extensions`, `ignore-extension-versions`, `ignore-unsupported`, `override-with-core-capabilities`, or `aliases`.
* `--consolidate`: Consolidate capabilities.
* `--strip [OPTIONS ...]`: Strip options: `helper-values`, `duplication`, or `promoted-extensions`.
* `--sort`: Sort profile capabilities.
* `--validate [MODES ...]`: Validate profile files before combining (`schema`, `analysis`). If specified without modes, both validation modes are used.
* `--profile-name`: Override output profile name.
* `--profile-version`: Set profile version number. Default: `1`.
* `--profile-label`: Set profile label string.
* `--profile-desc`: Set profile description string.
* `--profile-date`: Set profile release date (`YYYY-MM-DD`).
* `--profile-api-version`: Set target Vulkan API version (e.g., `1.3.280`).
* `--profile-stage`: Set development stage (`ALPHA`, `BETA`, `STABLE`). Default: `STABLE`.
* `--profile-required-profiles`: Comma-separated list of required profiles by the generated profile.

#### Combining Multiple Profiles into a Single Output File (`--config`)

When passing a JSON configuration file via `--config`, the `combine` command can process multiple profile definitions at once and pack them into a single output JSON file. The configuration file specifies a `profiles` object mapping target profile names (for example `VP_LUNARG_desktop_baseline_2022`, `VP_LUNARG_desktop_baseline_2023`, `VP_LUNARG_desktop_baseline_2024`, `VP_LUNARG_desktop_baseline_2026`) to their respective source input directories, label, description, API version, and required profiles. Root-level metadata such as `contributors` and `history` are also embedded into the final combined file.

#### Target Vulkan API Version Selection (`--profile-api-version`)

When specifying `--profile-api-version` (or setting `api-version` within a `--config` JSON file), developers must specify a Vulkan API version that exposes all Vulkan capabilities defined by the profile data. Selecting a target API version lower than what is required by any included core structure or feature will result in an invalid profile definition.

To assist developers in determining and verifying the correct API version:

* `vkprofiles validate` with `--mode analysis` checks the profile data against `vk.xml` and explicitly reports any capabilities or core structures that are not supported by the designated Vulkan major/minor version.
* Published Vulkan Profiles Schemas in the Khronos-Schemas repository can be used to validate against a specific Vulkan Header version.

**Example (Creating a combined baseline profile with configuration and transformation):**

```bash
vkprofiles combine \
    --registry vk.xml \
    --config profiles/LunarG/VP_LUNARG_desktop_baseline_config.json \
    --output profiles/LunarG/VP_LUNARG_desktop_baseline.json \
    --pull aliases \
    --strip duplication \
    --validate
```

**Example (Combining a directory into a single profile):**

```bash
vkprofiles combine \
    --registry vk.xml \
    --input profiles/LunarG/VP_LUNARG_desktop_max_2026 \
    --output profiles/test/data/VP_LUNARG_desktop_max_2026.json \
    --profile-name VP_LUNARG_desktop_max_2026 \
    --profile-label "LunarG Vulkan Desktop Max 2026 profile" \
    --profile-date 2026-06-22 \
    --profile-stage BETA \
    --profile-api-version 1.4.353
```

---

### 6. `extract`

Extracts a single named profile from a larger profile JSON file or directory into a standalone output profile JSON file.

```bash
vkprofiles extract --input path/to/input.json --output path/to/output.json --profile-name PROFILE_NAME [options]
```

* `--input`, `-i`: *(Required)* Path to input profile JSON file or directory.
* `--output`, `-o`: *(Required)* Path to output profile JSON file.
* `--profile-name`, `-p`: *(Required)* Specific profile name to extract.
* `--mode`, `-m`: Extraction mode: `reference-required-profiles` or `pull-required-profiles`. Default: `reference-required-profiles`.
* `--format`: Output formatting style (`flatten` or `pretty`). Default: `pretty`.

**Example:**

```bash
vkprofiles extract \
    --input profiles/LunarG \
    --output profiles/extracted/VP_LUNARG_desktop_baseline.json \
    --profile-name VP_LUNARG_desktop_baseline \
    --mode reference-required-profiles
```

---

### 7. `min-api-version`

Displays the profile `api-version` and schema URI from input JSON, or evaluates the minimum compatible Vulkan profile schema version and updates profile JSON files as needed.

```bash
vkprofiles min-api-version --input path/to/profiles [options]
```

* `--input`, `-i`: *(Required)* Path to input profiles file or directory.
* `--output`, `-o`: Path to output profiles file or directory when updating JSONs in `evaluate` mode.
* `--input-profiles`: Comma-separated list of profile names to process.
* `--schemas`, `-s`: Path to a directory containing Vulkan profile schemas (`profiles-*.json`).
* `--mode`, `-m`: Operation mode: `display` or `evaluate`. Default: `display`.
* `--format`: Output formatting style (`flatten` or `pretty`). Default: `pretty`.

#### `--mode` behavior

| Mode Value | Description |
| --- | --- |
| `display` | Prints each profile's `api-version` and the schema URI read from the input JSON. |
| `evaluate` | Checks the profile against the available schema set, determines the minimum Vulkan header version that satisfies the data, and optionally updates the JSON in the output path. |

**Example (display values):**

```bash
vkprofiles min-api-version \
    --input profiles/LunarG \
    --input-profiles VP_LUNARG_desktop_baseline_2022,VP_LUNARG_desktop_baseline_2026 \
    --mode display
```

**Example (evaluate and rewrite JSON):**

```bash
vkprofiles min-api-version \
    --input profiles/LunarG \
    --output build/profiles_lunarg \
    --mode evaluate \
    --format flatten
```

---

### 8. `library`

Generates C/C++ Vulkan Profiles API library headers (`vulkan_profiles.h`, `vulkan_profiles.hpp`) and source files (`vulkan_profiles.cpp`).

> [!IMPORTANT]
> **Device Creation Requirements & `pNext` Chain Structure Duplication**
> When calling `vkCreateDevice`, the Vulkan specification prohibits passing duplicate or aliased feature structures simultaneously in the `VkDeviceCreateInfo` `pNext` chain. Specifically, two different structures enabling or configuring the same underlying Vulkan feature cannot both be present in `pNext`, even if their member boolean values match.
>
> **Resolution:**
> To ensure the generated library creates valid `VkDevice` instances, input profile JSON files should not contain unexpanded or redundant feature structures. Use `--pull aliases --strip` before generating the library when appropriate.

```bash
vkprofiles library --registry vk.xml --input path/to/profiles --output path/to/include [options]
```

* `--registry`, `-r`: *(Required)* Path to `vk.xml`.
* `--input`, `-i`: *(Required)* Directory containing input profile JSON files.
* `--input-filenames`: Comma-separated list of profile filenames.
* `--output`, `-o`, `--output-inc`: Target header output directory.
* `--output-src`: Target source output directory. If omitted in `header+source` mode, defaults to `--output`.
* `--output-filename`: Base filename for generated files. Default: `vulkan_profiles`.
* `--mode [MODES ...]`: Library generation modes (`header-only`, `header+source`). Default: both modes are generated.
* `--pull [OPTIONS ...]`: Pull capability options: `required-capabilities`, `promoted-extensions`, `ignore-extension-versions`, `ignore-unsupported`, `override-with-core-capabilities`, or `aliases`.
* `--consolidate`: Consolidate capabilities.
* `--strip`: Strip redundant capability structures before generating the library to keep `VkDevice` creation valid.
* `--sort`: Sort profile capabilities.
* `--intermediate`: Directory path for intermediate transformed JSON files (used when transformation options are provided).
* `--validate [MODES ...]`: Validate profiles during generation (`schema`, `analysis`). If specified without modes, both validation modes are used.
* `--debug`, `-d`: Generate debug variant of library code.
* `--config`, `-c`: Build configuration (`release` or `debug`). Default: `release`.
* `--include-header`: Override the header file include directive in generated C++ source files.

**Example (Header + Source with automatic conversion):**

```bash
vkprofiles library \
    --api vulkan \
    --registry vk.xml \
    --input profiles \
    --input-filenames "VP_KHR_roadmap.json,VP_LUNARG_minimum_requirements.json,VP_LUNARG_desktop_baseline.json" \
    --output library/include/vulkan \
    --output-src library/source \
    --output-filename vulkan_profiles \
    --mode header+source \
    --pull aliases \
    --strip \
    --config release
```

**Example (Header-Only):**

```bash
vkprofiles library \
    --api vulkan \
    --registry vk.xml \
    --input profiles \
    --output library/include/vulkan \
    --mode header-only
```

---

### 9. `doc`

Generates Markdown documentation from profile JSON files.

```bash
vkprofiles doc --registry vk.xml --input path/to/profiles --output PROFILES.md [options]
```

* `--registry`, `-r`: *(Required)* Path to `vk.xml`.
* `--input`, `-i`: *(Required)* Directory containing profile JSON files.
* `--output`, `-o`: *(Required)* Output Markdown file path.
* `--input-filenames`: Comma-separated list of profile filenames.
* `--validate [MODES ...]`: Validate profile JSON files before generating documentation (`schema`, `analysis`). If specified without modes, both validation modes are used. By default, no validation is requested.
* `--api`: Target API variant (`vulkan`). Default: `vulkan`.

**Example:**

```bash
vkprofiles doc \
    --api vulkan \
    --registry vk.xml \
    --input profiles \
    --input-filenames "VP_KHR_roadmap.json,VP_LUNARG_minimum_requirements.json" \
    --output PROFILES.md \
    --validate
```

---

## Tool Developer Subcommands

> [!NOTE]
> **Internal Tool Developer Commands**
> The `layer` and `tests` subcommands are designed exclusively for developers maintaining the Vulkan Profiles repository to generate layer and test code.

### 10. `layer` *(Internal)*

Generates `profiles_generated.cpp` for the Vulkan Profiles layer (`VkLayer_khronos_profiles`).

```bash
vkprofiles layer --registry vk.xml --output layer/profiles_generated.cpp
```

* `--registry`, `-r`: Path to `vk.xml`.
* `--output`, `-o`, `--out-layer`: Output path for the generated layer source file.
* `--api`: Target API variant (`vulkan`). Default: `vulkan`.

**Example:**

```bash
vkprofiles layer \
    --api vulkan \
    --registry vk.xml \
    --output layer/profiles_generated.cpp
```

---

### 11. `tests` *(Internal)*

Generates synthetic test profile JSON (`VP_LUNARG_test_api_generated.json`) and a C++ test suite (`tests_generated.cpp`).

```bash
vkprofiles tests --registry vk.xml --output-profile path/to/test.json --output-cpp path/to/tests.cpp
```

* `--registry`, `-r`: *(Required)* Path to `vk.xml`.
* `--output-profile`: *(Required)* Output path for synthetic test profile JSON.
* `--output-cpp`: Output path for C++ test source file.
* `--api`: Target API variant (`vulkan`). Default: `vulkan`.

**Example:**

```bash
vkprofiles tests \
    --api vulkan \
    --registry vk.xml \
    --output-profile profiles/test/data/VP_LUNARG_test_api_generated.json \
    --output-cpp layer/tests/tests_generated.cpp
```

---

### 12. `version`

Prints the `vkprofiles` version string based on the Vulkan Headers version used when building the tool.

```bash
vkprofiles version
```

**Example:**

```bash
vkprofiles version
```