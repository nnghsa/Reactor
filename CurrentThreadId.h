
#ifndef _MHG_CURRENT_THREAD_ID_H_
#define _MHG_CURRENT_THREAD_ID_H_


#include<string>
#include<iostream>
#include<sys/syscall.h>
#include<unistd.h>
class CurrentThreadId
{
public:
    static int  tid()
    {
        int tid=syscall(SYS_gettid);
        return tid;
    }

};


#endif
