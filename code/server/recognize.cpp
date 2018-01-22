

#include "qisr.h"
#include "msp_cmn.h"
#include "msp_errors.h"

#include <sstream>

#include <sys/types.h>
#include <unistd.h>
#include "message.hpp"
#include "forkandpipe.hpp"
#include "recognize.hpp"

#include "debug_throw.hpp"

Recognize::Recognize(safe_queue<const Message *> *qi, safe_queue<std::string> *qo)
    : queue_in(qi), queue_instr(qo)
{
}

void Recognize::do_recognize()
{
    if (pipe.fork())
    {
        recognize_child();
    }
    else
    {
        recognize_parent();
    }
}

void Recognize::write_audio(const void *buffer)
{
    if (!inited)
    {
        int errcode = MSP_SUCCESS;
        const char *session_begin_params = "sub = iat, domain = iat, language = zh_cn, accent = mandarin, sample_rate = 16000, result_type = plain, result_encoding = utf8";
        session_id = QISRSessionBegin(NULL, session_begin_params, &errcode);
        if (MSP_SUCCESS != errcode)
            my_throw("QISRSessionBegin() failed");
        inited = true;
        first = true;
    }

    int ep_stat = MSP_EP_LOOKING_FOR_SPEECH;
    int rec_stat = MSP_REC_STATUS_SUCCESS;
    int ret = QISRAudioWrite(
        session_id,
        buffer,
        INPUT_TRUNKSIZE_BYTE,
        first ? MSP_AUDIO_SAMPLE_FIRST : MSP_AUDIO_SAMPLE_CONTINUE,
        &ep_stat,
        &rec_stat);
    if (MSP_SUCCESS != ret)
        my_throw("QISRAudioWrite() failed");
    first = false;

    int errcode = MSP_SUCCESS;
    if (MSP_REC_STATUS_SUCCESS == rec_stat)
    {
        const char *rslt = QISRGetResult(session_id, &rec_stat, 0, &errcode);
        if (MSP_SUCCESS != errcode)
            my_throw("QISRGetResult failed");
        if (NULL != rslt)
            ss << rslt;
    }

    if (MSP_EP_AFTER_SPEECH == ep_stat)
    {
        errcode = QISRAudioWrite(session_id, NULL, 0, MSP_AUDIO_SAMPLE_LAST, &ep_stat, &rec_stat);
        if (MSP_SUCCESS != errcode)
            my_throw("QISRGetResult failed");
        while (MSP_REC_STATUS_COMPLETE != rec_stat)
        {
            const char *rslt = QISRGetResult(session_id, &rec_stat, 0, &errcode);
            if (MSP_SUCCESS != errcode)
                my_throw("QISRGetResult failed");
            if (NULL != rslt)
                ss << rslt;
        }

        QISRSessionEnd(session_id, "END");
        inited = false;
    }
}

void Recognize::flush_audio()
{
    int errcode = MSP_SUCCESS;
    int ep_stat = MSP_EP_LOOKING_FOR_SPEECH;
    int rec_stat = MSP_REC_STATUS_SUCCESS;

    errcode = QISRAudioWrite(session_id, NULL, 0, MSP_AUDIO_SAMPLE_LAST, &ep_stat, &rec_stat);
    if (MSP_SUCCESS != errcode)
        my_throw("QISRGetResult failed");
    while (MSP_REC_STATUS_COMPLETE != rec_stat)
    {
        const char *rslt = QISRGetResult(session_id, &rec_stat, 0, &errcode);
        if (MSP_SUCCESS != errcode)
            my_throw("QISRGetResult failed");
        if (NULL != rslt)
            ss << rslt;
    }

    QISRSessionEnd(session_id, "END");
    inited = false;

    std::string result = ss.str();
    ss.str("");
    int len = result.length() + 1;
    pipe.write(&len, sizeof(int), 1);
    pipe.write(result.c_str(), len, 1);
}

void Recognize::recognize_parent()
{
    while (true)
    {
        int len;
        pipe.read(&len, sizeof(int), 1);
        if (len)
        {
            char *text;
            text = new char[len];
            pipe.read(text, len, 1);
            std::string str(text);
            if(str.size())
            {
                queue_instr->emplace(str);
#ifdef DEBUG
            printf("RECOGNIZE: %s\n", text);
#endif
            }
            else
            {
#ifdef DEBUG
            printf("RECOGNIZE: <NONE>\n");
#endif
            }

            delete[] text;
        }
        else
        {
            // printf("0\n");
            // fflush(stdout);
            const Message *msg = queue_in->pop();
            // safe_queue_use<const Message *> top{*queue_in};
            // printf("%d\n", (int)queue_in->queue.size());
            switch (msg->type)
            {
            case Message::Type::AudioBegin:
            case Message::Type::AudioEnd:
                // printf("1\n");
                // fflush(stdout);
                pipe.write(&msg->type, sizeof(Message::Type), 1);
                break;
            case Message::Type::AudioData:
                // printf("2\n");
                // fflush(stdout);
                pipe.write(&msg->type, sizeof(Message::Type), 1);
                pipe.write((dynamic_cast<const AudioDataMessage *>(msg))->data.data(), INPUT_TRUNKSIZE_BYTE, 1);
                break;
            default:
                my_throw("invaild message type");
                break;
            }

            delete msg;
        }
    }
}

void Recognize::recognize_child()
{
    int errcode = MSP_SUCCESS;
    const char *login_params = "appid = 5a2e1454, work_dir = .";
    errcode = MSPLogin(NULL, NULL, login_params);
    if (MSP_SUCCESS != errcode)
        my_throw("MSPLogin() failed.");

    char *audiobuffer = new char[INPUT_TRUNKSIZE_BYTE];

    while (true)
    {
        // printf("x\n");
        // fflush(stdout);
        int len = 0;
        pipe.write(&len, sizeof(int), 1);
        // printf("y\n");
        // fflush(stdout);
        Message::Type t;
        pipe.read(&t, sizeof(Message::Type), 1);
        // printf("z\n");
        // fflush(stdout);
        switch (t)
        {
        case Message::Type::AudioData:
            // printf("a\n");
            // fflush(stdout);
            pipe.read(audiobuffer, INPUT_TRUNKSIZE_BYTE, 1);
            write_audio(audiobuffer);
            break;
        case Message::Type::AudioBegin:
            // printf("b\n");
            // fflush(stdout);
            printf("RECOGNIZE BEGIN.\n");
            break;
        case Message::Type::AudioEnd:
            // printf("c\n");
            // fflush(stdout);
            printf("RECOGNIZE END.\n");
            flush_audio();
            break;
        default:
            my_throw("invaild message type.");
        }
    }
}
