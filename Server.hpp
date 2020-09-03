#pragma once
#include"EventLoop.h"
#include"EventLoopThreadPool.hpp"
#include<memory>
#include<netinet/in.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<sys/epoll.h>
#include<unistd.h>
#include<sys/types.h>
#include<fcntl.h>
#include<string.h>
#include"EventHandler.h"
#include<signal.h>
class Server
{
private:
    EventLoop* _loop;
    int _threadNum;
    std::unique_ptr<EventLoopThreadPool>p_eventLoopThreadPool;
    bool _started;
    std::shared_ptr<EventHandler>sp_acceptEventHandler;
    int _port;
    int _listenFd;
    static const int MAXFDS = 100000;
public:
    Server(EventLoop* loop, int threadNum, int port);
    ~Server();
    EventLoop* getLoop()const { return _loop; }
    void start();
    void handNewConn();
    void handThisConn() { _loop->updateEpoller(sp_acceptEventHandler); }
};



int cretesocketAndBind(int port)
{
    struct sockaddr_in addr;
    addr.sin_port = htons(port);
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    bind(sockfd, (struct sockaddr*)&addr, sizeof addr);
    listen(sockfd, 2048);
    return sockfd;
}
int setSocketNonBlocking(int fd) {
    int flag = fcntl(fd, F_GETFL, 0);
    if (flag == -1) return -1;

    flag |= O_NONBLOCK;
    if (fcntl(fd, F_SETFL, flag) == -1) return -1;
    return 0;
}
void handle_for_sigpipe() {
    struct sigaction sa;
    memset(&sa, '\0', sizeof(sa));
    sa.sa_handler = SIG_IGN;
    sa.sa_flags = 0;
    if (sigaction(SIGPIPE, &sa, NULL)) return;
}
Server::Server(EventLoop* loop, int threadNum, int port) :
    _loop(loop),
    _threadNum(threadNum),
    p_eventLoopThreadPool(new EventLoopThreadPool(_loop, threadNum)),
    _started(false),
    sp_acceptEventHandler(new EventHandler(_loop)),
    _port(port),
    _listenFd(cretesocketAndBind(port))
{
    sp_acceptEventHandler->setFd(_listenFd);
    handle_for_sigpipe();
    if (setSocketNonBlocking(_listenFd) < 0)
    {
        printf("set socket non block failed\n");
        abort();
    }
}

Server::~Server()
{
}
void Server::start()
{
    p_eventLoopThreadPool->start();
    sp_acceptEventHandler->setEvents(EPOLLIN | EPOLLET);
    sp_acceptEventHandler->setReadCallBack(std::bind(&Server::handNewConn, this));
    sp_acceptEventHandler->setConnHandler(std::bind(&Server::handThisConn, this));
    _loop->addToPoller(sp_acceptEventHandler, 0);
    _started = true;
}
void show()
{
    printf("new thread and eventloop create\n");
}
void Server::handNewConn()
{
    struct sockaddr_in client_addr;
    memset(&client_addr, 0, sizeof client_addr);
    socklen_t client_addr_len = sizeof client_addr;
    int accept_fd = 0;
    while ((accept_fd = accept(_listenFd, (struct sockaddr*)&client_addr, &client_addr_len)) > 0)
    {
        EventLoop* loop = p_eventLoopThreadPool->getNextLoop();
        std::cout << "New connection from " << inet_ntoa(client_addr.sin_addr) << " : "
            << ntohs(client_addr.sin_port) << std::endl;
        if (accept_fd >= MAXFDS)
        {
            close(accept_fd);
            continue;
        }
        if (setSocketNonBlocking(accept_fd) < 0)
        {
            std::cout << "Set non block failed" << std::endl;
            abort();
        }
        std::shared_ptr< EventHandler>eh(new EventHandler(loop,accept_fd));
        eh->setReadCallBack(std::bind(&EventHandler::echofunc,eh));
        eh->setEvents(EPOLLIN|EPOLLPRI);
        loop->addToPoller(eh,2000);
        loop->queueInLoop(show);
    }
    sp_acceptEventHandler->setEvents(EPOLLIN | EPOLLET);

}