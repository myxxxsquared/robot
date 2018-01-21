
#ifndef SOCKETPIPE_HEADER
#define SOCKETPIPE_HEADER

#include <cstring>
#include "message.hpp"
#include <mutex>
#include <thread>
#include <deque>
#include <condition_variable>

class Message;
class MessageProcess;

class SocketPipe
{
  public:
    int socketval;

    std::mutex m_recv;
    std::thread thread_recv;

    std::thread thread_send;
    std::deque<const Message *> queue_send;
    std::mutex mutex_send;
    std::condition_variable cv_send;

    MessageProcess* proc;

    SocketPipe(MessageProcess* p);

    SocketPipe(const SocketPipe&) = delete;
    SocketPipe &operator=(const SocketPipe&) = delete;

    void init(bool server, const char *ip, int port);
    void send(const void *buffer, size_t n);
    void recv(void *buffer, size_t n);

    static void recv_thread_static(SocketPipe* pipe);
    void recv_thread();
    static void send_thread_static(SocketPipe* pipe);
    void send_thread();

    Message* recvmsg();
    void sendmsg(const Message* msg);
    void sendmsg_sync(const Message* msg);
    void sendmsg_tag(Message::Type t);
};

#endif
