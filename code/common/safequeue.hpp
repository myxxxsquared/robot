
#ifndef SAFEQUEUE_HEADER
#define SAFEQUEUE_HEADER

#include <deque>
#include <mutex>
#include <condition_variable>

template<typename T>
class safe_queue
{
public:
    std::deque<T> queue;
    std::mutex mutex;
    std::condition_variable cv;

    safe_queue()
    {
    }

    ~safe_queue()
    {
    }

    safe_queue(const safe_queue<T>&) = delete;
    safe_queue<T>& operator=(const safe_queue<T>&) = delete;

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
class safe_queue_use
{
public:
    safe_queue<T> &queue;
    bool vaild;

    std::unique_lock<std::mutex> mutex;

    safe_queue_use(safe_queue<T>& q, bool tryget = false)
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

    ~safe_queue_use()
    {
        release();
    }

    void release()
    {
        if(vaild)
        {
            mutex.release();
            queue.queue.pop_front();
        }
        vaild=false;
    }

    safe_queue_use(const safe_queue_use<T>&) = delete;
    safe_queue_use<T>& operator=(const safe_queue_use<T>&) = delete;

    T& operator*()
    {
        return queue.queue.front();
    }
};

#endif