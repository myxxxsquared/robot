
#ifndef MICARRAY_HEADER
#define MICARRAY_HEADER

#include <portaudio.h>
#include <deque>
#include <mutex>
#include <condition_variable>
#include <vector>

#include "audiotype.hpp"

class MicArray
{
  public:
    PaStream *stream_input, *stream_output;
    std::deque<AudioInputBuffer> deque_input;
    std::mutex mutex_input;
    std::condition_variable cv_input;
    AudioInputBuffer buffer_input;

    MicArray();
    ~MicArray();

    MicArray(const MicArray &) = delete;
    MicArray &operator=(const MicArray &) = delete;

    void init_stream();
    const AudioInputBuffer *fetch_next_input();
    void start();

    int inputCallback(
        const void *input,
        void *output,
        unsigned long frameCount,
        const PaStreamCallbackTimeInfo *timeInfo,
        PaStreamCallbackFlags statusFlags);

    static int inputCallback_static(
        const void *input,
        void *output,
        unsigned long frameCount,
        const PaStreamCallbackTimeInfo *timeInfo,
        PaStreamCallbackFlags statusFlags,
        void *userData);

    int outputCallback(
        const void *input,
        void *output,
        unsigned long frameCount,
        const PaStreamCallbackTimeInfo *timeInfo,
        PaStreamCallbackFlags statusFlags);

    static int outputCallback_static(
        const void *input,
        void *output,
        unsigned long frameCount,
        const PaStreamCallbackTimeInfo *timeInfo,
        PaStreamCallbackFlags statusFlags,
        void *userData);
};

#endif
