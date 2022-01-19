/*
 * Copyright (c) 2021-2022 Valve Corporation
 * Copyright (c) 2021-2022 LunarG, Inc.
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
 * - Daniel Rakos <daniel.rakos@rastergrid.com>
 */

#pragma once

#include <gtest/gtest.h>

#include <vector>
#include <string>

#define VP_DEBUG_MESSAGE_CALLBACK mockDebugMessageCallback

class MockDebugMessageCallback final
{
private:
    std::vector<std::string>            m_expectedMessages;
    size_t                              m_matchedMessages;

    static MockDebugMessageCallback*    sInstance;

public:
    MockDebugMessageCallback(std::vector<std::string>&& messages)
        : m_expectedMessages{ std::move(messages) }
        , m_matchedMessages{ 0 }
    {
        sInstance = this;
    }

    ~MockDebugMessageCallback()
    {
        EXPECT_TRUE(m_matchedMessages == m_expectedMessages.size()) << "Not all expected debug messages have been received";
        sInstance = nullptr;
    }

    static void callback(const char* pMessage)
    {
        EXPECT_NE(sInstance, nullptr) << "No debug message callback mock is configured";
        if (sInstance != nullptr) {
            if (sInstance->m_matchedMessages < sInstance->m_expectedMessages.size()) {
                const char* pExpected = sInstance->m_expectedMessages[sInstance->m_matchedMessages++].c_str();
                EXPECT_STREQ(pMessage, pExpected);
            } else {
                EXPECT_TRUE(false) << "More debug messages have been received than expected";
            }
        }
    }
};

MockDebugMessageCallback* MockDebugMessageCallback::sInstance = nullptr;

void mockDebugMessageCallback(const char* pMessage)
{
    MockDebugMessageCallback::callback(pMessage);
}
