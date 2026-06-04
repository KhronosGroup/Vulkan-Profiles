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
    V1_0 = "VK_VERSION_1_0"
    V1_1 = "VK_VERSION_1_1"
    V1_2 = "VK_VERSION_1_2"
    V1_3 = "VK_VERSION_1_3"
    V1_4 = "VK_VERSION_1_4"

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


def collect_extensions(current_version: VK_VERSION, expression_str: str) -> list:
    """
    Parses the expression tree and returns the mandatory extension strings 
    required given the system's current string-backed Vulkan version enum.
    """
    
    # GUARD CLAUSE: Handle None, empty strings, or whitespace-only strings gracefully
    if not expression_str or not expression_str.strip():
        return []
    
    tree = ExpressionTree(expression_str)

    def _evaluate(node):
        if not node:
            return False

        # Base case: Leaf Nodes
        if node.is_leaf():
            try:
                # Try to map the string token directly to the VK_VERSION Enum Value
                token_ver = VK_VERSION(node.value)
                # Compare their hierarchical position in the ordered list
                return VERSION_ORDER.index(current_version) >= VERSION_ORDER.index(token_ver)
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
    return [] if isinstance(result, bool) else result

# class Node:
#     """Represents a single node in the Expression Tree."""
#     def __init__(self, value, left=None, right=None):
#         self.value = value
#         self.left = left
#         self.right = right

#     def is_leaf(self):
#         return self.left is None and self.right is None


# class ExpressionTree:
#     """Generates a binary expression tree from a string formula."""
#     def __init__(self, expression_str: str):
#         self.tokens = self._tokenize(expression_str)
#         self.index = 0
#         self.root = self._parse_expression()

#     def _tokenize(self, expr_str: str):
#         # Extracts identifiers (words/numbers/underscores) and operators/parentheses
#         # It implicitly ignores any whitespace
#         return re.findall(r'[\w_]+|[()+,]', expr_str)

#     def _peek(self):
#         """Look at the current token without consuming it."""
#         if self.index < len(self.tokens):
#             return self.tokens[self.index]
#         return None

#     def _consume(self):
#         """Consume and return the current token."""
#         tok = self._peek()
#         if tok:
#             self.index += 1
#         return tok

#     def _parse_expression(self) -> Node:
#         """Parses expressions with operators '+' and ',' dynamically adjusting for left-associativity."""
#         node = self._parse_primary()
        
#         # While the next token is an operator, chain it into the tree
#         while self._peek() in ('+', ','):
#             op = self._consume()
#             right_node = self._parse_primary()
#             # The current node becomes the left child of the new operator node
#             node = Node(value=op, left=node, right=right_node)
            
#         return node

#     def _parse_primary(self) -> Node:
#         """Parses a primary factor: either an identifier or a parenthesized expression."""
#         tok = self._peek()
        
#         if tok == '(':
#             self._consume()  # Consume '('
#             node = self._parse_expression()
#             if self._peek() == ')':
#                 self._consume()  # Consume ')'
#             return node
#         else:
#             # It's a leaf node identifier (e.g., "VK_VERSION_1_3")
#             return Node(value=self._consume())

#     def print_tree(self, node=None, level=0, prefix="Root: "):
#         """Helper method to pretty-print the tree structure visually."""
#         if node is None:
#             node = self.root
            
#         if node:
#             indent = "    " * level
#             print(f"{indent}{prefix}{node.value}")
#             if not node.is_leaf():
#                 if node.left:
#                     self.print_tree(node.left, level + 1, "L-> ")
#                 if node.right:
#                     self.print_tree(node.right, level + 1, "R-> ")

