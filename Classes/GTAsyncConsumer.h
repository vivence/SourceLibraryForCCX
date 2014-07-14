//
//  GTAsyncConsumer.h
//  SourceLibraryForCCX
//
//  Created by 李 侠懿 on 7/14/14.
//
//

#ifndef SourceLibraryForCCX_GTAsyncConsumer_h
#define SourceLibraryForCCX_GTAsyncConsumer_h

#include "GTMacros.h"
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>

GHOST_NAMESPACE_BEGIN

template <typename _Consumer>
class AsyncConsumer{
public:
    typedef _Consumer ConsumerType;
    typedef typename ConsumerType::ProductType ProductType;
    typedef typename ConsumerType::QueueType QueueType;
    
private:
    ConsumerType consumer_;
    
    bool& queueDestroyed_;
    std::mutex& queueMutex_;
    std::condition_variable& queueNotEmpty_;
    
    std::function<bool(std::unique_lock<std::mutex>&)> waitForQueueNotEmpty_;
    
    std::atomic_bool exit_;
    std::thread thread_;
    
public:
    template<typename _Queue, typename _Rep, typename _Period>
    AsyncConsumer(_Queue& queue, bool& queueDestroyed, std::mutex& queueMutex, std::condition_variable& queueNotEmpty, const std::chrono::duration<_Rep, _Period>& queueNotEmptyWaitTimeout)
    : consumer_(queue)
    , queueDestroyed_(queueDestroyed)
    , queueMutex_(queueMutex)
    , queueNotEmpty_(queueNotEmpty)
    , exit_(false)
    {
        if (std::chrono::duration<_Rep, _Period>::zero().count() < queueNotEmptyWaitTimeout.count())
        {
            waitForQueueNotEmpty_ = [this, queueNotEmptyWaitTimeout](std::unique_lock<std::mutex>& lock) ->bool{
                return std::cv_status::timeout != queueNotEmpty_.wait_for(lock, queueNotEmptyWaitTimeout);
            };
        }
        else
        {
            waitForQueueNotEmpty_ = [this](std::unique_lock<std::mutex>& lock) ->bool{
                queueNotEmpty_.wait(lock);
                return true;
            };
        }
        // start work
        thread_ = std::thread(std::bind(std::mem_fn(&AsyncConsumer::workProc), this));
    }
    ~AsyncConsumer();
    
    AsyncConsumer(const AsyncConsumer&) = delete;
    AsyncConsumer& operator =(const AsyncConsumer&) = delete;
    
public:
    void workProc();
    
};

template <typename _Consumer>
AsyncConsumer<_Consumer>::~AsyncConsumer()
{
    try
    {
        if (!exit_.exchange(true))
        {
            if (thread_.joinable())
            {
                thread_.join();
            }
        }
    }
    catch (...)
    {
        
    }
}

template <typename _Consumer>
void AsyncConsumer<_Consumer>::workProc()
{
    while (!exit_)
    {
        std::unique_lock<std::mutex> lock(queueMutex_);
        if (queueDestroyed_)
        {
            exit_ = true;
            break;
        }
        if (0 >= consumer_.consume(-1))
        {
            if (waitForQueueNotEmpty_(lock))
            {
                consumer_.consume(-1);
            }
        }
    }
}

GHOST_NAMESPACE_END

#endif
