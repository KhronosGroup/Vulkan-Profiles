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

Pass `--quiet`, `--verbose`, and `--log` after the subcommand name (for example `vkprofiles validate --input profiles --verbose`). They are also listed in the top-level help, but when given before the subcommand they are reset by the subcommand defaults, and `--log` placed before the subcommand consumes the subcommand name as a log type. Without any of them, WARNING and ERROR messages are printed. With `--verbose` (or `--log all`/`--log info`), the execution time of the command is reported at the end.

The parser registers commands in the following order in [scripts/vkprofiles.py](vkprofiles.py): `graph`, `validate`, `schema`, `transform`, `combine`, `extract`, `min-api-version`, `library`, `doc`, `layer`, `tests`, `summary`, and `version`.

Within a graph file, `nodes[].command` is limited to the pipeline operations supported by the executor (`schema`, `transform`, `combine`, `validate`, `library`, `doc`, `extract`, `min-api-version`, `summary`, `layer`, and `tests`). Top-level `graph` and `version` are CLI entry points, not valid node commands.

---

## Subcommands

### 1. `graph`

Executes a dependency-driven pipeline graph of `vkprofiles` commands from a JSON file. This is useful for automating profile generation, validation, document generation, and library generation in one workflow.

```bash
vkprofiles graph --registry vk.xml --input path/to/graph.json
```

* `--input`, `-i`: *(Required)* Path to the graph execution JSON configuration file.
* `--registry`, `-r`: *(Required)* Shared Vulkan registry file (`vk.xml`) applied to every step in the graph. It overrides any `registry` value set in node `args`.

`--quiet` and `--verbose` given to `graph` are forwarded to every step.

#### Graph JSON Schema

The graph file format is defined by `GRAPH_JSON_SCHEMA` in [scripts/source/graph_schema.py](source/graph_schema.py), a JSON Schema draft-07 document with `$id` `https://schema.khronos.org/vulkan/profiles-graph.json#`. The graph file is validated against it before any node runs; if validation fails, `graph` exits with an error and nothing is executed.

**Top-level object** (`additionalProperties: false`):

| Property | Type | Required | Description |
| --- | --- | --- | --- |
| `$schema` | string | No | Schema identifier for the graph file itself. |
| `variables` | object of strings | No | Global variables, referenced in node `args` as `${name}`. |
| `nodes` | array of node objects | Yes | Execution nodes. |

**Node object** (`additionalProperties: false`):

| Property | Type | Required | Description |
| --- | --- | --- | --- |
| `id` | string | Yes | Unique node identifier. |
| `description` | string | No | Human-readable description, printed in the step log. |
| `ignore` | boolean | No | When `true`, the node is skipped (logged as `[SKIPPED]`) but still counts as completed for its dependents. |
| `depends_on` | array of strings | No | Ids of nodes that must run before this one. |
| `command` | string | Yes | One of `schema`, `transform`, `combine`, `validate`, `library`, `doc`, `extract`, `min-api-version`, `summary`, `layer`, `tests`. |
| `args` | object | Yes | Arguments for `command`, validated against the matching `*_args` definition below. |

A `oneOf` on the node object binds each `command` value to its `args` definition (`schema` → `schema_args`, `min-api-version` → `min_api_version_args`, and so on). Every `*_args` definition sets `additionalProperties: false`, so a key that is not listed for that command fails validation. Keys are the long CLI flag names without the leading `--` (for example `input-profiles`, not `input_profiles`).

**Arguments accepted per command:**

| `command` | `args` keys |
| --- | --- |
| `schema` | `registry`, `input`, `output`, `upgrade`, `format`, `api` |
| `transform` | `registry`, `input`, `input-profiles`, `output`, `format`, `pull`, `consolidate`, `strip`, `sort`, `api` |
| `combine` | `registry`, `input`, `input-profiles`, `output`, `profile-name`, `profile-version`, `profile-label`, `profile-desc`, `profile-date`, `profile-api-version`, `profile-stage`, `profile-required-profiles`, `mode`, `format`, `contributors`, `history`, `profiles`, `api` |
| `validate` | `registry`, `input`, `schema`, `analyse`, `api` |
| `library` | `registry`, `input`, `input-profiles`, `output`, `output-inc`, `output-src`, `output-filename`, `mode`, `strip`, `intermediate`, `debug`, `config`, `include-header`, `api` |
| `doc` | `registry`, `input`, `input-profiles`, `output`, `api` |
| `extract` | `registry`, `input`, `output`, `input-profiles`, `mode`, `format` |
| `min-api-version` | `registry`, `input`, `input-profiles`, `output`, `schemas`, `mode`, `format` |
| `summary` | `registry`, `input`, `input-profiles`, `output` |
| `layer` | `registry`, `output`, `out-layer`, `api` |
| `tests` | `registry`, `output-profile`, `output-cpp`, `api` |

