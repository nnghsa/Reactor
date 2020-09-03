

#include<string.h>
#include<sys/timerfd.h>
#include"EventLoop.h"
#include"EventHandler.h"
#include<iostream>
#include<unistd.h>
#include"Acceptor.hpp"
#include"Server.hpp"
void testFunc()
{
    printf("this is a test function\n");
}
// int  accpett(int port)
// {
//     int listenfd,connfd;
//     socklen_t len;
//     struct sockaddr_in servaddr,cliaddr;
//     char    buff[1024];

//     listenfd=socket(AF_INET,SOCK_STREAM,0);
//     memset(&servaddr,0,sizeof servaddr);
//     servaddr.sin_family=AF_INET;
//     servaddr.sin_addr.s_addr=htonl(INADDR_ANY);
//     servaddr.sin_port=htons(port);
//     bind(listenfd,(struct sockaddr*)&servaddr,sizeof servaddr);
//     listen(listenfd,128);
//     connfd=accept(listenfd,(struct sockaddr*)&cliaddr,&len);
//     return connfd;
// }
int main()
{

    //int timerfd=::timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK |TFD_CLOEXEC);
    //  Acceptor a(8877);
    //  int timerfd=a.handle_accept();
    //  //int timerfd=accpett(8877);
    // EventLoop loop;
    // EventLoop*g_loop=&loop;

    // EventHandler eh(timerfd);
    // eh.setReadCallBack(testFunc);

    // struct itimerspec howlong;
    // bzero(&howlong, sizeof(howlong));
    // howlong.it_value.tv_sec=3;

    // loop.loop();

    // ::close(timerfd);
    // return 0;
    EventLoop mainLoop;
    Server mysever(&mainLoop,3,8787);
    mysever.start();
    mainLoop.loop();
    return 0;
}