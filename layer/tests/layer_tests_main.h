/*
 * Copyright (c) 2015-2022 The Khronos Group Inc.
 * Copyright (c) 2015-2022 Valve Corporation
 * Copyright (c) 2015-2022 LunarG, Inc.
 * Copyright (c) 2015-2022 Google, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Author: Mark Lobodzinski <mark@lunarg.com>
 */

#pragma once

#ifdef __ANDROID__
#include "vulkan_wrapper.h"
#else
#include <vulkan/vulkan.h>
#endif

#include "gtest/gtest.h"

#ifdef __ANDROID__
#include <android/log.h>
#if defined(PROFILES_LAYER_APK)
#include <android_native_app_glue.h>
#endif
#endif

static const char kSkipPrefix[] = "             TEST SKIPPED:";

void print_android(const char *c);

#ifdef __ANDROID__
void ReadAssetFile(android_app *app);
#endif