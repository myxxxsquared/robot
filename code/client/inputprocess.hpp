
#ifndef INPUTPROCESS_HEADER
#define INPUTPROCESS_HEADER

#include "message.hpp"
#include "micarray.hpp"
#include "vad.hpp"

#include <thread>
#include <mutex>

class SocketPipe;
class MessageProcess;

class InputProcess
{
  public:
    std::condition_variable cv_start;
    std::mutex mutex_start;
    std::thread thread;
    MicArray *micarray;
    VAD vad;
    SocketPipe *pipe;
    MessageProcess *proc;


    InputProcess(MicArray *m, SocketPipe *p, MessageProcess *pr);
    InputProcess(const InputProcess &) = delete;
    const InputProcess &operator=(const InputProcess &) = delete;

    static void thread_func_static(InputProcess *p);
    void start_process();
    void wait_start();
    void thread_func();
    void send_tag(Message::Type t);
    void send(const AudioInputStruct &data);
    void doa_result(double angle);
};

#endif
