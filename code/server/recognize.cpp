
#include "chatrobot.hpp"

#include "qisr.h"
#include "msp_cmn.h"
#include "msp_errors.h"

#include <sstream>

#include <sys/types.h>
#include <unistd.h>

static FILE* file_recognize[2];
static char recognize_buffer[BUFFER_SIZE];

static void recognize_sentence()
{
    int errcode = MSP_SUCCESS;
    const char* session_begin_params = "sub = iat, domain = iat, language = zh_cn, accent = mandarin, sample_rate = 16000, result_type = plain, result_encoding = utf8";
    const char* session_id = QISRSessionBegin(NULL, session_begin_params, &errcode);
    if(MSP_SUCCESS != errcode)
    {
        fprintf(stderr, "QISRSessionBegin failed\n");
        throw "QISRSessionBegin failed";
    }

    bool first = true;
    int ep_stat = MSP_EP_LOOKING_FOR_SPEECH;
    int rec_stat = MSP_REC_STATUS_SUCCESS;

    std::stringstream ss;

    while(true)
    {
        int ret;

        {
            {
                int dummy = 0;
                myfwrite(&dummy, sizeof(int), 1, file_recognize[1]);
                fflush(file_recognize[1]);
                myfread(recognize_buffer, BUFFER_SIZE, 1, file_recognize[0]);
            }
            ret = QISRAudioWrite(
                session_id,
                recognize_buffer,
                FRAMES_PER_BUFFER*sizeof(SAMPLE),
                first ? MSP_AUDIO_SAMPLE_FIRST : MSP_AUDIO_SAMPLE_CONTINUE,
                &ep_stat,
                &rec_stat);
        }

        if (MSP_SUCCESS != ret)
        {
            fprintf(stderr, "\nQISRAudioWrite failed! error code:%d\n", ret);
            throw "QISRAudioWrite failed";
        }
        first = false;

        if (MSP_REC_STATUS_SUCCESS == rec_stat)
        {
            const char *rslt = QISRGetResult(session_id, &rec_stat, 0, &errcode);
            if (MSP_SUCCESS != errcode)
            {
                fprintf(stderr, "\nQISRGetResult failed! error code: %d\n", errcode);
                throw "QISRGetResult failed";
            }
            if (NULL != rslt)
            {
                ss<<rslt;
            }
        }

        if (MSP_EP_AFTER_SPEECH == ep_stat)
        {
            errcode = QISRAudioWrite(session_id, NULL, 0, MSP_AUDIO_SAMPLE_LAST, &ep_stat, &rec_stat);
            if (MSP_SUCCESS != errcode)
            {
                fprintf(stderr, "\nQISRGetResult failed! error code: %d\n", errcode);
                throw "QISRGetResult failed";
            }
            while (MSP_REC_STATUS_COMPLETE != rec_stat) 
            {
                const char *rslt = QISRGetResult(session_id, &rec_stat, 0, &errcode);
                if (MSP_SUCCESS != errcode)
                {
                    fprintf(stderr, "\nQISRGetResult failed, error code: %d\n", errcode);
                    throw "QISRGetResult failed";
                }
                if (NULL != rslt)
                    ss<<rslt;
            }
            break;
        }
    }

    std::string str = ss.str();
    int len = str.size() + 1;
    myfwrite(&len, sizeof(int), 1, file_recognize[1]);
    fflush(file_recognize[1]);
    myfwrite(str.c_str(), len, 1, file_recognize[1]);
    fflush(file_recognize[1]);
    fprintf(stderr, "IN : %s\n", ss.str().c_str());

    QISRSessionEnd(session_id, "END");

}

static void recognize_parent()
{
    while(true)
    {
        int len;
        myfread(&len, sizeof(int), 1, file_recognize[0]);
        if(len)
        {
            char* text;
            text = (char*)malloc(len);
            myfread(text, len, 1, file_recognize[0]);
            q_in.emplace(std::string(text));
            free(text);
        }
        else
        {
            frame_queue_use top{q_record};
            void* buffer = (*top).buffer;
            myfwrite(buffer, BUFFER_SIZE, 1, file_recognize[1]);
            fflush(file_recognize[1]);
        }
    }
}

static void recognize_child()
{
    int errcode = MSP_SUCCESS;
    const char* login_params = "appid = 5a2e1454, work_dir = .";
    errcode = MSPLogin(NULL, NULL, login_params);
    if (MSP_SUCCESS != errcode)
    {
        fprintf(stderr, "MSPLogin() failed.\n");
        throw "MSPLogin() failed.";
    }

    while(true)
        recognize_sentence();
}

void* do_recognize(void*)
{
#ifdef USE_STDIN
    std::string line;
    while(true)
    {
        std::getline(std::cin, line);
        q_in.emplace(line);
    }
#endif

    pid_t pid;
    int pipe_recognize[2][2];
    if(pipe(pipe_recognize[0]) || pipe(pipe_recognize[1]))
    {
        fprintf(stderr, "pipe error\n");
        throw "pipe error";
    }

    pid = fork();
    if(pid == 0)
    {
        file_recognize[0] = fdopen(pipe_recognize[0][0], "r");
        file_recognize[1] = fdopen(pipe_recognize[1][1], "w");
        recognize_child();
    }
    else
    {
        file_recognize[0] = fdopen(pipe_recognize[1][0], "r");
        file_recognize[1] = fdopen(pipe_recognize[0][1], "w");
        recognize_parent();
    }

    return NULL;
}
