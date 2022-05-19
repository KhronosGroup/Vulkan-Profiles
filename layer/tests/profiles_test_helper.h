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

#include <vulkan/vulkan.h>
#include "../profiles.h"
#include "gtest/gtest.h"
#include "vktestframework.h"

namespace profiles_test {

void setEnvironmentSetting(std::string setting, const char* val);
void unsetEnvironmentSetting(std::string setting);

std::string getAbsolutePath(std::string filepath);

std::string GetSimulateCapabilitiesLog(SimulateCapabilityFlags flags);

VkApplicationInfo GetDefaultApplicationInfo();

enum Mode {
    MODE_NATIVE = 0,
    MODE_PROFILE,

    MODE_FIRST = MODE_NATIVE,
    MODE_LAST = MODE_PROFILE
};

enum { MODE_COUNT = MODE_LAST - MODE_FIRST + 1 };

class VulkanInstanceBuilder {
   public:
    //void setApiVersion(uint32_t apiVersion) { _app_info.apiVersion = apiVersion; }
    void addLayer(const char* layer_name) { _layer_names.push_back(layer_name); }
    void addExtension(const char* extension_name) { _extension_names.push_back(extension_name); }
    VkResult init();
    VkResult init(void* pnext);
    VkResult getPhysicalDevice(Mode mode, VkPhysicalDevice* phys_dev);

    void clean();
    void reset();

    VkInstance getInstance(Mode mode) { return _instances[mode]; }

   protected:
    std::array<VkInstance, MODE_COUNT> _instances = {VK_NULL_HANDLE, VK_NULL_HANDLE};

    std::vector<const char*> _layer_names;
    std::vector<const char*> _extension_names;
};
}  // namespace profiles_test