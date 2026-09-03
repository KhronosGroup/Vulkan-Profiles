# `vkprofiles` Command-Line Tool

`vkprofiles` is a command-line utility for working with Vulkan Profiles JSON files, generating JSON schemas, merging profiles, building C/C++ API libraries, and generating documentation.

```bash
vkprofiles <command> [options]
vkprofiles [--help | -h]
vkprofiles [--version | -v]
```

* `--help`, `-h`: Print help message and exit (available at top-level and for all subcommands).
* `--version`, `-v`: Print `vkprofiles` version.

---

## Subcommands

### 1. `validate`

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

### 2. `schema`

Generates a Vulkan Profiles JSON schema file from `vk.xml`.

```bash
vkprofiles schema --registry vk.xml --output path/to/schema.json [options]
```

* `--output`, `-o`: *(Required)* Output path for generated JSON schema file.
* `--registry`, `-r`: Path to `vk.xml`.
* `--api`: Target API variant (`vulkan`). Default: `vulkan`.

**Example:**

```bash
vkprofiles schema \
    --api vulkan \
    --registry vk.xml \
    --output schema/profiles-0.8-latest.json
```

---

### 3. `convert`

Converts implicit profile JSON files to explicit profile JSON files by pulling Vulkan capabilities, extension dependencies, and feature aliases directly from `vk.xml`.

```bash
vkprofiles convert --registry vk.xml --input path/to/input_dir --output path/to/output_dir [options]
```

* `--input`, `-i`: *(Required)* Path to input profile JSON file or directory.
* `--output`, `-o`: *(Required)* Path to output directory or file.
* `--registry`, `-r`: Path to `vk.xml`.
* `--api`: Target API variant (`vulkan`). Default: `vulkan`.
* `--format`: Output formatting style (`flatten` or `pretty`). Default: `flatten`.
* `--mode`: Space-separated list of conversion capabilities to apply. Default: all flags.
* `--validate`: Validate profile files before conversion (choices: `schema`, `analysis`). Default: `schema analysis`.

#### Conversion Mode Flags (`--mode`)

Conversion flags are processed in a deterministic internal pipeline order regardless of the order specified on the command line. This multi-phase sequence ensures that all extension dependencies and core promotions are populated first, structural feature/property/format aliases are subsequently expanded across all required structures, redundant inherited definitions are stripped, capability blocks are consolidated, and promoted extensions are cleaned up.

| Mode Value | Description |
| --- | --- |
| `pull-required-capabilities` | Evaluates extension dependencies and pulls satisfied core/extension feature and property requirements into capability blocks. |
| `pull-promoted-extensions` | Requires all extensions promoted to core up to the profile's target Vulkan version. |
| `ignore-extension-versions` | Sets all required extension versions to 1, overriding specific extension spec versions. |
| `pull-aliases` | Resolves and populates all equivalent capability aliases across core structures and extensions. |
| `strip-duplication` | Removes redundant duplicate features, properties, and extension requirements across inheritance trees and within blocks. |
| `consolidate` | Merges all mandatory capability blocks into a single consolidated requirements block per profile. |
| `strip-promoted-extensions` | Removes extensions that are already promoted to the profile's target core Vulkan version. |
| `sort` | Sorts capability blocks, structures, and extension lists into canonical Vulkan order. |

**Example:**

```bash
vkprofiles convert \
    --registry vk.xml \
    --input profiles/LunarG \
    --output profiles/generated \
    --mode pull-required-capabilities pull-aliases strip-duplication strip-promoted-extensions \
    --validate
```

---

### 4. `merge`

Combines multiple profile JSON files into a single merged profile JSON file via `intersection` or `union`.

```bash
vkprofiles merge --registry vk.xml --input path/to/profiles --output path/to/merged.json [options]
```

