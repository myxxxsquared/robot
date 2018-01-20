
#include "config.h"

#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <iostream>

#include <deque>
#include <string>
using std::string;
using std::deque;

#include <portaudio.h>
#include <semaphore.h>
#include <pthread.h>

#include "datatypes.hpp"

extern PaStream *inputStream, *outputStream;
extern volatile bool signal_exit;
extern pthread_t thread_dialog, thread_recognize, thread_generate;

extern frame_queue q_record, q_play;
extern string_queue q_in, q_out;

void init_audio();
void exit_audio();
void init_threads();
void init_xf();

int recordCallback(
    const void *input, void *output,
    unsigned long frameCount,
    const PaStreamCallbackTimeInfo* timeInfo,
    PaStreamCallbackFlags statusFlags,
    void *userData );

int playCallback(
    const void *input, void *output,
    unsigned long frameCount,
    const PaStreamCallbackTimeInfo* timeInfo,
    PaStreamCallbackFlags statusFlags,
    void *userData );

void* do_recognize(void*);
void* do_dialog(void*);
void* do_generate(void*);
