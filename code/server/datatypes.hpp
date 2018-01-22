
#ifndef DATATYPES_HEADER
#define DATATYPES_HEADER

// #include <portaudio.h>
#include <cstring>
#include <string>
#include <deque>
#include <mutex>
#include <condition_variable>

typedef unsigned long PaSampleFormat;
#define paInt16          ((PaSampleFormat) 0x00000008)

constexpr int CHANNEL_COUNT = 1;
constexpr int SAMPLE_FORMAT = paInt16;
constexpr int SAMPLE_RATE = 16000;
constexpr int FRAMES_PER_BUFFER = 16000;
constexpr int RECORD_MULTIPLY = 3;
typedef unsigned short SAMPLE;
constexpr int BUFFER_SIZE = FRAMES_PER_BUFFER*sizeof(SAMPLE);

struct BUFFER
{
    SAMPLE buffer[FRAMES_PER_BUFFER];
    inline BUFFER() {}
    explicit inline BUFFER(const void* src) {memcpy(buffer, src, sizeof(buffer));}
    inline BUFFER(const void* src, int mult) {
        for(int i = 0; i < FRAMES_PER_BUFFER; ++i)
            buffer[i] = ((SAMPLE*)src)[i*3];
    }
    explicit inline BUFFER(FILE* file)
    {
        if(fread(buffer, sizeof(SAMPLE)*FRAMES_PER_BUFFER, 1, file)!=1)
            throw "Reading error.";
    }
};

template<typename T>
class SAFE_DEQUE
{
public:
    std::deque<T> queue;
    std::mutex mutex;
    std::condition_variable cv;

    SAFE_DEQUE()
    {
    }

    ~SAFE_DEQUE()
    {
    }

    SAFE_DEQUE(const SAFE_DEQUE<T>&) = delete;
    SAFE_DEQUE<T>& operator=(const SAFE_DEQUE<T>&) = delete;

    template<typename ... Types>
    void emplace(Types ... args)
    {
        std::unique_lock<std::mutex> lock{mutex};
        queue.emplace_back(args ...);
        cv.notify_one();
    }

    T pop()
    {
        std::unique_lock<std::mutex> lock{mutex};
        cv.wait(lock, [&](){return !queue.empty();});
        T result = std::move(queue.front());
        queue.pop_front();
        return result;
    }
};

template<typename T>
class SAFE_DEQUE_USE
{
public:
    SAFE_DEQUE<T> &queue;
    bool vaild;

    std::unique_lock<std::mutex> mutex;

    SAFE_DEQUE_USE(SAFE_DEQUE<T>& q, bool tryget = false)
        : queue(q)
    {
        mutex = std::unique_lock<std::mutex>(q.mutex);

        if(!tryget)
        {
            q.cv.wait(mutex, [&](){ return !q.queue.empty(); });
            vaild = true;
        }
        else
        {
            if(q.queue.empty())
            {
                vaild = false;
                mutex.release();
            }
            else
            {
                vaild = true;
            }
        }
    }

    ~SAFE_DEQUE_USE()
    {
        if(vaild)
            release();
    }

    void release()
    {
        if(vaild)
            mutex.release();
        vaild=false;
    }

    SAFE_DEQUE_USE(const SAFE_DEQUE_USE<T>&) = delete;
    SAFE_DEQUE_USE<T>& operator=(const SAFE_DEQUE_USE<T>&) = delete;

    T& operator*()
    {
        return queue.queue.front();
    }
};

typedef SAFE_DEQUE<BUFFER> frame_queue;
typedef SAFE_DEQUE_USE<BUFFER> frame_queue_use;
typedef SAFE_DEQUE<std::string> string_queue;
typedef SAFE_DEQUE_USE<std::string> string_queue_use;

inline void myfread(void* data, int size, int count, FILE* file)
{
    if((int)fread(data, size, count, file) != count)
        throw "Reading Error!";
}

inline void myfwrite(const void* data, int size, int count, FILE* file)
{
    if((int)fwrite(data, size, count, file) != count)
        throw "Writing Error!";
}

#endif
