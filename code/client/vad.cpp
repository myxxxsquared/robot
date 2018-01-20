
#include "vad.hpp"

#include <stdexcept>
#include "webrtc/common_audio/vad/include/webrtc_vad.h"

VAD::VAD()
{
    if(0 != WebRtcVad_Create(&handle))
        throw std::runtime_error("WebRtcVad_Create() Failed.");
    WebRtcVad_Init(handle);
}

VAD::~VAD()
{
    WebRtcVad_Free(handle);
}

void VAD::setmode(int i)
{
    if(0 != WebRtcVad_set_mode(handle, i))
        throw std::runtime_error("WebRtcVad_set_mode() Failed.");
}

bool VAD::check_vaild(int rate, int frame_length)
{
    return WebRtcVad_ValidRateAndFrameLength(rate, frame_length) == 0;
}

bool VAD::process(int fs, const int16_t* audio_frame, int frame_length)
{
    int result = WebRtcVad_Process(handle, fs, audio_frame, frame_length);
    if(result == 0)
        return false;
    if(result == 1)
        return true;
    throw std::runtime_error("WebRtcVad_Process() Failed.");
}
