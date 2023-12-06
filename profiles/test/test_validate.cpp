/*
 * Copyright (c) 2020-2022 Valve Corporation
 * Copyright (c) 2020-2022 LunarG, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * Authors:
 * - Christophe Riccio <christophe@lunarg.com>
 */

#include <string>
#include <cstdarg>
#include <cassert>
#include <iostream>
#include <fstream>

#include <valijson/adapters/jsoncpp_adapter.hpp>
#include <valijson/schema.hpp>
#include <valijson/schema_parser.hpp>
#include <valijson/validation_results.hpp>
#include <valijson/validator.hpp>

#include "json/json.h"  // https://github.com/open-source-parsers/jsoncpp

#include <gtest/gtest.h>

#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif
#endif  // _WIN32

using valijson::Schema;
using valijson::SchemaParser;
using valijson::ValidationResults;
using valijson::Validator;
using valijson::adapters::JsonCppAdapter;

static std::unique_ptr<Schema> schema;

static std::string format(const char *message, ...) {
    std::size_t const STRING_BUFFER(4096);

    assert(message != nullptr);
    assert(strlen(message) >= 0 && strlen(message) < STRING_BUFFER);

    char buffer[STRING_BUFFER];
    va_list list;

    va_start(list, message);
    vsnprintf(buffer, STRING_BUFFER, message, list);
    va_end(list);

    return buffer;
}

static Json::Value ParseJsonFile(const char *filename) {
    std::ifstream file;
    file.open(filename);
    assert(file.is_open());

    Json::Value root = Json::nullValue;
    std::string errs;
    Json::CharReaderBuilder builder;
    bool success = Json::parseFromStream(builder, file, &root, &errs);
    assert(success);

    file.close();

    if (success)
        return root;
    else
        return Json::Value();
}

struct JsonValidator {
    JsonValidator() {
#ifdef _WIN32
        _set_abort_behavior(0, _WRITE_ABORT_MSG | _CALL_REPORTFAULT);
        SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOGPFAULTERRORBOX);
        _CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_FILE);
        _CrtSetReportFile(_CRT_ASSERT, _CRTDBG_FILE_STDERR);
#endif  // _WIN32
    }

    bool Check(const Json::Value& json_document) {
        assert(!json_document.empty());

        if (!schema) {
            const Json::Value schema_document = ParseJsonFile(PROFILES_SCHEMA_FILENAME);

            schema.reset(new Schema);

            SchemaParser parser;
            JsonCppAdapter schema_adapter(schema_document);
            parser.populateSchema(schema_adapter, *schema);
        }

        Validator validator(Validator::kWeakTypes);
        JsonCppAdapter document_adapter(json_document);

        ValidationResults results;
        if (!validator.validate(*schema, document_adapter, &results)) {
            ValidationResults::Error error;
            unsigned int error_num = 1;
            while (results.popError(error)) {
                std::string context;
                std::vector<std::string>::iterator itr = error.context.begin();
                for (; itr != error.context.end(); itr++) {
                    context += *itr;
                }

                if (error_num <= 3) {
                    std::string log = format("Error #%d\n", error_num);
                    log += "\t context: " + context + "\n";
                    log += "\t desc:    " + error.description + "\n\n";

                    message += log.c_str();
                }

                ++error_num;
            }

            message += format("Total Error Count: %d\n", error_num).c_str();

            return false;
        }

        return true;
    }

    std::string message;
};

TEST(test_validate, VP_LUNARG_test_structure_simple) {
    JsonValidator validator;

    const std::string path = std::string(JSON_TEST_FILES_PATH) + "VP_LUNARG_test_structure_simple.json";
    const Json::Value document = ParseJsonFile(path.c_str());
    EXPECT_TRUE(validator.Check(document));
}

TEST(test_validate, VP_LUNARG_test_structure_complex) {
    JsonValidator validator;

    const std::string path = std::string(JSON_TEST_FILES_PATH) + "VP_LUNARG_test_structure_complex.json";
    const Json::Value document = ParseJsonFile(path.c_str());
    EXPECT_TRUE(validator.Check(document));
}

TEST(test_validate, VP_LUNARG_test_structure_fallback) {
    JsonValidator validator;

    const std::string path = std::string(JSON_TEST_FILES_PATH) + "VP_LUNARG_test_structure_fallback.json";
    const Json::Value document = ParseJsonFile(path.c_str());
    EXPECT_TRUE(validator.Check(document));
}

TEST(test_validate, VP_LUNARG_test_vkformatproperties) {
    JsonValidator validator;

    const std::string path = std::string(JSON_TEST_FILES_PATH) + "VP_LUNARG_test_vkformatproperties.json";
    const Json::Value document = ParseJsonFile(path.c_str());
    EXPECT_TRUE(validator.Check(document));
}

TEST(test_validate, VP_LUNARG_test_vkqueuefamilyproperties) {
    JsonValidator validator;

    const std::string path = std::string(JSON_TEST_FILES_PATH) + "VP_LUNARG_test_vkqueuefamilyproperties.json";
    const Json::Value document = ParseJsonFile(path.c_str());
    EXPECT_TRUE(validator.Check(document));
}

