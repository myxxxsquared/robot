
#include "micarray.hpp"
#include "inputprocess.hpp"
#include "socketpipe.hpp"
#include "messageprocess.hpp"

int main2()
{
    MessageProcess proc;

    SocketPipe pipe{&proc};
    pipe.init(true, NULL, 8999, true);

    MicArray mic;
    InputProcess inp{&mic, &pipe, &proc};
    mic.start();
    proc.process_message();

    return 0;
}
