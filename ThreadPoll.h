#pragma once
#include"EventHandler.h"
#include<pthread.h>
#include<functional>
#include<memory>
#include<vector>


const int THREADPOOL_INVALID = -1;
const int THREADPOOL_LOCK_FAILURE = -2;
const int THREADPOOL_QUEUE_FULL = -3;
const int THREADPOOL_SHUTDOWN = -4;
const int THREADPOOL_THREAD_FAILURE = -5;
const int THREADPOOL_GRACEFUL = 1;

const int MAX_THREADS = 1024;
const int MAX_QUEUE = 65535;

typedef enum
{
    immediate_shutdown = 1,
    graceful_shtudown = 2
}ShutDownOption;

struct ThreadPoolTask
{
    std::function<void(std::shared_ptr<void>)>func;
    std::shared_ptr<void>args;
};

class ThreadPool
{
private:
    static pthread_mutex_t _lock;
    static pthread_cond_t _notify;

    static std::vector<pthread_t> _threads;
    static std::vector<ThreadPoolTask>_queue;
    static int _thread_count;
    static int _queue_size;
    static int _head;

    static int _tail;
    static int _count;
    static int _shutdown;
    static int _started;
public:
    static int threadpool_create(int thread_count, int queue_size);
    static int threadpool_free();
    static int threadpool_add(std::shared_ptr<void>args, std::function<void(std::shared_ptr<void>)>fun);
    static int threadpool_destroy(ShutDownOption shut_option = graceful_shtudown);
    static void* threadpool_thread(void* args);
};