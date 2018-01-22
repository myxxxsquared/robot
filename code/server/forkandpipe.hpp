
#ifndef FORKANDPIPE_HEADER
#define FORKANDPIPE_HEADER

#include <unistd.h>
#include <cstdio>

class ForkAndPipe
{
  public:
    FILE *file_r, *file_w;
    pid_t pid;

    bool fork();
    void read(void *data, size_t size, size_t count);
    void write(const void *data, size_t size, size_t count);
};

#endif