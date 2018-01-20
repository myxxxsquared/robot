
#include "chatrobot.hpp"
#include <unistd.h>

frame_queue q_record, q_play;
string_queue q_in, q_out;

volatile bool signal_exit = false;

int main()
{
    try
    {
        init_audio();
        init_threads();
    }
    catch(const char* str)
    {
        fprintf(stderr, "Error: %s\n", str);
        return -1;
    }

    while(1)
        usleep(1000);

    exit_audio();
}
