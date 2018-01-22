
#ifndef RECOGNIZE_HEADER
#define RECOGNIZE_HEADER

#include "forkandpipe.hpp"
#include <sstream>
#include "safequeue.hpp"

class Message;

class Recognize
{
  public:
    ForkAndPipe pipe;
    std::stringstream ss;
    safe_queue<const Message *> *queue_in;
    safe_queue<std::string> *queue_instr;

    bool first = false;
    bool inited = false;
    const char *session_id;

    Recognize(safe_queue<const Message *> *qi, safe_queue<std::string> *qo);

    void do_recognize();
    void write_audio(const void *buffer);
    void flush_audio();
    void recognize_parent();
    void recognize_child();
};

#endif