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
 * - Jeremy Kniager <jeremyk@lunarg.com>
 * - Mark Lobodzinski <mark@lunarg.com>
 */

#include <string>
#include <vector>
#include <array>
#include <cmath>
#include <cstdarg>

#include <vulkan/vulkan.h>
#include "../profiles.h"
#include "gtest/gtest.h"
#include "vktestframework.h"

namespace profiles_test {

inline std::string format(const char* message, ...) {
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

void setEnvironmentSetting(std::string setting, const char* val);
void unsetEnvironmentSetting(std::string setting);

std::string getAbsolutePath(std::string filepath);

VkApplicationInfo GetDefaultApplicationInfo();

bool IsExtensionSupported(VkPhysicalDevice physical_device, const char* extension_name);

enum Mode {
    MODE_NATIVE = 0,
    MODE_PROFILE,

    MODE_FIRST = MODE_NATIVE,
    MODE_LAST = MODE_PROFILE
};

enum { MODE_COUNT = MODE_LAST - MODE_FIRST + 1 };

class VulkanInstanceBuilder {
   public:
    ~VulkanInstanceBuilder() { this->reset(); }

    void addExtension(const char* extension_name) { _extension_names.push_back(extension_name); }

    VkResult init();
    VkResult init(const std::vector<VkLayerSettingEXT>& settings);
    VkResult init(uint32_t apiVersion);
    VkResult init(uint32_t apiVersion, const std::vector<VkLayerSettingEXT>& settings);
    VkResult getPhysicalDevice(Mode mode, VkPhysicalDevice* phys_dev);

    void reset();

    VkInstance getInstance(Mode mode) { return _instances[mode]; }

   protected:
    std::array<VkInstance, MODE_COUNT> _instances;

    std::vector<const char*> _layer_names;
    std::vector<const char*> _extension_names;
};

}  // namespace profiles_test
