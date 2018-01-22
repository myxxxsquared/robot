
#include "forkandpipe.hpp"
#include <stdexcept>


bool ForkAndPipe::fork()
{
    int pipe_recognize[2][2];
    if (pipe(pipe_recognize[0]) || pipe(pipe_recognize[1]))
        throw std::runtime_error("pipe() failed.");

    pid = ::fork();

    if (-1 == pid)
        throw std::runtime_error("fork() failed.");

    if (pid == 0)
    {
        file_r = fdopen(pipe_recognize[0][0], "r");
        file_w = fdopen(pipe_recognize[1][1], "w");
        return true;
    }
    else
    {
        file_r = fdopen(pipe_recognize[1][0], "r");
        file_w = fdopen(pipe_recognize[0][1], "w");
        return false;
    }
}

void ForkAndPipe::read(void *data, size_t size, size_t count)
{
    size_t r = fread(data, size, count, file_r);
    if (r != count)
        throw std::runtime_error("fread count is incorrect");
}

void ForkAndPipe::write(const void *data, size_t size, size_t count)
{
    size_t r = fwrite(data, size, count, file_w);
    if (r != count)
        throw std::runtime_error("fwrite count is incorrect");
    fflush(file_w);
}
