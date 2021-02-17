// h264_encoder.cpp : Defines the exported functions for the DLL application.
//

#include <vector>
#include <memory>
#include "h264lib.h"
#include "pvavcencoder_factory.h"
#include "pvavcencoderinterface.h"

#include <assert.h>

struct EncoderContext {
	PVAVCEncoderInterface*	pEncoder;
	TAVCEIInputFormat		inputFormat;
	TAVCEIEncodeParam		encodeParam;
	long					frameNum;
	long					border;
	long					inputWidth;
	long					inputHeight;
};

#if defined(h264lib_opencore_EXPORTS) || defined(EXPORT_DLL)
//zextern "C" {
#else
    namespace h264lib_opencore {
#endif

void* 
 h264_encoder_create()
{
	PVAVCEncoderInterface* pEncoder = PVAVCEncoderFactory::CreatePVAVCEncoder();
	EncoderContext* pContext = new EncoderContext();
	pContext->border = 4;
	pContext->inputWidth = 0;
	pContext->inputHeight = 0;
	pContext->frameNum = 0;
	pContext->pEncoder = pEncoder;
	pContext->inputFormat.iFrameWidth = 0;
	pContext->inputFormat.iFrameHeight = 0;
	
	// Contains the input frame rate in the unit of frame per second.
	pContext->inputFormat.iFrameRate = 15;
	
	// Contains Frame Orientation. Used for RGB input. 1 means Bottom_UP RGB, 0 means Top_Down RGB, -1 for video formats other than RGB
	pContext->inputFormat.iFrameOrientation = 1; 
	
	// Contains the format of the input video, e.g., YUV 4:2:0, UYVY, RGB24, etc.
    pContext->inputFormat.iVideoFormat = EAVCEI_VDOFMT_RGB24;
	
	pContext->encodeParam.iEncodeID = 0;
	pContext->encodeParam.iProfile = EAVCEI_PROFILE_BASELINE;
	pContext->encodeParam.iLevel = EAVCEI_LEVEL_AUTODETECT;//EAVCEI_LEVEL_41;
	pContext->encodeParam.iNumLayer = 1;
	pContext->encodeParam.iBitRate[0] = 48000;
	pContext->encodeParam.iEncMode = EAVCEI_ENCMODE_RECORDER;
	pContext->encodeParam.iOutOfBandParamSet = false;
	pContext->encodeParam.iOutputFormat = EAVCEI_OUTPUT_ANNEXB;
	pContext->encodeParam.iPacketSize = 0;
	pContext->encodeParam.iRateControlType = EAVCEI_RC_CONSTANT_Q;
	pContext->encodeParam.iBufferDelay = 1.0;
	pContext->encodeParam.iIquant[0] = 24;
	pContext->encodeParam.iBquant[0] = 24;
	pContext->encodeParam.iPquant[0] = 24;
	pContext->encodeParam.iSceneDetection = false;
	pContext->encodeParam.iIFrameInterval = 2;
	pContext->encodeParam.iNumIntraMBRefresh = 0;
	pContext->encodeParam.iClipDuration = 0;
	pContext->encodeParam.iFSIBuff = 0;
	pContext->encodeParam.iFSIBuffLength = 0;
	return reinterpret_cast<void*>(pContext);
}

void  
 h264_encoder_destroy(void *pVoid)
{
	EncoderContext* pContext = (EncoderContext*)pVoid;
	PVAVCEncoderFactory::DeletePVAVCEncoder(pContext->pEncoder);
}

int  
 h264_encoder_get_frame_number(void *pVoid)
{
	EncoderContext* pContext = (EncoderContext*)pVoid;
	return pContext->frameNum;
}

void  
 h264_encoder_set_frame_number(void *pVoid, int fn)
{
	EncoderContext* pContext = (EncoderContext*)pVoid;
    if (pContext->frameNum != fn) {
        //pContext->frameNum = fn;
        // abrupt frame change, force IDR
        //pContext->pEncoder->IDRRequest();
    }
}

void  
 h264_encoder_set_dims_from_bitmap_size(void *pVoid, int width, int height)
{
	EncoderContext* pContext = (EncoderContext*)pVoid;
	pContext->inputWidth= width;
	pContext->inputHeight = height;
	width += pContext->border * 2;
	height += pContext->border * 2;
	pContext->inputFormat.iFrameWidth = width;
	pContext->inputFormat.iFrameHeight = height;
	pContext->encodeParam.iFrameWidth[0] = width;
	pContext->encodeParam.iFrameHeight[0] = height;

}

int 
	h264_encoder_initialize(void *pVoid)
{
	EncoderContext* pContext = (EncoderContext*)pVoid;
	TAVCEI_RETVAL result = pContext->pEncoder->Initialize(&pContext->inputFormat, &pContext->encodeParam);
	if (result != EAVCEI_SUCCESS)
	{
		return 0;
	}
	return 1;
}

int
    h264_encoder_initialize_live (void *pVoid, int w, int h, int br, int fr)
{// Not implemented obviously...
    return h264_encoder_initialize(pVoid);
}

int
	h264_encoder_initialize_highres(void *pVoid, int res)
{
    // no special code for hires
    return h264_encoder_initialize(pVoid);
}

void  
	h264_encoder_deinitialize(void *pContext)
{
}

void expandCopy(uint8_t* destBits, uint8_t* srcBits, unsigned long srcWidth, unsigned long srcHeight, unsigned long destWidth, unsigned long destHeight)
{
	destBits += 3 * destWidth * (destHeight - srcHeight)/2;
	unsigned long cbWidthBorderSkip = 3 * (destWidth-srcWidth)/2;
	for (int y = 0; y < srcHeight; ++y) 
	{
		destBits += cbWidthBorderSkip;
		for (int x = 0; x < srcWidth; ++x)
		{
			destBits[0] = srcBits[0];
			destBits[1] = srcBits[1];
			destBits[2] = srcBits[2];
			srcBits += 3;
			destBits += 3;
		}
		destBits += cbWidthBorderSkip;
	}
}

static void StripEmulationBytes(uint8_t* nalUnit, unsigned int& nalUnitSize) {
	std::vector<uint8_t> nalUnitOrig;
	nalUnitOrig.resize(nalUnitSize);
	memcpy(&nalUnitOrig[0], nalUnit, nalUnitSize);
	uint8_t* nalUnitCopy = nalUnit;
    unsigned int nalUnitCopySize = 0;

    for (unsigned i = 0; i < nalUnitSize; ++i) {
		if (i+2 < nalUnitSize && nalUnitOrig[i] == 0 && nalUnitOrig[i+1] == 0 && nalUnitOrig[i+2] == 3) {
			nalUnitCopy[nalUnitCopySize++] = nalUnitOrig[i++];
			nalUnitCopy[nalUnitCopySize++] = nalUnitOrig[i++];
        } else {
			nalUnitCopy[nalUnitCopySize++] = nalUnitOrig[i];
		}
	}
	nalUnitSize = nalUnitCopySize;
}

int
    h264_encoder_process_frame(void *pVoid, void *pRGB, void *pBits, int *type)
{
    // NOT IMPLEMENTED YET FOR OPEN CORE!  Not using the encoder for opencore
    return h264_encoder_process_frame_rgb( pVoid, pRGB, pBits, type );
}


int	
	h264_encoder_process_frame_rgb(void *pVoid, void *pRGB, void *pBits, int *type)
{
	TAVCEI_RETVAL result;
	EncoderContext* pContext = (EncoderContext*)pVoid;
	TAVCEIInputData inputData;
	inputData.iTimeStamp = 1000 * (pContext->frameNum / pContext->inputFormat.iFrameRate);
	if (pContext->border > 0) {
		std::vector<uint8_t> paddedRGB(pContext->inputFormat.iFrameWidth * pContext->inputFormat.iFrameHeight * 3);
		expandCopy(&paddedRGB[0], (uint8_t*)pRGB, pContext->inputWidth, pContext->inputHeight, pContext->inputFormat.iFrameWidth, pContext->inputFormat.iFrameHeight);
		inputData.iSource = &paddedRGB[0];
		result = pContext->pEncoder->Encode(&inputData); 
	} else {
		inputData.iSource = (uint8_t*)pRGB;
		result = pContext->pEncoder->Encode(&inputData); 
	}
	if (result != EAVCEI_SUCCESS)
	{
		return 0;
	}

	TAVCEIOutputData outputData;
	outputData.iBitstream = (uint8_t*)pBits;
	int cbOutputSize = pContext->inputFormat.iFrameWidth * pContext->inputFormat.iFrameHeight * 10;
	outputData.iBitstreamSize = cbOutputSize;
	bool bFirstNal = true;
	do {
		outputData.iBitstream[0] = 0x00;
		outputData.iBitstream[1] = 0x00;
		if (bFirstNal) {
			outputData.iBitstream[2] = 0x00;
			outputData.iBitstream[3] = 0x01;
			outputData.iBitstream += 4;
			outputData.iBitstreamSize -= 4;
			bFirstNal = false;
		} else {
			outputData.iBitstream[2] = 0x01;
			outputData.iBitstream += 3;
			outputData.iBitstreamSize -= 3;
		}
		int remainingBytes;
		result = pContext->pEncoder->GetOutput(&outputData, &remainingBytes);
		outputData.iBitstream += outputData.iBitstreamSize;
		cbOutputSize -= outputData.iBitstreamSize;
		outputData.iBitstreamSize = cbOutputSize;
	} while (result == EAVCEI_MORE_NAL);

	if (result != EAVCEI_SUCCESS)
		return 0;

    *type = (outputData.iKeyFrame) ? 0 : 2;
    
	pContext->frameNum++;
	unsigned int cbResultSize = outputData.iBitstream - (uint8_t*)pBits;

	// strip emulation bytes
	StripEmulationBytes((uint8_t*)pBits, cbResultSize);
	return cbResultSize;
}

void
 h264_encoder_set_high_bandwidth_rate_control(void *pContext, int flag)
{
}

void
 h264_encoder_force_idr_frame (void *pContext)
{
    // Can't do nuffin here.   Looks like we don't have apriori control of this encoder's frame type...
}

void
	h264_encoder_set_motion_estimator_high_quality(void *pContext, int flag)
{
}

void
	h264_encoder_set_frame_rate(void *pVoid, float rate)
{
	EncoderContext* pContext = (EncoderContext*)pVoid;
	pContext->inputFormat.iFrameRate = rate;
	pContext->encodeParam.iFrameRate[0] = rate;
}

void
	h264_encoder_set_iframe_interval(void *pContext, int interval)
{
}

void
	h264_encoder_set_idr_interval(void *pContext, int interval)
{
}

void
	h264_encoder_set_key_quant(void *pVoid, int code)
{
	EncoderContext* pContext = (EncoderContext*)pVoid;
	pContext->encodeParam.iIquant[0] = code-1;
}

void
	h264_encoder_set_quant(void *pVoid, int code)
{
	EncoderContext* pContext = (EncoderContext*)pVoid;
	pContext->encodeParam.iPquant[0] = code-1;
}
        
}
