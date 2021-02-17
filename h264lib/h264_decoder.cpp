// h264_decoder.cpp : Defines the exported functions for the DLL application.
//
#include <vector>
#include "h264lib.h"
#include "pvavcdecoder_factory.h"
#include "pvavcdecoderinterface.h"
#include "avcdec_api.h"
#include "ccrgb24toyuv420.h"

#include <iostream>
#include <sstream>

#define INTERNAL_BYTES_PER_PIXEL 3

struct DecoderDPBFrame {
	std::vector<uint8_t> frame;
	bool			   bLocked;
};

struct DecoderContext {
	PVAVCDecoderInterface* pDecoder;
	std::vector< DecoderDPBFrame >	dpbFrames;
	int cropWidth;
	int cropHeight;
	bool	bSPSInited;
	bool	bPPSInited;
	bool	bIDRInited;

};

/** @brief AVC library calls this function once a bound frame is not needed for decoding
    operation (falls out of the sliding window, or marked unused for reference).
    @param userData  The same value of userData in AVCHandle object.
    @param indx      Index of frame to be unbound (AVC library keeps track of the index).
    @return  none.
*/
//typedef void (*FuctionType_FrameUnbind)(void *userData, int);
void decoder_frame_unbind(void *userData, int indx)
{
	DecoderContext* pContext = (DecoderContext*)userData;
	pContext->dpbFrames[indx].bLocked = false;
}



/** @brief AVC library calls this function is reserve a memory of one frame from the DPB.
    Once reserved, this frame shall not be deleted or over-written by the app.
    @param userData  The same value of userData in AVCHandle object.
    @param indx      Index of a frame in DPB (AVC library keeps track of the index).
    @param yuv      The address of the yuv pointer returned to the AVC lib.
    @return         1 for success, 0 for fail (no frames available to bind).
    */
typedef int (*FunctionType_FrameBind)(void *userData, int indx, uint8_t **yuv);
int decoder_frame_bind(void *userData, int indx, uint8_t **yuv)
{
	DecoderContext* pContext = (DecoderContext*)userData;
	if (!pContext->dpbFrames[indx].bLocked) {
		pContext->dpbFrames[indx].bLocked = true;
		*yuv = &pContext->dpbFrames[indx].frame[0];
		return 1;
	}
	*yuv = 0;
	return 0;
}

/** @brief Decoded picture buffers (DPB) must be allocated or re-allocated before an
    IDR frame is decoded. If PV_MEMORY_POOL is not defined, AVC lib will allocate DPB
    internally which cannot be shared with the application. In that case, this function
    will not be called.
    @param userData  The same value of userData in AVCHandle object.
    @param frame_size_in_mbs  The size of each frame in number of macroblocks.
    @param num_frames The number of frames in DPB.
    @return 1 for success, 0 for fail (cannot allocate DPB)
*/

//typedef int (*FunctionType_DPBAlloc)(void *userData, uint frame_size_in_mbs, uint num_buffers);
int decoder_dpballoc(void *userData, unsigned int frame_size_in_mbs, unsigned int num_frames)
{
	DecoderContext* pContext = (DecoderContext*)userData;
	int32_t width, height, top, left, bottom, right;
	pContext->pDecoder->GetVideoDimensions(&width, &height, &top, &left, &bottom, &right);

	pContext->dpbFrames.resize(num_frames);
	for (unsigned int i = 0; i < pContext->dpbFrames.size(); ++i)
	{
		pContext->dpbFrames[i].frame.resize(width * height * 3);
		pContext->dpbFrames[i].bLocked = false;
	}
	return 1;
}

/** Pointer to malloc function for general memory allocation, so that application can keep track of
    memory usage.
\param "size" "Size of requested memory in bytes."
\param "attribute" "Some value specifying types, priority, etc. of the memory."
\return "The address of the allocated memory casted to int"
*/
#ifdef __LP64__
// typedef int (*FunctionType_Malloc)(void *userData, int32_t size, int attribute);
int64_t decoder_malloc(void *userData, int32_t cbSize, int attribute)
{
	DecoderContext* pContext = (DecoderContext*)userData;
	return (int64_t)malloc(cbSize);
}
#else
// typedef int (*FunctionType_Malloc)(void *userData, int32 size, int attribute);
int decoder_malloc(void *userData, int32_t cbSize, int attribute)
{
	DecoderContext* pContext = (DecoderContext*)userData;
	return (int)malloc(cbSize);
}
#endif