Every `*_args` definition also accepts `verbose` (boolean), `quiet` (boolean), and `log` (array of `none`, `all`, `info`, `warning`, `error`, `critical`). The schema does not mark any `args` key as required; missing required CLI options are reported by the subcommand when the node runs. `registry` is accepted everywhere but is always replaced by the `graph --registry` value.

**Value constraints** (shared definitions and inline enums):

| Key | Schema type and allowed values |
| --- | --- |
| `api` | string: `vulkan` |
| `format` | `output_format`: `pretty`, `flatten`, `compact` |
| `pull` (`transform`) | array of `pull_bits`: `requirements`, `promoted`, `aliases`, `ignore-extension-versions`, `ignore-unsupported`, `ignore-subcore`, `ignore-unratified` |
| `strip` (`transform`) | array of `strip_bits`: `helper-values`, `duplication`, `promoted-extensions`, `unratified` |
| `strip` (`library`) | boolean |
| `analyse` (`validate`) | array of `analyse_bits`: `api-version`, `extensions`, `aliases`, `unratified`, `subcore` |
| `mode` (`combine`) | string: `intersection`, `union`, `difference`, `limittype` |
| `mode` (`library`) | array of `header-only`, `header+source` |
| `mode` (`extract`) | string: `reference-required-profiles`, `pull-required-profiles` |
| `mode` (`min-api-version`) | string: `display`, `evaluate` |
| `profile-stage` (`combine`) | string: `ALPHA`, `BETA`, `STABLE` |
| `profile-version` (`combine`) | string or integer |
| `config` (`library`) | string: `release`, `debug` |
| `contributors`, `profiles` (`combine`) | object |
| `history` (`combine`) | array, object, or string |
| `upgrade`, `consolidate`, `sort`, `debug` | boolean |
| All other keys | string |

> [!NOTE]
> The schema accepts `format: "compact"` and combine `mode: "limittype"`, but the CLI parsers do not, so a node using either value passes graph validation and then fails when its subcommand parses its arguments.

#### Node Execution

Graph node arguments are not a custom DSL: they are translated directly into argv for the underlying subcommand. The executor in [scripts/source/main_graph.py](source/main_graph.py) converts each key/value pair into CLI flags, with lists expanded into space-separated values, boolean `true` mapped to a bare flag, and `false`/`null` omitted. For example, `"pull": ["aliases", "requirements"]` becomes `--pull aliases requirements`, while `"sort": true` becomes `--sort`.

String values in `args`, including strings nested in lists and objects, support two kinds of substitution:

* `${name}` is replaced by the value of `name` in the top-level `variables` map.
* `$node_id.attribute` is replaced by the resolved value of `attribute` in the `args` of an earlier node, for example `$desktop_baseline.output`. Only attribute names made of letters, digits, and underscores can be referenced this way, so hyphenated keys such as `input-profiles` cannot. Referencing a node this way also adds it as an implicit dependency.

Nodes run in dependency order (from `depends_on` and `$node_id.` references); a dependency cycle aborts the graph before any node runs.

