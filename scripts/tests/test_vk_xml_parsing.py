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

import unittest

from vulkan_object import get_vulkan_object
import vulkan_object

from ..source.vk_xml_parsing import find_extension_version
from ..source.vk_xml_parsing import find_dependent_extensions
from ..source.expression_parsing import VK_VERSION

class TestVKXMLParsing(unittest.TestCase):
    def test_find_extension_version(self):
        vk = get_vulkan_object()

        extension_version0 = find_extension_version(vk, "VK_KHR_dynamic_rendering")
        self.assertEqual(extension_version0, 1)

        extension_version1 = find_extension_version(vk, "VK_EXT_debug_report")
        self.assertEqual(extension_version1, 10)
        
        extension_version2 = find_extension_version(vk, "VK_KHR_dedicated_allocation")
        self.assertEqual(extension_version2, 3)

    def test_find_dependent_extensions(self):
        self.maxDiff = 1024
        
        vk = get_vulkan_object()

        extensions_data = {
            "VK_KHR_dynamic_rendering": 1,
            "VK_EXT_debug_report": 10,
            "VK_KHR_dedicated_allocation": 3
        }
        
        # Case 0
        dependent_extensions0 = find_dependent_extensions(vk, VK_VERSION.V1_0, False, extensions_data)
        
        expected_extensions0 = {
            "VK_KHR_get_physical_device_properties2": 2,
            "VK_KHR_depth_stencil_resolve": 1,
            "VK_KHR_dynamic_rendering": 1,
            "VK_EXT_debug_report": 10,
            "VK_KHR_get_memory_requirements2": 1,
            "VK_KHR_dedicated_allocation": 3
        }
        
        self.assertEqual(dependent_extensions0, expected_extensions0)
       
       # Case 1
        dependent_extensions1 = find_dependent_extensions(vk, VK_VERSION.V1_1, False, extensions_data)

        expected_extensions1 = {
            "VK_KHR_depth_stencil_resolve": 1,
            "VK_KHR_dynamic_rendering": 1,
            "VK_EXT_debug_report": 10,
            "VK_KHR_dedicated_allocation": 3
        }

        self.assertEqual(dependent_extensions1, expected_extensions1)
        
        # Case 2
        
        extensions_data2 = {
            "VK_KHR_load_store_op_none": 1,
            "VK_KHR_maintenance6": 1,
            "VK_KHR_map_memory2": 1,
            "VK_KHR_shader_expect_assume": 1,
            "VK_KHR_shader_float_controls2": 1,
            "VK_KHR_shader_maximal_reconvergence": 1,
            "VK_KHR_shader_subgroup_rotate": 1,
            "VK_KHR_shader_subgroup_uniform_control_flow": 1,
            "VK_KHR_swapchain_mutable_format": 1,
            "VK_EXT_host_image_copy": 1,
            "VK_EXT_image_2d_view_of_3d": 1,
            "VK_EXT_pipeline_protected_access": 1,
            "VK_EXT_pipeline_robustness": 1,
            "VK_EXT_transform_feedback": 1
        }
        
        dependent_extensions2 = find_dependent_extensions(vk, VK_VERSION.V1_3, False, extensions_data2)

        expected_extensions2 = {
            "VK_KHR_load_store_op_none": 1,
            "VK_KHR_maintenance6": 1,
            "VK_KHR_map_memory2": 1,
            "VK_KHR_shader_expect_assume": 1,
            "VK_KHR_shader_float_controls": 4,
            "VK_KHR_shader_float_controls2": 1,
            "VK_KHR_shader_maximal_reconvergence": 1,
            "VK_KHR_shader_subgroup_rotate": 2,
            "VK_KHR_shader_subgroup_uniform_control_flow": 1,
            "VK_KHR_swapchain": 70,
            "VK_KHR_swapchain_mutable_format": 1,
            "VK_EXT_host_image_copy": 1,
            "VK_EXT_image_2d_view_of_3d": 1,
            "VK_EXT_pipeline_protected_access": 1,
            "VK_EXT_pipeline_robustness": 1,
            "VK_EXT_transform_feedback": 1
        }

        self.assertEqual(dependent_extensions2, expected_extensions2)
        
        # Case 3
        
        extensions_data3 = {
            "VK_KHR_load_store_op_none": 1,
            "VK_KHR_maintenance6": 1,
            "VK_KHR_map_memory2": 1,
            "VK_KHR_shader_expect_assume": 1,
            "VK_KHR_shader_float_controls2": 1,
            "VK_KHR_shader_maximal_reconvergence": 1,
            "VK_KHR_shader_subgroup_rotate": 2,
            "VK_KHR_shader_subgroup_uniform_control_flow": 1,
            "VK_KHR_swapchain_mutable_format": 1,
            "VK_EXT_host_image_copy": 1,
            "VK_EXT_image_2d_view_of_3d": 1,
            "VK_EXT_pipeline_protected_access": 1,
            "VK_EXT_pipeline_robustness": 1,
            "VK_EXT_transform_feedback": 1
        }
        
        dependent_extensions3 = find_dependent_extensions(vk, VK_VERSION.V1_3, True, extensions_data3)

        expected_extensions3 = {
            "VK_KHR_load_store_op_none": 1,
            "VK_KHR_maintenance6": 1,
            "VK_KHR_map_memory2": 1,
            "VK_KHR_shader_expect_assume": 1,
            "VK_KHR_shader_float_controls": 1,
            "VK_KHR_shader_float_controls2": 1,
            "VK_KHR_shader_maximal_reconvergence": 1,
            "VK_KHR_shader_subgroup_rotate": 1,
            "VK_KHR_shader_subgroup_uniform_control_flow": 1,
            "VK_KHR_swapchain": 1,
            "VK_KHR_swapchain_mutable_format": 1,
            "VK_EXT_host_image_copy": 1,
            "VK_EXT_image_2d_view_of_3d": 1,
            "VK_EXT_pipeline_protected_access": 1,
            "VK_EXT_pipeline_robustness": 1,
            "VK_EXT_transform_feedback": 1
        }

        self.assertEqual(dependent_extensions3, expected_extensions3)
        
        return
    

if __name__ == '__main__':
    unittest.main()

