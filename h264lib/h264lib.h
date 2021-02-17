#ifndef _H264LIB_OPENCORE_H_
#define _H264LIB_OPENCORE_H_

#if defined(h264lib_opencore_EXPORTS) || defined(EXPORT_DLL)
//extern "C" {
#else
namespace h264lib_opencore {
#endif

#define FORMAT_RGB 0
#define FORMAT_YUV420P 1
#define FORMAT_YUV420P_LIVE 2

  //  extern "C" {

    /* Encoder */
    void* h264_encoder_create();
    void  h264_encoder_destroy(void *pContext);

    int  h264_encoder_initialize(void *pContext);
    int  h264_encoder_initialize_live(void *pContext, int w, int h, int br, int fr);
    int  h264_encoder_initialize_highres(void *pContext, int res);
    void  h264_encoder_deinitialize(void *pContext);

    int  h264_encoder_get_frame_number(void *pContext);
    void  h264_encoder_set_dims_from_bitmap_size(void *pContext, int width, int height);

    int  h264_encoder_process_frame(void *pContext, void *pRGB, void *pBits, int *type);
    int  h264_encoder_process_frame_rgb(void *pContext, void *pRGB, void *pBits, int *type);

    void  h264_encoder_set_high_bandwidth_rate_control(void *pContext, int flag);
    void  h264_encoder_set_motion_estimator_high_quality(void *pContext, int flag);
    void  h264_encoder_set_frame_rate(void *pContext, float rate);
    void  h264_encoder_set_frame_number(void *pContext, int fn);
    void  h264_encoder_set_iframe_interval(void *pContext, int interval);
    void  h264_encoder_set_idr_interval(void *pContext, int interval);
    void  h264_encoder_set_key_quant(void *pContext, int code);
    void  h264_encoder_set_quant(void *pContext, int code);
    void  h264_encoder_force_idr_frame(void *pContext);

    /* Decoder */
    void *        h264_decoder_create();
    void  h264_decoder_destroy(void *pContext);

    void  h264_decoder_reset(void *pContext);

    void  h264_decoder_initialize(void *pContext);
    void  h264_decoder_deinitialize(void *pContext);

    int  h264_decoder_get_width(void *pContext);
    int  h264_decoder_get_height(void *pContext);

    int 
    h264_decoder_process_first_frame(void *pContext, void *pInputBits, int input_length, void *pOutputRGB, int bs,
                                     int *width, int *height, int format);
    int 
    h264_decoder_process_frame(void *pContext, void *input, int input_length, void *pOutputRGB, int format);

    }
//}

#endif
