
#include <thread>
#include <condition_variable>
#include <cassert>
#include <algorithm>
#include <cstring>

#include "audiotype.hpp"
#include "vad.hpp"
#include "doa.hpp"

#include "socketpipe.hpp"
#include "inputprocess.hpp"
#include "messageprocess.hpp"

const int INPUT_DOA_TRUNKS = 20;

const int ON_CHANGE = 5;
const int OFF_CHANGE = 5;
const int OFF_APPEND_DATA = 10;
const int DATA_MAX_SIZE = 20;

InputProcess::InputProcess(MicArray *m, SocketPipe *p, MessageProcess *pr)
    : cv_start(),
        mutex_start(),
        thread(InputProcess::thread_func_static, this),
        micarray(m),
        vad(),
        pipe(p),
        proc(pr)
{
    vad.setmode(1);
    assert(VAD::check_vaild(INPUT_SAMPLERATE, INPUT_TRUNKSIZE));
}

void InputProcess::thread_func_static(InputProcess *p)
{
    p->thread_func();
}

void InputProcess::start_process()
{
    cv_start.notify_one();
}

void InputProcess::wait_start()
{
    std::unique_lock<std::mutex> lock(mutex_start);
    cv_start.wait(lock);
}

void InputProcess::thread_func()
{
    wait_start();

    const AudioInputBuffer *buffer;

    bool curraudio = false;
    int curchange = 0;
    std::deque<AudioInputStruct> inputs;
    std::vector<AudioInputStruct> doainputs;

    while (true)
    {
        buffer = micarray->fetch_next_input();

        AudioInputStruct data;
        for (intptr_t i = 0; i < INPUT_CHANNELS; ++i)
            data.data[i].resize(INPUT_TRUNKSIZE);
        for (intptr_t i = 0; i < INPUT_TRUNKSIZE * INPUT_CHANNELS; ++i)
            data.data[i % INPUT_CHANNELS][i / INPUT_CHANNELS] = buffer->data[i];

        bool cur = vad.process(INPUT_SAMPLERATE, data.data[0].data(), INPUT_TRUNKSIZE);

        inputs.emplace_back(data);
        if (inputs.size() > DATA_MAX_SIZE)
            inputs.pop_front();

        if (curraudio)
        {
            doainputs.emplace_back(data);
            if (doainputs.size() == INPUT_DOA_TRUNKS)
            {
                AudioInputArray doadata[4];
                for (int i = 0; i < 4; ++i)
                    doadata[i].resize(INPUT_DOA_TRUNKS * INPUT_TRUNKSIZE);
                for (intptr_t i = 0; i < INPUT_DOA_TRUNKS; ++i)
                    for (int j = 0; j < 4; ++j)
                        memcpy(&doadata[j][i * INPUT_TRUNKSIZE], doainputs[i].data[j].data(), sizeof(INPUT_TYPE) * INPUT_TRUNKSIZE);
                doa_result(doa(doadata));
                doainputs.clear();
            }

            send(data);
            if (cur)
            {
                curchange = 0;
            }
            else
            {
                curchange++;
                if (curchange > ON_CHANGE)
                {
                    curraudio = false;
                    curchange = 0;
                    send_tag(Message::Type::AudioEnd);
                }
            }
        }
        else
        {
            if (cur)
            {
                curchange++;
                if (curchange > OFF_CHANGE)
                {
                    curraudio = true;
                    curchange = 0;
                    intptr_t i = std::max((intptr_t)(inputs.size() - OFF_APPEND_DATA), (intptr_t)0);
                    send_tag(Message::Type::AudioBegin);
                    for (; i < (intptr_t)inputs.size(); ++i)
                        send(inputs[i]);
                }
            }
            else
            {
                curchange = 0;
            }
        }
    }
}

void InputProcess::send_tag(Message::Type tag)
{
    pipe->sendmsg_tag(tag);
}

void InputProcess::send(const AudioInputStruct &data)
{
    AudioDataMessage *msg = new AudioDataMessage(data);
    pipe->sendmsg(msg);
    proc->put_message(msg);
}

void InputProcess::doa_result(double angle)
{
    Message *msg = new DOAMessage(angle);
    proc->put_message(msg);
}
