

#include "qisr.h"
#include "msp_cmn.h"
#include "msp_errors.h"

#include <sstream>

#include <sys/types.h>
#include <unistd.h>
#include "message.hpp"
#include "forkandpipe.hpp"
#include "recognize.hpp"

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
            throw std::runtime_error("QISRSessionBegin() failed");
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
        throw std::runtime_error("QISRAudioWrite() failed");
    first = false;

    int errcode = MSP_SUCCESS;
    if (MSP_REC_STATUS_SUCCESS == rec_stat)
    {
        const char *rslt = QISRGetResult(session_id, &rec_stat, 0, &errcode);
        if (MSP_SUCCESS != errcode)
            throw std::runtime_error("QISRGetResult failed");
        if (NULL != rslt)
            ss << rslt;
    }

    if (MSP_EP_AFTER_SPEECH == ep_stat)
    {
        errcode = QISRAudioWrite(session_id, NULL, 0, MSP_AUDIO_SAMPLE_LAST, &ep_stat, &rec_stat);
        if (MSP_SUCCESS != errcode)
            throw std::runtime_error("QISRGetResult failed");
        while (MSP_REC_STATUS_COMPLETE != rec_stat)
        {
            const char *rslt = QISRGetResult(session_id, &rec_stat, 0, &errcode);
            if (MSP_SUCCESS != errcode)
                throw std::runtime_error("QISRGetResult failed");
            if (NULL != rslt)
                ss << rslt;
        }

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
        throw std::runtime_error("QISRGetResult failed");
    while (MSP_REC_STATUS_COMPLETE != rec_stat)
    {
        const char *rslt = QISRGetResult(session_id, &rec_stat, 0, &errcode);
        if (MSP_SUCCESS != errcode)
            throw std::runtime_error("QISRGetResult failed");
        if (NULL != rslt)
            ss << rslt;
    }

    inited = false;

    std::string result = ss.str();
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
            queue_instr->emplace(std::string(text));
            delete[] text;
        }
        else
        {
            safe_queue_use<const Message *> top{*queue_in};
            switch ((*top)->type)
            {
            case Message::Type::AudioBegin:
            case Message::Type::AudioEnd:
                pipe.write(&(*top)->type, sizeof(Message::Type), 1);
                break;
            case Message::Type::AudioData:
                pipe.write(&(*top)->type, sizeof(Message::Type), 1);
                pipe.write((dynamic_cast<const AudioDataMessage *>(*top))->data.data(), INPUT_TRUNKSIZE_BYTE, 1);
            default:
                throw std::runtime_error("invaild message type");
            }
        }
    }
}

void Recognize::recognize_child()
{
    int errcode = MSP_SUCCESS;
    const char *login_params = "appid = 5a2e1454, work_dir = .";
    errcode = MSPLogin(NULL, NULL, login_params);
    if (MSP_SUCCESS != errcode)
        throw std::runtime_error("MSPLogin() failed.");

    char *audiobuffer = new char[INPUT_TRUNKSIZE_BYTE];

    while (true)
    {
        int len = 0;
        pipe.write(&len, sizeof(int), 1);
        Message::Type t;
        pipe.read(&t, sizeof(Message::Type), 1);
        switch (t)
        {
        case Message::Type::AudioData:
            pipe.read(audiobuffer, INPUT_TRUNKSIZE_BYTE, 1);
            write_audio(audiobuffer);
            break;
        case Message::Type::AudioBegin:
            break;
        case Message::Type::AudioEnd:
            flush_audio();
            break;
        default:
            throw std::runtime_error("invaild message type.");
        }
    }
}
