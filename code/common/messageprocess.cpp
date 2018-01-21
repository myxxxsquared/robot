

#include "messageprocess.hpp"

MessageProcess::MessageProcess(std::function<void(const Message *msg)> p)
    : processor(p)
{
}

void MessageProcess::put_message(const Message *msg)
{
    std::unique_lock<std::mutex> lock(mut_msg);
    que_msg.push_back(msg);
    cv_msg.notify_one();
}

void MessageProcess::process_message()
{
    bool ctn = true;

    while (ctn)
    {
        const Message *msg;

        {
            std::unique_lock<std::mutex> lock(mut_msg);
            cv_msg.wait(lock, [&]() { return !que_msg.empty(); });
            msg = que_msg.front();
            que_msg.pop_front();
        }

        processor(msg);

        delete msg;
    }
}
