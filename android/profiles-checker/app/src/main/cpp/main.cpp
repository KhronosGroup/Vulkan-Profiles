/*
 * Copyright 2026 Google LLC
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
 */

// Vulkan Profiles Checker - a pure C++ NativeActivity that reports GPU/driver
// details and Vulkan profile support, explaining exactly which requirements a
// driver fails to meet.

#include <android/configuration.h>
#include <android/input.h>
#include <android/log.h>
#include <android/looper.h>
#include <android_native_app_glue.h>

#include <atomic>
#include <memory>
#include <mutex>
#include <string>
#include <thread>

#include "profile_checker.h"
#include "report.h"
#include "text_renderer.h"
#include "ui.h"

namespace {

constexpr const char* kLogTag = "VulkanProfilesChecker";

struct AppState {
    android_app* app = nullptr;
    TextRenderer text;
    Ui ui{text};

    // Results produced on the worker thread, consumed on the main thread.
    std::mutex mutex;
    std::shared_ptr<const CheckReport> pendingReport;
    std::string pendingReportPath;
    std::atomic<bool> hasPending{false};
    std::thread worker;
    ALooper* mainLooper = nullptr;
};

float QueryDensity(android_app* app) {
    if (app->config == nullptr) return 0.0f;
    const int32_t density = AConfiguration_getDensity(app->config);
    if (density <= 0 || density >= ACONFIGURATION_DENSITY_ANY) return 0.0f;
    return static_cast<float>(density) / 160.0f;
}

void StartChecks(AppState& state) {
    if (state.worker.joinable()) return;
    const std::string dataPath = state.app->activity->internalDataPath ? state.app->activity->internalDataPath : "";
    state.worker = std::thread([&state, dataPath]() {
        __android_log_print(ANDROID_LOG_INFO, kLogTag, "Running Vulkan profile checks");
        auto report = std::make_shared<CheckReport>(RunProfileChecks());

        const std::string text = BuildTextReport(*report);
        LogTextReport(text);
        const std::string path = WriteTextReport(text, dataPath);

        {
            std::lock_guard<std::mutex> lock(state.mutex);
            state.pendingReport = report;
            state.pendingReportPath = path;
        }
        state.hasPending.store(true);
        if (state.mainLooper != nullptr) ALooper_wake(state.mainLooper);
    });
}

void HandleCommand(android_app* app, int32_t cmd) {
    AppState& state = *static_cast<AppState*>(app->userData);
    switch (cmd) {
        case APP_CMD_INIT_WINDOW:
            state.ui.SetDensity(QueryDensity(app));
            state.ui.SetWindow(app->window);
            state.ui.SetContentRect(app->contentRect);
            break;
        case APP_CMD_TERM_WINDOW:
            state.ui.SetWindow(nullptr);
            break;
        case APP_CMD_CONTENT_RECT_CHANGED:
            state.ui.SetContentRect(app->contentRect);
            break;
        case APP_CMD_CONFIG_CHANGED:
            state.ui.SetDensity(QueryDensity(app));
            state.ui.Invalidate();
            break;
        case APP_CMD_WINDOW_RESIZED:
        case APP_CMD_WINDOW_REDRAW_NEEDED:
        case APP_CMD_GAINED_FOCUS:
            state.ui.Invalidate();
            break;
        default:
            break;
    }
}

int32_t HandleInput(android_app* app, AInputEvent* event) {
    AppState& state = *static_cast<AppState*>(app->userData);
    if (AInputEvent_getType(event) != AINPUT_EVENT_TYPE_MOTION) return 0;

    const int32_t action = AMotionEvent_getAction(event) & AMOTION_EVENT_ACTION_MASK;
    const float x = AMotionEvent_getX(event, 0);
    const float y = AMotionEvent_getY(event, 0);
    switch (action) {
        case AMOTION_EVENT_ACTION_DOWN:
            state.ui.OnTouchDown(x, y);
            return 1;
        case AMOTION_EVENT_ACTION_MOVE:
            state.ui.OnTouchMove(x, y);
            return 1;
        case AMOTION_EVENT_ACTION_UP:
            state.ui.OnTouchUp(x, y);
            return 1;
        case AMOTION_EVENT_ACTION_CANCEL:
            state.ui.OnTouchCancel();
            return 1;
        default:
            return 0;
    }
}

}  // namespace

void android_main(android_app* app) {
    AppState state;
    state.app = app;
    state.mainLooper = ALooper_forThread();
    app->userData = &state;
    app->onAppCmd = HandleCommand;
    app->onInputEvent = HandleInput;

    if (!state.text.LoadSystemFont()) {
        __android_log_print(ANDROID_LOG_ERROR, kLogTag,
                            "Text rendering unavailable; the report is still written to logcat and report.txt");
    }

    StartChecks(state);

    while (true) {
        int events = 0;
        android_poll_source* source = nullptr;
        // Block while idle; wake up for app events, input, or the worker thread.
        const int timeoutMs = state.ui.NeedsRedraw() ? 0 : -1;
        while (ALooper_pollOnce(timeoutMs, nullptr, &events, reinterpret_cast<void**>(&source)) >= 0) {
            if (source != nullptr) source->process(app, source);
            if (app->destroyRequested != 0) break;
            if (state.ui.NeedsRedraw() || state.hasPending.load()) break;
        }

        if (app->destroyRequested != 0) break;

        if (state.hasPending.exchange(false)) {
            std::shared_ptr<const CheckReport> report;
            std::string path;
            {
                std::lock_guard<std::mutex> lock(state.mutex);
                report = state.pendingReport;
                path = state.pendingReportPath;
            }
            state.ui.SetReport(report, path);
        }

        if (state.ui.NeedsRedraw()) state.ui.Render();
    }

    if (state.worker.joinable()) state.worker.join();
    app->userData = nullptr;
}
