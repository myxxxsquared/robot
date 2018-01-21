
#include "socketpipe.hpp"
#include "message.hpp"
#include "messageprocess.hpp"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <stdexcept>
#include <memory>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

SocketPipe::SocketPipe(MessageProcess* p)
    : proc(p)
{
}

void SocketPipe::init(bool server, const char *ip, int port, bool startrecv)
{
    struct sockaddr_in addr_self, addr_other;

    if (server)
    {
        int server_fd;
        if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
            throw std::runtime_error("socket failed");
        int opt = 1;
#ifndef __CYGWIN__
        if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
            throw std::runtime_error("setsockopt failed");
#endif

        addr_self.sin_family = AF_INET;
        addr_self.sin_addr.s_addr = INADDR_ANY;
        addr_self.sin_port = htons(port);

        if (bind(server_fd, (struct sockaddr *)&addr_self, sizeof(addr_self)) < 0)
            throw std::runtime_error("bind failed");

        if (listen(server_fd, 3) < 0)
            throw std::runtime_error("listen failed");

        int addrlen = sizeof(struct sockaddr_in);
        if ((socketval = accept(server_fd, (struct sockaddr *)&addr_other, (socklen_t *)&addrlen)) < 0)
            throw std::runtime_error("accept failed");
    }
    else
    {
        if ((socketval = socket(AF_INET, SOCK_STREAM, 0)) < 0)
            throw std::runtime_error("socket failed");

        memset(&addr_other, '0', sizeof(addr_other));
        addr_other.sin_family = AF_INET;
        addr_other.sin_port = htons(port);

        if (inet_pton(AF_INET, ip, &addr_other.sin_addr) <= 0)
            throw std::runtime_error("inet_pton failed");

        if (connect(socketval, (struct sockaddr *)&addr_other, sizeof(addr_other)) < 0)
            throw std::runtime_error("connect failed");
    }

    if(startrecv)
    {
        std::thread th{SocketPipe::recv_thread_static, this};
        this->thread_recv = std::move(th);
    }
}

void SocketPipe::send(const void *buffer, size_t n)
{
    ssize_t result = ::send(socketval, buffer, n, 0);
    if(result == -1)
    {
        char buffer[64];
        sprintf(buffer, "send failed, %d", (int)errno);
        throw std::runtime_error(buffer);
    }
    else if((intptr_t)result != (intptr_t)n)
    {
        throw std::runtime_error("send length error!");
    }
}

void SocketPipe::recv(void *buffer, size_t n)
{
    ssize_t result = ::recv(socketval, buffer, n, 0);
    if(result == -1)
    {
        char buffer[64];
        sprintf(buffer, "recv failed, %d", (int)errno);
        throw std::runtime_error(buffer);
    }
    else if((intptr_t)result != (intptr_t)n)
    {
        throw std::runtime_error("recv length error!");
    }
}

Message* SocketPipe::recvmsg()
{
    std::unique_lock<std::mutex> lock{m_recv};
    Message::Type t;
    recv(&t, sizeof(t));
    Message* msg = Message::construct(t);
    msg->recvfrom(*this);
    return msg;
}

void SocketPipe::sendmsg(const Message* msg)
{
    std::unique_lock<std::mutex> lock{m_send};
    send(&msg->type, sizeof(msg->type));
    msg->sendto(*this);
}

void SocketPipe::sendmsg_tag(Message::Type t)
{
    std::unique_ptr<Message> msg{Message::construct(t)};
    sendmsg(msg.get());
}

void SocketPipe::recv_thread_static(SocketPipe* pipe)
{
    pipe->recv_thread();
}

void SocketPipe::recv_thread()
{
    while(true)
    {
        Message *msg = recvmsg();
        proc->put_message(msg);
    }
}
