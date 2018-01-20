
#include "message.hpp"

#include <deque>
#include <mutex>
#include <condition_variable>

class MessageProcess
{
  public:
    std::deque<const Message *> que_msg;
    std::mutex mut_msg;
    std::condition_variable cv_msg;

    void put_message(const Message *msg);
    void process_message();
};
