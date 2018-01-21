
#include "message.hpp"

#include <deque>
#include <mutex>
#include <condition_variable>
#include <functional>

class MessageProcess
{
  public:
    std::deque<const Message *> que_msg;
    std::mutex mut_msg;
    std::condition_variable cv_msg;
    std::function<void(const Message *msg)> processor;

    MessageProcess(std::function<void(const Message *msg)> p);

    MessageProcess(const MessageProcess&) = delete;
    MessageProcess &operator=(const MessageProcess&) = delete;

    void put_message(const Message *msg);
    void process_message();
};
