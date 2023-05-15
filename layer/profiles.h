/*
 * Copyright (C) 2015-2022 Valve Corporation
 * Copyright (C) 2015-2022 LunarG, Inc.
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
 * Author: Christophe Riccio <mark@lunarg.com>
 */

#pragma once
#include "profiles_settings.h"

VKAPI_ATTR PFN_vkVoidFunction VKAPI_CALL GetInstanceProcAddr(VkInstance instance, const char *pName);
VKAPI_ATTR VkResult VKAPI_CALL CreateInstance(const VkInstanceCreateInfo *pCreateInfo, const VkAllocationCallbacks *pAllocator,
                                              VkInstance *pInstance);
VKAPI_ATTR VkResult VKAPI_CALL EnumerateInstanceLayerProperties(uint32_t *pCount, VkLayerProperties *pProperties);
VKAPI_ATTR VkResult VKAPI_CALL EnumerateInstanceExtensionProperties(const char *pLayerName, uint32_t *pCount,
                                                                    VkExtensionProperties *pProperties);
VKAPI_ATTR VkResult VKAPI_CALL EnumeratePhysicalDevices(VkInstance instance, uint32_t *pPhysicalDeviceCount,
                                                        VkPhysicalDevice *pPhysicalDevices);

std::string GetFormatFeatureString(VkFormatFeatureFlags flags);

std::string GetFormatFeature2String(VkFormatFeatureFlags2 flags);

SimulateCapabilityFlags GetSimulateCapabilityFlags(const std::vector<std::string> &values);

std::string GetSimulateCapabilitiesLog(SimulateCapabilityFlags flags);

std::string GetDebugReportsLog(DebugActionFlags flags);

#define kLayerName "VK_LAYER_KHRONOS_profiles"
#define kLayerSettingsProfileFile "profile_file"
#define kLayerSettingsProfileName "profile_name"
#define kLayerSettingsProfileValidation "profile_validation"
#define kLayerSettingsEmulatePortability "emulate_portability"
#define kLayerSettings_constantAlphaColorBlendFactors "constantAlphaColorBlendFactors"
#define kLayerSettings_events "events"
#define kLayerSettings_imageViewFormatReinterpretation "imageViewFormatReinterpretation"
#define kLayerSettings_imageViewFormatSwizzle "imageViewFormatSwizzle"
#define kLayerSettings_imageView2DOn3DImage "imageView2DOn3DImage"
#define kLayerSettings_multisampleArrayImage "multisampleArrayImage"
#define kLayerSettings_mutableComparisonSamplers "mutableComparisonSamplers"
#define kLayerSettings_pointPolygons "pointPolygons"
#define kLayerSettings_samplerMipLodBias "samplerMipLodBias"
#define kLayerSettings_separateStencilMaskRef "separateStencilMaskRef"
#define kLayerSettings_shaderSampleRateInterpolationFunctions "shaderSampleRateInterpolationFunctions"
#define kLayerSettings_tessellationIsolines "tessellationIsolines"
#define kLayerSettings_tessellationPointMode "tessellationPointMode"
#define kLayerSettings_triangleFans "triangleFans"
#define kLayerSettings_vertexAttributeAccessBeyondStride "vertexAttributeAccessBeyondStride"
#define kLayerSettings_minVertexInputBindingStrideAlignment "minVertexInputBindingStrideAlignment"
#define kLayerSettingsSimulateCapabilities "simulate_capabilities"
#define kLayerSettingsDebugActions "debug_actions"
#define kLayerSettingsDebugFilename "debug_filename"
#define kLayerSettingsDebugFileClear "debug_file_clear"
#define kLayerSettingsDebugFailOnError "debug_fail_on_error"
#define kLayerSettingsDebugReports "debug_reports"
#define kLayerSettingsExcludeDeviceExtensions "exclude_device_extensions"
#define kLayerSettingsExcludeFormats "exclude_formats"
#define kLayerSettingsDefaultFeatureValues "default_feature_values"
#define kLayerSettingsForceDevice "force_device"
#define kLayerSettingsForceDeviceUUID "force_device_uuid"
#define kLayerSettingsForceDeviceName "force_device_name"

