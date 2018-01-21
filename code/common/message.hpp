
#ifndef MESSAGE_HEADER
#define MESSAGE_HEADER

#include "audiotype.hpp"

class SocketPipe;

class Message
{
public:
    enum Type : int{
        None,
        AudioBegin, AudioEnd, AudioData,
        SpeechBegin, SpeechEnd, SpeechData,
        GazeBegin, GazeEnd,
        DOA,
        Exit
    } type;

    Message();
    explicit Message(Type t);
    virtual ~Message();

    virtual void sendto(SocketPipe& pipe) const;
    virtual void recvfrom(SocketPipe& pipe);

    static Message* construct(Type t);

    Message(const Message&) = delete;
    Message &operator=(const Message&) = delete;
};

class AudioDataMessage : public Message
{
public:
    AudioInputStruct data;

    AudioDataMessage();
    explicit AudioDataMessage(const AudioInputStruct &data);
    virtual void sendto(SocketPipe& pipe) const;
    virtual void recvfrom(SocketPipe& pipe);
};

class SpeechDataMessage : public Message
{
public:
    AudioOutputArray data;

    SpeechDataMessage();
    explicit SpeechDataMessage(const AudioOutputArray &data);
    virtual void sendto(SocketPipe& pipe) const;
    virtual void recvfrom(SocketPipe& pipe);
};

class DOAMessage : public Message
{
public:
    double angle;

    DOAMessage();
    explicit DOAMessage(double a);
    virtual void sendto(SocketPipe& pipe) const;
    virtual void recvfrom(SocketPipe& pipe);
};

#endif