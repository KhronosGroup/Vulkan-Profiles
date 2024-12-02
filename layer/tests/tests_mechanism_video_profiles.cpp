/*
 * Copyright (C) 2021-2024 Valve Corporation
 * Copyright (C) 2021-2024 LunarG, Inc.
 * Copyright (c) 2024 RasterGrid Kft.
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
 * Authors:
 * - Daniel Rakos <daniel.rakos@rastergrid.com>
 */

#include <vulkan/vulkan_core.h>
#include <vulkan/vulkan_beta.h>

#include <gtest/gtest.h>
#include "profiles_test_helper.h"

#include <cstdarg>

static VkPhysicalDevice gpu = VK_NULL_HANDLE;
static profiles_test::VulkanInstanceBuilder inst_builder;
static PFN_vkGetPhysicalDeviceVideoCapabilitiesKHR pfnGetPhysicalDeviceVideoCapabilitiesKHR;
static PFN_vkGetPhysicalDeviceVideoFormatPropertiesKHR pfnGetPhysicalDeviceVideoFormatPropertiesKHR;

class TestsMechanismVideoProfiles : public VkTestFramework {
   public:
    TestsMechanismVideoProfiles() {}

    ~TestsMechanismVideoProfiles() {}

    static void SetUpTestSuite() {
        VkResult err = VK_SUCCESS;

        const char* profile_file_data = JSON_TEST_FILES_PATH "VP_RASTERGRID_test_video_profiles.json";
        const char* profile_name_data = "VP_RASTERGRID_test_video_profiles";
        VkBool32 emulate_portability_data = VK_TRUE;
        const char* simulate_capabilities = "SIMULATE_MAX_ENUM";

        std::vector<VkLayerSettingEXT> settings = {
            {kLayerName, kLayerSettingsProfileFile, VK_LAYER_SETTING_TYPE_STRING_EXT, 1, &profile_file_data},
            {kLayerName, kLayerSettingsProfileName, VK_LAYER_SETTING_TYPE_STRING_EXT, 1, &profile_name_data},
            {kLayerName, kLayerSettingsEmulatePortability, VK_LAYER_SETTING_TYPE_BOOL32_EXT, 1, &emulate_portability_data},
            {kLayerName, kLayerSettingsSimulateCapabilities, VK_LAYER_SETTING_TYPE_STRING_EXT, 1, &simulate_capabilities}};

        err = inst_builder.init(settings);
        ASSERT_EQ(err, VK_SUCCESS);

        err = inst_builder.getPhysicalDevice(profiles_test::MODE_PROFILE, &gpu);
        ASSERT_EQ(err, VK_SUCCESS);

        pfnGetPhysicalDeviceVideoCapabilitiesKHR =
            reinterpret_cast<PFN_vkGetPhysicalDeviceVideoCapabilitiesKHR>(vkGetInstanceProcAddr(
                inst_builder.getInstance(profiles_test::MODE_PROFILE), "vkGetPhysicalDeviceVideoCapabilitiesKHR"));
        ASSERT_NE(pfnGetPhysicalDeviceVideoCapabilitiesKHR, nullptr);

        pfnGetPhysicalDeviceVideoFormatPropertiesKHR =
            reinterpret_cast<PFN_vkGetPhysicalDeviceVideoFormatPropertiesKHR>(vkGetInstanceProcAddr(
                inst_builder.getInstance(profiles_test::MODE_PROFILE), "vkGetPhysicalDeviceVideoFormatPropertiesKHR"));
        ASSERT_NE(pfnGetPhysicalDeviceVideoFormatPropertiesKHR, nullptr);
    }

    const VkVideoProfileInfoKHR* GetDecodeH264BaselineProfile() const {
        static const VkVideoDecodeH264ProfileInfoKHR decode_h264_baseline = {VK_STRUCTURE_TYPE_VIDEO_DECODE_H264_PROFILE_INFO_KHR,
                                                                             nullptr, STD_VIDEO_H264_PROFILE_IDC_BASELINE,
                                                                             VK_VIDEO_DECODE_H264_PICTURE_LAYOUT_PROGRESSIVE_KHR};
        static const VkVideoProfileInfoKHR decode_h264 = {
            VK_STRUCTURE_TYPE_VIDEO_PROFILE_INFO_KHR,     &decode_h264_baseline,
            VK_VIDEO_CODEC_OPERATION_DECODE_H264_BIT_KHR, VK_VIDEO_CHROMA_SUBSAMPLING_420_BIT_KHR,
            VK_VIDEO_COMPONENT_BIT_DEPTH_8_BIT_KHR,       VK_VIDEO_COMPONENT_BIT_DEPTH_8_BIT_KHR};
        return &decode_h264;
    }

    const VkVideoProfileInfoKHR* GetDecodeH264MainProfile() const {
        static const VkVideoDecodeH264ProfileInfoKHR decode_h264_main = {
            VK_STRUCTURE_TYPE_VIDEO_DECODE_H264_PROFILE_INFO_KHR, nullptr, STD_VIDEO_H264_PROFILE_IDC_MAIN,
            VK_VIDEO_DECODE_H264_PICTURE_LAYOUT_INTERLACED_SEPARATE_PLANES_BIT_KHR};
        static const VkVideoProfileInfoKHR decode_h264 = {
            VK_STRUCTURE_TYPE_VIDEO_PROFILE_INFO_KHR,     &decode_h264_main,
            VK_VIDEO_CODEC_OPERATION_DECODE_H264_BIT_KHR, VK_VIDEO_CHROMA_SUBSAMPLING_420_BIT_KHR,
            VK_VIDEO_COMPONENT_BIT_DEPTH_8_BIT_KHR,       VK_VIDEO_COMPONENT_BIT_DEPTH_8_BIT_KHR};
        return &decode_h264;
    }

    const VkVideoProfileInfoKHR* GetDecodeH265MainProfile() const {
        static const VkVideoDecodeH265ProfileInfoKHR decode_h265_main = {VK_STRUCTURE_TYPE_VIDEO_DECODE_H265_PROFILE_INFO_KHR,
                                                                         nullptr, STD_VIDEO_H265_PROFILE_IDC_MAIN};
        static const VkVideoProfileInfoKHR decode_h265 = {
            VK_STRUCTURE_TYPE_VIDEO_PROFILE_INFO_KHR,     &decode_h265_main,
            VK_VIDEO_CODEC_OPERATION_DECODE_H265_BIT_KHR, VK_VIDEO_CHROMA_SUBSAMPLING_420_BIT_KHR,
            VK_VIDEO_COMPONENT_BIT_DEPTH_8_BIT_KHR,       VK_VIDEO_COMPONENT_BIT_DEPTH_8_BIT_KHR};
        return &decode_h265;
    }

    const VkVideoProfileInfoKHR* GetDecodeH265Main10Profile() const {
        static const VkVideoDecodeH265ProfileInfoKHR decode_h265_main_10 = {VK_STRUCTURE_TYPE_VIDEO_DECODE_H265_PROFILE_INFO_KHR,
                                                                            nullptr, STD_VIDEO_H265_PROFILE_IDC_MAIN_10};
        static const VkVideoProfileInfoKHR decode_h265 = {
            VK_STRUCTURE_TYPE_VIDEO_PROFILE_INFO_KHR,     &decode_h265_main_10,
            VK_VIDEO_CODEC_OPERATION_DECODE_H265_BIT_KHR, VK_VIDEO_CHROMA_SUBSAMPLING_420_BIT_KHR,
            VK_VIDEO_COMPONENT_BIT_DEPTH_10_BIT_KHR,      VK_VIDEO_COMPONENT_BIT_DEPTH_10_BIT_KHR};
        return &decode_h265;
    }

    const VkVideoProfileInfoKHR* GetEncodeH264BaselineProfile() const {
        static const VkVideoEncodeH264ProfileInfoKHR encode_h264_baseline = {VK_STRUCTURE_TYPE_VIDEO_ENCODE_H264_PROFILE_INFO_KHR,
                                                                             nullptr, STD_VIDEO_H264_PROFILE_IDC_BASELINE};
        static const VkVideoProfileInfoKHR encode_h264 = {
            VK_STRUCTURE_TYPE_VIDEO_PROFILE_INFO_KHR,     &encode_h264_baseline,
            VK_VIDEO_CODEC_OPERATION_ENCODE_H264_BIT_KHR, VK_VIDEO_CHROMA_SUBSAMPLING_420_BIT_KHR,
            VK_VIDEO_COMPONENT_BIT_DEPTH_8_BIT_KHR,       VK_VIDEO_COMPONENT_BIT_DEPTH_8_BIT_KHR};
        return &encode_h264;
    }

    const VkVideoProfileInfoKHR* GetEncodeH264MainProfile() const {
        static const VkVideoEncodeH264ProfileInfoKHR encode_h264_main = {VK_STRUCTURE_TYPE_VIDEO_ENCODE_H264_PROFILE_INFO_KHR,
                                                                         nullptr, STD_VIDEO_H264_PROFILE_IDC_MAIN};
        static const VkVideoProfileInfoKHR encode_h264 = {
            VK_STRUCTURE_TYPE_VIDEO_PROFILE_INFO_KHR,     &encode_h264_main,
            VK_VIDEO_CODEC_OPERATION_ENCODE_H264_BIT_KHR, VK_VIDEO_CHROMA_SUBSAMPLING_420_BIT_KHR,
            VK_VIDEO_COMPONENT_BIT_DEPTH_8_BIT_KHR,       VK_VIDEO_COMPONENT_BIT_DEPTH_8_BIT_KHR};
        return &encode_h264;
    }

    const VkVideoProfileInfoKHR* GetEncodeH265MainProfile() const {
        static const VkVideoEncodeH265ProfileInfoKHR encode_h265_main = {VK_STRUCTURE_TYPE_VIDEO_ENCODE_H265_PROFILE_INFO_KHR,
                                                                         nullptr, STD_VIDEO_H265_PROFILE_IDC_MAIN};
        static const VkVideoProfileInfoKHR encode_h265 = {
            VK_STRUCTURE_TYPE_VIDEO_PROFILE_INFO_KHR,     &encode_h265_main,
            VK_VIDEO_CODEC_OPERATION_ENCODE_H265_BIT_KHR, VK_VIDEO_CHROMA_SUBSAMPLING_420_BIT_KHR,
            VK_VIDEO_COMPONENT_BIT_DEPTH_8_BIT_KHR,       VK_VIDEO_COMPONENT_BIT_DEPTH_8_BIT_KHR};
        return &encode_h265;
    }

    const VkVideoProfileInfoKHR* GetEncodeH265Main10Profile() const {
        static const VkVideoEncodeH265ProfileInfoKHR encode_h265_main_10 = {VK_STRUCTURE_TYPE_VIDEO_ENCODE_H265_PROFILE_INFO_KHR,
                                                                            nullptr, STD_VIDEO_H265_PROFILE_IDC_MAIN_10};
        static const VkVideoProfileInfoKHR encode_h265 = {
            VK_STRUCTURE_TYPE_VIDEO_PROFILE_INFO_KHR,     &encode_h265_main_10,
            VK_VIDEO_CODEC_OPERATION_ENCODE_H265_BIT_KHR, VK_VIDEO_CHROMA_SUBSAMPLING_420_BIT_KHR,
            VK_VIDEO_COMPONENT_BIT_DEPTH_10_BIT_KHR,      VK_VIDEO_COMPONENT_BIT_DEPTH_10_BIT_KHR};
        return &encode_h265;
    }

    static void TearDownTestSuite() { inst_builder.reset(); }
};

