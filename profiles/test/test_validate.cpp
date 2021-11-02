/*
 * Copyright (c) 2020-2021 Valve Corporation
 * Copyright (c) 2020-2021 LunarG, Inc.
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

    Json::Reader reader;
    Json::Value root = Json::nullValue;
    bool success = reader.parse(file, root, false);
    assert(success);

    file.close();

    return root;
}

struct JsonValidator {
    JsonValidator() {}

    bool Check(const Json::Value& json_document) {
        assert(!json_document.empty());

        if (!schema) {
            const Json::Value schema_document = ParseJsonFile("profile.json");

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

TEST(test_validate, adapter) {
    JsonValidator validator;

    const Json::Value json_document1 = ParseJsonFile("VP_KHR_minimum_requirements.json");
    EXPECT_TRUE(validator.Check(json_document1));

    return;

    const Json::Value json_document2 = ParseJsonFile("VP_LUNARG_1_1_desktop_portability_2022.json");
    EXPECT_TRUE(validator.Check(json_document2));

    const Json::Value json_document3 = ParseJsonFile("VP_KHR_1_2_roadmap_2022.json");
    EXPECT_TRUE(validator.Check(json_document3));

    const Json::Value json_document4 = ParseJsonFile("VP_KHR_1_3_roadmap_2022.json");
    EXPECT_TRUE(validator.Check(json_document4));
}
