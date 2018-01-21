
#include "messageprocess.hpp"
#include "socketpipe.hpp"
#include "headdirection.hpp"

const char *const SERVER_IP_ADDRESS = "192.168.1.101";
const int SERVER_PORT = 8999;

void ProcessMessageServer(const Message *msg)
{

}

int main()
{
    MessageProcess proc{ProcessMessageServer};
    SocketPipe pipe{&proc};
    pipe.init(false, SERVER_IP_ADDRESS, SERVER_PORT);

    HeadDirection head{&pipe};
    head.start();

    proc.process_message();
}