TEST_F(TestsMechanismVideoProfiles, VideoCapabilities) {
    TEST_DESCRIPTION("Test video capabilities of the profile defined video profiles");

    struct VideoCapabilities {
        VideoCapabilities(const VkVideoProfileInfoKHR* profile_info) {
            switch (profile_info->videoCodecOperation) {
                case VK_VIDEO_CODEC_OPERATION_DECODE_H264_BIT_KHR:
                    base.pNext = &decode;
                    decode.pNext = &decode_h264;
                    break;
                case VK_VIDEO_CODEC_OPERATION_DECODE_H265_BIT_KHR:
                    base.pNext = &decode;
                    decode.pNext = &decode_h265;
                    break;
                case VK_VIDEO_CODEC_OPERATION_ENCODE_H264_BIT_KHR:
                    base.pNext = &encode;
                    encode.pNext = &encode_h264;
                    break;
                case VK_VIDEO_CODEC_OPERATION_ENCODE_H265_BIT_KHR:
                    base.pNext = &encode;
                    encode.pNext = &encode_h265;
                    break;
                default:
                    assert(false);
                    break;
            }
        }

        VkVideoCapabilitiesKHR base{VK_STRUCTURE_TYPE_VIDEO_CAPABILITIES_KHR};
        VkVideoDecodeCapabilitiesKHR decode{VK_STRUCTURE_TYPE_VIDEO_DECODE_CAPABILITIES_KHR};
        VkVideoDecodeH264CapabilitiesKHR decode_h264{VK_STRUCTURE_TYPE_VIDEO_DECODE_H264_CAPABILITIES_KHR};
        VkVideoDecodeH265CapabilitiesKHR decode_h265{VK_STRUCTURE_TYPE_VIDEO_DECODE_H265_CAPABILITIES_KHR};
        VkVideoEncodeCapabilitiesKHR encode{VK_STRUCTURE_TYPE_VIDEO_ENCODE_CAPABILITIES_KHR};
        VkVideoEncodeH264CapabilitiesKHR encode_h264{VK_STRUCTURE_TYPE_VIDEO_ENCODE_H264_CAPABILITIES_KHR};
        VkVideoEncodeH265CapabilitiesKHR encode_h265{VK_STRUCTURE_TYPE_VIDEO_ENCODE_H265_CAPABILITIES_KHR};
    };

    // H.264 Decode (4:2:0 8-bit) Baseline progressive
    {
        auto profile = GetDecodeH264BaselineProfile();
        VideoCapabilities caps(profile);

        EXPECT_EQ(pfnGetPhysicalDeviceVideoCapabilitiesKHR(gpu, profile, &caps.base), VK_SUCCESS);

        EXPECT_EQ(caps.base.minBitstreamBufferOffsetAlignment, 256);
        EXPECT_EQ(caps.base.minBitstreamBufferSizeAlignment, 4096);
        EXPECT_EQ(caps.base.maxCodedExtent.width, 1920);
        EXPECT_EQ(caps.base.maxCodedExtent.height, 1080);
        EXPECT_EQ(caps.base.maxDpbSlots, 17);
        EXPECT_EQ(caps.base.maxActiveReferencePictures, 16);
        EXPECT_EQ(caps.decode_h264.maxLevelIdc, STD_VIDEO_H264_LEVEL_IDC_6_1);
    }

    // H.264 Decode (4:2:0 8-bit) Main interlaced
    {
        auto profile = GetDecodeH264MainProfile();
        VideoCapabilities caps(profile);

        EXPECT_EQ(pfnGetPhysicalDeviceVideoCapabilitiesKHR(gpu, profile, &caps.base), VK_SUCCESS);

        EXPECT_EQ(caps.base.minBitstreamBufferOffsetAlignment, 256);
        EXPECT_EQ(caps.base.minBitstreamBufferSizeAlignment, 4096);
        EXPECT_EQ(caps.base.maxCodedExtent.width, 1280);
        EXPECT_EQ(caps.base.maxCodedExtent.height, 1024);
        EXPECT_EQ(caps.base.maxDpbSlots, 33);
        EXPECT_EQ(caps.base.maxActiveReferencePictures, 32);
        EXPECT_EQ(caps.decode_h264.maxLevelIdc, STD_VIDEO_H264_LEVEL_IDC_5_0);
        EXPECT_EQ(caps.decode_h264.fieldOffsetGranularity.x, 48);
        EXPECT_EQ(caps.decode_h264.fieldOffsetGranularity.y, 16);
    }

    // H.264 Decode (4:2:0 8-bit) Main
    {
        auto profile = GetDecodeH265MainProfile();
        VideoCapabilities caps(profile);

        EXPECT_EQ(pfnGetPhysicalDeviceVideoCapabilitiesKHR(gpu, profile, &caps.base), VK_SUCCESS);

        EXPECT_EQ(caps.base.minBitstreamBufferOffsetAlignment, 1024);
        EXPECT_EQ(caps.base.minBitstreamBufferSizeAlignment, 4096);
        EXPECT_EQ(caps.base.maxCodedExtent.width, 3840);
        EXPECT_EQ(caps.base.maxCodedExtent.height, 2160);
        EXPECT_EQ(caps.base.maxDpbSlots, 17);
        EXPECT_EQ(caps.base.maxActiveReferencePictures, 16);
        EXPECT_EQ(caps.decode_h265.maxLevelIdc, STD_VIDEO_H265_LEVEL_IDC_5_0);
    }

    // H.264 Decode (4:2:0 10-bit) Main 10
    {
        auto profile = GetDecodeH265Main10Profile();
        VideoCapabilities caps(profile);

        EXPECT_EQ(pfnGetPhysicalDeviceVideoCapabilitiesKHR(gpu, profile, &caps.base), VK_SUCCESS);

        EXPECT_EQ(caps.base.minBitstreamBufferOffsetAlignment, 4096);
        EXPECT_EQ(caps.base.minBitstreamBufferSizeAlignment, 4096);
        EXPECT_EQ(caps.base.maxCodedExtent.width, 2560);
        EXPECT_EQ(caps.base.maxCodedExtent.height, 1440);
        EXPECT_EQ(caps.base.maxDpbSlots, 17);
        EXPECT_EQ(caps.base.maxActiveReferencePictures, 16);
        EXPECT_EQ(caps.decode_h265.maxLevelIdc, STD_VIDEO_H265_LEVEL_IDC_4_1);
    }

    // H.264 Encode (4:2:0 8-bit) Baseline
    {
        auto profile = GetEncodeH264BaselineProfile();
        VideoCapabilities caps(profile);

        // Test with and without quantization map capabilities
        VkVideoEncodeQuantizationMapCapabilitiesKHR caps_quant_map{VK_STRUCTURE_TYPE_VIDEO_ENCODE_QUANTIZATION_MAP_CAPABILITIES_KHR,
                                                                   caps.base.pNext};
        for (uint32_t i = 0; i < 2; ++i) {
            if (i == 1) {
                caps.base.pNext = &caps_quant_map;
            }

            EXPECT_EQ(pfnGetPhysicalDeviceVideoCapabilitiesKHR(gpu, profile, &caps.base), VK_SUCCESS);

            EXPECT_EQ(caps.base.minBitstreamBufferOffsetAlignment, 4096);
            EXPECT_EQ(caps.base.minBitstreamBufferSizeAlignment, 4096);
            EXPECT_EQ(caps.base.maxCodedExtent.width, 1920);
            EXPECT_EQ(caps.base.maxCodedExtent.height, 1080);
            EXPECT_EQ(caps.base.maxDpbSlots, 2);
            EXPECT_EQ(caps.base.maxActiveReferencePictures, 1);
            EXPECT_EQ(caps.encode.flags, VK_VIDEO_ENCODE_CAPABILITY_INSUFFICIENT_BITSTREAM_BUFFER_RANGE_DETECTION_BIT_KHR |
                                             VK_VIDEO_ENCODE_CAPABILITY_EMPHASIS_MAP_BIT_KHR |
                                             VK_VIDEO_ENCODE_CAPABILITY_QUANTIZATION_DELTA_MAP_BIT_KHR);
            EXPECT_EQ(caps.encode.maxRateControlLayers, 1);
            EXPECT_EQ(caps.encode.supportedEncodeFeedbackFlags, VK_VIDEO_ENCODE_FEEDBACK_BITSTREAM_BUFFER_OFFSET_BIT_KHR |
                                                                    VK_VIDEO_ENCODE_FEEDBACK_BITSTREAM_BYTES_WRITTEN_BIT_KHR);
            EXPECT_EQ(caps.encode_h264.flags, VK_VIDEO_ENCODE_H264_CAPABILITY_GENERATE_PREFIX_NALU_BIT_KHR);
            EXPECT_EQ(caps.encode_h264.maxSliceCount, 4);
            EXPECT_EQ(caps.encode_h264.maxPPictureL0ReferenceCount, 1);

            if (i == 1) {
                EXPECT_EQ(caps_quant_map.maxQuantizationMapExtent.width, 256);
                EXPECT_EQ(caps_quant_map.maxQuantizationMapExtent.height, 256);
            }
        }
    }

    // H.264 Encode (4:2:0 8-bit) Main
    {
        auto profile = GetEncodeH264MainProfile();
        VideoCapabilities caps(profile);

        EXPECT_EQ(pfnGetPhysicalDeviceVideoCapabilitiesKHR(gpu, profile, &caps.base), VK_SUCCESS);

        // Test with and without quantization map capabilities
        VkVideoEncodeQuantizationMapCapabilitiesKHR caps_quant_map{VK_STRUCTURE_TYPE_VIDEO_ENCODE_QUANTIZATION_MAP_CAPABILITIES_KHR,
                                                                   caps.base.pNext};
        for (uint32_t i = 0; i < 2; ++i) {
            if (i == 1) {
                caps.base.pNext = &caps_quant_map;
            }

            EXPECT_EQ(pfnGetPhysicalDeviceVideoCapabilitiesKHR(gpu, profile, &caps.base), VK_SUCCESS);

            EXPECT_EQ(caps.base.minBitstreamBufferOffsetAlignment, 4096);
            EXPECT_EQ(caps.base.minBitstreamBufferSizeAlignment, 4096);
            EXPECT_EQ(caps.base.maxCodedExtent.width, 960);
            EXPECT_EQ(caps.base.maxCodedExtent.height, 1024);
            EXPECT_EQ(caps.base.maxDpbSlots, 2);
            EXPECT_EQ(caps.base.maxActiveReferencePictures, 1);
            EXPECT_EQ(caps.encode.flags, VK_VIDEO_ENCODE_CAPABILITY_INSUFFICIENT_BITSTREAM_BUFFER_RANGE_DETECTION_BIT_KHR |
                                             VK_VIDEO_ENCODE_CAPABILITY_EMPHASIS_MAP_BIT_KHR |
                                             VK_VIDEO_ENCODE_CAPABILITY_QUANTIZATION_DELTA_MAP_BIT_KHR);
            EXPECT_EQ(caps.encode.maxRateControlLayers, 1);
            EXPECT_EQ(caps.encode.supportedEncodeFeedbackFlags, VK_VIDEO_ENCODE_FEEDBACK_BITSTREAM_BUFFER_OFFSET_BIT_KHR |
                                                                    VK_VIDEO_ENCODE_FEEDBACK_BITSTREAM_BYTES_WRITTEN_BIT_KHR);
            EXPECT_EQ(caps.encode_h264.flags, VK_VIDEO_ENCODE_H264_CAPABILITY_GENERATE_PREFIX_NALU_BIT_KHR);
            EXPECT_EQ(caps.encode_h264.maxSliceCount, 1);
            EXPECT_EQ(caps.encode_h264.maxPPictureL0ReferenceCount, 2);

            if (i == 1) {
                EXPECT_EQ(caps_quant_map.maxQuantizationMapExtent.width, 256);
                EXPECT_EQ(caps_quant_map.maxQuantizationMapExtent.height, 256);
            }
        }
    }

    // H.265 Encode (4:2:0 8-bit) Main
    {
        auto profile = GetEncodeH265MainProfile();
        VideoCapabilities caps(profile);

        EXPECT_EQ(pfnGetPhysicalDeviceVideoCapabilitiesKHR(gpu, profile, &caps.base), VK_SUCCESS);

        // Test with and without quantization map capabilities
        VkVideoEncodeQuantizationMapCapabilitiesKHR caps_quant_map{VK_STRUCTURE_TYPE_VIDEO_ENCODE_QUANTIZATION_MAP_CAPABILITIES_KHR,
                                                                   caps.base.pNext};
        for (uint32_t i = 0; i < 2; ++i) {
            if (i == 1) {
                caps.base.pNext = &caps_quant_map;
            }

            EXPECT_EQ(pfnGetPhysicalDeviceVideoCapabilitiesKHR(gpu, profile, &caps.base), VK_SUCCESS);

            EXPECT_EQ(caps.base.minBitstreamBufferOffsetAlignment, 4096);
            EXPECT_EQ(caps.base.minBitstreamBufferSizeAlignment, 4096);
            EXPECT_EQ(caps.base.maxCodedExtent.width, 960);
            EXPECT_EQ(caps.base.maxCodedExtent.height, 1024);
            EXPECT_EQ(caps.base.maxDpbSlots, 3);
            EXPECT_EQ(caps.base.maxActiveReferencePictures, 2);
            EXPECT_EQ(caps.encode.flags, VK_VIDEO_ENCODE_CAPABILITY_INSUFFICIENT_BITSTREAM_BUFFER_RANGE_DETECTION_BIT_KHR |
                                             VK_VIDEO_ENCODE_CAPABILITY_PRECEDING_EXTERNALLY_ENCODED_BYTES_BIT_KHR |
                                             VK_VIDEO_ENCODE_CAPABILITY_EMPHASIS_MAP_BIT_KHR |
                                             VK_VIDEO_ENCODE_CAPABILITY_QUANTIZATION_DELTA_MAP_BIT_KHR);
            EXPECT_EQ(caps.encode.maxRateControlLayers, 1);
            EXPECT_EQ(caps.encode.supportedEncodeFeedbackFlags, VK_VIDEO_ENCODE_FEEDBACK_BITSTREAM_BUFFER_OFFSET_BIT_KHR |
                                                                    VK_VIDEO_ENCODE_FEEDBACK_BITSTREAM_BYTES_WRITTEN_BIT_KHR);
            EXPECT_EQ(caps.encode_h265.maxSliceSegmentCount, 3);
            EXPECT_EQ(caps.encode_h265.maxBPictureL0ReferenceCount, 1);
            EXPECT_EQ(caps.encode_h265.maxL1ReferenceCount, 1);

            if (i == 1) {
                EXPECT_EQ(caps_quant_map.maxQuantizationMapExtent.width, 192);
                EXPECT_EQ(caps_quant_map.maxQuantizationMapExtent.height, 128);
            }
        }
    }

    // H.265 Encode (4:2:0 10-bit) Main 10
    {
        auto profile = GetEncodeH265Main10Profile();
        VideoCapabilities caps(profile);

        EXPECT_EQ(pfnGetPhysicalDeviceVideoCapabilitiesKHR(gpu, profile, &caps.base), VK_SUCCESS);

        // Test with and without quantization map capabilities
        VkVideoEncodeQuantizationMapCapabilitiesKHR caps_quant_map{VK_STRUCTURE_TYPE_VIDEO_ENCODE_QUANTIZATION_MAP_CAPABILITIES_KHR,
                                                                   caps.base.pNext};
        for (uint32_t i = 0; i < 2; ++i) {
            if (i == 1) {
                caps.base.pNext = &caps_quant_map;
            }

            EXPECT_EQ(pfnGetPhysicalDeviceVideoCapabilitiesKHR(gpu, profile, &caps.base), VK_SUCCESS);

            EXPECT_EQ(caps.base.minBitstreamBufferOffsetAlignment, 4096);
            EXPECT_EQ(caps.base.minBitstreamBufferSizeAlignment, 4096);
            EXPECT_EQ(caps.base.maxCodedExtent.width, 1280);
            EXPECT_EQ(caps.base.maxCodedExtent.height, 1024);
            EXPECT_EQ(caps.base.maxDpbSlots, 2);
            EXPECT_EQ(caps.base.maxActiveReferencePictures, 1);
            EXPECT_EQ(caps.encode.flags, VK_VIDEO_ENCODE_CAPABILITY_INSUFFICIENT_BITSTREAM_BUFFER_RANGE_DETECTION_BIT_KHR |
                                             VK_VIDEO_ENCODE_CAPABILITY_EMPHASIS_MAP_BIT_KHR);
            EXPECT_EQ(caps.encode.maxRateControlLayers, 1);
            EXPECT_EQ(caps.encode.supportedEncodeFeedbackFlags, VK_VIDEO_ENCODE_FEEDBACK_BITSTREAM_BUFFER_OFFSET_BIT_KHR |
                                                                    VK_VIDEO_ENCODE_FEEDBACK_BITSTREAM_BYTES_WRITTEN_BIT_KHR);
            EXPECT_EQ(caps.encode_h265.maxSliceSegmentCount, 1);

            if (i == 1) {
                EXPECT_EQ(caps_quant_map.maxQuantizationMapExtent.width, 64);
                EXPECT_EQ(caps_quant_map.maxQuantizationMapExtent.height, 64);
            }
        }
    }
}

