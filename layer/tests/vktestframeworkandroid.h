//  VK tests
//
//  Copyright (c) 2015-2021 The Khronos Group Inc.
//  Copyright (c) 2015-2021 Valve Corporation
//  Copyright (c) 2015-2021 LunarG, Inc.
//  Copyright (c) 2015-2021 Google, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef VKTESTFRAMEWORKANDROID_H
#define VKTESTFRAMEWORKANDROID_H

#include "gtest/gtest.h"
#include <android/native_window.h>


// Can be used by tests to record additional details / description of test
#define TEST_DESCRIPTION(desc) RecordProperty("description", desc)



class VkTestFramework : public ::testing::Test {
  public:
    VkTestFramework();
    ~VkTestFramework();

    static ANativeWindow *window;

    // Per-test-suite set-up.
    // Called before the first test in this test suite.
    static void SetUpTestSuite();

    // Per-test-suite tear-down.
    // Called after the last test in this test suite.
    static void TearDownTestSuite();
};

#endif
