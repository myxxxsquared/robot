
#include <cstdio>
#include <stdexcept>

#include "debug_throw.hpp"

[[noreturn]] void _my_throw(const char* x, const char* file, int line, const char* func)
{
    char *buffer = new char[1024];
    sprintf(buffer, "%s:%d:%s %s", file, line, func, x);

    puts(buffer);

#ifdef DEBUG
    *((char*)(nullptr)) = 0;
#endif

    throw std::runtime_error(buffer);
}
