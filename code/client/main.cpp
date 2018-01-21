
#include "micarray.hpp"
#include "inputprocess.hpp"
#include "socketpipe.hpp"
#include "messageprocess.hpp"

void ProcessMessageClinet(const Message *msg)
{

}

int main()
{
    MessageProcess proc(ProcessMessageClinet);

    SocketPipe pipe{&proc};
    pipe.init(true, NULL, 8999);

    MicArray mic;
    InputProcess inp{&mic, &pipe, &proc};
    mic.start();
    proc.process_message();

    return 0;
}
