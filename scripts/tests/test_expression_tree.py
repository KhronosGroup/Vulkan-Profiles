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

from ..source.expression_parsing import collect_extensions
from ..source.expression_parsing import VK_VERSION

class TestExpressionTrees(unittest.TestCase):
    def test_collect_extensions2(self):
        O = collect_extensions(VK_VERSION.V1_1, "")
        self.assertEqual(len(O), 0)
        
        P = collect_extensions(VK_VERSION.V1_1, None)
        self.assertEqual(len(P), 0)
        
        A = collect_extensions(VK_VERSION.V1_1, "(VK_VERSION_1_1+VK_KHR_synchronization2),VK_VERSION_1_3")
        self.assertEqual(A[0], "VK_KHR_synchronization2")
        
        B = collect_extensions(VK_VERSION.V1_3, "(VK_VERSION_1_1+VK_KHR_synchronization2),VK_VERSION_1_3")
        self.assertEqual(len(B), 0)
        
        C = collect_extensions(VK_VERSION.V1_0, "(VK_VERSION_1_1+VK_KHR_synchronization2),VK_VERSION_1_3")
        self.assertEqual(len(C), 0)

        D = collect_extensions(VK_VERSION.V1_0, "VK_KHR_swapchain+VK_KHR_display")
        self.assertEqual(len(D), 2)
        self.assertEqual(D[0], "VK_KHR_swapchain")
        self.assertEqual(D[1], "VK_KHR_display")

        E = collect_extensions(VK_VERSION.V1_2, "VK_KHR_swapchain+VK_KHR_display")
        self.assertEqual(len(E), 2)
        self.assertEqual(E[0], "VK_KHR_swapchain")
        self.assertEqual(E[1], "VK_KHR_display")
        
        F = collect_extensions(VK_VERSION.V1_2, "VK_KHR_video_queue+(VK_KHR_synchronization2,VK_VERSION_1_3)")
        self.assertEqual(len(F), 2)
        self.assertEqual(F[0], "VK_KHR_video_queue")
        self.assertEqual(F[1], "VK_KHR_synchronization2")

        G = collect_extensions(VK_VERSION.V1_3, "VK_KHR_video_queue+(VK_KHR_synchronization2,VK_VERSION_1_3)")
        self.assertEqual(len(G), 1)
        self.assertEqual(G[0], "VK_KHR_video_queue")
    
        H = collect_extensions(VK_VERSION.V1_3, "VK_KHR_video_queue")
        self.assertEqual(len(H), 1)
        self.assertEqual(H[0], "VK_KHR_video_queue")
        
        I = collect_extensions(VK_VERSION.V1_0, "((VK_KHR_get_physical_device_properties2,VK_VERSION_1_1)+VK_KHR_depth_stencil_resolve),VK_VERSION_1_2")
        self.assertEqual(len(I), 2)
        self.assertEqual(I[0], "VK_KHR_get_physical_device_properties2")
        self.assertEqual(I[1], "VK_KHR_depth_stencil_resolve")
        
        J = collect_extensions(VK_VERSION.V1_1, "VK_VERSION_1_1+VK_KHR_shader_float_controls")
        self.assertEqual(len(J), 1)
        self.assertEqual(J[0], "VK_KHR_shader_float_controls")
        
        K = collect_extensions(VK_VERSION.NONE, "VK_VERSION_1_1+VK_KHR_shader_float_controls")
        self.assertEqual(len(K), 1)
        self.assertEqual(K[0], "VK_KHR_shader_float_controls")

        L = collect_extensions(VK_VERSION.V1_0, "VK_VERSION_1_1+VK_KHR_shader_float_controls")
        self.assertEqual(len(L), 0)
        
        M = collect_extensions(VK_VERSION.V1_3, "VK_KHR_swapchain+(VK_KHR_maintenance2,VK_VERSION_1_1)+(VK_KHR_image_format_list,VK_VERSION_1_2)")
        self.assertEqual(len(M), 1)
        self.assertEqual(M[0], "VK_KHR_swapchain")
     
        R = collect_extensions(VK_VERSION.V1_0, "VK_KHR_swapchain+(VK_KHR_maintenance2,VK_VERSION_1_1)+(VK_KHR_image_format_list,VK_VERSION_1_2)")
        self.assertEqual(len(R), 3)
        self.assertEqual(R[0], "VK_KHR_swapchain")   
        self.assertEqual(R[1], "VK_KHR_maintenance2")   
        self.assertEqual(R[2], "VK_KHR_image_format_list")   


if __name__ == '__main__':
    unittest.main()

