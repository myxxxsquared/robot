
#include "forkandpipe.hpp"
#include "safequeue.hpp"

class Message;
class MessageProcess;
class SocketPipe;

class Generate
{
  public:
    ForkAndPipe pipe;
    SocketPipe *socketpipe;
    safe_queue<std::string> *queue_out;

    Generate(SocketPipe *sp, safe_queue<std::string> *q);

    void generate();
    void generate_child();
    void generate_parent();
    void do_generate();
};
