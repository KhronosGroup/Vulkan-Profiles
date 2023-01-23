/*
 * Copyright (c) 2015-2022 The Khronos Group Inc.
 * Copyright (c) 2015-2022 Valve Corporation
 * Copyright (c) 2015-2022 LunarG, Inc.
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
 * Author: Chia-I Wu <olvaffe@gmail.com>
 * Author: Courtney Goeltzenleuchter <courtney@LunarG.com>
 * Author: Tony Barbour <tony@LunarG.com>
 * Author: Mark Lobodzinski <mark@LunarG.com>
 */

#include "vktestframework.h"
#ifdef __ANDROID__
#include <android/log.h>
#endif

#if defined(PATH_MAX) && !defined(MAX_PATH)
#define MAX_PATH PATH_MAX
#endif

#ifdef _WIN32
#define ERR_EXIT(err_msg, err_class)                    \
    do {                                                \
        MessageBox(nullptr, err_msg, err_class, MB_OK); \
        exit(1);                                        \
    } while (0)
#else  // _WIN32

#define ERR_EXIT(err_msg, err_class) \
    do {                             \
        printf(err_msg);             \
        fflush(stdout);              \
        exit(1);                     \
    } while (0)
#endif  // _WIN32

VkTestFramework::VkTestFramework() {
#ifdef _WIN32
    _set_abort_behavior(0, _WRITE_ABORT_MSG | _CALL_REPORTFAULT);
    SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOGPFAULTERRORBOX);
    _CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_FILE);
    _CrtSetReportFile(_CRT_ASSERT, _CRTDBG_FILE_STDERR);
#endif  // _WIN32
}
VkTestFramework::~VkTestFramework() {}

#ifdef __ANDROID__
// Define static elements
ANativeWindow *VkTestFramework::window = nullptr;
#endif