In addition, a `combine` node can embed a batch combine configuration directly in `args` using `profiles`, `contributors`, and `history`. When `profiles` is present, the executor does not call the `combine` CLI: it combines each entry of `profiles` (keyed by profile name, with `input`, `version`, `label`, `description`, `date`, `stage`, `api-version`, and `required-profiles`) and writes all of them into the single `output` file, using the node `mode` and `format`. This is especially useful for multi-profile generation pipelines.

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
        "analyse": ["api-version", "extensions", "aliases", "unratified"]
      }
    },
    {
      "id": "library_profiles",
      "description": "Generate the Vulkan API library for the baseline profiles",
      "depends_on": ["validate_profiles"],
      "command": "library",
      "args": {
        "input": "profiles",
        "input-profiles": "VP_KHR_roadmap,VP_LUNARG_desktop_baseline",
        "output": "library/include/vulkan",
        "output-src": "library/source",
        "output-filename": "vulkan_profiles",
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
        "input-profiles": "VP_KHR_roadmap,VP_LUNARG_desktop_baseline",
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
* `--registry`, `-r`: Path to `vk.xml` (used to generate schema if `--schema` is omitted and required for static analysis checks).
* `--schema`, `-s`: Path to an explicit JSON schema file (`profiles-*.json`). By default, generates a schema from `vk.xml`.
* `--analyse [CHECK ...]`: Optional static analysis checks to execute (`api-version`, `extensions`, `aliases`, `unratified`, `subcore`). Values may be comma-separated or space-separated. If specified without values, all checks run.
* `--api`: Target API variant (`vulkan`). Default: `vulkan`.

#### Static Analysis Checks (`--analyse`)

| Check Value | Description |
| --- | --- |
| `api-version` | Verifies inherited required profiles and profile API version constraints stay compatible with Vulkan versioning rules. |
| `extensions` | Checks extension declarations, core structure coverage, and extension-related compatibility against `vk.xml`. |
| `aliases` | Validates member consistency across aliased capability structures. |
| `unratified` | Flags enabled extensions or structures that are not ratified for Vulkan. |
| `subcore` | Flags profile capability values (features/properties) below the Vulkan core specification requirements for the profile API version. |

Schema validation always runs by default. Static analysis is enabled only when `--analyse` is supplied.

**Example (Using explicit external schema and running all analysis checks):**

```bash
vkprofiles validate \
    --api vulkan \
    --registry vk.xml \
    --schema schema/profiles-0.8-latest.json \
    --input profiles/LunarG \
    --analyse
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
* `--input-profiles`: Comma-separated list of profile names to write to the output. All input profiles are loaded and transformed so that dependencies resolve, then only the listed profiles are kept.
* `--output`, `-o`: *(Required)* Path to output directory or file.
* `--registry`, `-r`: Path to `vk.xml`.
* `--api`: Target API variant (`vulkan`). Default: `vulkan`.
* `--format`: Output formatting style (`flatten` or `pretty`). Default: `pretty`.
* `--pull [OPTION ...]`: Pull capability options: `requirements`, `promoted`, `aliases`, `ignore-extension-versions`, `ignore-unsupported`, `ignore-subcore`, or `ignore-unratified`.
* `--consolidate`: Consolidate capabilities.
* `--strip [OPTION ...]`: Strip options: `helper-values`, `duplication`, `promoted-extensions`, or `unratified`.
* `--sort`: Sort profile capabilities.

#### Transformation Options

Conversion flags are processed in a fixed internal order regardless of the order specified on the command line: `--pull requirements`, `--pull promoted`, `--pull aliases`, `--consolidate`, `--strip helper-values`, `--strip duplication`, `--strip promoted-extensions`, `--strip unratified`, then `--sort`. The `ignore-*` pull values do not run a step on their own; they modify the behavior of the `requirements`, `promoted`, and `aliases` pull steps.

| Option value | Description |
| --- | --- |
| `--pull requirements` | Pulls extension dependencies, then evaluates them and pulls satisfied core/extension feature and property requirements into capability blocks. |
| `--pull promoted` | Requires all extensions promoted to core up to the profile's target Vulkan version. |
| `--pull aliases` | Resolves and populates equivalent capability aliases across core structures and extensions. |
| `--pull ignore-extension-versions` | Sets all required extension versions to 1, overriding specific extension spec versions. |
| `--pull ignore-unsupported` | Skips pulling default values for feature-dependent properties when the corresponding feature is unsupported. |
| `--pull ignore-subcore` | Allows `requirements` pulling to override profile capability values that are below Vulkan core specification requirements. |
| `--pull ignore-unratified` | Skips pulling unratified extensions and capability aliases. |
| `--consolidate` | Combines capability blocks into a consolidated requirements block per profile. |
| `--strip helper-values` | Removes non-bit-position bitmask helper values (composites, all-flags, and zero/none constants) from capability blocks. |
| `--strip duplication` | Removes redundant duplicate features, properties, and extension requirements across inheritance trees and within blocks. |
| `--strip promoted-extensions` | Removes extensions already promoted to the profile's target core Vulkan version. |
| `--strip unratified` | Removes extensions and extension structures that are not ratified for Vulkan. |
| `--sort` | Sorts profile capabilities into canonical Vulkan order. |

**Example:**

```bash
vkprofiles transform \
    --registry vk.xml \
    --input profiles/LunarG \
    --output profiles/generated \
    --pull requirements aliases ignore-unratified \
    --strip duplication promoted-extensions \
    --consolidate
```

---

### 5. `combine`

Combines multiple profile JSON files into a single output profile JSON file using one of the supported combination modes: `intersection`, `union`, or `difference`.

```bash
vkprofiles combine --registry vk.xml --input path/to/profiles --output path/to/combined.json [options]
```

* `--api`: Target API variant (`vulkan`). Default: `vulkan`.
* `--registry`, `-r`: *(Required)* Path to `vk.xml`.
* `--input`, `-i`: *(Required)* Directory containing profiles to combine.
* `--input-profiles`: Comma-separated list of profile names to combine.
* `--output`, `-o`: *(Required)* Output JSON file path.
* `--mode`, `-m`: Combination mode (`intersection`, `union`, or `difference`). Default: `intersection`.
* `--format`: Output formatting style (`flatten` or `pretty`). Default: `pretty`.
* `--profile-name`: Override output profile name.
* `--profile-version`: Set profile version number. Default: `1`.
* `--profile-label`: Set profile label string.
* `--profile-desc`: Set profile description string.
* `--profile-date`: Set profile release date (`YYYY-MM-DD`).
* `--profile-api-version`: Set target Vulkan API version (e.g., `1.3.280`).
* `--profile-stage`: Set development stage (`ALPHA`, `BETA`, `STABLE`). Default: `STABLE`.
* `--profile-required-profiles`: Comma-separated list of required profiles by the generated profile.

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

Extracts one or more named profiles from a larger profile JSON file or directory into a standalone output profile JSON file.

```bash
vkprofiles extract --input path/to/input.json --input-profiles PROFILE_NAME[,PROFILE_2] --output path/to/output.json [options]
```

* `--input`, `-i`: *(Required)* Path to input profile JSON file or directory.
* `--input-profiles`: *(Required)* Comma-separated list of profile names to extract.
* `--output`, `-o`: *(Required)* Path to output profile JSON file.
* `--registry`, `-r`: Path to `vk.xml` (accepted for graph consistency; not used).
* `--mode`, `-m`: Extraction mode. Default: `reference-required-profiles`.
  * `reference-required-profiles`: Keeps required parent profiles as external references.
  * `pull-required-profiles`: Includes the required parent profiles and their capability blocks in the output file.
* `--format`: Output formatting style (`flatten` or `pretty`). Default: `pretty`.

**Example:**

```bash
vkprofiles extract \
    --input profiles/LunarG \
    --input-profiles VP_LUNARG_desktop_baseline,VP_LUNARG_desktop_max_2026 \
    --output profiles/extracted/VP_LUNARG_desktop_selected.json \
    --mode reference-required-profiles
```

---

### 7. `min-api-version`

Displays the profile `api-version` and schema URI from input JSON, or evaluates the minimum compatible Vulkan profile schema version and updates profile JSON files as needed.

```bash
vkprofiles min-api-version --input path/to/profiles [options]
```

* `--input`, `-i`: *(Required)* Path to input profiles file or directory.
* `--input-profiles`: Comma-separated list of profile names to process.
* `--output`, `-o`: Path to output profiles file or directory when updating JSONs in `evaluate` mode.
* `--registry`, `-r`: Path to `vk.xml` (used in `evaluate` mode).
* `--schemas`: Path to a directory containing Vulkan profile schemas (`profiles-*.json`).
* `--mode`, `-m`: Operation mode: `display` or `evaluate`. Default: `display`.
* `--format`: Output formatting style (`flatten` or `pretty`). Default: `pretty`.

#### `--mode` behavior

| Mode Value | Description |
| --- | --- |
| `display` | Prints each profile's `api-version` and the schema URI read from the input JSON. Output is emitted at INFO level, so add `--verbose`. |
| `evaluate` | Checks the profile against the available schema set, determines the minimum Vulkan header version that satisfies the data, and optionally updates the JSON in the output path. |

**Example (display values):**

```bash
vkprofiles min-api-version \
    --input profiles/LunarG \
    --input-profiles VP_LUNARG_desktop_baseline_2022,VP_LUNARG_desktop_baseline_2026 \
    --mode display \
    --verbose
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
> To ensure the generated library creates valid `VkDevice` instances, use `--strip` when generating the library to remove redundant capability structures.

```bash
vkprofiles library --registry vk.xml --input path/to/profiles --output path/to/include [options]
```

* `--api`: Target API variant (`vulkan`). Default: `vulkan`.
* `--registry`, `-r`: *(Required)* Path to `vk.xml`.
* `--input`, `-i`: *(Required)* Directory containing input profile JSON files.
* `--input-profiles`: Comma-separated list of profile names to process.
* `--output`, `-o`, `--output-inc`: Target header output directory.
* `--output-src`: Target source output directory. If omitted in `header+source` mode, defaults to `--output`.
* `--output-filename`: Base filename for generated files. Default: `vulkan_profiles`.
* `--mode [MODES ...]`: Library generation modes (`header-only`, `header+source`). Default: both modes are generated.
* `--strip`: Strip redundant capability structures before generating the library to keep `VkDevice` creation valid. This is equivalent to running `transform --pull aliases --strip duplication helper-values` on the input profiles first.
* `--intermediate`: Directory path where the transformed profile JSON files are saved (only used with `--strip`).
* `--debug`, `-d`: Generate the library with debug messages. Same effect as `--config debug`.
* `--config`, `-c`: Build configuration (`release` or `debug`). Default: `release`.

When `--input-profiles` is set, every required parent profile of a listed profile must also be listed, otherwise the command fails.
* `--include-header`: Override the header file include directive in generated C++ source files.

**Example (Header + Source with automatic conversion):**

```bash
vkprofiles library \
    --api vulkan \
    --registry vk.xml \
    --input profiles \
  --input-profiles "VP_KHR_roadmap_2022,VP_KHR_roadmap_2024,VP_KHR_roadmap_2026,VP_LUNARG_desktop_baseline_2022,VP_LUNARG_desktop_baseline_2023,VP_LUNARG_desktop_baseline_2024,VP_LUNARG_desktop_baseline_2026" \
    --output library/include/vulkan \
    --output-src library/source \
    --output-filename vulkan_profiles \
    --mode header+source \
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
* `--input-profiles`: Comma-separated list of profile names to document.
* `--output`, `-o`: *(Required)* Output Markdown file path.
* `--api`: Target API variant (`vulkan`). Default: `vulkan`.

**Example:**

```bash
vkprofiles doc \
    --api vulkan \
    --registry vk.xml \
    --input profiles \
    --input-profiles "VP_KHR_roadmap,VP_LUNARG_minimum_requirements" \
    --output PROFILES.md
```

---

### 10. `summary`

Lists the profiles found in a profile JSON file or directory as a table sorted by profile name, with columns for profile version, Vulkan API version, schema header version, and source filename.

```bash
vkprofiles summary --input path/to/profiles [options]
```

* `--input`, `-i`: *(Required)* Path to input profile JSON file or directory.
* `--input-profiles`: Comma-separated list of profile names to summarize.
* `--output`, `-o`: Write the summary table to this file instead of printing it to stdout.
* `--registry`, `-r`: Path to `vk.xml` (accepted for graph consistency; not used).

**Example:**

```bash
vkprofiles summary \
    --input profiles/LunarG
```

---

### 11. `version`

Prints the `vkprofiles` version string based on the Vulkan Headers version used when building the tool.

```bash
vkprofiles version
```

**Example:**

```bash
vkprofiles version
```

---

## Tool Developer Subcommands

> [!NOTE]
> **Internal Tool Developer Commands**
> The `layer` and `tests` subcommands are designed exclusively for developers maintaining the Vulkan Profiles repository to generate layer and test code.

### 12. `layer` *(Internal)*

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

### 13. `tests` *(Internal)*

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
