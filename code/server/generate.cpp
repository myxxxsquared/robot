
#include "chatrobot.hpp"
#include "qtts.h"
#include "msp_cmn.h"
#include "msp_errors.h"

#include <sys/types.h>
#include <unistd.h>

static char tempbuffer[BUFFER_SIZE];
static int tempbufferlen = 0;

static FILE* file_generate[2];
static char generate_buffer[BUFFER_SIZE];

static void generate()
{
    int dummy = 0;
    myfwrite(&dummy, sizeof(int), 1, file_generate[1]);
    fflush(file_generate[1]);
    dummy = 1;

    std::string str;
    {
        int len;
        myfread(&len, sizeof(int), 1, file_generate[0]);
        char* text = (char*)malloc(len);
        myfread(text, len, 1, file_generate[0]);
        str = text;
        free(text);
    }

    fprintf(stderr, "OUT: %s\n", str.c_str());

    int ret = -1;
    const char* params = "voice_name = xiaofeng, text_encoding = utf8, sample_rate = 16000, speed = 50, volume = 50, pitch = 50, rdn = 2";
    const char* sessionID = QTTSSessionBegin(params, &ret);
    if (MSP_SUCCESS != ret)
    {
        fprintf(stderr, "QTTSSessionBegin() failed\n");
        throw "QTTSSessionBegin() failed";
    }
        
    ret = QTTSTextPut(sessionID, str.c_str(), str.size(), NULL);
    if (MSP_SUCCESS != ret)
    {
        fprintf(stderr, "QTTSTextPut() failed\n");
        throw "QTTSTextPut() failed";
    }

    unsigned int audio_len = 0;
    int synth_status = MSP_TTS_FLAG_STILL_HAVE_DATA;
    while(true)
    {
        const char* data = (const char*)QTTSAudioGet(sessionID, &audio_len, &synth_status, &ret);
        if (MSP_SUCCESS != ret)
            break;
        if (NULL != data)
        {
            if(tempbufferlen)
            {
                if(tempbufferlen+audio_len >= BUFFER_SIZE)
                {
                    int left_size = BUFFER_SIZE - tempbufferlen;
                    memcpy(tempbuffer+tempbufferlen, data, left_size);
                    audio_len -= left_size;
                    data += left_size;
                    myfwrite(&dummy, sizeof(int), 1, file_generate[1]);
                    myfwrite(tempbuffer, BUFFER_SIZE, 1, file_generate[1]);
                    fflush(file_generate[1]);
                    tempbufferlen = 0;
                }
                else
                {
                    memcpy(tempbuffer+tempbufferlen, data, audio_len);
                    tempbufferlen += audio_len;
                    audio_len = 0;
                }
            }

            while(audio_len > BUFFER_SIZE)
            {
                myfwrite(&dummy, sizeof(int), 1, file_generate[1]);
                myfwrite(data, BUFFER_SIZE, 1, file_generate[1]);
                fflush(file_generate[1]);
                data += BUFFER_SIZE;
                audio_len -= BUFFER_SIZE;
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
    {
        fprintf(stderr, "QTTSAudioGet() failed\n");
        throw "QTTSAudioGet() failed";
    }

    if(tempbufferlen)
    {
        memset(tempbuffer+tempbufferlen, 0, BUFFER_SIZE-tempbufferlen);
        tempbufferlen = 0;
        myfwrite(&dummy, sizeof(int), 1, file_generate[1]);
        myfwrite(tempbuffer, BUFFER_SIZE, 1, file_generate[1]);
        fflush(file_generate[1]);
    }
    
    QTTSSessionEnd(sessionID, "END");
}

static void generate_child()
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
        generate();
}

static void generate_parent()
{
    {
        int dummy;
        myfread(&dummy, sizeof(int), 1, file_generate[0]);
    }
    while(true)
    {
        std::string str;
        {
            string_queue_use use{q_out};
            str = *use;
        }
        int len = str.size() + 1;
        myfwrite(&len, sizeof(int), 1, file_generate[1]);
        myfwrite(str.c_str(), len, 1, file_generate[1]);
        fflush(file_generate[1]);

        while(true)
        {
            int curr;
            myfread(&curr, sizeof(int), 1, file_generate[0]);
            if(!curr)
                break;
            myfread(generate_buffer, BUFFER_SIZE, 1, file_generate[0]);
            q_play.emplace(generate_buffer);
        }
    }
}

void* do_generate(void*)
{
#ifdef USE_STDOUT
    while(true)
    {
        string_queue_use use{q_out};
        printf("%s\n", (*use).c_str());
    }
#endif

    pid_t pid;
    int pipe_generate[2][2];
    if(pipe(pipe_generate[0]) || pipe(pipe_generate[1]))
    {
        fprintf(stderr, "pipe error\n");
        throw "pipe error";
    }

    pid = fork();
    if(pid == 0)
    {
        file_generate[0] = fdopen(pipe_generate[0][0], "r");
        file_generate[1] = fdopen(pipe_generate[1][1], "w");
        generate_child();
    }
    else
    {
        file_generate[0] = fdopen(pipe_generate[1][0], "r");
        file_generate[1] = fdopen(pipe_generate[0][1], "w");
        generate_parent();
    }


    return NULL;
}
