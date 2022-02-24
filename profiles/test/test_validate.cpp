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

using valijson::Schema;
using valijson::SchemaParser;
using valijson::ValidationResults;
using valijson::Validator;
using valijson::adapters::JsonCppAdapter;

static std::unique_ptr<Schema> schema;
static std::unique_ptr<Validator> validator;

static std::string format(const char *message, ...) {
    std::size_t const STRING_BUFFER(4096);

    assert(message != nullptr);
    assert(strlen(message) >= 0 && strlen(message) < STRING_BUFFER);

    char buffer[STRING_BUFFER];
    va_list list;

    va_start(list, message);
    vsprintf(buffer, message, list);
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

    return root;
}

struct JsonValidator {
    JsonValidator() {}

    bool Check(const Json::Value& json_document) {
        assert(!json_document.empty());

        if (!schema) {
            const Json::Value schema_document = ParseJsonFile("profiles-latest.json");

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

    const Json::Value document = ParseJsonFile("VP_LUNARG_test_structure_simple.json");
    EXPECT_TRUE(validator.Check(document));
}

TEST(test_validate, VP_LUNARG_test_structure_complex) {
    JsonValidator validator;

    const Json::Value document = ParseJsonFile("VP_LUNARG_test_structure_complex.json");
    EXPECT_TRUE(validator.Check(document));
}

TEST(test_validate, VP_LUNARG_test_structure_fallback) {
    JsonValidator validator;

    const Json::Value document = ParseJsonFile("VP_LUNARG_test_structure_fallback.json");
    EXPECT_TRUE(validator.Check(document));
}

TEST(test_validate, VP_LUNARG_test_vkformatproperties) {
    JsonValidator validator;

    const Json::Value document = ParseJsonFile("VP_LUNARG_test_vkformatproperties.json");
    EXPECT_TRUE(validator.Check(document));
}

TEST(test_validate, VP_LUNARG_test_vkqueuefamilyproperties) {
    JsonValidator validator;

    const Json::Value document = ParseJsonFile("VP_LUNARG_test_vkqueuefamilyproperties.json");
    EXPECT_TRUE(validator.Check(document));
}

TEST(test_validate, VP_LUNARG_test_api) {
    JsonValidator validator;

    const Json::Value document = ParseJsonFile("VP_LUNARG_test_api.json");
    EXPECT_TRUE(validator.Check(document));
}

TEST(test_validate, VP_LUNARG_test_api_alternate) {
    JsonValidator validator;

    const Json::Value document = ParseJsonFile("VP_LUNARG_test_api_alternate.json");
    EXPECT_TRUE(validator.Check(document));
}

TEST(test_validate, VP_LUNARG_test_promoted_api) {
    JsonValidator validator;

    const Json::Value document = ParseJsonFile("VP_LUNARG_test_promoted_api.json");
    EXPECT_TRUE(validator.Check(document));
}

TEST(test_validate, VP_LUNARG_test_duplicated) {
    JsonValidator validator;

    const Json::Value document = ParseJsonFile("VP_LUNARG_test_duplicated.json");
    EXPECT_TRUE(validator.Check(document));
}

TEST(test_validate, VP_LUNARG_test_formats) {
    JsonValidator validator;

    const Json::Value document = ParseJsonFile("VP_LUNARG_test_formats.json");
    EXPECT_TRUE(validator.Check(document));
}

TEST(test_validate, VP_LUNARG_desktop_portability_2021) {
    JsonValidator validator;

    const Json::Value json_document2 = ParseJsonFile("VP_LUNARG_desktop_portability_2021.json");
    EXPECT_TRUE(validator.Check(json_document2));
}

TEST(test_validate, VP_KHR_roadmap_2022) {
    JsonValidator validator;

    const Json::Value json_document3 = ParseJsonFile("VP_KHR_roadmap_2022.json");
    EXPECT_TRUE(validator.Check(json_document3));
}

TEST(test_validate, VP_ANDROID_baseline_2021) {
    JsonValidator validator;

    const Json::Value json_document3 = ParseJsonFile("VP_ANDROID_baseline_2021.json");
    EXPECT_TRUE(validator.Check(json_document3));
}