* `--registry`, `-r`: *(Required)* Path to `vk.xml`.
* `--output`, `-o`: *(Required)* Output JSON file path.
* `--input`, `-i`: Directory path containing profiles to merge.
* `--config`, `-c`: Path to JSON merge config file.
* `--mode`, `-m`: Combination mode (`intersection` or `union`). Default: `intersection`.
* `--format`: Output formatting style (`flatten` or `pretty`). Default: `pretty`.
* `--convert`: Apply conversion capabilities to the merged profile output (choices: `pull-required-capabilities`, `pull-promoted-extensions`, `ignore-extension-versions`, `pull-aliases`, `strip-duplication`, `consolidate`, `strip-promoted-extensions`, `sort`).
* `--validate`: Validate profile files before merging (choices: `schema`, `analysis`). Default: `schema analysis`.
* `--profile-name`: Override output profile name.
* `--profile-version`: Set profile version number. Default: `1`.
* `--profile-label`: Set profile label string.
* `--profile-desc`: Set profile description string.
* `--profile-date`: Set profile release date (`YYYY-MM-DD`).
* `--profile-api-version`: Set target Vulkan API version (e.g., `1.3.280`).
* `--profile-stage`: Set development stage (`ALPHA`, `BETA`, `STABLE`). Default: `STABLE`.

#### Merging Multiple Profiles into a Single Output File (`--config`)

When passing a JSON configuration file via `--config`, the `merge` command can process multiple profile definitions at once and pack them into a single output JSON file. The configuration file specifies a `profiles` object mapping target profile names (e.g. `VP_LUNARG_desktop_baseline_2022`, `VP_LUNARG_desktop_baseline_2023`, `VP_LUNARG_desktop_baseline_2024`, `VP_LUNARG_desktop_baseline_2026`) to their respective source input directories, label, description, API version, and required profiles. Root-level metadata such as `contributors` and `history` are also embedded into the final merged file.

#### Target Vulkan API Version Selection (`--profile-api-version`)

When specifying `--profile-api-version` (or setting `api-version` within a `--config` JSON file), developers must specify a Vulkan API version that exposes all Vulkan capabilities defined by the profile data. Selecting a target API version lower than what is required by any included core structure or feature will result in an invalid profile definition.

