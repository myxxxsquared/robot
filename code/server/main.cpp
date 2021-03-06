
#include "messageprocess.hpp"
#include "socketpipe.hpp"
#include "headdirection.hpp"
#include "safequeue.hpp"

#include "dialog.hpp"
#include "recognize.hpp"
#include "generate.hpp"

#include "debug_throw.hpp"

const char *const SERVER_IP_ADDRESS = "192.168.1.101";
const int SERVER_PORT = 8999;

std::thread thread_dialog, thread_recognize, thread_generate;
safe_queue<const Message*> queue_in;
safe_queue<std::string> queue_instr;
safe_queue<std::string> queue_out;

void ProcessMessageServer(const Message *msg)
{
    switch(msg->type)
    {
    case Message::Type::AudioBegin:
    case Message::Type::AudioEnd:
    case Message::Type::AudioData:
        // printf("print\n");
        queue_in.emplace(msg);
        break;
    default:
        my_throw("invaild message type");
    }
}

void do_dialog(Dialog *obj)
{
    obj->do_dialog();
}

void do_recognize(Recognize *obj)
{
    obj->do_recognize();
}

void do_generate(Generate *obj)
{
    obj->do_generate();
}

int main()
{

    MessageProcess proc{ProcessMessageServer};
    SocketPipe pipe{&proc};
    Recognize recognize{&queue_in, &queue_instr};
    Dialog dialog{&queue_instr, &queue_out};
    Generate generate{&pipe, &queue_out};
    HeadDirection head{&pipe};

    pipe.init(false, SERVER_IP_ADDRESS, SERVER_PORT);
    thread_dialog = std::thread(do_dialog, &dialog);
    thread_recognize = std::thread(do_recognize, &recognize);
    thread_generate = std::thread(do_generate, &generate);
    head.start();

    proc.process_message();
}