TEST(test_validate, VP_LUNARG_test_api) {
    JsonValidator validator;

    const std::string path = std::string(JSON_TEST_FILES_PATH) + "VP_LUNARG_test_api.json";
    const Json::Value document = ParseJsonFile(path.c_str());
    EXPECT_TRUE(validator.Check(document));
}

TEST(test_validate, VP_LUNARG_test_api_alternate) {
    JsonValidator validator;

    const std::string path = std::string(JSON_TEST_FILES_PATH) + "VP_LUNARG_test_api_alternate.json";
    const Json::Value document = ParseJsonFile(path.c_str());
    EXPECT_TRUE(validator.Check(document));
}

TEST(test_validate, VP_LUNARG_test_promoted_api) {
    JsonValidator validator;

    const std::string path = std::string(JSON_TEST_FILES_PATH) + "VP_LUNARG_test_promoted_api.json";
    const Json::Value document = ParseJsonFile(path.c_str());
    EXPECT_TRUE(validator.Check(document));
}

TEST(test_validate, VP_LUNARG_test_api_generated) {
    JsonValidator validator;

    const std::string path = std::string(JSON_TEST_FILES_PATH) + "VP_LUNARG_test_api_generated.json";
    const Json::Value document = ParseJsonFile(path.c_str());
    EXPECT_TRUE(validator.Check(document));
}

TEST(test_validate, VP_LUNARG_test_duplicated) {
    JsonValidator validator;

    const std::string path = std::string(JSON_TEST_FILES_PATH) + "VP_LUNARG_test_duplicated.json";
    const Json::Value document = ParseJsonFile(path.c_str());
    EXPECT_TRUE(validator.Check(document));
}

TEST(test_validate, VP_LUNARG_test_formats) {
    JsonValidator validator;

    const std::string path = std::string(JSON_TEST_FILES_PATH) + "VP_LUNARG_test_formats.json";
    const Json::Value document = ParseJsonFile(path.c_str());
    EXPECT_TRUE(validator.Check(document));
}

TEST(test_validate, VP_LUNARG_desktop_max_2024) {
    JsonValidator validator;

    const std::string path = std::string(JSON_TEST_FILES_PATH) + "VP_LUNARG_desktop_max_2024.json";
    const Json::Value json_document2 = ParseJsonFile(path.c_str());
    EXPECT_TRUE(validator.Check(json_document2));
}

TEST(test_validate, VP_LUNARG_minimum_requirements) {
    JsonValidator validator;

    const std::string path = std::string(PROFILE_FILES_PATH) + "VP_LUNARG_minimum_requirements.json";
    const Json::Value json_document2 = ParseJsonFile(path.c_str());
    EXPECT_TRUE(validator.Check(json_document2));
}

TEST(test_validate, VP_LUNARG_desktop_baseline_2023) {
    JsonValidator validator;

    const std::string path = std::string(PROFILE_FILES_PATH) + "VP_LUNARG_desktop_baseline_2023.json";
    const Json::Value json_document2 = ParseJsonFile(path.c_str());
    EXPECT_TRUE(validator.Check(json_document2));
}

TEST(test_validate, VP_LUNARG_desktop_baseline_2024) {
    JsonValidator validator;

    const std::string path = std::string(PROFILE_FILES_PATH) + "VP_LUNARG_desktop_baseline_2024.json";
    const Json::Value json_document2 = ParseJsonFile(path.c_str());
    EXPECT_TRUE(validator.Check(json_document2));
}

TEST(test_validate, VP_KHR_roadmap) {
    JsonValidator validator;

    const std::string path = std::string(PROFILE_FILES_PATH) + "VP_KHR_roadmap.json";
    const Json::Value json_document3 = ParseJsonFile(path.c_str());
    EXPECT_TRUE(validator.Check(json_document3));
}

TEST(test_validate, VP_ANDROID_baseline_2021) {
    JsonValidator validator;

    const std::string path = std::string(PROFILE_FILES_PATH) + "VP_ANDROID_baseline_2021.json";
    const Json::Value json_document3 = ParseJsonFile(path.c_str());
    EXPECT_TRUE(validator.Check(json_document3));
}

TEST(test_validate, VP_ANDROID_baseline_2022) {
    JsonValidator validator;

    const std::string path = std::string(PROFILE_FILES_PATH) + "VP_ANDROID_baseline_2022.json";
    const Json::Value json_document3 = ParseJsonFile(path.c_str());
    EXPECT_TRUE(validator.Check(json_document3));
}

TEST(test_validate, VP_ANDROID_15_minimums) {
    JsonValidator validator;

    const std::string path = std::string(PROFILE_FILES_PATH) + "VP_ANDROID_15_minimums.json";
    const Json::Value json_document3 = ParseJsonFile(path.c_str());
    EXPECT_TRUE(validator.Check(json_document3));
}
