
#ifndef H264LIB_OPENCORE_H264ADAPTER_H
#define H264LIB_OPENCORE_H264ADAPTER_H

#include <stdint.h>
#include "h264lib.h"

namespace h264libAdapter {

//    extern "C" {

#pragma region Viewer/Decoder side

        int CreateViewer();

        void DestroyViewerInstance(int id);

        void Shutdown();

        bool ProcessFirstH264Frame(
                int viewerId,
                uint8_t *buf,
                int bytesRead,
                uint8_t *jig,
                short borderSize,
                int *intendedWidth,
                int *intendedHeight,
                int format);

        bool ProcessH264Frame(int viewerId, uint8_t *buf, int bytesRead, uint8_t *jig, int format);
        int GetH264Width(int viewerId);
        int GetH264Height(int viewerId);

#pragma endregion

    }
//}

#endif //H264LIB_OPENCORE_H264ADAPTER_H
