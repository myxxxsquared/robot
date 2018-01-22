
#include "micarray.hpp"
#include "inputprocess.hpp"
#include "socketpipe.hpp"
#include "messageprocess.hpp"
#include "debug_throw.hpp"

MicArray *parray;

void ProcessMessageClinet(const Message *msg)
{
    switch(msg->type)
    {
        case Message::Type::AudioBegin:
        break;
        case Message::Type::AudioEnd:
        break;
        case Message::Type::DOA:
        break;
        case Message::Type::GazeBegin:
        break;
        case Message::Type::GazeEnd:
        break;
        case Message::Type::SpeechBegin:
        break;
        case Message::Type::SpeechData:
        {
            auto msg2 = dynamic_cast<const SpeechDataMessage*>(msg);
            parray->queue_output.emplace((void *)msg2->data.data());
        }
        break;
        case Message::Type::SpeechEnd:
        break;
        default:
        my_throw("invaild message type");
    }
    delete msg;
}

int main()
{
    MessageProcess proc(ProcessMessageClinet);

    SocketPipe pipe{&proc};
    pipe.init(true, NULL, 8999);

    MicArray mic;
    parray = &mic;
    InputProcess inp{&mic, &pipe, &proc};

    mic.start();
    inp.start_process();
    proc.process_message();

    return 0;
}
