/* Copyright (c) 2015-2025 The Khronos Group Inc.
 * Copyright (c) 2015-2025 Valve Corporation
 * Copyright (c) 2015-2025 LunarG, Inc.
 * Copyright (c) 2015-2025 Google, Inc.
 * Copyright (c) 2023-2023 RasterGrid Kft.
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
 * Author: Tobin Ehlis <tobin@lunarg.com>
 */
#include <assert.h>
#include <sstream>
#include <iomanip>
#include <unordered_map>
#include <vulkan/utility/vk_dispatch_table.h>
#include "vulkan/vk_layer.h"
#include "vk_layer_table.h"
static device_table_map tableMap;
static instance_table_map tableInstanceMap;

dispatch_key get_dispatch_key(const void *object) { return (dispatch_key) * (VkuDeviceDispatchTable **)object; }

// Map lookup must be thread safe
VkuDeviceDispatchTable *device_dispatch_table(void *object) {
    dispatch_key key = get_dispatch_key(object);
    device_table_map::const_iterator it = tableMap.find((void *)key);
    assert(it != tableMap.end() && "Not able to find device dispatch entry");
    return it->second.get();
}

VkuInstanceDispatchTable *instance_dispatch_table(void *object) {
    dispatch_key key = get_dispatch_key(object);
    instance_table_map::const_iterator it = tableInstanceMap.find((void *)key);
    assert(it != tableInstanceMap.end() && "Not able to find instance dispatch entry");
    return it->second.get();
}

void destroy_dispatch_table(device_table_map &map, dispatch_key key) {
    device_table_map::const_iterator it = map.find((void *)key);
    if (it != map.end()) {
        map.erase(it);
    }
}

void destroy_dispatch_table(instance_table_map &map, dispatch_key key) {
    instance_table_map::const_iterator it = map.find((void *)key);
    if (it != map.end()) {
        map.erase(it);
    }
}

void destroy_device_dispatch_table(dispatch_key key) { destroy_dispatch_table(tableMap, key); }

void destroy_instance_dispatch_table(dispatch_key key) { destroy_dispatch_table(tableInstanceMap, key); }

VkuDeviceDispatchTable *get_dispatch_table(device_table_map &map, void *object) {
    dispatch_key key = get_dispatch_key(object);
    device_table_map::const_iterator it = map.find((void *)key);
    assert(it != map.end() && "Not able to find device dispatch entry");
    return it->second.get();
}

VkuInstanceDispatchTable *get_dispatch_table(instance_table_map &map, void *object) {
    dispatch_key key = get_dispatch_key(object);
    instance_table_map::const_iterator it = map.find((void *)key);
    assert(it != map.end() && "Not able to find instance dispatch entry");
    return it->second.get();
}

VkLayerInstanceCreateInfo *get_chain_info(const VkInstanceCreateInfo *pCreateInfo, VkLayerFunction func) {
    VkLayerInstanceCreateInfo *chain_info = (VkLayerInstanceCreateInfo *)pCreateInfo->pNext;
    while (chain_info && !(chain_info->sType == VK_STRUCTURE_TYPE_LOADER_INSTANCE_CREATE_INFO && chain_info->function == func)) {
        chain_info = (VkLayerInstanceCreateInfo *)chain_info->pNext;
    }
    assert(chain_info != NULL);
    return chain_info;
}

VkLayerDeviceCreateInfo *get_chain_info(const VkDeviceCreateInfo *pCreateInfo, VkLayerFunction func) {
    VkLayerDeviceCreateInfo *chain_info = (VkLayerDeviceCreateInfo *)pCreateInfo->pNext;
    while (chain_info && !(chain_info->sType == VK_STRUCTURE_TYPE_LOADER_DEVICE_CREATE_INFO && chain_info->function == func)) {
        chain_info = (VkLayerDeviceCreateInfo *)chain_info->pNext;
    }
    assert(chain_info != NULL);
    return chain_info;
}

/* Various dispatchable objects will use the same underlying dispatch table if they
 * are created from that "parent" object. Thus use pointer to dispatch table
 * as the key to these table maps.
 *    Instance -> PhysicalDevice
 *    Device -> CommandBuffer or Queue
 * If use the object themselves as key to map then implies Create entrypoints have to be intercepted
 * and a new key inserted into map */
VkuInstanceDispatchTable *initInstanceTable(VkInstance instance, const PFN_vkGetInstanceProcAddr gpa, instance_table_map &map) {
    VkuInstanceDispatchTable *pTable;
    dispatch_key key = get_dispatch_key(instance);
    instance_table_map::const_iterator it = map.find((void *)key);

    if (it == map.end()) {
        auto table = std::make_unique<VkuInstanceDispatchTable>();
        pTable = table.get();
        map[(void *)key] = std::move(table);
    } else {
        return it->second.get();
    }

    vkuInitInstanceDispatchTable(instance, pTable, gpa);

    // Setup func pointers that are required but not externally exposed.  These won't be added to the instance dispatch table by
    // default.
    pTable->GetPhysicalDeviceProcAddr = (PFN_GetPhysicalDeviceProcAddr)gpa(instance, "vk_layerGetPhysicalDeviceProcAddr");

    return pTable;
}

VkuInstanceDispatchTable *initInstanceTable(VkInstance instance, const PFN_vkGetInstanceProcAddr gpa) {
    return initInstanceTable(instance, gpa, tableInstanceMap);
}

VkuDeviceDispatchTable *initDeviceTable(VkDevice device, const PFN_vkGetDeviceProcAddr gpa, device_table_map &map) {
    VkuDeviceDispatchTable *pTable;
    dispatch_key key = get_dispatch_key(device);
    device_table_map::const_iterator it = map.find((void *)key);

    if (it == map.end()) {
        auto table = std::make_unique<VkuDeviceDispatchTable>();
        pTable = table.get();
        map[(void *)key] = std::move(table);
    } else {
        return it->second.get();
    }

    vkuInitDeviceDispatchTable(device, pTable, gpa);

    return pTable;
}

VkuDeviceDispatchTable *initDeviceTable(VkDevice device, const PFN_vkGetDeviceProcAddr gpa) {
    return initDeviceTable(device, gpa, tableMap);
}

// Convert integer API version to a string
std::string StringAPIVersion(uint32_t version) {
    std::stringstream version_name;
    if (version == 0) {
        return "<unrecognized>";
    }
    version_name << VK_API_VERSION_MAJOR(version) << "." << VK_API_VERSION_MINOR(version) << "." << VK_API_VERSION_PATCH(version)
                 << " (0x" << std::setfill('0') << std::setw(8) << std::hex << version << ")";
    return version_name.str();
}
