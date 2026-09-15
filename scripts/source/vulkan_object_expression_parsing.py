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
import re
from typing import Callable
from source.vulkan_object_version import VK_VERSION

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


def collectExtensions(current_version: VK_VERSION, expression_str: str, extension_name: str, profile_name: str) -> list:
    """
    Parses the expression tree and returns the mandatory extension strings 
    required given the system's current string-backed Vulkan version enum.
    """
    if not expression_str or not expression_str.strip():
        return []
    
    VERSION_ORDER = list(VK_VERSION)
    
    tree = ExpressionTree(expression_str)
    errors = []

    def _evaluate(node):
        if not node:
            return False

        if node.is_leaf():
            try:
                token_ver = VK_VERSION(node.value)
                
                if current_version == VK_VERSION.NONE:
                    return True
                
                if VERSION_ORDER.index(current_version) < VERSION_ORDER.index(token_ver):
                    by_ext = f"by extension '{extension_name}'" if extension_name else "by an extension"
                    prefix = f"Profile '{profile_name}'" if profile_name else "Invalid Profiles File"
                    errors.append(
                        f"{prefix}, the required Vulkan version '{current_version.value}' by the profile is older than the required Vulkan version '{token_ver.value}' {by_ext}"
                    )
                    return False
                return True
                
            except ValueError:
                return [node.value]

        left_res = _evaluate(node.left)
        right_res = _evaluate(node.right)

        if node.value == '+':
            if left_res is False or right_res is False:
                return False
            if left_res is True: return right_res
            if right_res is True: return left_res
            return left_res + right_res

        elif node.value == ',':
            if left_res is True or right_res is True:
                return True
            if left_res is False: return right_res
            if right_res is False: return left_res
            return left_res

        return False

    result = _evaluate(tree.root)
    
    if result is False and errors:
        for err in errors:
            logging.warning(err)

    return [] if isinstance(result, bool) else result


def evalExpression(expression: str, is_symbol_enabled: Callable[[str], bool]) -> bool:
    """
    Evaluates a Vulkan XML boolean dependency expression.
    
    Operator precedence in vk.xml:
    - '+' = AND (higher precedence)
    - ',' = OR  (lower precedence)
    - '(' / ')' = Grouping
    """
    if not expression or not expression.strip():
        return True

    raw_tokens = re.split(r'([(),+])', expression)
    tokens = [t.strip() for t in raw_tokens if t and t.strip()]

    pos = 0

    def parse_expr() -> bool:
        nonlocal pos
        result = parse_term()
        while pos < len(tokens) and tokens[pos] == ',':
            pos += 1
            rhs = parse_term()
            result = result or rhs
        return result

    def parse_term() -> bool:
        nonlocal pos
        result = parse_factor()
        while pos < len(tokens) and tokens[pos] == '+':
            pos += 1
            rhs = parse_factor()
            result = result and rhs
        return result

    def parse_factor() -> bool:
        nonlocal pos
        if pos >= len(tokens):
            return False

        token = tokens[pos]
        if token == '(':
            pos += 1
            res = parse_expr()
            if pos < len(tokens) and tokens[pos] == ')':
                pos += 1
            return res
        else:
            pos += 1
            return is_symbol_enabled(token)

    return parse_expr()
