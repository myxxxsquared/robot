
#include "datatypes.hpp"
#include "forkandpipe.hpp"

class Message;
class MessageProcess;
class SocketPipe;

class Generate
{
  public:
    ForkAndPipe pipe;
    SocketPipe *socketpipe;
    SAFE_DEQUE<std::string> *queue_out;

    Generate(SocketPipe *sp, SAFE_DEQUE<std::string> *q);

    void generate();
    void generate_child();
    void generate_parent();
    void do_generate();
};
