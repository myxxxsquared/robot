
#include "chatrobot.hpp"

pthread_t thread_dialog, thread_recognize, thread_generate;

void init_threads()
{
    if(0 != pthread_create(&thread_dialog, NULL, &do_dialog, NULL))
    {
        throw "pthread_create(thread_dialog) Failed.";
        fprintf(stderr, "pthread_create(thread_dialog) Failed.\n");
    }
        
    if(0 != pthread_create(&thread_recognize, NULL, &do_recognize, NULL))
    {
        throw "pthread_create(thread_dialog) Failed.";
        fprintf(stderr, "pthread_create(thread_dialog) Failed.\n");
    }
        
    if(0 != pthread_create(&thread_generate, NULL, &do_generate, NULL))
    {
        throw "pthread_create(thread_dialog) Failed.";
        fprintf(stderr, "pthread_create(thread_dialog) Failed.\n");
    }
}
