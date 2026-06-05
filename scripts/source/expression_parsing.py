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

from enum import Enum
from enum import IntEnum
import re


# --- Updated Enum Definition ---
class VK_VERSION(Enum):
    NONE = "VK_NONE"
    V1_0 = "VK_VERSION_1_0"
    V1_1 = "VK_VERSION_1_1"
    V1_2 = "VK_VERSION_1_2"
    V1_3 = "VK_VERSION_1_3"
    V1_4 = "VK_VERSION_1_4"

    @classmethod
    def from_string(cls, version_str: str) -> "VK_VERSION":
        # 1. Split the version string (e.g., "1.3.276" -> ["1", "3", "276"])
        parts = version_str.split('.')
        
        # 2. Extract major and minor, ignoring the revision
        major, minor = parts[0], parts[1]
        
        # 3. Construct the matching Enum value string (e.g., "VK_VERSION_1_3")
        target_value = f"VK_VERSION_{major}_{minor}"
        
        return cls(target_value)
        

# Establish sequential hierarchy based on the Enum's definition order
VERSION_ORDER = list(VK_VERSION)


class Node:
    def __init__(self, value, left=None, right=None):
        self.value = value
        self.left = left
        self.right = right

    def is_leaf(self):
        return self.left is None and self.right is None


class ExpressionTree:
    def __init__(self, expression_str: str):
        self.tokens = re.findall(r'[\w_]+|[()+,]', expression_str)
        self.index = 0
        self.root = self._parse_expression()

    def _peek(self):
        return self.tokens[self.index] if self.index < len(self.tokens) else None

    def _consume(self):
        tok = self._peek()
        if tok: self.index += 1
        return tok

    def _parse_expression(self) -> Node:
        node = self._parse_primary()
        while self._peek() in ('+', ','):
            op = self._consume()
            right_node = self._parse_primary()
            node = Node(value=op, left=node, right=right_node)
        return node

    def _parse_primary(self) -> Node:
        tok = self._peek()
        if tok == '(':
            self._consume()
            node = self._parse_expression()
            if self._peek() == ')': self._consume()
            return node
        return Node(value=self._consume())


# def collect_extensions(current_version: VK_VERSION, expression_str: str) -> list:
#     """
#     Parses the expression tree and returns the mandatory extension strings 
#     required given the system's current string-backed Vulkan version enum.
#     """
    
#     # GUARD CLAUSE: Handle None, empty strings, or whitespace-only strings gracefully
#     if not expression_str or not expression_str.strip():
#         return []
    
#     tree = ExpressionTree(expression_str)

#     def _evaluate(node):
#         if not node:
#             return False

#         # Base case: Leaf Nodes
#         if node.is_leaf():
#             try:
#                 # Try to map the string token directly to the VK_VERSION Enum Value
#                 token_ver = VK_VERSION(node.value)
#                 # Compare their hierarchical position in the ordered list
#                 return VERSION_ORDER.index(current_version) >= VERSION_ORDER.index(token_ver)
#             except ValueError:
#                 # If it's not a valid version string, treat it as an extension requirement
#                 return [node.value]

#         left_res = _evaluate(node.left)
#         right_res = _evaluate(node.right)

#         # Logical AND (+) handling
#         if node.value == '+':
#             if left_res is False or right_res is False:
#                 return False
#             if left_res is True: return right_res
#             if right_res is True: return left_res
#             return left_res + right_res

#         # Logical OR (,) handling
#         elif node.value == ',':
#             if left_res is True or right_res is True:
#                 return True
#             if left_res is False: return right_res
#             if right_res is False: return left_res
#             return left_res

#         return False

#     result = _evaluate(tree.root)
#     return [] if isinstance(result, bool) else result

def collect_extensions(current_version: VK_VERSION, expression_str: str) -> list:
    """
    Parses the expression tree and returns the mandatory extension strings 
    required given the system's current string-backed Vulkan version enum.
    """
    # GUARD CLAUSE: Handle None, empty strings, or whitespace-only strings gracefully
    if not expression_str or not expression_str.strip():
        return []
    
    tree = ExpressionTree(expression_str)
    errors = []  # To track version mismatches during evaluation

    def _evaluate(node):
        if not node:
            return False

        # Base case: Leaf Nodes
        if node.is_leaf():
            try:
                # Try to map the string token directly to the VK_VERSION Enum Value
                token_ver = VK_VERSION(node.value)
                
                # FEATURE 2: If current_version is NONE, bypass the version check entirely
                if current_version == VK_VERSION.NONE:
                    return True
                
                # Compare their hierarchical position in the ordered list
                if VERSION_ORDER.index(current_version) < VERSION_ORDER.index(token_ver):
                    # FEATURE 1: Record the error message in case the evaluation fails
                    errors.append(
                        f"ERROR: Invalid Profiles File, the required Vulkan version '{current_version.value}' by the profile is older than the requied Vulkan version '{token_ver.value}' by an extension"
                    )
                    return False
                return True
                
            except ValueError:
                # If it's not a valid version string, treat it as an extension requirement
                return [node.value]

        left_res = _evaluate(node.left)
        right_res = _evaluate(node.right)

        # Logical AND (+) handling
        if node.value == '+':
            if left_res is False or right_res is False:
                return False
            if left_res is True: return right_res
            if right_res is True: return left_res
            return left_res + right_res

        # Logical OR (,) handling
        elif node.value == ',':
            if left_res is True or right_res is True:
                return True
            if left_res is False: return right_res
            if right_res is False: return left_res
            return left_res

        return False

    result = _evaluate(tree.root)
    
    # If evaluation yields False, it means the version requirements completely failed.
    # Print the logged errors without crashing.
    if result is False and errors:
        for err in errors:
            print(err)

    return [] if isinstance(result, bool) else result