To assist developers in determining and verifying the correct API version:
* **`vkprofiles validate`**: Running static analysis (`--mode analysis`) checks the profile data against `vk.xml` and explicitly reports any capabilities or core structures that are not supported by the designated Vulkan major/minor version.
* **Published Vulkan Profiles Schemas**: Published for every Vulkan Header release in the [Khronos-Schemas repository](https://github.com/KhronosGroup/Khronos-Schemas/tree/main/vulkan), these schemas can be used to obtain detailed validation reports per Vulkan Header version.

**Example (Creating `VP_LUNARG_desktop_baseline.json` with configuration file and conversion):**

```bash
vkprofiles merge \
    --registry vk.xml \
    --config profiles/LunarG/VP_LUNARG_desktop_baseline_config.json \
    --output profiles/LunarG/VP_LUNARG_desktop_baseline.json \
    --convert pull-aliases strip-duplication \
    --validate
```

**Example (Merging Directory to Single Profile):**

```bash
vkprofiles merge \
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

### 5. `library`

Generates C/C++ Vulkan Profiles API library headers (`vulkan_profiles.h`, `vulkan_profiles.hpp`) and source file (`vulkan_profiles.cpp`).

> [!IMPORTANT]
> **Device Creation Requirements & `pNext` Chain Structure Duplication**
> When calling `vkCreateDevice`, the Vulkan specification prohibits passing duplicate or aliased feature structures simultaneously in the `VkDeviceCreateInfo` `pNext` chain. Specifically, two different structures enabling or configuring the same underlying Vulkan feature cannot both be present in `pNext`, even if their member boolean values match.
> 
> **Example:**
> Enabling `timelineSemaphore` using both `VkPhysicalDeviceTimelineSemaphoreFeaturesKHR` (from `VK_KHR_timeline_semaphore`) and `VkPhysicalDeviceVulkan12Features` (the Vulkan 1.2 core structure into which the feature was promoted) simultaneously in the `pNext` chain during `vkCreateDevice` is invalid. Vulkan drivers and Validation Layers will generate an error.
> 
> **Resolution:**
> To ensure the generated library creates valid `VkDevice` instances, input profile JSON files must not contain unexpanded or redundant feature structures across capability blocks. Developers can use the `--convert` option with `pull-aliases` and `strip-duplication` during library generation to automatically collapse aliased structures into their target core equivalents and deduplicate feature chains before C/C++ code generation.

```bash
vkprofiles library --registry vk.xml --input path/to/profiles --output path/to/include [options]
```

* `--registry`, `-r`: *(Required)* Path to `vk.xml`.
* `--input`, `-i`: *(Required)* Directory containing input profile JSON files.
* `--input-filenames`: Comma-separated list of profile filenames.
* `--output`, `-o`, `--output-inc`: Target header output directory.
* `--output-src`: Target source output directory. If omitted in `header+source` mode, defaults to `--output`.
* `--output-filename`: Base filename for generated files. Default: `vulkan_profiles`.
* `--mode`: Library generation mode (`header-only`, `header+source`). Default: `header-only` and `header+source`.
* `--convert`: Apply profiles data conversion prior to generation (choices: `pull-required-capabilities`, `pull-promoted-extensions`, `ignore-extension-versions`, `pull-aliases`, `strip-duplication`, `consolidate`, `strip-promoted-extensions`, `sort`).
* `--intermediate`: Directory path for intermediate converted JSON files (used when `--convert` is provided).
* `--validate`: Validate profiles (choices: `schema`, `analysis`) during generation. Default: `schema analysis`.
* `--debug`, `-d`: Generate debug variant of library code.
* `--config`, `-c`: Build configuration (`release` or `debug`).

**Example (Header + Source with Automatic Conversion):**

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
    --convert pull-aliases strip-duplication \
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

### 6. `doc`

Generates Markdown documentation from profile JSON files.

```bash
vkprofiles doc --registry vk.xml --input path/to/profiles --output PROFILES.md [options]
```

* `--registry`, `-r`: *(Required)* Path to `vk.xml`.
* `--input`, `-i`: *(Required)* Directory containing profile JSON files.
* `--output`, `-o`: *(Required)* Output Markdown file path.
* `--input-filenames`: Comma-separated list of profile filenames.
* `--validate`: Validate profile JSON files before generating documentation (choices: `schema`, `analysis`). Default: `schema analysis`.

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

### 7. `version`

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

### 8. `layer` *(Internal)*

Generates `profiles_generated.cpp` for the Vulkan Profiles layer (`VkLayer_khronos_profiles`).

```bash
vkprofiles layer --registry vk.xml --output layer/profiles_generated.cpp
```

* `--registry`, `-r`: Path to `vk.xml`.
* `--output`, `-o`: Output path for generated layer source file.
* `--api`: Target API variant (`vulkan`).

**Example:**

```bash
vkprofiles layer \
    --api vulkan \
    --registry vk.xml \
    --output layer/profiles_generated.cpp
```

---

### 9. `tests` *(Internal)*

Generates synthetic test profile JSON (`VP_LUNARG_test_api_generated.json`) and C++ test suite (`tests_generated.cpp`).

```bash
vkprofiles tests --registry vk.xml --output-profile path/to/test.json --output-cpp path/to/tests.cpp
```

* `--registry`, `-r`: *(Required)* Path to `vk.xml`.
* `--output-profile`: *(Required)* Output path for synthetic test profile JSON.
* `--output-cpp`: Output path for C++ test source file.
* `--api`: Target API variant (`vulkan`).

**Example:**

```bash
vkprofiles tests \
    --api vulkan \
    --registry vk.xml \
    --output-profile profiles/test/data/VP_LUNARG_test_api_generated.json \
    --output-cpp layer/tests/tests_generated.cpp
```