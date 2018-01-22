
#include "micarray.hpp"
#include "inputprocess.hpp"
#include "socketpipe.hpp"
#include "messageprocess.hpp"
#include "debug_throw.hpp"
#include "Animate.h"
#include "StateSeries.h"
#include <iostream>

#ifdef __CYGWIN__
#define PORT "/dev/ttyS3"
#define VIRTUAL_PORT "/dev/ttyS4"
#elif defined(__unix__)
#define PORT "/dev/ttyUSB0"
#endif

MicArray *parray;
InputProcess *pinp;

Animate* p_animate;
auto SpeakSeries = GetSpeakSeries();
auto NodSeries = GetNodSeries();
auto LeftSeries = GetLeftSeries();
auto RightSeries = GetRightSeries();
void ProcessMessageClinet(const Message *msg)
{
    switch (msg->type)
    {
    case Message::Type::AudioBegin:
    {
        std::unique_lock<std::mutex> lock(parray->queue_output.mutex);
        parray->queue_output.queue.clear();
        std::cout<<"--- AudioBegin RECV"<<std::endl;
    }
        break;
    case Message::Type::AudioEnd:
        break;
    case Message::Type::DOA:
    {
        auto msg2 = dynamic_cast<const DOAMessage *>(msg);
        if (msg2->angle > 0)
            p_animate->SetContinuousWork(RightSeries);
        else
            p_animate->SetContinuousWork(LeftSeries);
    }
        break;
    case Message::Type::GazeBegin:
    {
        pinp->working = true;
    }
        break;
    case Message::Type::GazeEnd:
    {
        std::unique_lock<std::mutex> lock(parray->queue_output.mutex);
        parray->queue_output.queue.clear();
    }
    {
        pinp->working = false;
    }
        break;
    case Message::Type::SpeechBegin:
    {
	std::cout<<"--- SpeechBegin"<<std::endl;
        p_animate->SetContinuousWork(SpeakSeries);
    }
        break;
    case Message::Type::SpeechData:
    {
        auto msg2 = dynamic_cast<const SpeechDataMessage *>(msg);
        parray->queue_output.emplace((void *)msg2->data.data());
    }
    break;
    case Message::Type::SpeechEnd:
    {   
	std::cout<<"+++ SpeechEnd"<<std::endl;
        p_animate->ClearWork();
    }
        break;
    default:
        my_throw("invaild message type");
    }
    delete msg;
}

int main()
{
    auto serial_test_ptr = new Serial();
    serial_test_ptr->arduino_port_name = PORT;
    p_animate = new Animate(serial_test_ptr);
    p_animate->Init();

    MessageProcess proc(ProcessMessageClinet);

    SocketPipe pipe{&proc};
    pipe.init(true, NULL, 8999);

    MicArray mic;
    parray = &mic;
    InputProcess inp{&mic, &pipe, &proc};
    pinp = &inp;

    mic.start();
    inp.start_process();
    proc.process_message();

    return 0;
}
