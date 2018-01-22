
#include "message.hpp"
#include "socketpipe.hpp"

#include <cassert>

Message::Message()
{
}

Message::Message(Type t)
    : type(t)
{
}

Message::~Message()
{
}

void Message::sendto(SocketPipe &pipe) const
{
}

void Message::recvfrom(SocketPipe &pipe)
{
}

Message *Message::construct(Message::Type t)
{
    switch (t)
    {
    case Message::Type::None:
    case Message::Type::Exit:
    case Message::Type::AudioBegin:
    case Message::Type::AudioEnd:
    case Message::Type::SpeechBegin:
    case Message::Type::SpeechEnd:
    case Message::Type::GazeBegin:
    case Message::Type::GazeEnd:
        return new Message(t);
    case Message::Type::AudioData:
        return new AudioDataMessage();
    case Message::Type::SpeechData:
        return new SpeechDataMessage();
    case Message::Type::DOA:
        return new DOAMessage();
    }

    throw std::runtime_error("Invild type");
}

AudioDataMessage::AudioDataMessage()
    : Message(AudioData)
{
}

AudioDataMessage::AudioDataMessage(const AudioInputArray &d)
    : Message(AudioData), data(d)
{
}

void AudioDataMessage::sendto(SocketPipe &pipe) const
{
    assert(data.data.size() == INPUT_TRUNKSIZE);
    pipe.send(data.data(), INPUT_TRUNKSIZE * sizeof(INPUT_TYPE));
}

void AudioDataMessage::recvfrom(SocketPipe &pipe)
{
    data.resize(INPUT_TRUNKSIZE);
    pipe.recv(data.data(), INPUT_TRUNKSIZE * sizeof(INPUT_TYPE));
}


SpeechDataMessage::SpeechDataMessage()
    : Message(SpeechData)
{

}

SpeechDataMessage::SpeechDataMessage(const AudioOutputArray &data)
    : Message(SpeechData)
{
    this->data = data;
}

void SpeechDataMessage::sendto(SocketPipe& pipe) const
{
    assert(data.size() == OUTPUT_TRUNKSIZE);
    pipe.send(data.data(), OUTPUT_TRUNKSIZE * sizeof(OUTPUT_TYPE));
}

void SpeechDataMessage::recvfrom(SocketPipe& pipe)
{
    data.resize(OUTPUT_TRUNKSIZE);
    pipe.recv(data.data(), OUTPUT_TRUNKSIZE * sizeof(OUTPUT_TYPE));
}

DOAMessage::DOAMessage()
    : Message(DOA)
{

}

DOAMessage::DOAMessage(double a)
    : Message(DOA),
    angle(a)
{
}

void DOAMessage::sendto(SocketPipe& pipe) const
{
    pipe.send(&this->angle, sizeof(double));
}

void DOAMessage::recvfrom(SocketPipe& pipe)
{
    pipe.recv(&this->angle, sizeof(double));
}