TEST_F(TestsMechanismVideoProfiles, VideoFormatProperties) {
    TEST_DESCRIPTION("Test video format properties of the profile defined video profiles");

    const uint32_t max_formats = 256;
    std::vector<VkVideoFormatPropertiesKHR> fmt_props(max_formats);
    std::vector<VkVideoFormatQuantizationMapPropertiesKHR> quant_map_props(max_formats);
    std::vector<VkVideoFormatH265QuantizationMapPropertiesKHR> quant_map_props_h265(max_formats);
    for (uint32_t i = 0; i < max_formats; ++i) {
        fmt_props[i].sType = VK_STRUCTURE_TYPE_VIDEO_FORMAT_PROPERTIES_KHR;
        fmt_props[i].pNext = &quant_map_props[i];
        quant_map_props[i].sType = VK_STRUCTURE_TYPE_VIDEO_FORMAT_QUANTIZATION_MAP_PROPERTIES_KHR;
        quant_map_props[i].pNext = &quant_map_props_h265[i];
        quant_map_props_h265[i].sType = VK_STRUCTURE_TYPE_VIDEO_FORMAT_H265_QUANTIZATION_MAP_PROPERTIES_KHR;
    }

    struct VideoFormatInfo {
        VideoFormatInfo(const VkVideoProfileInfoKHR* profile, VkImageUsageFlags usage) {
            info.pNext = &profile_list;
            info.imageUsage = usage;
            profile_list.profileCount = 1;
            profile_list.pProfiles = profile;
        }

        VkPhysicalDeviceVideoFormatInfoKHR info{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VIDEO_FORMAT_INFO_KHR};
        VkVideoProfileListInfoKHR profile_list{VK_STRUCTURE_TYPE_VIDEO_PROFILE_LIST_INFO_KHR};
    };

    // H.264 Decode (4:2:0 8-bit) Baseline progressive
    {
        auto profile = GetDecodeH264BaselineProfile();

        // Decode Output
        {
            VideoFormatInfo info(profile, VK_IMAGE_USAGE_VIDEO_DECODE_DST_BIT_KHR);
            uint32_t count = 0;

            EXPECT_EQ(pfnGetPhysicalDeviceVideoFormatPropertiesKHR(gpu, &info.info, &count, nullptr), VK_SUCCESS);
            EXPECT_GE(count, 2);
            const uint32_t count_received = count;

            count = max_formats;
            EXPECT_EQ(pfnGetPhysicalDeviceVideoFormatPropertiesKHR(gpu, &info.info, &count, fmt_props.data()), VK_SUCCESS);
            EXPECT_EQ(count, count_received);

            bool found_g8_b8r8 = false;
            bool found_g8_b8_r8 = false;

            for (uint32_t i = 0; i < count; ++i) {
                if (fmt_props[i].format == VK_FORMAT_G8_B8R8_2PLANE_420_UNORM &&
                    fmt_props[i].imageTiling == VK_IMAGE_TILING_LINEAR) {
                    found_g8_b8r8 = true;
                    EXPECT_EQ(fmt_props[i].componentMapping.r, VK_COMPONENT_SWIZZLE_IDENTITY);
                    EXPECT_EQ(fmt_props[i].componentMapping.g, VK_COMPONENT_SWIZZLE_IDENTITY);
                    EXPECT_EQ(fmt_props[i].componentMapping.b, VK_COMPONENT_SWIZZLE_IDENTITY);
                    EXPECT_EQ(fmt_props[i].componentMapping.a, VK_COMPONENT_SWIZZLE_IDENTITY);
                    EXPECT_EQ(fmt_props[i].imageCreateFlags,
                              VK_IMAGE_CREATE_MUTABLE_FORMAT_BIT | VK_IMAGE_CREATE_EXTENDED_USAGE_BIT);
                    EXPECT_EQ(fmt_props[i].imageType, VK_IMAGE_TYPE_2D);
                    EXPECT_EQ(fmt_props[i].imageUsageFlags,
                              VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT |
                                  VK_IMAGE_USAGE_VIDEO_DECODE_DST_BIT_KHR | VK_IMAGE_USAGE_VIDEO_ENCODE_SRC_BIT_KHR);
                }

                if (fmt_props[i].format == VK_FORMAT_G8_B8_R8_3PLANE_420_UNORM &&
                    fmt_props[i].imageTiling == VK_IMAGE_TILING_OPTIMAL) {
                    found_g8_b8_r8 = true;
                    EXPECT_EQ(fmt_props[i].componentMapping.r, VK_COMPONENT_SWIZZLE_IDENTITY);
                    EXPECT_EQ(fmt_props[i].componentMapping.g, VK_COMPONENT_SWIZZLE_IDENTITY);
                    EXPECT_EQ(fmt_props[i].componentMapping.b, VK_COMPONENT_SWIZZLE_IDENTITY);
                    EXPECT_EQ(fmt_props[i].componentMapping.a, VK_COMPONENT_SWIZZLE_IDENTITY);
                    EXPECT_EQ(fmt_props[i].imageCreateFlags,
                              VK_IMAGE_CREATE_MUTABLE_FORMAT_BIT | VK_IMAGE_CREATE_EXTENDED_USAGE_BIT);
                    EXPECT_EQ(fmt_props[i].imageType, VK_IMAGE_TYPE_2D);
                    EXPECT_EQ(fmt_props[i].imageUsageFlags,
                              VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT |
                                  VK_IMAGE_USAGE_VIDEO_DECODE_DST_BIT_KHR | VK_IMAGE_USAGE_VIDEO_ENCODE_SRC_BIT_KHR);
                }
            }

            EXPECT_TRUE(found_g8_b8r8);
            EXPECT_TRUE(found_g8_b8_r8);
        }

        // Decode DPB
        {
            VideoFormatInfo info(profile, VK_IMAGE_USAGE_VIDEO_DECODE_DPB_BIT_KHR);
            uint32_t count = 0;

            EXPECT_EQ(pfnGetPhysicalDeviceVideoFormatPropertiesKHR(gpu, &info.info, &count, nullptr), VK_SUCCESS);
            EXPECT_GE(count, 1);
            const uint32_t count_received = count;

            count = max_formats;
            EXPECT_EQ(pfnGetPhysicalDeviceVideoFormatPropertiesKHR(gpu, &info.info, &count, fmt_props.data()), VK_SUCCESS);
            EXPECT_EQ(count, count_received);

            bool found = false;

            for (uint32_t i = 0; i < count; ++i) {
                if (fmt_props[i].format == VK_FORMAT_G16_B16R16_2PLANE_420_UNORM &&
                    fmt_props[i].imageTiling == VK_IMAGE_TILING_OPTIMAL) {
                    found = true;
                    EXPECT_EQ(fmt_props[i].componentMapping.r, VK_COMPONENT_SWIZZLE_IDENTITY);
                    EXPECT_EQ(fmt_props[i].componentMapping.g, VK_COMPONENT_SWIZZLE_IDENTITY);
                    EXPECT_EQ(fmt_props[i].componentMapping.b, VK_COMPONENT_SWIZZLE_IDENTITY);
                    EXPECT_EQ(fmt_props[i].componentMapping.a, VK_COMPONENT_SWIZZLE_IDENTITY);
                    EXPECT_EQ(fmt_props[i].imageCreateFlags, 0);
                    EXPECT_EQ(fmt_props[i].imageType, VK_IMAGE_TYPE_2D);
                    EXPECT_EQ(fmt_props[i].imageUsageFlags, VK_IMAGE_USAGE_VIDEO_DECODE_DPB_BIT_KHR);
                }
            }

            EXPECT_TRUE(found);
        }
    }

    // H.264 Decode (4:2:0 8-bit) Main interlaced
    {
        auto profile = GetDecodeH264MainProfile();

        // Decode Output
        {
            VideoFormatInfo info(profile, VK_IMAGE_USAGE_VIDEO_DECODE_DST_BIT_KHR);
            uint32_t count = 0;

            EXPECT_EQ(pfnGetPhysicalDeviceVideoFormatPropertiesKHR(gpu, &info.info, &count, nullptr), VK_SUCCESS);
            EXPECT_GE(count, 2);
            const uint32_t count_received = count;

            count = max_formats;
            EXPECT_EQ(pfnGetPhysicalDeviceVideoFormatPropertiesKHR(gpu, &info.info, &count, fmt_props.data()), VK_SUCCESS);
            EXPECT_EQ(count, count_received);

            bool found_g8_b8r8 = false;
            bool found_g8_b8_r8 = false;

            for (uint32_t i = 0; i < count; ++i) {
                if (fmt_props[i].format == VK_FORMAT_G8_B8R8_2PLANE_420_UNORM &&
                    fmt_props[i].imageTiling == VK_IMAGE_TILING_LINEAR) {
                    found_g8_b8r8 = true;
                    EXPECT_EQ(fmt_props[i].componentMapping.r, VK_COMPONENT_SWIZZLE_IDENTITY);
                    EXPECT_EQ(fmt_props[i].componentMapping.g, VK_COMPONENT_SWIZZLE_IDENTITY);
                    EXPECT_EQ(fmt_props[i].componentMapping.b, VK_COMPONENT_SWIZZLE_IDENTITY);
                    EXPECT_EQ(fmt_props[i].componentMapping.a, VK_COMPONENT_SWIZZLE_IDENTITY);
                    EXPECT_EQ(fmt_props[i].imageCreateFlags,
                              VK_IMAGE_CREATE_MUTABLE_FORMAT_BIT | VK_IMAGE_CREATE_EXTENDED_USAGE_BIT);
                    EXPECT_EQ(fmt_props[i].imageType, VK_IMAGE_TYPE_2D);
                    EXPECT_EQ(fmt_props[i].imageUsageFlags,
                              VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT |
                                  VK_IMAGE_USAGE_VIDEO_DECODE_DST_BIT_KHR | VK_IMAGE_USAGE_VIDEO_ENCODE_SRC_BIT_KHR);
                }

                if (fmt_props[i].format == VK_FORMAT_G8_B8_R8_3PLANE_420_UNORM &&
                    fmt_props[i].imageTiling == VK_IMAGE_TILING_OPTIMAL) {
                    found_g8_b8_r8 = true;
                    EXPECT_EQ(fmt_props[i].componentMapping.r, VK_COMPONENT_SWIZZLE_IDENTITY);
                    EXPECT_EQ(fmt_props[i].componentMapping.g, VK_COMPONENT_SWIZZLE_IDENTITY);
                    EXPECT_EQ(fmt_props[i].componentMapping.b, VK_COMPONENT_SWIZZLE_IDENTITY);
                    EXPECT_EQ(fmt_props[i].componentMapping.a, VK_COMPONENT_SWIZZLE_IDENTITY);
                    EXPECT_EQ(fmt_props[i].imageCreateFlags,
                              VK_IMAGE_CREATE_MUTABLE_FORMAT_BIT | VK_IMAGE_CREATE_EXTENDED_USAGE_BIT);
                    EXPECT_EQ(fmt_props[i].imageType, VK_IMAGE_TYPE_2D);
                    EXPECT_EQ(fmt_props[i].imageUsageFlags,
                              VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT |
                                  VK_IMAGE_USAGE_VIDEO_DECODE_DST_BIT_KHR | VK_IMAGE_USAGE_VIDEO_ENCODE_SRC_BIT_KHR);
                }
            }

            EXPECT_TRUE(found_g8_b8r8);
            EXPECT_TRUE(found_g8_b8_r8);
        }

        // Decode DPB
        {
            VideoFormatInfo info(profile, VK_IMAGE_USAGE_VIDEO_DECODE_DPB_BIT_KHR);
            uint32_t count = 0;

            EXPECT_EQ(pfnGetPhysicalDeviceVideoFormatPropertiesKHR(gpu, &info.info, &count, nullptr), VK_SUCCESS);
            EXPECT_GE(count, 1);
            const uint32_t count_received = count;

            count = max_formats;
            EXPECT_EQ(pfnGetPhysicalDeviceVideoFormatPropertiesKHR(gpu, &info.info, &count, fmt_props.data()), VK_SUCCESS);
            EXPECT_EQ(count, count_received);

            bool found = false;

            for (uint32_t i = 0; i < count; ++i) {
                if (fmt_props[i].format == VK_FORMAT_G16_B16R16_2PLANE_420_UNORM &&
                    fmt_props[i].imageTiling == VK_IMAGE_TILING_OPTIMAL) {
                    found = true;
                    EXPECT_EQ(fmt_props[i].componentMapping.r, VK_COMPONENT_SWIZZLE_IDENTITY);
                    EXPECT_EQ(fmt_props[i].componentMapping.g, VK_COMPONENT_SWIZZLE_IDENTITY);
                    EXPECT_EQ(fmt_props[i].componentMapping.b, VK_COMPONENT_SWIZZLE_IDENTITY);
                    EXPECT_EQ(fmt_props[i].componentMapping.a, VK_COMPONENT_SWIZZLE_IDENTITY);
                    EXPECT_EQ(fmt_props[i].imageCreateFlags, 0);
                    EXPECT_EQ(fmt_props[i].imageType, VK_IMAGE_TYPE_2D);
                    EXPECT_EQ(fmt_props[i].imageUsageFlags, VK_IMAGE_USAGE_VIDEO_DECODE_DPB_BIT_KHR);
                }
            }

            EXPECT_TRUE(found);
        }
    }

    // H.264 Decode (4:2:0 8-bit) Main
    {
        auto profile = GetDecodeH265MainProfile();

        // Decode Output
        {
            VideoFormatInfo info(profile, VK_IMAGE_USAGE_VIDEO_DECODE_DST_BIT_KHR);
            uint32_t count = 0;

            EXPECT_EQ(pfnGetPhysicalDeviceVideoFormatPropertiesKHR(gpu, &info.info, &count, nullptr), VK_SUCCESS);
            EXPECT_GE(count, 2);
            const uint32_t count_received = count;

            count = max_formats;
            EXPECT_EQ(pfnGetPhysicalDeviceVideoFormatPropertiesKHR(gpu, &info.info, &count, fmt_props.data()), VK_SUCCESS);
            EXPECT_EQ(count, count_received);

            bool found_g8_b8r8 = false;
            bool found_g8_b8_r8 = false;

            for (uint32_t i = 0; i < count; ++i) {
                if (fmt_props[i].format == VK_FORMAT_G8_B8R8_2PLANE_420_UNORM &&
                    fmt_props[i].imageTiling == VK_IMAGE_TILING_LINEAR) {
                    found_g8_b8r8 = true;
                    EXPECT_EQ(fmt_props[i].componentMapping.r, VK_COMPONENT_SWIZZLE_IDENTITY);
                    EXPECT_EQ(fmt_props[i].componentMapping.g, VK_COMPONENT_SWIZZLE_IDENTITY);
                    EXPECT_EQ(fmt_props[i].componentMapping.b, VK_COMPONENT_SWIZZLE_IDENTITY);
                    EXPECT_EQ(fmt_props[i].componentMapping.a, VK_COMPONENT_SWIZZLE_IDENTITY);
                    EXPECT_EQ(fmt_props[i].imageCreateFlags,
                              VK_IMAGE_CREATE_MUTABLE_FORMAT_BIT | VK_IMAGE_CREATE_EXTENDED_USAGE_BIT);
                    EXPECT_EQ(fmt_props[i].imageType, VK_IMAGE_TYPE_2D);
                    EXPECT_EQ(fmt_props[i].imageUsageFlags,
                              VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT |
                                  VK_IMAGE_USAGE_VIDEO_DECODE_DST_BIT_KHR | VK_IMAGE_USAGE_VIDEO_ENCODE_SRC_BIT_KHR);
                }

                if (fmt_props[i].format == VK_FORMAT_G8_B8_R8_3PLANE_420_UNORM &&
                    fmt_props[i].imageTiling == VK_IMAGE_TILING_OPTIMAL) {
                    found_g8_b8_r8 = true;
                    EXPECT_EQ(fmt_props[i].componentMapping.r, VK_COMPONENT_SWIZZLE_IDENTITY);
                    EXPECT_EQ(fmt_props[i].componentMapping.g, VK_COMPONENT_SWIZZLE_IDENTITY);
                    EXPECT_EQ(fmt_props[i].componentMapping.b, VK_COMPONENT_SWIZZLE_IDENTITY);
                    EXPECT_EQ(fmt_props[i].componentMapping.a, VK_COMPONENT_SWIZZLE_IDENTITY);
                    EXPECT_EQ(fmt_props[i].imageCreateFlags,
                              VK_IMAGE_CREATE_MUTABLE_FORMAT_BIT | VK_IMAGE_CREATE_EXTENDED_USAGE_BIT);
                    EXPECT_EQ(fmt_props[i].imageType, VK_IMAGE_TYPE_2D);
                    EXPECT_EQ(fmt_props[i].imageUsageFlags,
                              VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT |
                                  VK_IMAGE_USAGE_VIDEO_DECODE_DST_BIT_KHR | VK_IMAGE_USAGE_VIDEO_ENCODE_SRC_BIT_KHR);
                }
            }

            EXPECT_TRUE(found_g8_b8r8);
            EXPECT_TRUE(found_g8_b8_r8);
        }

        // Decode DPB
        {
            VideoFormatInfo info(profile, VK_IMAGE_USAGE_VIDEO_DECODE_DPB_BIT_KHR);
            uint32_t count = 0;

            EXPECT_EQ(pfnGetPhysicalDeviceVideoFormatPropertiesKHR(gpu, &info.info, &count, nullptr), VK_SUCCESS);
            EXPECT_GE(count, 1);
            const uint32_t count_received = count;

            count = max_formats;
            EXPECT_EQ(pfnGetPhysicalDeviceVideoFormatPropertiesKHR(gpu, &info.info, &count, fmt_props.data()), VK_SUCCESS);
            EXPECT_EQ(count, count_received);

            bool found = false;

            for (uint32_t i = 0; i < count; ++i) {
                if (fmt_props[i].format == VK_FORMAT_G16_B16R16_2PLANE_420_UNORM &&
                    fmt_props[i].imageTiling == VK_IMAGE_TILING_OPTIMAL) {
                    found = true;
                    EXPECT_EQ(fmt_props[i].componentMapping.r, VK_COMPONENT_SWIZZLE_IDENTITY);
                    EXPECT_EQ(fmt_props[i].componentMapping.g, VK_COMPONENT_SWIZZLE_IDENTITY);
                    EXPECT_EQ(fmt_props[i].componentMapping.b, VK_COMPONENT_SWIZZLE_IDENTITY);
                    EXPECT_EQ(fmt_props[i].componentMapping.a, VK_COMPONENT_SWIZZLE_IDENTITY);
                    EXPECT_EQ(fmt_props[i].imageCreateFlags, 0);
                    EXPECT_EQ(fmt_props[i].imageType, VK_IMAGE_TYPE_2D);
                    EXPECT_EQ(fmt_props[i].imageUsageFlags, VK_IMAGE_USAGE_VIDEO_DECODE_DPB_BIT_KHR);
                }
            }

            EXPECT_TRUE(found);
        }
    }

    // H.264 Decode (4:2:0 10-bit) Main 10
    {
        auto profile = GetDecodeH265Main10Profile();

        // Decode Output
        {
            VideoFormatInfo info(profile, VK_IMAGE_USAGE_VIDEO_DECODE_DST_BIT_KHR);
            uint32_t count = 0;

            EXPECT_EQ(pfnGetPhysicalDeviceVideoFormatPropertiesKHR(gpu, &info.info, &count, nullptr), VK_SUCCESS);
            EXPECT_GE(count, 1);
            const uint32_t count_received = count;

            count = max_formats;
            EXPECT_EQ(pfnGetPhysicalDeviceVideoFormatPropertiesKHR(gpu, &info.info, &count, fmt_props.data()), VK_SUCCESS);
            EXPECT_EQ(count, count_received);

            bool found = false;

            for (uint32_t i = 0; i < count; ++i) {
                if (fmt_props[i].format == VK_FORMAT_G10X6_B10X6R10X6_2PLANE_420_UNORM_3PACK16 &&
                    fmt_props[i].imageTiling == VK_IMAGE_TILING_LINEAR) {
                    found = true;
                    EXPECT_EQ(fmt_props[i].componentMapping.r, VK_COMPONENT_SWIZZLE_IDENTITY);
                    EXPECT_EQ(fmt_props[i].componentMapping.g, VK_COMPONENT_SWIZZLE_IDENTITY);
                    EXPECT_EQ(fmt_props[i].componentMapping.b, VK_COMPONENT_SWIZZLE_IDENTITY);
                    EXPECT_EQ(fmt_props[i].componentMapping.a, VK_COMPONENT_SWIZZLE_IDENTITY);
                    EXPECT_EQ(fmt_props[i].imageCreateFlags,
                              VK_IMAGE_CREATE_MUTABLE_FORMAT_BIT | VK_IMAGE_CREATE_EXTENDED_USAGE_BIT);
                    EXPECT_EQ(fmt_props[i].imageType, VK_IMAGE_TYPE_2D);
                    EXPECT_EQ(fmt_props[i].imageUsageFlags,
                              VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT |
                                  VK_IMAGE_USAGE_VIDEO_DECODE_DST_BIT_KHR | VK_IMAGE_USAGE_VIDEO_ENCODE_SRC_BIT_KHR);
                }
            }

            EXPECT_TRUE(found);
        }

        // Decode DPB
        {
            VideoFormatInfo info(profile, VK_IMAGE_USAGE_VIDEO_DECODE_DPB_BIT_KHR);
            uint32_t count = 0;

            EXPECT_EQ(pfnGetPhysicalDeviceVideoFormatPropertiesKHR(gpu, &info.info, &count, nullptr), VK_SUCCESS);
            EXPECT_GE(count, 1);
            const uint32_t count_received = count;

            count = max_formats;
            EXPECT_EQ(pfnGetPhysicalDeviceVideoFormatPropertiesKHR(gpu, &info.info, &count, fmt_props.data()), VK_SUCCESS);
            EXPECT_EQ(count, count_received);

            bool found = false;

            for (uint32_t i = 0; i < count; ++i) {
                if (fmt_props[i].format == VK_FORMAT_G16_B16R16_2PLANE_420_UNORM &&
                    fmt_props[i].imageTiling == VK_IMAGE_TILING_OPTIMAL) {
                    found = true;
                    EXPECT_EQ(fmt_props[i].componentMapping.r, VK_COMPONENT_SWIZZLE_IDENTITY);
                    EXPECT_EQ(fmt_props[i].componentMapping.g, VK_COMPONENT_SWIZZLE_IDENTITY);
                    EXPECT_EQ(fmt_props[i].componentMapping.b, VK_COMPONENT_SWIZZLE_IDENTITY);
                    EXPECT_EQ(fmt_props[i].componentMapping.a, VK_COMPONENT_SWIZZLE_IDENTITY);
                    EXPECT_EQ(fmt_props[i].imageCreateFlags, 0);
                    EXPECT_EQ(fmt_props[i].imageType, VK_IMAGE_TYPE_2D);
                    EXPECT_EQ(fmt_props[i].imageUsageFlags, VK_IMAGE_USAGE_VIDEO_DECODE_DPB_BIT_KHR);
                }
            }

            EXPECT_TRUE(found);
        }
    }

    // H.264 Encode (4:2:0 8-bit) Baseline
    {
        auto profile = GetEncodeH264BaselineProfile();

        // Encode Input
        {
            VideoFormatInfo info(profile, VK_IMAGE_USAGE_VIDEO_ENCODE_SRC_BIT_KHR);
            uint32_t count = 0;

            EXPECT_EQ(pfnGetPhysicalDeviceVideoFormatPropertiesKHR(gpu, &info.info, &count, nullptr), VK_SUCCESS);
            EXPECT_GE(count, 2);
            const uint32_t count_received = count;

            count = max_formats;
            EXPECT_EQ(pfnGetPhysicalDeviceVideoFormatPropertiesKHR(gpu, &info.info, &count, fmt_props.data()), VK_SUCCESS);
            EXPECT_EQ(count, count_received);

            bool found_g8_b8r8 = false;
            bool found_g8_b8_r8 = false;

            for (uint32_t i = 0; i < count; ++i) {
                if (fmt_props[i].format == VK_FORMAT_G8_B8R8_2PLANE_420_UNORM &&
                    fmt_props[i].imageTiling == VK_IMAGE_TILING_LINEAR) {
                    found_g8_b8r8 = true;
                    EXPECT_EQ(fmt_props[i].componentMapping.r, VK_COMPONENT_SWIZZLE_IDENTITY);
                    EXPECT_EQ(fmt_props[i].componentMapping.g, VK_COMPONENT_SWIZZLE_IDENTITY);
                    EXPECT_EQ(fmt_props[i].componentMapping.b, VK_COMPONENT_SWIZZLE_IDENTITY);
                    EXPECT_EQ(fmt_props[i].componentMapping.a, VK_COMPONENT_SWIZZLE_IDENTITY);
                    EXPECT_EQ(fmt_props[i].imageCreateFlags,
                              VK_IMAGE_CREATE_MUTABLE_FORMAT_BIT | VK_IMAGE_CREATE_EXTENDED_USAGE_BIT);
                    EXPECT_EQ(fmt_props[i].imageType, VK_IMAGE_TYPE_2D);
                    EXPECT_EQ(fmt_props[i].imageUsageFlags, VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT |
                                                                VK_IMAGE_USAGE_VIDEO_DECODE_DST_BIT_KHR |
                                                                VK_IMAGE_USAGE_VIDEO_ENCODE_SRC_BIT_KHR);
                }

                if (fmt_props[i].format == VK_FORMAT_G8_B8_R8_3PLANE_420_UNORM &&
                    fmt_props[i].imageTiling == VK_IMAGE_TILING_OPTIMAL) {
                    found_g8_b8_r8 = true;
                    EXPECT_EQ(fmt_props[i].componentMapping.r, VK_COMPONENT_SWIZZLE_IDENTITY);
                    EXPECT_EQ(fmt_props[i].componentMapping.g, VK_COMPONENT_SWIZZLE_IDENTITY);
                    EXPECT_EQ(fmt_props[i].componentMapping.b, VK_COMPONENT_SWIZZLE_IDENTITY);
                    EXPECT_EQ(fmt_props[i].componentMapping.a, VK_COMPONENT_SWIZZLE_IDENTITY);
                    EXPECT_EQ(fmt_props[i].imageCreateFlags,
                              VK_IMAGE_CREATE_MUTABLE_FORMAT_BIT | VK_IMAGE_CREATE_EXTENDED_USAGE_BIT);
                    EXPECT_EQ(fmt_props[i].imageType, VK_IMAGE_TYPE_2D);
                    EXPECT_EQ(fmt_props[i].imageUsageFlags, VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT |
                                                                VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT |
                                                                VK_IMAGE_USAGE_VIDEO_DECODE_DST_BIT_KHR |
                                                                VK_IMAGE_USAGE_VIDEO_ENCODE_SRC_BIT_KHR);
                }
            }

            EXPECT_TRUE(found_g8_b8r8);
            EXPECT_TRUE(found_g8_b8_r8);
        }

        // Encode DPB
        {
            VideoFormatInfo info(profile, VK_IMAGE_USAGE_VIDEO_ENCODE_DPB_BIT_KHR);
            uint32_t count = 0;

            EXPECT_EQ(pfnGetPhysicalDeviceVideoFormatPropertiesKHR(gpu, &info.info, &count, nullptr), VK_SUCCESS);
            EXPECT_GE(count, 1);
            const uint32_t count_received = count;

            count = max_formats;
            EXPECT_EQ(pfnGetPhysicalDeviceVideoFormatPropertiesKHR(gpu, &info.info, &count, fmt_props.data()), VK_SUCCESS);
            EXPECT_EQ(count, count_received);

            bool found = false;

            for (uint32_t i = 0; i < count; ++i) {
                if (fmt_props[i].format == VK_FORMAT_G16_B16R16_2PLANE_420_UNORM &&
                    fmt_props[i].imageTiling == VK_IMAGE_TILING_OPTIMAL) {
                    found = true;
                    EXPECT_EQ(fmt_props[i].componentMapping.r, VK_COMPONENT_SWIZZLE_IDENTITY);
                    EXPECT_EQ(fmt_props[i].componentMapping.g, VK_COMPONENT_SWIZZLE_IDENTITY);
                    EXPECT_EQ(fmt_props[i].componentMapping.b, VK_COMPONENT_SWIZZLE_IDENTITY);
                    EXPECT_EQ(fmt_props[i].componentMapping.a, VK_COMPONENT_SWIZZLE_IDENTITY);
                    EXPECT_EQ(fmt_props[i].imageCreateFlags, 0);
                    EXPECT_EQ(fmt_props[i].imageType, VK_IMAGE_TYPE_2D);
                    EXPECT_EQ(fmt_props[i].imageUsageFlags, VK_IMAGE_USAGE_VIDEO_ENCODE_DPB_BIT_KHR);
                }
            }

            EXPECT_TRUE(found);
        }

        // Quantization Delta Map
        {
            VideoFormatInfo info(profile, VK_IMAGE_USAGE_VIDEO_ENCODE_QUANTIZATION_DELTA_MAP_BIT_KHR);
            uint32_t count = 0;

            EXPECT_EQ(pfnGetPhysicalDeviceVideoFormatPropertiesKHR(gpu, &info.info, &count, nullptr), VK_SUCCESS);
            EXPECT_GE(count, 1);
            const uint32_t count_received = count;

            count = max_formats;
            EXPECT_EQ(pfnGetPhysicalDeviceVideoFormatPropertiesKHR(gpu, &info.info, &count, fmt_props.data()), VK_SUCCESS);
            EXPECT_EQ(count, count_received);

            bool found = false;

            for (uint32_t i = 0; i < count; ++i) {
                if (fmt_props[i].format == VK_FORMAT_R8_SINT && fmt_props[i].imageTiling == VK_IMAGE_TILING_LINEAR &&
                    quant_map_props[i].quantizationMapTexelSize.width == 16 &&
                    quant_map_props[i].quantizationMapTexelSize.height == 16) {
                    found = true;
                    EXPECT_EQ(fmt_props[i].componentMapping.r, VK_COMPONENT_SWIZZLE_IDENTITY);
                    EXPECT_EQ(fmt_props[i].componentMapping.g, VK_COMPONENT_SWIZZLE_IDENTITY);
                    EXPECT_EQ(fmt_props[i].componentMapping.b, VK_COMPONENT_SWIZZLE_IDENTITY);
                    EXPECT_EQ(fmt_props[i].componentMapping.a, VK_COMPONENT_SWIZZLE_IDENTITY);
                    EXPECT_EQ(fmt_props[i].imageCreateFlags, 0);
                    EXPECT_EQ(fmt_props[i].imageType, VK_IMAGE_TYPE_2D);
                    EXPECT_EQ(fmt_props[i].imageUsageFlags, VK_IMAGE_USAGE_VIDEO_ENCODE_QUANTIZATION_DELTA_MAP_BIT_KHR |
                                                                VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT);
                }
            }

            EXPECT_TRUE(found);
        }

        // Emphasis Map
        {
            VideoFormatInfo info(profile, VK_IMAGE_USAGE_VIDEO_ENCODE_EMPHASIS_MAP_BIT_KHR);
            uint32_t count = 0;

            EXPECT_EQ(pfnGetPhysicalDeviceVideoFormatPropertiesKHR(gpu, &info.info, &count, nullptr), VK_SUCCESS);
            EXPECT_GE(count, 1);
            const uint32_t count_received = count;

            count = max_formats;
            EXPECT_EQ(pfnGetPhysicalDeviceVideoFormatPropertiesKHR(gpu, &info.info, &count, fmt_props.data()), VK_SUCCESS);
            EXPECT_EQ(count, count_received);

            bool found = false;

            for (uint32_t i = 0; i < count; ++i) {
                if (fmt_props[i].format == VK_FORMAT_R8_UNORM && fmt_props[i].imageTiling == VK_IMAGE_TILING_LINEAR &&
                    quant_map_props[i].quantizationMapTexelSize.width == 64 &&
                    quant_map_props[i].quantizationMapTexelSize.height == 64) {
                    found = true;
                    EXPECT_EQ(fmt_props[i].componentMapping.r, VK_COMPONENT_SWIZZLE_IDENTITY);
                    EXPECT_EQ(fmt_props[i].componentMapping.g, VK_COMPONENT_SWIZZLE_IDENTITY);
                    EXPECT_EQ(fmt_props[i].componentMapping.b, VK_COMPONENT_SWIZZLE_IDENTITY);
                    EXPECT_EQ(fmt_props[i].componentMapping.a, VK_COMPONENT_SWIZZLE_IDENTITY);
                    EXPECT_EQ(fmt_props[i].imageCreateFlags, 0);
                    EXPECT_EQ(fmt_props[i].imageType, VK_IMAGE_TYPE_2D);
                    EXPECT_EQ(fmt_props[i].imageUsageFlags, VK_IMAGE_USAGE_VIDEO_ENCODE_EMPHASIS_MAP_BIT_KHR);
                }
            }

            EXPECT_TRUE(found);
        }
    }

    // H.264 Encode (4:2:0 8-bit) Main
    {
        auto profile = GetEncodeH264MainProfile();

        // Encode Input
        {
            VideoFormatInfo info(profile, VK_IMAGE_USAGE_VIDEO_ENCODE_SRC_BIT_KHR);
            uint32_t count = 0;

            EXPECT_EQ(pfnGetPhysicalDeviceVideoFormatPropertiesKHR(gpu, &info.info, &count, nullptr), VK_SUCCESS);
            EXPECT_GE(count, 2);
            const uint32_t count_received = count;

            count = max_formats;
            EXPECT_EQ(pfnGetPhysicalDeviceVideoFormatPropertiesKHR(gpu, &info.info, &count, fmt_props.data()), VK_SUCCESS);
            EXPECT_EQ(count, count_received);

            bool found_g8_b8r8 = false;
            bool found_g8_b8_r8 = false;

            for (uint32_t i = 0; i < count; ++i) {
                if (fmt_props[i].format == VK_FORMAT_G8_B8R8_2PLANE_420_UNORM &&
                    fmt_props[i].imageTiling == VK_IMAGE_TILING_LINEAR) {
                    found_g8_b8r8 = true;
                    EXPECT_EQ(fmt_props[i].componentMapping.r, VK_COMPONENT_SWIZZLE_IDENTITY);
                    EXPECT_EQ(fmt_props[i].componentMapping.g, VK_COMPONENT_SWIZZLE_IDENTITY);
                    EXPECT_EQ(fmt_props[i].componentMapping.b, VK_COMPONENT_SWIZZLE_IDENTITY);
                    EXPECT_EQ(fmt_props[i].componentMapping.a, VK_COMPONENT_SWIZZLE_IDENTITY);
                    EXPECT_EQ(fmt_props[i].imageCreateFlags,
                              VK_IMAGE_CREATE_MUTABLE_FORMAT_BIT | VK_IMAGE_CREATE_EXTENDED_USAGE_BIT);
                    EXPECT_EQ(fmt_props[i].imageType, VK_IMAGE_TYPE_2D);
                    EXPECT_EQ(fmt_props[i].imageUsageFlags, VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT |
                                                                VK_IMAGE_USAGE_VIDEO_DECODE_DST_BIT_KHR |
                                                                VK_IMAGE_USAGE_VIDEO_ENCODE_SRC_BIT_KHR);
                }

                if (fmt_props[i].format == VK_FORMAT_G8_B8_R8_3PLANE_420_UNORM &&
                    fmt_props[i].imageTiling == VK_IMAGE_TILING_OPTIMAL) {
                    found_g8_b8_r8 = true;
                    EXPECT_EQ(fmt_props[i].componentMapping.r, VK_COMPONENT_SWIZZLE_IDENTITY);
                    EXPECT_EQ(fmt_props[i].componentMapping.g, VK_COMPONENT_SWIZZLE_IDENTITY);
                    EXPECT_EQ(fmt_props[i].componentMapping.b, VK_COMPONENT_SWIZZLE_IDENTITY);
                    EXPECT_EQ(fmt_props[i].componentMapping.a, VK_COMPONENT_SWIZZLE_IDENTITY);
                    EXPECT_EQ(fmt_props[i].imageCreateFlags,
                              VK_IMAGE_CREATE_MUTABLE_FORMAT_BIT | VK_IMAGE_CREATE_EXTENDED_USAGE_BIT);
                    EXPECT_EQ(fmt_props[i].imageType, VK_IMAGE_TYPE_2D);
                    EXPECT_EQ(fmt_props[i].imageUsageFlags, VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT |
                                                                VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT |
                                                                VK_IMAGE_USAGE_VIDEO_DECODE_DST_BIT_KHR |
                                                                VK_IMAGE_USAGE_VIDEO_ENCODE_SRC_BIT_KHR);
                }
            }

            EXPECT_TRUE(found_g8_b8r8);
            EXPECT_TRUE(found_g8_b8_r8);
        }

        // Encode DPB
        {
            VideoFormatInfo info(profile, VK_IMAGE_USAGE_VIDEO_ENCODE_DPB_BIT_KHR);
            uint32_t count = 0;

            EXPECT_EQ(pfnGetPhysicalDeviceVideoFormatPropertiesKHR(gpu, &info.info, &count, nullptr), VK_SUCCESS);
            EXPECT_GE(count, 1);
            const uint32_t count_received = count;

            count = max_formats;
            EXPECT_EQ(pfnGetPhysicalDeviceVideoFormatPropertiesKHR(gpu, &info.info, &count, fmt_props.data()), VK_SUCCESS);
            EXPECT_EQ(count, count_received);

            bool found = false;

            for (uint32_t i = 0; i < count; ++i) {
                if (fmt_props[i].format == VK_FORMAT_G16_B16R16_2PLANE_420_UNORM &&
                    fmt_props[i].imageTiling == VK_IMAGE_TILING_OPTIMAL) {
                    found = true;
                    EXPECT_EQ(fmt_props[i].componentMapping.r, VK_COMPONENT_SWIZZLE_IDENTITY);
                    EXPECT_EQ(fmt_props[i].componentMapping.g, VK_COMPONENT_SWIZZLE_IDENTITY);
                    EXPECT_EQ(fmt_props[i].componentMapping.b, VK_COMPONENT_SWIZZLE_IDENTITY);
                    EXPECT_EQ(fmt_props[i].componentMapping.a, VK_COMPONENT_SWIZZLE_IDENTITY);
                    EXPECT_EQ(fmt_props[i].imageCreateFlags, 0);
                    EXPECT_EQ(fmt_props[i].imageType, VK_IMAGE_TYPE_2D);
                    EXPECT_EQ(fmt_props[i].imageUsageFlags, VK_IMAGE_USAGE_VIDEO_ENCODE_DPB_BIT_KHR);
                }
            }

            EXPECT_TRUE(found);
        }

        // Quantization Delta Map
        {
            VideoFormatInfo info(profile, VK_IMAGE_USAGE_VIDEO_ENCODE_QUANTIZATION_DELTA_MAP_BIT_KHR);
            uint32_t count = 0;

            EXPECT_EQ(pfnGetPhysicalDeviceVideoFormatPropertiesKHR(gpu, &info.info, &count, nullptr), VK_SUCCESS);
            EXPECT_GE(count, 1);
            const uint32_t count_received = count;

            count = max_formats;
            EXPECT_EQ(pfnGetPhysicalDeviceVideoFormatPropertiesKHR(gpu, &info.info, &count, fmt_props.data()), VK_SUCCESS);
            EXPECT_EQ(count, count_received);

            bool found = false;

            for (uint32_t i = 0; i < count; ++i) {
                if (fmt_props[i].format == VK_FORMAT_R8_SINT && fmt_props[i].imageTiling == VK_IMAGE_TILING_LINEAR &&
                    quant_map_props[i].quantizationMapTexelSize.width == 16 &&
                    quant_map_props[i].quantizationMapTexelSize.height == 16) {
                    found = true;
                    EXPECT_EQ(fmt_props[i].componentMapping.r, VK_COMPONENT_SWIZZLE_IDENTITY);
                    EXPECT_EQ(fmt_props[i].componentMapping.g, VK_COMPONENT_SWIZZLE_IDENTITY);
                    EXPECT_EQ(fmt_props[i].componentMapping.b, VK_COMPONENT_SWIZZLE_IDENTITY);
                    EXPECT_EQ(fmt_props[i].componentMapping.a, VK_COMPONENT_SWIZZLE_IDENTITY);
                    EXPECT_EQ(fmt_props[i].imageCreateFlags, 0);
                    EXPECT_EQ(fmt_props[i].imageType, VK_IMAGE_TYPE_2D);
                    EXPECT_EQ(fmt_props[i].imageUsageFlags, VK_IMAGE_USAGE_VIDEO_ENCODE_QUANTIZATION_DELTA_MAP_BIT_KHR |
                                                                VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT);
                }
            }

            EXPECT_TRUE(found);
        }

        // Emphasis Map
        {
            VideoFormatInfo info(profile, VK_IMAGE_USAGE_VIDEO_ENCODE_EMPHASIS_MAP_BIT_KHR);
            uint32_t count = 0;

            EXPECT_EQ(pfnGetPhysicalDeviceVideoFormatPropertiesKHR(gpu, &info.info, &count, nullptr), VK_SUCCESS);
            EXPECT_GE(count, 1);
            const uint32_t count_received = count;

            count = max_formats;
            EXPECT_EQ(pfnGetPhysicalDeviceVideoFormatPropertiesKHR(gpu, &info.info, &count, fmt_props.data()), VK_SUCCESS);
            EXPECT_EQ(count, count_received);

            bool found = false;

            for (uint32_t i = 0; i < count; ++i) {
                if (fmt_props[i].format == VK_FORMAT_R8_UNORM && fmt_props[i].imageTiling == VK_IMAGE_TILING_LINEAR &&
                    quant_map_props[i].quantizationMapTexelSize.width == 64 &&
                    quant_map_props[i].quantizationMapTexelSize.height == 64) {
                    found = true;
                    EXPECT_EQ(fmt_props[i].componentMapping.r, VK_COMPONENT_SWIZZLE_IDENTITY);
                    EXPECT_EQ(fmt_props[i].componentMapping.g, VK_COMPONENT_SWIZZLE_IDENTITY);
                    EXPECT_EQ(fmt_props[i].componentMapping.b, VK_COMPONENT_SWIZZLE_IDENTITY);
                    EXPECT_EQ(fmt_props[i].componentMapping.a, VK_COMPONENT_SWIZZLE_IDENTITY);
                    EXPECT_EQ(fmt_props[i].imageCreateFlags, 0);
                    EXPECT_EQ(fmt_props[i].imageType, VK_IMAGE_TYPE_2D);
                    EXPECT_EQ(fmt_props[i].imageUsageFlags, VK_IMAGE_USAGE_VIDEO_ENCODE_EMPHASIS_MAP_BIT_KHR);
                }
            }

            EXPECT_TRUE(found);
        }
    }

    // H.265 Encode (4:2:0 8-bit) Main
    {
        auto profile = GetEncodeH265MainProfile();

        // Encode Input
        {
            VideoFormatInfo info(profile, VK_IMAGE_USAGE_VIDEO_ENCODE_SRC_BIT_KHR);
            uint32_t count = 0;

            EXPECT_EQ(pfnGetPhysicalDeviceVideoFormatPropertiesKHR(gpu, &info.info, &count, nullptr), VK_SUCCESS);
            EXPECT_GE(count, 2);
            const uint32_t count_received = count;

            count = max_formats;
            EXPECT_EQ(pfnGetPhysicalDeviceVideoFormatPropertiesKHR(gpu, &info.info, &count, fmt_props.data()), VK_SUCCESS);
            EXPECT_EQ(count, count_received);

            bool found_g8_b8r8 = false;
            bool found_g8_b8_r8 = false;

            for (uint32_t i = 0; i < count; ++i) {
                if (fmt_props[i].format == VK_FORMAT_G8_B8R8_2PLANE_420_UNORM &&
                    fmt_props[i].imageTiling == VK_IMAGE_TILING_LINEAR) {
                    found_g8_b8r8 = true;
                    EXPECT_EQ(fmt_props[i].componentMapping.r, VK_COMPONENT_SWIZZLE_IDENTITY);
                    EXPECT_EQ(fmt_props[i].componentMapping.g, VK_COMPONENT_SWIZZLE_IDENTITY);
                    EXPECT_EQ(fmt_props[i].componentMapping.b, VK_COMPONENT_SWIZZLE_IDENTITY);
                    EXPECT_EQ(fmt_props[i].componentMapping.a, VK_COMPONENT_SWIZZLE_IDENTITY);
                    EXPECT_EQ(fmt_props[i].imageCreateFlags,
                              VK_IMAGE_CREATE_MUTABLE_FORMAT_BIT | VK_IMAGE_CREATE_EXTENDED_USAGE_BIT);
                    EXPECT_EQ(fmt_props[i].imageType, VK_IMAGE_TYPE_2D);
                    EXPECT_EQ(fmt_props[i].imageUsageFlags, VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT |
                                                                VK_IMAGE_USAGE_VIDEO_DECODE_DST_BIT_KHR |
                                                                VK_IMAGE_USAGE_VIDEO_ENCODE_SRC_BIT_KHR);
                }

                if (fmt_props[i].format == VK_FORMAT_G8_B8_R8_3PLANE_420_UNORM &&
                    fmt_props[i].imageTiling == VK_IMAGE_TILING_OPTIMAL) {
                    found_g8_b8_r8 = true;
                    EXPECT_EQ(fmt_props[i].componentMapping.r, VK_COMPONENT_SWIZZLE_IDENTITY);
                    EXPECT_EQ(fmt_props[i].componentMapping.g, VK_COMPONENT_SWIZZLE_IDENTITY);
                    EXPECT_EQ(fmt_props[i].componentMapping.b, VK_COMPONENT_SWIZZLE_IDENTITY);
                    EXPECT_EQ(fmt_props[i].componentMapping.a, VK_COMPONENT_SWIZZLE_IDENTITY);
                    EXPECT_EQ(fmt_props[i].imageCreateFlags,
                              VK_IMAGE_CREATE_MUTABLE_FORMAT_BIT | VK_IMAGE_CREATE_EXTENDED_USAGE_BIT);
                    EXPECT_EQ(fmt_props[i].imageType, VK_IMAGE_TYPE_2D);
                    EXPECT_EQ(fmt_props[i].imageUsageFlags, VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT |
                                                                VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT |
                                                                VK_IMAGE_USAGE_VIDEO_DECODE_DST_BIT_KHR |
                                                                VK_IMAGE_USAGE_VIDEO_ENCODE_SRC_BIT_KHR);
                }
            }

            EXPECT_TRUE(found_g8_b8r8);
            EXPECT_TRUE(found_g8_b8_r8);
        }

        // Encode DPB
        {
            VideoFormatInfo info(profile, VK_IMAGE_USAGE_VIDEO_ENCODE_DPB_BIT_KHR);
            uint32_t count = 0;

            EXPECT_EQ(pfnGetPhysicalDeviceVideoFormatPropertiesKHR(gpu, &info.info, &count, nullptr), VK_SUCCESS);
            EXPECT_GE(count, 1);
            const uint32_t count_received = count;

            count = max_formats;
            EXPECT_EQ(pfnGetPhysicalDeviceVideoFormatPropertiesKHR(gpu, &info.info, &count, fmt_props.data()), VK_SUCCESS);
            EXPECT_EQ(count, count_received);

            bool found = false;

            for (uint32_t i = 0; i < count; ++i) {
                if (fmt_props[i].format == VK_FORMAT_G16_B16R16_2PLANE_420_UNORM &&
                    fmt_props[i].imageTiling == VK_IMAGE_TILING_OPTIMAL) {
                    found = true;
                    EXPECT_EQ(fmt_props[i].componentMapping.r, VK_COMPONENT_SWIZZLE_IDENTITY);
                    EXPECT_EQ(fmt_props[i].componentMapping.g, VK_COMPONENT_SWIZZLE_IDENTITY);
                    EXPECT_EQ(fmt_props[i].componentMapping.b, VK_COMPONENT_SWIZZLE_IDENTITY);
                    EXPECT_EQ(fmt_props[i].componentMapping.a, VK_COMPONENT_SWIZZLE_IDENTITY);
                    EXPECT_EQ(fmt_props[i].imageCreateFlags, 0);
                    EXPECT_EQ(fmt_props[i].imageType, VK_IMAGE_TYPE_2D);
                    EXPECT_EQ(fmt_props[i].imageUsageFlags, VK_IMAGE_USAGE_VIDEO_ENCODE_DPB_BIT_KHR);
                }
            }

            EXPECT_TRUE(found);
        }

        // Quantization Delta Map
        {
            VideoFormatInfo info(profile, VK_IMAGE_USAGE_VIDEO_ENCODE_QUANTIZATION_DELTA_MAP_BIT_KHR);
            uint32_t count = 0;

            EXPECT_EQ(pfnGetPhysicalDeviceVideoFormatPropertiesKHR(gpu, &info.info, &count, nullptr), VK_SUCCESS);
            EXPECT_GE(count, 2);
            const uint32_t count_received = count;

            count = max_formats;
            EXPECT_EQ(pfnGetPhysicalDeviceVideoFormatPropertiesKHR(gpu, &info.info, &count, fmt_props.data()), VK_SUCCESS);
            EXPECT_EQ(count, count_received);

            bool found_16x16 = false;
            bool found_32x32 = false;

            for (uint32_t i = 0; i < count; ++i) {
                if (fmt_props[i].format == VK_FORMAT_R32_SINT && fmt_props[i].imageTiling == VK_IMAGE_TILING_LINEAR &&
                    quant_map_props[i].quantizationMapTexelSize.width == 16 &&
                    quant_map_props[i].quantizationMapTexelSize.height == 16) {
                    found_16x16 = true;
                    EXPECT_EQ(fmt_props[i].componentMapping.r, VK_COMPONENT_SWIZZLE_IDENTITY);
                    EXPECT_EQ(fmt_props[i].componentMapping.g, VK_COMPONENT_SWIZZLE_IDENTITY);
                    EXPECT_EQ(fmt_props[i].componentMapping.b, VK_COMPONENT_SWIZZLE_IDENTITY);
                    EXPECT_EQ(fmt_props[i].componentMapping.a, VK_COMPONENT_SWIZZLE_IDENTITY);
                    EXPECT_EQ(fmt_props[i].imageCreateFlags, 0);
                    EXPECT_EQ(fmt_props[i].imageType, VK_IMAGE_TYPE_2D);
                    EXPECT_EQ(fmt_props[i].imageUsageFlags, VK_IMAGE_USAGE_VIDEO_ENCODE_QUANTIZATION_DELTA_MAP_BIT_KHR |
                                                                VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT);

                    EXPECT_EQ(quant_map_props_h265[i].compatibleCtbSizes, VK_VIDEO_ENCODE_H265_CTB_SIZE_16_BIT_KHR);
                }

                if (fmt_props[i].format == VK_FORMAT_R32_SINT && fmt_props[i].imageTiling == VK_IMAGE_TILING_LINEAR &&
                    quant_map_props[i].quantizationMapTexelSize.width == 32 &&
                    quant_map_props[i].quantizationMapTexelSize.height == 32) {
                    found_32x32 = true;
                    EXPECT_EQ(fmt_props[i].componentMapping.r, VK_COMPONENT_SWIZZLE_IDENTITY);
                    EXPECT_EQ(fmt_props[i].componentMapping.g, VK_COMPONENT_SWIZZLE_IDENTITY);
                    EXPECT_EQ(fmt_props[i].componentMapping.b, VK_COMPONENT_SWIZZLE_IDENTITY);
                    EXPECT_EQ(fmt_props[i].componentMapping.a, VK_COMPONENT_SWIZZLE_IDENTITY);
                    EXPECT_EQ(fmt_props[i].imageCreateFlags, 0);
                    EXPECT_EQ(fmt_props[i].imageType, VK_IMAGE_TYPE_2D);
                    EXPECT_EQ(fmt_props[i].imageUsageFlags,
                              VK_IMAGE_USAGE_VIDEO_ENCODE_QUANTIZATION_DELTA_MAP_BIT_KHR | VK_IMAGE_USAGE_TRANSFER_DST_BIT);

                    EXPECT_EQ(quant_map_props_h265[i].compatibleCtbSizes,
                              VK_VIDEO_ENCODE_H265_CTB_SIZE_16_BIT_KHR | VK_VIDEO_ENCODE_H265_CTB_SIZE_32_BIT_KHR);
                }
            }

            EXPECT_TRUE(found_16x16);
            EXPECT_TRUE(found_32x32);
        }

        // Emphasis Map
        {
            VideoFormatInfo info(profile, VK_IMAGE_USAGE_VIDEO_ENCODE_EMPHASIS_MAP_BIT_KHR);
            uint32_t count = 0;

            EXPECT_EQ(pfnGetPhysicalDeviceVideoFormatPropertiesKHR(gpu, &info.info, &count, nullptr), VK_SUCCESS);
            EXPECT_GE(count, 1);
            const uint32_t count_received = count;

            count = max_formats;
            EXPECT_EQ(pfnGetPhysicalDeviceVideoFormatPropertiesKHR(gpu, &info.info, &count, fmt_props.data()), VK_SUCCESS);
            EXPECT_EQ(count, count_received);

            bool found = false;

            for (uint32_t i = 0; i < count; ++i) {
                if (fmt_props[i].format == VK_FORMAT_R8_UNORM && fmt_props[i].imageTiling == VK_IMAGE_TILING_LINEAR &&
                    quant_map_props[i].quantizationMapTexelSize.width == 64 &&
                    quant_map_props[i].quantizationMapTexelSize.height == 64) {
                    found = true;
                    EXPECT_EQ(fmt_props[i].componentMapping.r, VK_COMPONENT_SWIZZLE_IDENTITY);
                    EXPECT_EQ(fmt_props[i].componentMapping.g, VK_COMPONENT_SWIZZLE_IDENTITY);
                    EXPECT_EQ(fmt_props[i].componentMapping.b, VK_COMPONENT_SWIZZLE_IDENTITY);
                    EXPECT_EQ(fmt_props[i].componentMapping.a, VK_COMPONENT_SWIZZLE_IDENTITY);
                    EXPECT_EQ(fmt_props[i].imageCreateFlags, 0);
                    EXPECT_EQ(fmt_props[i].imageType, VK_IMAGE_TYPE_2D);
                    EXPECT_EQ(fmt_props[i].imageUsageFlags, VK_IMAGE_USAGE_VIDEO_ENCODE_EMPHASIS_MAP_BIT_KHR);
                }
            }

            EXPECT_TRUE(found);
        }
    }

    // H.265 Encode (4:2:0 10-bit) Main 10
    {
        auto profile = GetEncodeH265Main10Profile();

        // Encode Input
        {
            VideoFormatInfo info(profile, VK_IMAGE_USAGE_VIDEO_ENCODE_SRC_BIT_KHR);
            uint32_t count = 0;

            EXPECT_EQ(pfnGetPhysicalDeviceVideoFormatPropertiesKHR(gpu, &info.info, &count, nullptr), VK_SUCCESS);
            EXPECT_GE(count, 1);
            const uint32_t count_received = count;

            count = max_formats;
            EXPECT_EQ(pfnGetPhysicalDeviceVideoFormatPropertiesKHR(gpu, &info.info, &count, fmt_props.data()), VK_SUCCESS);
            EXPECT_EQ(count, count_received);

            bool found = false;

            for (uint32_t i = 0; i < count; ++i) {
                if (fmt_props[i].format == VK_FORMAT_G10X6_B10X6R10X6_2PLANE_420_UNORM_3PACK16 &&
                    fmt_props[i].imageTiling == VK_IMAGE_TILING_LINEAR) {
                    found = true;
                    EXPECT_EQ(fmt_props[i].componentMapping.r, VK_COMPONENT_SWIZZLE_IDENTITY);
                    EXPECT_EQ(fmt_props[i].componentMapping.g, VK_COMPONENT_SWIZZLE_IDENTITY);
                    EXPECT_EQ(fmt_props[i].componentMapping.b, VK_COMPONENT_SWIZZLE_IDENTITY);
                    EXPECT_EQ(fmt_props[i].componentMapping.a, VK_COMPONENT_SWIZZLE_IDENTITY);
                    EXPECT_EQ(fmt_props[i].imageCreateFlags,
                              VK_IMAGE_CREATE_MUTABLE_FORMAT_BIT | VK_IMAGE_CREATE_EXTENDED_USAGE_BIT);
                    EXPECT_EQ(fmt_props[i].imageType, VK_IMAGE_TYPE_2D);
                    EXPECT_EQ(fmt_props[i].imageUsageFlags, VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT |
                                                                VK_IMAGE_USAGE_VIDEO_DECODE_DST_BIT_KHR |
                                                                VK_IMAGE_USAGE_VIDEO_ENCODE_SRC_BIT_KHR);
                }
            }

            EXPECT_TRUE(found);
        }

        // Encode DPB
        {
            VideoFormatInfo info(profile, VK_IMAGE_USAGE_VIDEO_ENCODE_DPB_BIT_KHR);
            uint32_t count = 0;

            EXPECT_EQ(pfnGetPhysicalDeviceVideoFormatPropertiesKHR(gpu, &info.info, &count, nullptr), VK_SUCCESS);
            EXPECT_GE(count, 1);
            const uint32_t count_received = count;

            count = max_formats;
            EXPECT_EQ(pfnGetPhysicalDeviceVideoFormatPropertiesKHR(gpu, &info.info, &count, fmt_props.data()), VK_SUCCESS);
            EXPECT_EQ(count, count_received);

            bool found = false;

            for (uint32_t i = 0; i < count; ++i) {
                if (fmt_props[i].format == VK_FORMAT_G16_B16R16_2PLANE_420_UNORM &&
                    fmt_props[i].imageTiling == VK_IMAGE_TILING_OPTIMAL) {
                    found = true;
                    EXPECT_EQ(fmt_props[i].componentMapping.r, VK_COMPONENT_SWIZZLE_IDENTITY);
                    EXPECT_EQ(fmt_props[i].componentMapping.g, VK_COMPONENT_SWIZZLE_IDENTITY);
                    EXPECT_EQ(fmt_props[i].componentMapping.b, VK_COMPONENT_SWIZZLE_IDENTITY);
                    EXPECT_EQ(fmt_props[i].componentMapping.a, VK_COMPONENT_SWIZZLE_IDENTITY);
                    EXPECT_EQ(fmt_props[i].imageCreateFlags, 0);
                    EXPECT_EQ(fmt_props[i].imageType, VK_IMAGE_TYPE_2D);
                    EXPECT_EQ(fmt_props[i].imageUsageFlags, VK_IMAGE_USAGE_VIDEO_ENCODE_DPB_BIT_KHR);
                }
            }

            EXPECT_TRUE(found);
        }

        // Emphasis Map
        {
            VideoFormatInfo info(profile, VK_IMAGE_USAGE_VIDEO_ENCODE_EMPHASIS_MAP_BIT_KHR);
            uint32_t count = 0;

            EXPECT_EQ(pfnGetPhysicalDeviceVideoFormatPropertiesKHR(gpu, &info.info, &count, nullptr), VK_SUCCESS);
            EXPECT_GE(count, 1);
            const uint32_t count_received = count;

            count = max_formats;
            EXPECT_EQ(pfnGetPhysicalDeviceVideoFormatPropertiesKHR(gpu, &info.info, &count, fmt_props.data()), VK_SUCCESS);
            EXPECT_EQ(count, count_received);

            bool found = false;

            for (uint32_t i = 0; i < count; ++i) {
                if (fmt_props[i].format == VK_FORMAT_R8_UNORM && fmt_props[i].imageTiling == VK_IMAGE_TILING_LINEAR &&
                    quant_map_props[i].quantizationMapTexelSize.width == 64 &&
                    quant_map_props[i].quantizationMapTexelSize.height == 64) {
                    found = true;
                    EXPECT_EQ(fmt_props[i].componentMapping.r, VK_COMPONENT_SWIZZLE_IDENTITY);
                    EXPECT_EQ(fmt_props[i].componentMapping.g, VK_COMPONENT_SWIZZLE_IDENTITY);
                    EXPECT_EQ(fmt_props[i].componentMapping.b, VK_COMPONENT_SWIZZLE_IDENTITY);
                    EXPECT_EQ(fmt_props[i].componentMapping.a, VK_COMPONENT_SWIZZLE_IDENTITY);
                    EXPECT_EQ(fmt_props[i].imageCreateFlags, 0);
                    EXPECT_EQ(fmt_props[i].imageType, VK_IMAGE_TYPE_2D);
                    EXPECT_EQ(fmt_props[i].imageUsageFlags, VK_IMAGE_USAGE_VIDEO_ENCODE_EMPHASIS_MAP_BIT_KHR);
                }
            }

            EXPECT_TRUE(found);
        }
    }
}

