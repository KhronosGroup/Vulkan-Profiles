/*
 * Copyright (C) 2023-2023 Valve Corporation
 * Copyright (C) 2023-2023 LunarG, Inc.
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
 * Author: Christophe Riccio <christophe@lunarg.com>
 */

#include <vulkan/vulkan_core.h>

#include <gtest/gtest.h>
#include "profiles_test_helper.h"

TEST(TestsUtil, DebugAction) {
    std::vector<std::string> strings = GetDebugActionStrings(DEBUG_ACTION_MAX_ENUM);

    EXPECT_STREQ("DEBUG_ACTION_FILE_BIT", strings[0].c_str());
    EXPECT_STREQ("DEBUG_ACTION_STDOUT_BIT", strings[1].c_str());
    EXPECT_STREQ("DEBUG_ACTION_OUTPUT_BIT", strings[2].c_str());
    EXPECT_STREQ("DEBUG_ACTION_BREAKPOINT_BIT", strings[3].c_str());

    DebugActionFlags flags = GetDebugActionFlags(strings);

    EXPECT_TRUE(flags & DEBUG_ACTION_FILE_BIT);
    EXPECT_TRUE(flags & DEBUG_ACTION_STDOUT_BIT);
    EXPECT_TRUE(flags & DEBUG_ACTION_OUTPUT_BIT);
    EXPECT_TRUE(flags & DEBUG_ACTION_BREAKPOINT_BIT);
}

static std::vector<std::string> GetDebugReportStrings(DebugReportFlags flags) {
    static const char *table[] = {"DEBUG_REPORT_NOTIFICATION_BIT", "DEBUG_REPORT_WARNING_BIT", "DEBUG_REPORT_ERROR_BIT",
                                  "DEBUG_REPORT_DEBUG_BIT"};

    std::vector<std::string> result;

    for (std::size_t i = 0, n = std::size(table); i < n; ++i) {
        if (flags & (1 << i)) {
            result.push_back(table[i]);
        }
    }

    return result;
}

TEST(TestsUtil, DebugReport) {
    std::vector<std::string> strings = GetDebugReportStrings(DEBUG_REPORT_MAX_ENUM);

    EXPECT_STREQ("DEBUG_REPORT_NOTIFICATION_BIT", strings[0].c_str());
    EXPECT_STREQ("DEBUG_REPORT_WARNING_BIT", strings[1].c_str());
    EXPECT_STREQ("DEBUG_REPORT_ERROR_BIT", strings[2].c_str());
    EXPECT_STREQ("DEBUG_REPORT_DEBUG_BIT", strings[3].c_str());

    DebugReportFlags flags = GetDebugReportFlags(strings);

    EXPECT_TRUE(flags & DEBUG_REPORT_NOTIFICATION_BIT);
    EXPECT_TRUE(flags & DEBUG_REPORT_WARNING_BIT);
    EXPECT_TRUE(flags & DEBUG_REPORT_ERROR_BIT);
    EXPECT_TRUE(flags & DEBUG_REPORT_DEBUG_BIT);
}

static SimulateCapabilityFlags GetSimulateCapability(const std::vector<std::string> &values) {
    SimulateCapabilityFlags result = 0;

    for (std::size_t i = 0, n = values.size(); i < n; ++i) {
        if (values[i] == "SIMULATE_API_VERSION_BIT") {
            result |= SIMULATE_API_VERSION_BIT;
        } else if (values[i] == "SIMULATE_FEATURES_BIT") {
            result |= SIMULATE_FEATURES_BIT;
        } else if (values[i] == "SIMULATE_PROPERTIES_BIT") {
            result |= SIMULATE_PROPERTIES_BIT;
        } else if (values[i] == "SIMULATE_EXTENSIONS_BIT") {
            result |= SIMULATE_EXTENSIONS_BIT;
        } else if (values[i] == "SIMULATE_FORMATS_BIT") {
            result |= SIMULATE_FORMATS_BIT;
        } else if (values[i] == "SIMULATE_QUEUE_FAMILY_PROPERTIES_BIT") {
            result |= SIMULATE_QUEUE_FAMILY_PROPERTIES_BIT;
        }
    }

    return result;
}

static std::vector<std::string> GetSimulateCapabilityStrings(SimulateCapabilityFlags flags) {
    static const char *table[] = {"SIMULATE_API_VERSION_BIT", "SIMULATE_FEATURES_BIT", "SIMULATE_PROPERTIES_BIT",
                                  "SIMULATE_EXTENSIONS_BIT",  "SIMULATE_FORMATS_BIT",  "SIMULATE_QUEUE_FAMILY_PROPERTIES_BIT"};

    std::vector<std::string> result;

    for (std::size_t i = 0, n = std::size(table); i < n; ++i) {
        if (flags & (1 << i)) {
            result.push_back(table[i]);
        }
    }

    return result;
}

TEST(TestsUtil, SimulateCapability) {
    std::vector<std::string> strings = GetSimulateCapabilityStrings(SIMULATE_MAX_ENUM);

    EXPECT_STREQ("SIMULATE_API_VERSION_BIT", strings[0].c_str());
    EXPECT_STREQ("SIMULATE_FEATURES_BIT", strings[1].c_str());
    EXPECT_STREQ("SIMULATE_PROPERTIES_BIT", strings[2].c_str());
    EXPECT_STREQ("SIMULATE_EXTENSIONS_BIT", strings[3].c_str());
    EXPECT_STREQ("SIMULATE_FORMATS_BIT", strings[4].c_str());
    EXPECT_STREQ("SIMULATE_QUEUE_FAMILY_PROPERTIES_BIT", strings[5].c_str());

    SimulateCapabilityFlags flags = GetSimulateCapability(strings);

    EXPECT_TRUE(flags & SIMULATE_API_VERSION_BIT);
    EXPECT_TRUE(flags & SIMULATE_FEATURES_BIT);
    EXPECT_TRUE(flags & SIMULATE_PROPERTIES_BIT);
    EXPECT_TRUE(flags & SIMULATE_EXTENSIONS_BIT);
    EXPECT_TRUE(flags & SIMULATE_FORMATS_BIT);
    EXPECT_TRUE(flags & SIMULATE_QUEUE_FAMILY_PROPERTIES_BIT);
}

TEST(TestsUtil, DefaultFeatureValues) {
    std::string string_value_false = GetDefaultFeatureValuesString(DEFAULT_FEATURE_VALUES_FALSE);
    EXPECT_STREQ("DEFAULT_FEATURE_VALUES_FALSE", string_value_false.c_str());

    EXPECT_EQ(DEFAULT_FEATURE_VALUES_FALSE, GetDefaultFeatureValues("DEFAULT_FEATURE_VALUES_FALSE"));

    std::string string_value_device = GetDefaultFeatureValuesString(DEFAULT_FEATURE_VALUES_DEVICE);
    EXPECT_STREQ("DEFAULT_FEATURE_VALUES_DEVICE", string_value_device.c_str());

    EXPECT_EQ(DEFAULT_FEATURE_VALUES_DEVICE, GetDefaultFeatureValues("DEFAULT_FEATURE_VALUES_DEVICE"));

    EXPECT_EQ(DEFAULT_FEATURE_VALUES_DEVICE, GetDefaultFeatureValues("POUET"));
}
