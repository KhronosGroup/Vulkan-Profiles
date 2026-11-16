#!/usr/bin/python3
#
# Copyright (c) 2026 LunarG, Inc.
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

"""
Fill the Profiles layer manifest lists generated from the Vulkan registry.

The layer manifest template (VkLayer_khronos_profiles.json.in) declares each
generated list with a single placeholder element, which keeps the template a
valid JSON file:

    "list": [ "@JSON_EXCLUDE_DEVICE_EXTENSIONS@" ]

This script replaces each placeholder list with the matching values from vk.xml:
- @JSON_EXCLUDE_DEVICE_EXTENSIONS@: every Vulkan device extension, KHR first,
  then EXT, then the other vendors alphabetically, sorted alphabetically
  within each vendor.
- @JSON_EXCLUDE_FORMATS@: every VkFormat, without aliases and without
  VK_FORMAT_UNDEFINED, sorted by enum value.

It runs before CMake configure_file(), which only replaces the other
placeholders, such as @JSON_LIBRARY_PATH@.
"""

import argparse
import os
import re
import sys
import xml.etree.ElementTree as ET

API_NAME = 'vulkan'
EXTENSION_ENUM_BASE = 1000000000
EXTENSION_ENUM_RANGE = 1000


def is_vulkan(api_attribute: str | None) -> bool:
    """True when an 'api' or 'supported' attribute includes Vulkan. A missing attribute applies to all APIs."""
    return api_attribute is None or API_NAME in api_attribute.split(',')


def get_device_extensions(registry: ET.Element) -> list[str]:
    """Return the names of all the Vulkan device extensions, KHR first, then EXT, then the other vendors."""
    names = [
        extension.get('name')
        for extension in registry.find('extensions').findall('extension')
        if extension.get('type') == 'device' and is_vulkan(extension.get('supported'))
        and 'disabled' not in extension.get('supported', '').split(',')
    ]

    def sort_key(name: str):
        vendor = name.split('_')[1]
        group = {'KHR': 0, 'EXT': 1}.get(vendor, 2)
        return (group, vendor if group == 2 else '', name)

    return sorted(names, key=sort_key)


def get_enum_value(enum: ET.Element, extension_number: str | None) -> int:
    """Return the value of an <enum> element extending an enumerated type."""
    if enum.get('value') is not None:
        return int(enum.get('value'), 0)

    number = int(enum.get('extnumber', extension_number))
    value = EXTENSION_ENUM_BASE + (number - 1) * EXTENSION_ENUM_RANGE + int(enum.get('offset'))
    return -value if enum.get('dir') == '-' else value


def get_formats(registry: ET.Element) -> list[str]:
    """Return the names of all the Vulkan formats, without aliases and VK_FORMAT_UNDEFINED, sorted by enum value."""
    formats: dict[str, int] = {}

    # Vulkan 1.0 formats
    for enum in registry.find("enums[@name='VkFormat']").findall('enum'):
        if enum.get('alias') is None:
            formats[enum.get('name')] = get_enum_value(enum, None)

    # Formats added by Vulkan versions and extensions
    blocks = [(feature, None) for feature in registry.findall('feature') if is_vulkan(feature.get('api'))]
    blocks += [
        (extension, extension.get('number'))
        for extension in registry.find('extensions').findall('extension')
        if is_vulkan(extension.get('supported')) and 'disabled' not in extension.get('supported', '').split(',')
    ]
    for block, extension_number in blocks:
        for require in block.findall('require'):
            if not is_vulkan(require.get('api')):
                continue
            for enum in require.findall('enum'):
                if enum.get('extends') != 'VkFormat' or enum.get('alias') is not None:
                    continue
                formats.setdefault(enum.get('name'), get_enum_value(enum, extension_number))

    formats.pop('VK_FORMAT_UNDEFINED', None)
    return sorted(formats, key=lambda name: (formats[name], name))


def fill_placeholder_list(text: str, placeholder: str, values: list[str], newline: str) -> str:
    """Replace the '"list": [ "@PLACEHOLDER@" ]' JSON array of the template with the values, one per line."""
    pattern = re.compile(r'^(?P<indent>[ \t]*)"list"\s*:\s*\[\s*"' + re.escape(placeholder) + r'"\s*\]', re.MULTILINE)
    matches = list(pattern.finditer(text))
    if len(matches) != 1:
        raise ValueError(f'Expected a single "list": [ "{placeholder}" ] in the template, found {len(matches)}')

    indent = matches[0].group('indent')
    item_indent = indent + '    '
    items = (',' + newline).join(f'{item_indent}"{value}"' for value in values)
    replacement = f'{indent}"list": [{newline}{items}{newline}{indent}]'
    return text[:matches[0].start()] + replacement + text[matches[0].end():]


def main(argv: list[str]) -> int:
    parser = argparse.ArgumentParser(description='Fill the Profiles layer manifest lists generated from the Vulkan registry.')
    parser.add_argument('--registry', required=True, help='Path to vk.xml')
    parser.add_argument('--input', required=True, help='Path to the layer manifest template')
    parser.add_argument('--output', required=True, help='Path to the layer manifest template with the generated lists')
    args = parser.parse_args(argv)

    registry = ET.parse(args.registry).getroot()

    with open(args.input, 'r', encoding='utf-8', newline='') as file:
        text = file.read()
    newline = '\r\n' if '\r\n' in text else '\n'

    try:
        text = fill_placeholder_list(text, '@JSON_EXCLUDE_DEVICE_EXTENSIONS@', get_device_extensions(registry), newline)
        text = fill_placeholder_list(text, '@JSON_EXCLUDE_FORMATS@', get_formats(registry), newline)
    except ValueError as error:
        print(f'{args.input}: {error}', file=sys.stderr)
        return 1

    # Only write the output when it changes, to avoid triggering rebuilds
    try:
        with open(args.output, 'r', encoding='utf-8', newline='') as file:
            if file.read() == text:
                return 0
    except FileNotFoundError:
        pass

    # The output directory doesn't exist yet when CMake configures a new build directory
    output_dir = os.path.dirname(args.output)
    if output_dir:
        os.makedirs(output_dir, exist_ok=True)

    with open(args.output, 'w', encoding='utf-8', newline='') as file:
        file.write(text)
    return 0


if __name__ == '__main__':
    sys.exit(main(sys.argv[1:]))
