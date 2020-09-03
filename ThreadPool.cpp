#include"ThreadPoll.h"
#include<iostream>
pthread_mutex_t ThreadPool::_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t ThreadPool::_notify = PTHREAD_COND_INITIALIZER;

std::vector<pthread_t>ThreadPool::_threads;
std::vector<ThreadPoolTask>ThreadPool::_queue;

int ThreadPool::_thread_count = 0;
int ThreadPool::_queue_size = 0;
int ThreadPool::_head = 0;
int ThreadPool::_tail = 0;
int ThreadPool::_count = 0;
int ThreadPool::_shutdown = 0;
int ThreadPool::_started = 0;


int ThreadPool::threadpool_create(int thread_count, int queue_size)
{
    //bool err=false;
    do
    {
        if (thread_count <= 0 || thread_count > MAX_THREADS ||
            queue_size <= 0 || queue_size > MAX_QUEUE)
        {
            thread_count = 4;
            queue_size = 1024;
        }
        _thread_count = 0;
        _queue_size = queue_size;
        _head = _tail = _count = 0;
        _shutdown = _started = 0;

        _threads.resize(thread_count);
        _queue.resize(queue_size);

        for (int i = 0;i < thread_count;++i)
        {
            if (pthread_create(&_threads[i], NULL, threadpool_thread, NULL) != 0)
            {
                return -1;
            }
            ++_thread_count;
            ++_started;
        }
    } while (false);

    // if(err)
    // {
    //     return -1;
    // }
    return 0;
}
int ThreadPool::threadpool_add(std::shared_ptr<void>args,
    std::function<void(std::shared_ptr<void>)>func)
{
    int next, err = 0;
    if (pthread_mutex_lock(&_lock) != 0)
        return THREADPOOL_LOCK_FAILURE;

    do {
        next = (_tail + 1) % _queue_size;
        if (_count == _queue_size)
        {
            err = THREADPOOL_QUEUE_FULL;
            break;
        }
        if (_shutdown)
        {
            err = THREADPOOL_SHUTDOWN;
            break;
        }
        _queue[_tail].func = func;
        _queue[_tail].args = args;
        _tail = next;
        ++_count;

        if (pthread_cond_signal(&_notify) != 0)
        {
            err = THREADPOOL_LOCK_FAILURE;
            break;
        }
    } while (false);
    if (pthread_mutex_unlock(&_lock) != 0)
        err = THREADPOOL_LOCK_FAILURE;
    return err;
}

int ThreadPool::threadpool_destroy(ShutDownOption shutdown_option)
{
    printf("Thread pool destroy !\n");
    int i=0,err=0;

    if(pthread_mutex_lock(&_lock)!=0)
        return THREADPOOL_LOCK_FAILURE;
    do
    {
        if(_shutdown)
        {
            err=THREADPOOL_SHUTDOWN;
            break;
        }
        _shutdown=shutdown_option;

        if((pthread_cond_broadcast(&_notify)!=0)||(pthread_mutex_unlock(&_lock)!=0))
        {
            err=THREADPOOL_GRACEFUL;
            break;
        }
        for(i=0;i<_thread_count;++i)
        {
            if(pthread_join(_threads[i],NULL)!=0)
                err=THREADPOOL_THREAD_FAILURE;          
        }
    } while (false);
    if(!err)
        threadpool_free();
    return err;
    
}

int ThreadPool::threadpool_free()
{
    if(_started>0)
        return -1;
    //pthread_mutex_lock(&_lock);
    pthread_mutex_destroy(&_lock);
    pthread_cond_destroy(&_notify);
    return 0;
}

void *ThreadPool::threadpool_thread(void *args)
{
    while(true)
    {
        ThreadPoolTask task;
        pthread_mutex_lock(&_lock);
        while((_count==0)&&(!_shutdown))
        {
            pthread_cond_wait(&_notify,&_lock);
        }
        if((_shutdown==immediate_shutdown)||(_shutdown==graceful_shtudown)
        &&(_count==0))
        {
            break;
        }

        task.func=_queue[_head].func;
        task.args=_queue[_head].args;
        _queue[_head].func=NULL;
        _queue[_head].args.reset();
        _head=(_head+1)%_queue_size;
        --_count;
        pthread_mutex_unlock(&_lock);
        (task.func)(task.args);
    }
    
    --_started;
    pthread_mutex_unlock(&_lock);
    printf("This threadpool thread finishs!\n");
    pthread_exit(0);
    return NULL;
}