TEST_F(TestsMechanismVideoProfiles, VideoFormatPropertiesSharedTranscodeImage) {
    TEST_DESCRIPTION("Test video format properties for transcoding case (one decode, multiple encode profiles)");

    std::vector<VkVideoProfileInfoKHR> profiles{*GetDecodeH264BaselineProfile(), *GetEncodeH264BaselineProfile(),
                                                *GetEncodeH264MainProfile(), *GetEncodeH265MainProfile()};

    VkVideoProfileListInfoKHR profile_list{VK_STRUCTURE_TYPE_VIDEO_PROFILE_LIST_INFO_KHR};
    profile_list.profileCount = static_cast<uint32_t>(profiles.size());
    profile_list.pProfiles = profiles.data();

    VkPhysicalDeviceVideoFormatInfoKHR fmt_info{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VIDEO_FORMAT_INFO_KHR, &profile_list};
    fmt_info.imageUsage = VK_IMAGE_USAGE_VIDEO_DECODE_DST_BIT_KHR | VK_IMAGE_USAGE_VIDEO_ENCODE_SRC_BIT_KHR;

    const uint32_t max_formats = 256;
    std::vector<VkVideoFormatPropertiesKHR> fmt_props(max_formats, {VK_STRUCTURE_TYPE_VIDEO_FORMAT_PROPERTIES_KHR});

    uint32_t count = 0;

    EXPECT_EQ(pfnGetPhysicalDeviceVideoFormatPropertiesKHR(gpu, &fmt_info, &count, nullptr), VK_SUCCESS);
    EXPECT_GE(count, 2);
    const uint32_t count_received = count;

    count = max_formats;
    EXPECT_EQ(pfnGetPhysicalDeviceVideoFormatPropertiesKHR(gpu, &fmt_info, &count, fmt_props.data()), VK_SUCCESS);
    EXPECT_EQ(count, count_received);

    bool found_g8_b8r8 = false;
    bool found_g8_b8_r8 = false;

    for (uint32_t i = 0; i < count; ++i) {
        if (fmt_props[i].format == VK_FORMAT_G8_B8R8_2PLANE_420_UNORM && fmt_props[i].imageTiling == VK_IMAGE_TILING_LINEAR) {
            found_g8_b8r8 = true;
            EXPECT_EQ(fmt_props[i].componentMapping.r, VK_COMPONENT_SWIZZLE_IDENTITY);
            EXPECT_EQ(fmt_props[i].componentMapping.g, VK_COMPONENT_SWIZZLE_IDENTITY);
            EXPECT_EQ(fmt_props[i].componentMapping.b, VK_COMPONENT_SWIZZLE_IDENTITY);
            EXPECT_EQ(fmt_props[i].componentMapping.a, VK_COMPONENT_SWIZZLE_IDENTITY);
            EXPECT_EQ(fmt_props[i].imageCreateFlags, VK_IMAGE_CREATE_MUTABLE_FORMAT_BIT | VK_IMAGE_CREATE_EXTENDED_USAGE_BIT);
            EXPECT_EQ(fmt_props[i].imageType, VK_IMAGE_TYPE_2D);
            EXPECT_EQ(fmt_props[i].imageUsageFlags, VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT |
                                                        VK_IMAGE_USAGE_VIDEO_DECODE_DST_BIT_KHR |
                                                        VK_IMAGE_USAGE_VIDEO_ENCODE_SRC_BIT_KHR);
        }

        if (fmt_props[i].format == VK_FORMAT_G8_B8_R8_3PLANE_420_UNORM && fmt_props[i].imageTiling == VK_IMAGE_TILING_OPTIMAL) {
            found_g8_b8_r8 = true;
            EXPECT_EQ(fmt_props[i].componentMapping.r, VK_COMPONENT_SWIZZLE_IDENTITY);
            EXPECT_EQ(fmt_props[i].componentMapping.g, VK_COMPONENT_SWIZZLE_IDENTITY);
            EXPECT_EQ(fmt_props[i].componentMapping.b, VK_COMPONENT_SWIZZLE_IDENTITY);
            EXPECT_EQ(fmt_props[i].componentMapping.a, VK_COMPONENT_SWIZZLE_IDENTITY);
            EXPECT_EQ(fmt_props[i].imageCreateFlags, VK_IMAGE_CREATE_MUTABLE_FORMAT_BIT | VK_IMAGE_CREATE_EXTENDED_USAGE_BIT);
            EXPECT_EQ(fmt_props[i].imageType, VK_IMAGE_TYPE_2D);
            EXPECT_EQ(fmt_props[i].imageUsageFlags, VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT |
                                                        VK_IMAGE_USAGE_VIDEO_DECODE_DST_BIT_KHR |
                                                        VK_IMAGE_USAGE_VIDEO_ENCODE_SRC_BIT_KHR);
        }
    }

    EXPECT_TRUE(found_g8_b8r8);
    EXPECT_TRUE(found_g8_b8_r8);
}

