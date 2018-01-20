
#include <cstdio>
#include <stdexcept>
#include <cstring>

#include "micarray.hpp"

void MicArray::init_stream()
{
    PaStreamParameters paramsinput, paramsoutput;

    PaError err = paNoError;
    char buffer[128];

    err = Pa_Initialize();
    if (paNoError != err)
    {
        sprintf(buffer, "Pa_Initialize() returned %d.", err);
        throw std::runtime_error(buffer);
    }

    int numDevices = Pa_GetDeviceCount();
    if (numDevices < 0)
    {
        sprintf(buffer, "Pa_GetDeviceCount() returned %d.", err);
        throw std::runtime_error(buffer);
    }

    int input_device = -1, output_device = -1;
    const PaDeviceInfo *input_device_info, *output_device_info;
    for (int i = 0; i < numDevices; ++i)
    {
        const PaDeviceInfo *info = Pa_GetDeviceInfo(i);
        if (info == NULL)
            continue;
        fprintf(stderr, "Device %d: %s\n", i, info->name);
        if (input_device == -1 && nullptr != strstr(info->name, "ac108"))
        {
            input_device_info = info;
            input_device = i;
        }
        if (output_device == -1 && nullptr != strstr(info->name, "bcm2835 ALSA"))
        {
            output_device_info = info;
            output_device = i;
        }
    }

    if (input_device == -1)
        throw std::runtime_error("Cannot find input device!");
    if (output_device == -1)
        throw std::runtime_error("Cannot find output device!");
    fprintf(stderr, "Device %d selected as input.\nDevice %d selected as output.\n", input_device, output_device);

    paramsinput.device = input_device;
    paramsinput.channelCount = INPUT_CHANNELS;
    paramsinput.sampleFormat = INPUT_FORMAT;
    paramsinput.hostApiSpecificStreamInfo = NULL;
    paramsinput.suggestedLatency = input_device_info->defaultLowInputLatency;
    err = Pa_OpenStream(
        &stream_input,
        &paramsinput,
        NULL,
        INPUT_SAMPLERATE,
        INPUT_TRUNKSIZE,
        paNoFlag,
        MicArray::inputCallback_static,
        this);
    if (paNoError != err)
    {
        sprintf(buffer, "Pa_OpenStream(INPUT) returned %d.", err);
        throw std::runtime_error(buffer);
    }

    paramsoutput.device = output_device;
    paramsoutput.channelCount = OUTPUT_CHANNELS;
    paramsoutput.sampleFormat = OUTPUT_FORMAT;
    paramsoutput.hostApiSpecificStreamInfo = NULL;
    paramsoutput.suggestedLatency = output_device_info->defaultLowOutputLatency;
    err = Pa_OpenStream(
        &stream_input,
        NULL,
        &paramsoutput,
        OUTPUT_SAMPLERATE,
        OUTPUT_TRUNKSIZE,
        paNoFlag,
        MicArray::inputCallback_static,
        this);
    if (paNoError != err)
    {
        sprintf(buffer, "Pa_OpenStream(OUTPUT) returned %d.", err);
        throw std::runtime_error(buffer);
    }
}

MicArray::MicArray()
{
    init_stream();
}

MicArray::~MicArray()
{
}

int MicArray::inputCallback(
    const void *input,
    void *output,
    unsigned long frameCount,
    const PaStreamCallbackTimeInfo *timeInfo,
    PaStreamCallbackFlags statusFlags)
{
    std::lock_guard<std::mutex> lock(this->mutex_input);
    this->deque_input.emplace_back();
    memcpy(&this->deque_input.back(), input, sizeof(AudioInputBuffer));
    this->cv_input.notify_one();
    return paContinue;
}

int MicArray::inputCallback_static(
    const void *input,
    void *output,
    unsigned long frameCount,
    const PaStreamCallbackTimeInfo *timeInfo,
    PaStreamCallbackFlags statusFlags,
    void *userData)
{
    return ((MicArray *)userData)->inputCallback(input, output, frameCount, timeInfo, statusFlags);
}

int MicArray::outputCallback(
    const void *input,
    void *output,
    unsigned long frameCount,
    const PaStreamCallbackTimeInfo *timeInfo,
    PaStreamCallbackFlags statusFlags)
{
    return paContinue;
}

int MicArray::outputCallback_static(
    const void *input,
    void *output,
    unsigned long frameCount,
    const PaStreamCallbackTimeInfo *timeInfo,
    PaStreamCallbackFlags statusFlags,
    void *userData)
{
    return ((MicArray *)userData)->outputCallback(input, output, frameCount, timeInfo, statusFlags);
}

const AudioInputBuffer *MicArray::fetch_next_input()
{
    std::unique_lock<std::mutex> lock{mutex_input};
    cv_input.wait(lock, [&]() { return !deque_input.empty(); });
    memcpy(&this->buffer_input, &this->deque_input.front(), sizeof(AudioInputBuffer));
    this->deque_input.pop_front();
    return &buffer_input;
}

void MicArray::start()
{
    PaError err;
    char buffer[128];

    err = Pa_StartStream(stream_input);
    if (paNoError != err)
    {
        sprintf(buffer, "Pa_StartStream(INPUT) returned %d.", err);
        throw std::runtime_error(buffer);
    }

    err = Pa_StartStream(stream_output);
    if (paNoError != err)
    {
        sprintf(buffer, "Pa_StartStream(OUTPUT) returned %d.", err);
        throw std::runtime_error(buffer);
    }
}
