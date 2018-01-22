

#include "qtts.h"
#include "msp_cmn.h"
#include "msp_errors.h"

#include <sys/types.h>
#include <unistd.h>

#include "forkandpipe.hpp"
#include "message.hpp"
#include "messageprocess.hpp"
#include "socketpipe.hpp"

#include "generate.hpp"

#include "debug_throw.hpp"

Generate::Generate(SocketPipe *sp, safe_queue<std::string> *q)
    : socketpipe(sp), queue_out(q)
{
}

void Generate::generate()
{
    std::string str;

    {
        int len;
        pipe.read(&len, sizeof(int), 1);
        char* text = new char[len];
        pipe.read(text, len, 1);
        str = text;
        delete[] text;
    }

    int ret = -1;
    const char* params = "voice_name = xiaofeng, text_encoding = utf8, sample_rate = 16000, speed = 50, volume = 50, pitch = 50, rdn = 2";
    const char* sessionID = QTTSSessionBegin(params, &ret);
    if (MSP_SUCCESS != ret)
        my_throw("QTTSSessionBegin() failed");

    ret = QTTSTextPut(sessionID, str.c_str(), str.size(), NULL);
    if (MSP_SUCCESS != ret)
        my_throw("QTTSTextPut() failed");

    unsigned int audio_len = 0;
    int synth_status = MSP_TTS_FLAG_STILL_HAVE_DATA;

    int tempbufferlen = 0;
    char *tempbuffer = new char[INPUT_TRUNKSIZE_BYTE];

    while(true)
    {
        const char* data = (const char*)QTTSAudioGet(sessionID, &audio_len, &synth_status, &ret);
        if (MSP_SUCCESS != ret)
            break;
        if (NULL != data)
        {
            if(tempbufferlen)
            {
                if(tempbufferlen+audio_len >= INPUT_TRUNKSIZE_BYTE)
                {
                    int left_size = INPUT_TRUNKSIZE_BYTE - tempbufferlen;
                    memcpy(tempbuffer+tempbufferlen, data, left_size);
                    audio_len -= left_size;
                    data += left_size;
                    int len = 1;
                    pipe.write(&len, sizeof(len), 1);
                    pipe.write(tempbuffer, INPUT_TRUNKSIZE_BYTE, 1);
                    tempbufferlen = 0;
                }
                else
                {
                    memcpy(tempbuffer+tempbufferlen, data, audio_len);
                    tempbufferlen += audio_len;
                    audio_len = 0;
                }
            }

            while(audio_len > INPUT_TRUNKSIZE_BYTE)
            {
                int len = 1;
                pipe.write(&len, sizeof(len), 1);
                pipe.write(data, INPUT_TRUNKSIZE_BYTE, 1);
                data += INPUT_TRUNKSIZE_BYTE;
                audio_len -= INPUT_TRUNKSIZE_BYTE;
            }

            if(audio_len > 0)
            {
                memcpy(tempbuffer, data, audio_len);
                tempbufferlen = audio_len;
            }
        }
        if (MSP_TTS_FLAG_DATA_END == synth_status)
            break;
    }

    if (MSP_SUCCESS != ret)
        my_throw("QTTSAudioGet() failed");

    if(tempbufferlen)
    {
        int len = 1;
        memset(tempbuffer+tempbufferlen, 0, INPUT_TRUNKSIZE_BYTE-tempbufferlen);
        tempbufferlen = 0;
        pipe.write(&len, sizeof(int), 1);
        pipe.write(tempbuffer, INPUT_TRUNKSIZE_BYTE, 1);
    }

    QTTSSessionEnd(sessionID, "END");

    int len = 0;
    pipe.write(&len, sizeof(int), 1);
}

void Generate::generate_child()
{
    int errcode = MSP_SUCCESS;
    const char* login_params = "appid = 5a2e1454, work_dir = .";
    errcode = MSPLogin(NULL, NULL, login_params);
    if (MSP_SUCCESS != errcode)
        my_throw("MSPLogin() failed.");
    while(true)
        generate();
}

void Generate::generate_parent()
{
    char* buffer = new char[INPUT_TRUNKSIZE_BYTE];

    while(true)
    {
        std::string str = queue_out->pop();
        int len = str.length() + 1;
        pipe.write(&len, sizeof(int), 1);
        pipe.write(str.c_str(), len, 1);

        socketpipe->sendmsg(Message::construct(Message::Type::SpeechBegin));
        while(true)
        {
            int status;
            pipe.read(&status, sizeof(int), 1);
            if(status == 0)
                break;
            pipe.read(buffer, INPUT_TRUNKSIZE_BYTE, 1);
            SpeechDataMessage *msg = new SpeechDataMessage();
            msg->data.resize(INPUT_TRUNKSIZE);
            memcpy(msg->data.data(), buffer, INPUT_TRUNKSIZE_BYTE);
            socketpipe->sendmsg(msg);
        }
        socketpipe->sendmsg(Message::construct(Message::Type::SpeechEnd));
    }
}

void Generate::do_generate()
{
    if (pipe.fork())
    {
        generate_child();
    }
    else
    {
        generate_parent();
    }
}
