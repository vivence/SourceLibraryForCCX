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
#include <atomic>
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
    
    std::mutex& queueMutex_;
    std::condition_variable& queueNotEmpty_;
    
    std::function<bool(std::unique_lock<std::mutex>&)> waitForQueueNotEmpty_;
    
    std::atomic_bool exit_;
    std::thread thread_;
    
public:
    template<typename _Queue, typename _Rep, typename _Period>
    AsyncConsumer(_Queue& queue, std::mutex& queueMutex, std::condition_variable& queueNotEmpty, const std::chrono::duration<_Rep, _Period>& queueNotEmptyWaitTimeout)
    : consumer_(queue)
    , queueMutex_(queueMutex)
    , queueNotEmpty_(queueNotEmpty)
    , waitForQueueNotEmpty_([this, queueNotEmptyWaitTimeout](std::unique_lock<std::mutex>& lock) ->bool{
        return std::cv_status::timeout != queueNotEmpty_.wait_for(lock, queueNotEmptyWaitTimeout);
    })
    , exit_(false)
    {
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
        exit_ = true;
        if (thread_.joinable())
        {
            thread_.join();
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
        if (0 >= consumer_.consume(-1))
        {
            waitForQueueNotEmpty_(lock);
        }
    }
}

GHOST_NAMESPACE_END

#endif