TEST_F(TestsMechanismVideoProfiles, VideoFormatPropertiesSharedQuantMap) {
    TEST_DESCRIPTION("Test video format properties for shared quantization map format (multiple encode profiles)");

    std::vector<VkVideoProfileInfoKHR> profiles{*GetEncodeH264BaselineProfile(), *GetEncodeH264MainProfile(),
                                                *GetEncodeH265MainProfile(), *GetEncodeH265Main10Profile()};

    VkVideoProfileListInfoKHR profile_list{VK_STRUCTURE_TYPE_VIDEO_PROFILE_LIST_INFO_KHR};
    profile_list.profileCount = static_cast<uint32_t>(profiles.size());
    profile_list.pProfiles = profiles.data();

    VkPhysicalDeviceVideoFormatInfoKHR fmt_info{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VIDEO_FORMAT_INFO_KHR, &profile_list};
    fmt_info.imageUsage = VK_IMAGE_USAGE_VIDEO_ENCODE_EMPHASIS_MAP_BIT_KHR;

    const uint32_t max_formats = 256;
    std::vector<VkVideoFormatPropertiesKHR> fmt_props(max_formats);
    std::vector<VkVideoFormatQuantizationMapPropertiesKHR> quant_map_props(max_formats);
    for (uint32_t i = 0; i < max_formats; ++i) {
        fmt_props[i].sType = VK_STRUCTURE_TYPE_VIDEO_FORMAT_PROPERTIES_KHR;
        fmt_props[i].pNext = &quant_map_props[i];
        quant_map_props[i].sType = VK_STRUCTURE_TYPE_VIDEO_FORMAT_QUANTIZATION_MAP_PROPERTIES_KHR;
    }

    uint32_t count = 0;

    EXPECT_EQ(pfnGetPhysicalDeviceVideoFormatPropertiesKHR(gpu, &fmt_info, &count, nullptr), VK_SUCCESS);
    EXPECT_GE(count, 1);
    const uint32_t count_received = count;

    count = max_formats;
    EXPECT_EQ(pfnGetPhysicalDeviceVideoFormatPropertiesKHR(gpu, &fmt_info, &count, fmt_props.data()), VK_SUCCESS);
    EXPECT_EQ(count, count_received);

    bool found = false;

    for (uint32_t i = 0; i < count; ++i) {
        if (fmt_props[i].format == VK_FORMAT_R8_UNORM && fmt_props[i].imageTiling == VK_IMAGE_TILING_LINEAR &&
            quant_map_props[i].quantizationMapTexelSize.width == 64 && quant_map_props[i].quantizationMapTexelSize.height == 64) {
            found = true;
            EXPECT_EQ(fmt_props[i].componentMapping.r, VK_COMPONENT_SWIZZLE_IDENTITY);
            EXPECT_EQ(fmt_props[i].componentMapping.g, VK_COMPONENT_SWIZZLE_IDENTITY);
            EXPECT_EQ(fmt_props[i].componentMapping.b, VK_COMPONENT_SWIZZLE_IDENTITY);
            EXPECT_EQ(fmt_props[i].componentMapping.a, VK_COMPONENT_SWIZZLE_IDENTITY);
            EXPECT_EQ(fmt_props[i].imageCreateFlags, 0);
            EXPECT_EQ(fmt_props[i].imageType, VK_IMAGE_TYPE_2D);
            EXPECT_EQ(fmt_props[i].imageUsageFlags, VK_IMAGE_USAGE_VIDEO_ENCODE_EMPHASIS_MAP_BIT_KHR);
        }
    }

    EXPECT_TRUE(found);
}

