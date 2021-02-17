cmake_minimum_required(VERSION 3.17)
project(h264_lib)

set(CMAKE_CXX_STANDARD 14)
if (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    # explicitly add this since the code does not detect WIN64
    add_definitions(-D__LP64__=1)
endif()

add_library(h264_lib STATIC
        adapter/h264Adapter.cpp
        h264lib/h264_decoder.cpp
        h264lib/h264_encoder.cpp
        h264lib/h264lib.cpp
        opencore/codecs_v2/video/avc_h264/dec/src/avc_bitstream.cpp
        opencore/codecs_v2/video/avc_h264/dec/src/avcdec_api.cpp
        opencore/codecs_v2/video/avc_h264/enc/src/avcenc_api.cpp
        opencore/codecs_v2/video/avc_h264/enc/src/bitstream_io.cpp
        opencore/codecs_v2/video/avc_h264/enc/src/block.cpp
        opencore/codecs_v2/video/avc_h264/common/src/deblock.cpp
        opencore/codecs_v2/video/avc_h264/common/src/dpb.cpp
        opencore/codecs_v2/video/avc_h264/common/src/fmo.cpp
        opencore/codecs_v2/video/avc_h264/enc/src/findhalfpel.cpp
        opencore/codecs_v2/video/avc_h264/dec/src/header_dec.cpp
        opencore/codecs_v2/video/avc_h264/enc/src/header_enc.cpp
        opencore/codecs_v2/video/avc_h264/enc/src/init.cpp
        opencore/codecs_v2/video/avc_h264/enc/src/intra_est.cpp
        opencore/codecs_v2/video/avc_h264/dec/src/itrans.cpp
        opencore/codecs_v2/video/avc_h264/common/src/mb_access.cpp
        opencore/codecs_v2/video/avc_h264/enc/src/motion_comp.cpp
        opencore/codecs_v2/video/avc_h264/enc/src/motion_est.cpp
        opencore/codecs_v2/video/avc_h264/dec/src/pred_inter.cpp
        opencore/codecs_v2/video/avc_h264/dec/src/pred_intra.cpp
        opencore/codecs_v2/video/avc_h264/dec/src/pvavcdecoder.cpp
        opencore/codecs_v2/video/avc_h264/dec/src/pvavcdecoder_factory.cpp
        opencore/codecs_v2/video/avc_h264/enc/src/pvavcencoder.cpp
        opencore/codecs_v2/video/avc_h264/enc/src/pvavcencoder_factory.cpp
        opencore/codecs_v2/video/avc_h264/enc/src/rate_control.cpp
        opencore/codecs_v2/video/avc_h264/common/src/reflist.cpp
        opencore/codecs_v2/video/avc_h264/dec/src/residual_dec.cpp
        opencore/codecs_v2/video/avc_h264/enc/src/residual_enc.cpp
        opencore/codecs_v2/video/avc_h264/enc/src/sad.cpp
        opencore/codecs_v2/video/avc_h264/enc/src/sad_halfpel.cpp
        opencore/codecs_v2/video/avc_h264/dec/src/slice_dec.cpp
        opencore/codecs_v2/video/avc_h264/enc/src/slice_enc.cpp
        opencore/codecs_v2/video/avc_h264/dec/src/vlc.cpp
        opencore/codecs_v2/video/avc_h264/enc/src/vlc_encode.cpp

        opencore/codecs_v2/utilities/colorconvert/src/ccrgb24toyuv420.cpp
        opencore/codecs_v2/utilities/colorconvert/src/cczoomrotationbase.cpp

        opencore/oscl/oscl/osclerror/src/oscl_errno.cpp
        opencore/oscl/oscl/osclerror/src/oscl_error_trapcleanup.cpp
        opencore/oscl/oscl/osclerror/src/oscl_error_imp_jumps.cpp
        
        opencore/oscl/oscl/osclbase/src/oscl_base.cpp
        opencore/oscl/oscl/osclbase/src/oscl_assert.cpp
        opencore/oscl/oscl/osclbase/src/oscl_byte_order.cpp
        opencore/oscl/oscl/osclerror/src/oscl_error.cpp
        opencore/oscl/oscl/osclerror/src/oscl_errno.cpp
        opencore/oscl/oscl/osclerror/src/oscl_error_imp_jumps.cpp
        opencore/oscl/oscl/osclerror/src/oscl_error_trapcleanup.cpp
        opencore/oscl/oscl/osclerror/src/oscl_heapbase.cpp
        opencore/oscl/oscl/osclbase/src/oscl_int64_utils.cpp
        opencore/oscl/oscl/osclbase/src/oscl_linked_list.cpp
        opencore/oscl/oscl/osclbase/src/oscl_mem_basic_functions.cpp
        opencore/oscl/oscl/osclbase/src/oscl_queue.cpp
        opencore/oscl/oscl/osclbase/src/oscl_singleton.cpp
        opencore/oscl/oscl/osclbase/src/oscl_stdstring.cpp
        opencore/oscl/oscl/osclbase/src/oscl_string_utils.cpp
        opencore/oscl/oscl/osclbase/src/oscl_tagtree.cpp
        opencore/oscl/oscl/osclbase/src/oscl_tree.cpp
        opencore/oscl/oscl/osclbase/src/oscl_tls.cpp
        opencore/oscl/oscl/osclbase/src/oscl_vector.cpp
        opencore/oscl/oscl/osclbase/src/pvlogger.cpp

        adapter/h264Adapter.h adapter/h264Adapter.cpp )

target_include_directories(h264_lib PUBLIC
        ported/
        h264lib/
        adapter/
        opencore/codecs_v2/video/avc_h264/common/include
        opencore/codecs_v2/utilities/colorconvert/include
        opencore/codecs_v2/video/avc_h264/enc/include
        opencore/codecs_v2/video/avc_h264/dec/include
        opencore/codecs_v2/video/avc_h264/enc/src

        opencore/build_config/opencore_dynamic
        opencore/oscl/oscl/osclbase/src
        opencore/oscl/oscl/config/shared
        opencore/oscl/oscl/osclmemory/src
        opencore/oscl/oscl/osclerror/src
        opencore/oscl/oscl/osclutil/src

        #for windows
        opencore/oscl/oscl/config/win32
        )

