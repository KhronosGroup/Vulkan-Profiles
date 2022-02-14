/*
 * Copyright (c) 2015-2021 The Khronos Group Inc.
 * Copyright (c) 2015-2021 Valve Corporation
 * Copyright (c) 2015-2021 LunarG, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * Author: Courtney Goeltzenleuchter <courtney@LunarG.com>
 * Author: Tony Barbour <tony@LunarG.com>
 */

#ifndef VKTESTFRAMEWORK_H
#define VKTESTFRAMEWORK_H

#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#endif

#include "gtest/gtest.h"

// Can be used by tests to record additional details / description of test
#define TEST_DESCRIPTION(desc) RecordProperty("description", desc)

class VkTestFramework : public ::testing::Test {
  public:

#ifdef __ANDROID__
    static ANativeWindow *window;
#endif

  protected:
    VkTestFramework();
    virtual ~VkTestFramework() = 0;

  private:
};

#endif  // VKTESTFRAMEWORK_H