TEST_F(TestsMechanismVideoProfiles, VideoImageFormatProperties) {
    TEST_DESCRIPTION("Test image format properties for video formats");

    std::vector<VkVideoProfileInfoKHR> profiles{*GetDecodeH264BaselineProfile(), *GetEncodeH264BaselineProfile(),
                                                *GetEncodeH264MainProfile(), *GetEncodeH265MainProfile()};

    VkVideoProfileListInfoKHR profile_list{VK_STRUCTURE_TYPE_VIDEO_PROFILE_LIST_INFO_KHR};
    profile_list.profileCount = static_cast<uint32_t>(profiles.size());
    profile_list.pProfiles = profiles.data();

    VkPhysicalDeviceImageFormatInfo2 fmt_info{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VIDEO_FORMAT_INFO_KHR, &profile_list};
    fmt_info.format = VK_FORMAT_G8_B8R8_2PLANE_420_UNORM;
    fmt_info.type = VK_IMAGE_TYPE_2D;
    fmt_info.tiling = VK_IMAGE_TILING_LINEAR;
    fmt_info.usage = VK_IMAGE_USAGE_VIDEO_DECODE_DST_BIT_KHR | VK_IMAGE_USAGE_VIDEO_ENCODE_SRC_BIT_KHR;
    fmt_info.flags = VK_IMAGE_CREATE_MUTABLE_FORMAT_BIT | VK_IMAGE_CREATE_EXTENDED_USAGE_BIT;

    VkImageFormatProperties2KHR fmt_props{VK_STRUCTURE_TYPE_IMAGE_FORMAT_PROPERTIES_2};

    EXPECT_NE(vkGetPhysicalDeviceImageFormatProperties2(gpu, &fmt_info, &fmt_props),
              VK_ERROR_VIDEO_PROFILE_FORMAT_NOT_SUPPORTED_KHR);

    fmt_info.usage |= VK_IMAGE_USAGE_STORAGE_BIT;

    EXPECT_EQ(vkGetPhysicalDeviceImageFormatProperties2(gpu, &fmt_info, &fmt_props),
              VK_ERROR_VIDEO_PROFILE_FORMAT_NOT_SUPPORTED_KHR);
}
