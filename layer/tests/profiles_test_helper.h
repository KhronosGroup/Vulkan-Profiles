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
 * - Jeremy Kniager <jeremyk@lunarg.com>
 */

#include <string>
#include <vector>

#include <vulkan/vulkan.h>

namespace profiles_test {

void setEnvironmentSetting(std::string setting, const char* val);
void unsetEnvironmentSetting(std::string setting);

std::string getAbsolutePath(std::string filepath);

enum SetCombinationMode { SET_CHECK_SUPPORT, SET_FROM_PROFILE, SET_FROM_DEVICE };

std::string setCombinationModeToString(SetCombinationMode mode);

void setProfilesFilenames(std::vector<std::string>& filepaths);
void setProfilesDebugEnable(bool enable);
void setProfilesExitOnError(bool enable);
void setProfilesEmulatePortabilitySubsetExtension(bool enable);
void setProfilesModifyExtensionList(SetCombinationMode mode);
void setProfilesModifyMemoryFlags(bool enable);
void setProfilesModifyFormatList(SetCombinationMode mode);
void setProfilesModifyFormatProperties(SetCombinationMode mode);
void setProfilesModifySurfaceFormats(SetCombinationMode mode);
void setProfilesModifyPresentModes(SetCombinationMode mode);
void setProfilesProfileName(const std::string& profile);

VkApplicationInfo GetDefaultApplicationInfo();

class VulkanInstanceBuilder {
   public:
    void setApiVersion(uint32_t apiVersion) { _app_info.apiVersion = apiVersion; }
    void addLayer(const char* layer_name) { _layer_names.push_back(layer_name); }
    void addExtension(const char* extension_name) { _extension_names.push_back(extension_name); }
    VkResult makeInstance();

    void reset();

    VkInstance getInstance() { return _instance; }

   protected:
    VkApplicationInfo _app_info{GetDefaultApplicationInfo()};
    VkInstanceCreateInfo _inst_create_info = {VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO};
    VkInstance _instance{VK_NULL_HANDLE};

    std::vector<const char*> _layer_names;
    std::vector<const char*> _extension_names;
};
}