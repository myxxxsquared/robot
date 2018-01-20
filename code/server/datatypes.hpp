
#include <portaudio.h>
#include <cstring>
#include <string>
#include <deque>

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
    pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
    sem_t sem;

    SAFE_DEQUE()
    {
        sem_init(&sem, 0, 0);
    }

    ~SAFE_DEQUE()
    {
        sem_destroy(&sem);
    }

    SAFE_DEQUE(const SAFE_DEQUE<T>&) = delete;
    SAFE_DEQUE<T>& operator=(const SAFE_DEQUE<T>&) = delete;

    void begin_use()
    {
        while(true)
        {
            pthread_mutex_lock(&mutex);
            if(queue.size() > 0)
                return;
            pthread_mutex_unlock(&mutex);
            sem_wait(&sem);
        }
    }

    bool try_begin_use()
    {
        pthread_mutex_lock(&mutex);
        if(queue.size() > 0)
            return true;
        pthread_mutex_unlock(&mutex);
        return false;
    }

    void end_use()
    {
        queue.pop_front();
        pthread_mutex_unlock(&mutex);
    }

    template<typename ... Types>
    void emplace(Types ... args)
    {
        pthread_mutex_lock(&mutex);
        queue.emplace_back(args ...);
        sem_trywait(&sem);
        sem_post(&sem);
        pthread_mutex_unlock(&mutex);
    }
};

template<typename T>
class SAFE_DEQUE_USE
{
public:
    SAFE_DEQUE<T> &queue;
    bool vaild;

    SAFE_DEQUE_USE(SAFE_DEQUE<T>& q, bool tryget = false)
        : queue(q)
    {
        if(!tryget)
        {
            queue.begin_use();
            vaild=true;
        }
        else
        {
            vaild = queue.try_begin_use();
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
            queue.end_use();
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