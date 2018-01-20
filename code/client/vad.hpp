
#ifndef VAD_HEADER
#define VAD_HEADER

struct WebRtcVadInst;
typedef struct WebRtcVadInst VadInst;

class VAD
{
public:
    VadInst* handle;

    VAD();
    ~VAD();

    VAD(const VAD&) = delete;
    VAD& operator=(const VAD&) = delete;

    void setmode(int i);
    static bool check_vaild(int rate, int frame_length);
    bool process(int fs, const short* audio_frame, int frame_length);
};

#endif