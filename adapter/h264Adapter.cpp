#include <string>
#include <map>
#include <stdint.h>

#include "h264Adapter.h"

#include "h264lib.h"

#include <mutex>

using namespace h264lib_opencore;
namespace h264libAdapter {

#pragma region Viewer/Decoder side

static std::map<int, void*> viewerList_;
static int lastViewerId_ = 0;
static std::mutex idMutex_;

void Shutdown() {
    auto iter = viewerList_.begin();
    while (iter != viewerList_.end()) {
        h264_decoder_destroy(iter->second);
        iter->second = nullptr;
        iter = viewerList_.erase(iter);
    }
}

void DestroyViewerInstance(int id) {
    auto iter = viewerList_.find(id);
    if (iter == viewerList_.end())
        return;

    h264_decoder_destroy(iter->second);
    iter->second = nullptr;
    viewerList_.erase(iter);
}

int CreateViewer() {

    idMutex_.lock();
    auto id = ++lastViewerId_;
    idMutex_.unlock();

    auto pContext = h264_decoder_create();
    viewerList_.insert(std::make_pair(id, pContext));

    return id;
}

bool ProcessFirstH264Frame(
        int viewerId,
        uint8_t *buf,
        int bytesRead,
        uint8_t *jig,
        short borderSize,
        int *intendedWidth,
        int *intendedHeight,
        int format) {

    auto instance = viewerList_.find(viewerId);
    if (instance == viewerList_.end())
        return false;

    auto context = instance->second;
    return h264_decoder_process_first_frame(context, buf, bytesRead, jig, borderSize, intendedWidth, intendedHeight, format);;
}

bool ProcessH264Frame(int viewerId, uint8_t* buf, int bytesRead, uint8_t* jig, int format)
{
    auto instance = viewerList_.find(viewerId);
    if (instance == viewerList_.end())
        return false;

    auto context = instance->second;
    return h264_decoder_process_frame(context, buf, bytesRead, jig, format);
}

int GetH264Width(int viewerId) {
    auto instance = viewerList_.find(viewerId);
    if (instance == viewerList_.end())
        return 0;

    auto context = instance->second;
    return h264_decoder_get_width(context);
}

int GetH264Height(int viewerId){
    auto instance = viewerList_.find(viewerId);
    if (instance == viewerList_.end())
        return 0;

    auto context = instance->second;
    return h264_decoder_get_height(context);
}

#pragma endregion

}
