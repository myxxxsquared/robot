
#ifndef RECOGNIZE_HEADER
#define RECOGNIZE_HEADER

#include "forkandpipe.hpp"
#include <sstream>
#include "datatypes.hpp"

class Message;

class Recognize
{
  public:
    ForkAndPipe pipe;
    std::stringstream ss;
    SAFE_DEQUE<const Message *> *queue_in;
    SAFE_DEQUE<std::string> *queue_instr;

    bool first = false;
    bool inited = false;
    const char *session_id;

    Recognize(SAFE_DEQUE<const Message *> *qi, SAFE_DEQUE<std::string> *qo);

    void do_recognize();
    void write_audio(const void *buffer);
    void flush_audio();
    void recognize_parent();
    void recognize_child();
};

#endif