/*
 * Copyright (C) 2022-2023 Valve Corporation
 * Copyright (C) 2022-2023 LunarG, Inc.
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

#include "profiles_json.h"
#include "profiles_util.h"

#include <valijson/adapters/jsoncpp_adapter.hpp>
#include <valijson/schema.hpp>
#include <valijson/schema_parser.hpp>
#include <valijson/validation_results.hpp>
#include <valijson/validator.hpp>

using valijson::Schema;
using valijson::SchemaParser;
using valijson::ValidationResults;
using valijson::Validator;
using valijson::adapters::JsonCppAdapter;

std::unique_ptr<Schema> schema;

static Json::Value ParseJsonFile(std::string filename) {
    Json::Value root = Json::nullValue;

    // Remove newline from filename
    if (int(filename.back() == 0xa)) {
        filename.pop_back();
    }

    std::ifstream file;
    file.open(filename.c_str());
    if (!file.is_open()) {
        return root;
    }

    std::string errs;
    Json::CharReaderBuilder builder;
    Json::parseFromStream(builder, file, &root, &errs);
    file.close();

    return root;
}

JsonValidator::~JsonValidator() { 
    ::schema.reset();
}

bool JsonValidator::Init() {
#ifdef __APPLE__
    const std::string schema_path = "/usr/local/share/vulkan/registry/profiles-0.8-latest.json";
#else
    const char *sdk_path = std::getenv("VULKAN_SDK");
    if (sdk_path == nullptr) return false;
    const std::string schema_path = std::string(sdk_path) + "/share/vulkan/registry/profiles-0.8-latest.json";
#endif

    if (!schema) {
        const Json::Value schema_document = ParseJsonFile(schema_path.c_str());
        if (schema_document == Json::nullValue) {
            return false;
        }

        schema.reset(new Schema);

        SchemaParser parser;
        JsonCppAdapter schema_adapter(schema_document);
        parser.populateSchema(schema_adapter, *schema);
    }

    return schema.get() != nullptr;
}

bool JsonValidator::Check(const Json::Value &json_document) {
    assert(!json_document.empty());

    if (schema.get() == nullptr) return true;

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

bool WarnDuplicated(const Json::Value &parent, const std::vector<std::string> &members) {
    std::vector<std::string> set;
    for (const auto &member : members) {
        if (parent.isMember(member)) {
            set.push_back(member);
        }
    }

    for (uint32_t i = 1; i < set.size(); ++i) {
        LogMessage(DEBUG_REPORT_WARNING_BIT, "Profile sets variables for %s while also using %s\n", set[0].c_str(), set[i].c_str());
    }

    return set.size() <= 1;
}
