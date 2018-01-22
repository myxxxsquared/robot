
[[noreturn]] void _my_throw(const char* x, const char* file, int line, const char* func);

#define my_throw(x) _my_throw(x, __FILE__, __LINE__, __FUNCTION__)