/** Function pointer to free
\param "mem" "Pointer to the memory to be freed casted to int"
\return "void"
*/
// typedef void (*FunctionType_Free)(void *userData, int mem);
#ifdef __LP64__
void decoder_free(void *userData, int64_t mem)
#else
void decoder_free(void *userData, int mem)
#endif
{
	DecoderContext* pContext = (DecoderContext*)userData;
	free((void*)mem);
}

#if defined(h264lib_opencore_EXPORTS) || defined(EXPORT_DLL)
//extern "C" {
#else
namespace h264lib_opencore {
#endif

/* Decoder */
void* 
	h264_decoder_create()
{
	DecoderContext* pContext = new DecoderContext();
	PVAVCDecoderInterface* pDecoder = PVAVCDecoderFactory::CreatePVAVCDecoder();
	pContext->pDecoder = pDecoder;
	return (void*)pContext;
}

void
	h264_decoder_destroy(void *pVoid)
{
	DecoderContext* pContext = (DecoderContext*)pVoid;
	pContext->pDecoder->CleanUpAVCDecoder();
	PVAVCDecoderFactory::DeletePVAVCDecoder(pContext->pDecoder);
	delete pContext;
}

void
 h264_decoder_reset(void *pVoid)
{
	DecoderContext* pContext = (DecoderContext*)pVoid;
	pContext->pDecoder->CleanUpAVCDecoder();
	pContext->pDecoder->ResetAVCDecoder();
	return;
}

void
	h264_decoder_initialize(void *pVoid)
{
}

void
	h264_decoder_deinitialize(void *pVoid)
{
}

int
	h264_decoder_get_width(void *pVoid)
{
	int32_t width, height, top, left, bottom, right;
	DecoderContext* pContext = (DecoderContext*)pVoid;
	pContext->pDecoder->GetVideoDimensions(&width, &height, &top, &left, &bottom, &right);
	//return (int)(width-16);
    
    int paddedWidth = abs(right - left) + 1;  //actual (padded) width based on left, right values
    int border = 0; //padding is fixed 4 pixels
    return (int)(paddedWidth - border * 2);  //return unpadded width
}

int
	h264_decoder_get_height(void *pVoid)
{
	int32_t width, height, top, left, bottom, right;
	DecoderContext* pContext = (DecoderContext*)pVoid;
	pContext->pDecoder->GetVideoDimensions(&width, &height, &top, &left, &bottom, &right);
	//return (int)(height-16);
    
    int paddedHeight = abs(bottom - top) + 1; //actual (padded) height based on top, bottom values
    int border = 0;  //padding is fixed 4 pixels
    return (int)(paddedHeight - border * 2); //return unpadded height
}

int  h264_decoder_process_first_frame(void *pVoid, void *pInputBits, int input_length, void *pOutputRGB, int bs, int *width, int *height, int format)
{
    if ( pOutputRGB != NULL ) {
        DecoderContext* pContext = (DecoderContext*)pVoid;
        pContext->cropWidth = *width;
        pContext->cropHeight = *height;
        pContext->bPPSInited = false;
        pContext->bSPSInited = false;
        pContext->bIDRInited = false;
        PVAVCDecoderInterface* pDecoder = pContext->pDecoder;
        bool result = pDecoder->InitAVCDecoder(decoder_dpballoc, decoder_frame_bind, decoder_frame_unbind, decoder_malloc, decoder_free, pContext);
        return h264_decoder_process_frame(pVoid, pInputBits, input_length, pOutputRGB, format);
    } else {
        
        uint8_t* encoded_bitstream = (uint8_t*) pInputBits;
        uint8_t* pInputStream = encoded_bitstream;
        uint8_t* pNal_unit;
        int encoded_size = input_length;
        int nal_unit_size = encoded_size;
        AVCDec_Status status;
        do {
            status = PVAVCAnnexBGetNALUnit(pInputStream, &pNal_unit, &nal_unit_size);
            if (AVCDEC_FAIL != status) {
                int nal_type;
                int nal_ref_idc;
                if (PVAVCDecGetNALType(pNal_unit, nal_unit_size, &nal_type, &nal_ref_idc)) {
                    if (AVC_NALTYPE_SPS == nal_type) {
                        DecoderContext* pContext = (DecoderContext*)pVoid;
                        PVAVCDecoderInterface* pDecoder = pContext->pDecoder;
                        bool result = pDecoder->InitAVCDecoder(decoder_dpballoc, decoder_frame_bind, decoder_frame_unbind, decoder_malloc, decoder_free, pContext);
                        int32_t ret = pDecoder->DecodeSPS(pNal_unit,nal_unit_size);
                        if ( ret == AVCDEC_SUCCESS ) {
                            int32_t w_, h_, top, left, bottom, right;
                            pContext->pDecoder->GetVideoDimensions(&w_, &h_, &top, &left, &bottom, &right);
                            if ( ret == AVCDEC_SUCCESS ) {
                                *width = right-left+1; *height = bottom - top +1;
                                return 1;
                            }
                        }
                    }
                }
            }
            pInputStream = pNal_unit + nal_unit_size;
            nal_unit_size = encoded_size - (pInputStream - encoded_bitstream);
        } while (AVCDEC_SUCCESS == status);
        
        return 0;
    }
}

#define MRNG(m, a, n) { if ((a) < (m)) { (a) = (m); } else if ((a) > (n)) { (a) = (n); } }

void YUV420toRGB888(unsigned char *rgb888, unsigned char *yuv420, int width, int height)
{
	int x, y;
	int Y, U, V;
	int R, G, B;
	int pos;
	int y_pos, u_pos, v_pos;
	unsigned char *rgb;
	unsigned char *y_ptr, *u_ptr, *v_ptr;

	pos = width * height;

	rgb = rgb888;

	y_ptr = yuv420;
	u_ptr = yuv420 + pos + (pos >> 2);
	v_ptr = yuv420 + pos;

	for (y = 0; y < height; y += 2)
	{
		for (x = 0; x < width; x += 2)
		{
			/*first*/
			y_pos = y * width + x;
			u_pos = (y * width >> 2) + (x >> 1);
			v_pos = u_pos;

			Y = y_ptr[y_pos];
			U = ((u_ptr[u_pos] - 127) * 1865970) >> 20;
			V = ((v_ptr[v_pos] - 127) * 1477914) >> 20;

			R = V + Y;
			B = U + Y;
			G = (Y * 1788871 - R * 533725 - B * 203424) >> 20; //G = 1.706 * Y - 0.509 * R - 0.194 * B

			MRNG(0, R, 255);
			MRNG(0, G, 255);
			MRNG(0, B, 255);

			pos = y_pos * 3;

			rgb[pos + 0] = (unsigned char)R;
			rgb[pos + 1] = (unsigned char)G;
			rgb[pos + 2] = (unsigned char)B;

			/*second*/
			y_pos++;

			Y = y_ptr[y_pos];

			R = V + Y;
			B = U + Y;
			G = (Y * 1788871 - R * 533725 - B * 203424) >> 20; //G = 1.706 * Y - 0.509 * R - 0.194 * B

			MRNG(0, R, 255);
			MRNG(0, G, 255);
			MRNG(0, B, 255);

			pos = y_pos * 3;

			rgb[pos + 0] = (unsigned char)R;
			rgb[pos + 1] = (unsigned char)G;
			rgb[pos + 2] = (unsigned char)B;

			/*third*/
			y_pos += (width - 1);

			Y = y_ptr[y_pos];

			R = V + Y;
			B = U + Y;
			G = (Y * 1788871 - R * 533725 - B * 203424) >> 20; //G = 1.706 * Y - 0.509 * R - 0.194 * B

			MRNG(0, R, 255);
			MRNG(0, G, 255);
			MRNG(0, B, 255);

			pos = y_pos * 3;

			rgb[pos + 0] = (unsigned char)R;
			rgb[pos + 1] = (unsigned char)G;
			rgb[pos + 2] = (unsigned char)B;

			/*fourth*/
			y_pos++;

			Y = y_ptr[y_pos];

			R = V + Y;
			B = U + Y;
			G = (Y * 1788871 - R * 533725 - B * 203424) >> 20; //G = 1.706 * Y - 0.509 * R - 0.194 * B

			MRNG(0, R, 255);
			MRNG(0, G, 255);
			MRNG(0, B, 255);

			pos = y_pos * 3;

			rgb[pos + 0] = (unsigned char)R;
			rgb[pos + 1] = (unsigned char)G;
			rgb[pos + 2] = (unsigned char)B;
		}
	}
}

void croppedChannelCopy(uint8_t* dest, uint8_t* src, unsigned long srcWidth, unsigned long srcHeight, unsigned long destWidth, unsigned long destHeight, int border)
{
    for (int y = 0; y < destHeight; ++y)
    {
        uint8_t* srcBits = src + (srcWidth * (y+border) + border);
        uint8_t* destBits  = dest + (destWidth * y);
        for (int x = 0; x < destWidth; ++x)
        {
            auto value = *srcBits;
            *destBits = *srcBits;
            srcBits++;
            destBits++;
        }
    }
}

void croppedCopy(uint8_t* destBits, uint8_t* srcBits, unsigned long srcWidth, unsigned long srcHeight, unsigned long destWidth, unsigned long destHeight)
{
	int defaultBorderWidth=0;
	//srcBits += 3 * srcWidth * (srcHeight - destHeight)/2;
	//unsigned long cbWidthBorderSkip = 3 * (srcWidth-destWidth)/2;
	srcBits += 3*srcWidth*defaultBorderWidth;
	unsigned long cbWidthBorderSkipLeft = 3*defaultBorderWidth;
	unsigned long cbWidthBorderSkipRight = srcWidth*3 - cbWidthBorderSkipLeft - 3*destWidth;
	for (int y = 0; y < destHeight; ++y) 
	{
		srcBits += cbWidthBorderSkipLeft;
		for (int x = 0; x < destWidth; ++x)
		{
			destBits[0] = srcBits[0];
			destBits[1] = srcBits[1];
			destBits[2] = srcBits[2];
			srcBits += 3;
			destBits += 3;
		}
		srcBits += cbWidthBorderSkipRight;
	}
}


    
int
	h264_decoder_process_frame(void* pVoid, void *pInputBits, int input_length, void *pOutputRGB, int format)
{
	int ret=0;
	DecoderContext* pContext = (DecoderContext*)pVoid;
	PVAVCDecoderInterface* pDecoder = pContext->pDecoder;

	uint8_t* pInputStream = (uint8_t*)pInputBits;
	uint8_t* pNal_unit;

	int nal_unit_size = input_length;
	AVCDec_Status status;
	do {
		status = PVAVCAnnexBGetNALUnit(pInputStream, &pNal_unit, &nal_unit_size);

        if (AVCDEC_FAIL != status) {
			int nal_type;
			int nal_ref_idc;
			PVAVCDecGetNALType(pNal_unit, nal_unit_size, &nal_type, &nal_ref_idc);
			if (AVC_NALTYPE_PPS == nal_type) {
                int32_t result = pDecoder->DecodePPS(pNal_unit, nal_unit_size);
                pContext->bPPSInited = true;
            }
			else if (AVC_NALTYPE_SPS == nal_type) {
                int32_t result = pDecoder->DecodeSPS(pNal_unit, nal_unit_size);
                pContext->bSPSInited = true;
            }
			else if ((pContext->bSPSInited && pContext->bPPSInited)) {
				if ((!pContext->bIDRInited && (AVC_NALTYPE_IDR == nal_type)) || (pContext->bIDRInited)) {
					if (AVC_NALTYPE_IDR == nal_type) {
						pContext->bIDRInited = true;
 					}
					int32_t buffer_size = nal_unit_size;
					int32_t decode_result = pDecoder->DecodeAVCSlice(pNal_unit, &buffer_size);

					// PWS Tomsk fix.
					if ( decode_result == AVCDEC_PICTURE_OUTPUT_READY ) 
						pContext->bIDRInited = false;
						
					int index;
					int release;
					bool decode_output_result = pDecoder->GetDecOutput(&index, &release);
					if (decode_output_result) {
						uint8_t* yuv = &pContext->dpbFrames[index].frame[0];
						uint32_t yuv_length = pContext->dpbFrames[index].frame.size();

						int32_t width, height, top, left, bottom, right;
						pDecoder->GetVideoDimensions(&width, &height, &top, &left, &bottom, &right);
                        
                        //if ( abs(width - pContext->cropWidth) > 100 ||
                         //   abs(height - pContext->cropHeight) > 100 )
                         //   return 0;
                        
                        if (FORMAT_RGB == format) {
                            std::vector<uint8_t> rgb;
                            rgb.resize(width * height * INTERNAL_BYTES_PER_PIXEL);

                            YUV420toRGB888(&rgb[0], yuv, width, height);

                            croppedCopy((uint8_t*)pOutputRGB, &rgb[0], width, height, pContext->cropWidth, pContext->cropHeight);
                        } else
                        if ( FORMAT_YUV420P == format ){
                            uint8_t* pOutputYUV = (uint8_t*)pOutputRGB;
                            uint8_t* pSrcY = yuv;
                            int width2 = width/2;
                            int height2 = height/2;
                            /*int cropWidth = pContext->cropWidth;
                            int cropHeight = pContext->cropHeight;
                            int cropWidth2 = cropWidth/2;
                            int cropHeight2 = cropHeight/2;*/
                            
                            // 16-pixel padding means that we should honor the cropped width and height
                            int cropWidth = right-left + 1;
                            int cropHeight = bottom-top + 1;
                            int cropWidth2 = cropWidth/2;
                            int cropHeight2 = cropHeight/2;

                            int border = 0;
                            int border2 = 0;
                            uint8_t* pSrcU = pSrcY + (width*height);
                            uint8_t* pSrcV = pSrcY + (width*height + width2*height2);

                            uint8_t* pDestY = pOutputYUV;
                            uint8_t* pDestU = pDestY + (cropWidth*cropHeight);
                            uint8_t* pDestV = pDestY + (cropWidth*cropHeight + cropWidth2*cropHeight2);

                            
                            int yuvSize = width * height + 2 * width2*height2;

                            // Y Channel
                            croppedChannelCopy(pDestY, pSrcY, width, height, cropWidth, cropHeight, border);

                            // need to swap U & V for yuv420p

                            // U Channel
                            croppedChannelCopy(pDestU, pSrcV, width2, height2, cropWidth2, cropHeight2, border2);

                            // V Channel
                            croppedChannelCopy(pDestV, pSrcU, width2, height2, cropWidth2, cropHeight2, border2);
                        } else { /*  this is the LIVE case where we crop according to what GetVideoDimensions gives us
                                    this is the 16-pixel padding issue which we need to fix in all of Ccor */
                            uint8_t* pOutputYUV = (uint8_t*)pOutputRGB;
                            uint8_t* pSrcY = yuv;
                            int width2 = width/2;
                            int height2 = height/2;
                            uint8_t* pSrcU = pSrcY + width*height;
                            uint8_t* pSrcV = pSrcY + width*height + width2*height2;
                            
                            // 16-pixel padding means that we should honor the cropped width and height
                            int cropWidth = right-left + 1;
                            int cropHeight = bottom-top + 1;
                            int cropWidth2 = cropWidth/2;
                            int cropHeight2 = cropHeight/2;
                            
                            uint8_t* pDestY = pOutputYUV;
                            uint8_t* pDestU = pDestY + cropWidth*cropHeight;
                            uint8_t* pDestV = pDestY + cropWidth*cropHeight + cropWidth2*cropHeight2;
                            
                            croppedChannelCopy(pDestY,pSrcY,width,height,cropWidth,cropHeight,0);
                            croppedChannelCopy(pDestU,pSrcU,width2,height2,cropWidth2,cropHeight2,0);
                            croppedChannelCopy(pDestV,pSrcV,width2,height2,cropWidth2,cropHeight2,0);
                                                    
                        }
                        
						ret = 1;
						// PWS Tomsk fix
						if ( release ) {
							decoder_frame_unbind( pContext, index );
						}
					} else
						ret = 0; // PWS Tomsk fix
				}
			}
		}
		pInputStream = pNal_unit + nal_unit_size;
		nal_unit_size = input_length - (pInputStream - (uint8_t*)pInputBits);
	} while (AVCDEC_SUCCESS == status);

	return ret;
}
    
} // namespace h264lib_opencore

