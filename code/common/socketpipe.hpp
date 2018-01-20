
#ifndef SOCKETPIPE_HEADER
#define SOCKETPIPE_HEADER

#include <cstring>
#include "message.hpp"
#include <mutex>
#include <thread>

class Message;
class MessageProcess;

class SocketPipe
{
  public:
    int socketval;
    std::mutex m_send, m_recv;
    std::thread thread_recv;

    MessageProcess* proc;

    SocketPipe(MessageProcess* p);

    SocketPipe(const SocketPipe&) = delete;
    SocketPipe &operator=(const SocketPipe&) = delete;

    void init(bool server, const char *ip, int port, bool startrecv);
    void send(const void *buffer, size_t n);
    void recv(void *buffer, size_t n);

    static void recv_thread_static(SocketPipe* pipe);
    void recv_thread();

    Message* recvmsg();
    void sendmsg(const Message* msg);
    void sendmsg_tag(Message::Type t);
};

#endif
