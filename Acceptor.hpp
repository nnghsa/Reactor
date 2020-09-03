#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<errno.h>

#include<netinet/in.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<sys/epoll.h>
#include<unistd.h>
#include<sys/types.h>
#include<functional>

#define IPADDRESS "192.168.138.128"
#define MAXSIZE 1024
#define LISTENQ 10000
#define FDSIZE 1000
#define EPOLLEVENTS 1000

class EventLoop;
class Acceptor
{
public:
    
    Acceptor(int port);
    typedef std::function<void(int fd)>Functor;
    int cretesocketAndBind(int port);
    int handle_accept();

    Functor newConnection;
    int getListenfd(){return _listenfd;}
    void setListenfd(int fd){_listenfd=fd;}

private:
    int sockfd;
    int _listenfd;
};

Acceptor::Acceptor(int port):sockfd(cretesocketAndBind(port))
{
    
    //handle_accept(sockfd);
}

int Acceptor::cretesocketAndBind(int port)
{
    struct sockaddr_in addr;
    addr.sin_port=htons(port);
    addr.sin_family=AF_INET;
    addr.sin_addr.s_addr=htonl(INADDR_ANY);

    int sockfd=socket(AF_INET, SOCK_STREAM, 0);
    bind(sockfd, (struct sockaddr*)&addr, sizeof addr);
    listen(sockfd, LISTENQ);
    return sockfd;

}


int Acceptor::handle_accept()
{
    struct sockaddr_in cliaddr;
    memset(&cliaddr,0,sizeof(struct sockaddr_in));
    socklen_t cliaddrlen=sizeof (cliaddr);
    printf("%d\n",sockfd);
    int clifd=accept(sockfd, (struct sockaddr*)&cliaddr, &cliaddrlen);
    std::cout<<"connection from %s ,port "<<8877<< std::endl;
    if(clifd>=0)
    {
        setListenfd(clifd);
    }
    else
    {
        close(clifd);
    }
    return clifd;
}
