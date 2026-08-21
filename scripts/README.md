# `vkprofiles` Command-Line Tool

`vkprofiles` is a command-line utility for working with Vulkan Profiles JSON files, generating JSON schemas, merging profiles, building C/C++ API libraries, and generating documentation.

```bash
vkprofiles <command> [options]
```

---

## Subcommands

### 1. `convert`

Converts implicit profile JSON files to explicit profile JSON files by pulling Vulkan capabilities, extension dependencies, and feature aliases directly from `vk.xml`.

```bash
vkprofiles convert --registry vk.xml --input path/to/input_dir --output path/to/output_dir [options]
```

* `--input`, `-i`: *(Required)* Path to input profile JSON file or directory.
* `--output`, `-o`: *(Required)* Path to output directory or file.
* `--registry`, `-r`: Path to `vk.xml`.
* `--api`: Target API variant (`vulkan`). Default: `vulkan`.
* `--format`: Output formatting style (`flatten` or `tree`). Default: `flatten`.
* `--mode`: Space-separated list of conversion capabilities to apply. Default: all flags.
* `--validate`, `-v`: Validate profile files against schema prior to conversion.

#### Conversion Mode Flags (`--mode`)

Conversion flags are processed in a deterministic internal pipeline order regardless of the order specified on the command line. This multi-phase sequence ensures that all extension dependencies and core promotions are populated first, structural feature/property/format aliases are subsequently expanded across all required structures, redundant inherited definitions are stripped, and capability blocks are consolidated as the final step.

| Mode Value | Description |
| --- | --- |
| `pull-dependences` | Queries `vk.xml` to automatically pull in and append all dependent extensions required by any extensions listed in the capability blocks.

 |
| `pull-promoted-extensions` | Adds version-specific `vulkan1Xpulledrequirements` capability blocks containing all extensions that were promoted into the target Vulkan core version.

 |
| `pull-aliases` | Automatically expands structural capability aliases (features, properties, and format flags) across core version bundle structures and extension structs (e.g., mapping `VkPhysicalDevice16BitStorageFeatures` to `VkPhysicalDeviceVulkan11Features`).

 |
| `strip-duplication` | Recursively removes duplicate extension requirements, features, properties, and format flags that are already satisfied by parent profiles in the inheritance chain.

 |
| `consolidate` | Combines all mandatory capability blocks across a profile into a single consolidated requirement block (`<profile_name>_requirements`).

 |
| `ignore-extension-versions` | Modifier flag: Forces all required extension spec versions to `1`, ignoring specific extension version numbers during extension pulling.

 |

**Example:**

```bash
vkprofiles convert \
    --registry vk.xml \
    --input profiles/LunarG \
    --output profiles/generated \
    --mode pull-dependences pull-aliases strip-duplication \
    --validate
```

---

### 2. `validate`

Validates one or more profile JSON files against the Vulkan Profiles JSON schema.

```bash
vkprofiles validate --registry vk.xml --input path/to/profiles [options]
```

* `--input`, `-i`: *(Required)* Path to profile JSON file or directory to validate.
* `--registry`, `-r`: Path to `vk.xml` (used to generate a schema if `--schema` is omitted).
* `--schema`, `-s`: Path to an explicit JSON schema file (`profiles-*.json`).
* `--api`: Target API variant (`vulkan`). Default: `vulkan`.

**Example:**

```bash
vkprofiles validate \
    --api vulkan \
    --registry vk.xml \
    --input profiles/LunarG
```

---

### 3. `schema`

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
* `--profile-name`: Override output profile name.
* `--profile-version`: Set profile version number. Default: `1`.
* `--profile-label`: Set profile label string.
* `--profile-desc`: Set profile description string.
* `--profile-date`: Set profile release date (`YYYY-MM-DD`).
* `--profile-api-version`: Set target Vulkan API version (e.g., `1.3.280`).
* `--profile-stage`: Set development stage (`ALPHA`, `BETA`, `STABLE`). Default: `STABLE`.
* `--strip-duplicate-structs`: Strip duplicate structure entries.

**Example (Creating `VP_LUNARG_desktop_baseline.json`):**
In the repository build process, `VP_LUNARG_desktop_baseline.json` is constructed by calculating the intersection of historical desktop baseline profiles listed in `VP_LUNARG_desktop_baseline_config.json`:

```bash
vkprofiles merge \
    --registry vk.xml \
    --config profiles/LunarG/VP_LUNARG_desktop_baseline_config.json \
    --output profiles/LunarG/VP_LUNARG_desktop_baseline.json \
    --strip-duplicate-structs
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
> **Device Creation Requirements**
> Generating a Vulkan Profiles API library suitable for creating a `VkDevice` instance requires that the input profile JSON files contain no duplicate Vulkan feature structures. The Vulkan specification prohibits passing superseded or duplicate feature structures simultaneously in the `pNext` chain during device creation.
> If the source profile JSON files contain redundant or unexpanded feature structures, developers can use the `--convert` flag with `pull-aliases` and `strip-duplication` modes during library generation to automatically sanitize, expand, and deduplicate feature structures before C/C++ code generation.

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
* `--output-schema`: Output file path for generated JSON schema.
* `--convert`: Apply profiles data conversion (e.g., `pull-aliases`, `strip-duplication`, `pull-dependences`) prior to generation.
* `--intermediate`: Directory path for intermediate converted JSON files (used with `--convert`).
* `--validate`, `-v`: Validate profiles against schema during generation.
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
* `--validate`, `-v`: Validate profile JSON files before generating documentation.

**Example:**

```bash
vkprofiles doc \
    --api vulkan \
    --registry vk.xml \
    --input profiles \
    --input-filenames "VP_KHR_roadmap.json,VP_LUNARG_minimum_requirements.json" \
    --output PROFILES.md
```

---

## Tool Developer Subcommands

> [!NOTE]
> **Internal Tool Developer Commands**
> The `layer` and `tests` subcommands are designed exclusively for developers maintaining the Vulkan Profiles repository to generate layer and test code.
> 
> 

### 7. `layer` *(Internal)*

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

### 8. `tests` *(Internal)*

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
