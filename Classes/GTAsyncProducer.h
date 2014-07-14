//
//  GTAsyncProducer.h
//  SourceLibraryForCCX
//
//  Created by 李 侠懿 on 7/14/14.
//
//

#ifndef SourceLibraryForCCX_GTAsyncProducer_h
#define SourceLibraryForCCX_GTAsyncProducer_h

#include "GTMacros.h"
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <cassert>

GHOST_NAMESPACE_BEGIN

/*
 * NOTICES!!!!!
 * before destry producer
 * 1 call producer.destroyAll()
 * 2 wait for consumers exiting
 * 3 destroy producer
 */
template <typename _Producer>
class AsyncProducer{
public:
    typedef _Producer ProducerType;
    typedef typename ProducerType::ProductType ProductType;
    typedef typename ProducerType::QueueType QueueType;
    
private:
    ProducerType producer_;
    
    bool queueDestroyed_;
    std::mutex queueMutex_;
    std::condition_variable queueNotEmpty_;
    
public:
    AsyncProducer()
    : queueDestroyed_(false)
    {
        
    }
    ~AsyncProducer();
    
    AsyncProducer(const AsyncProducer&) = delete;
    AsyncProducer& operator =(const AsyncProducer&) = delete;
    
public:
    QueueType& getQueue()
    {
        return producer_.getQueue();
    }
    bool& getQueueDestroyed()
    {
        return queueDestroyed_;
    }
    std::mutex& getQueueMutex()
    {
        return queueMutex_;
    }
    std::condition_variable& getQueueNotEmpty()
    {
        return queueNotEmpty_;
    }
    
    void produce(const ProductType& product);
    void destroyAll();
};

template <typename _Producer>
AsyncProducer<_Producer>::~AsyncProducer()
{
    std::unique_lock<std::mutex> lock(queueMutex_);
    if (!queueDestroyed_)
    {
        assert(false); // Checkout the NOTICES above the class
    }
    lock.unlock();
}

template <typename _Producer>
void AsyncProducer<_Producer>::produce(const ProductType& product)
{
    std::unique_lock<std::mutex> lock(queueMutex_);
    producer_.produce(product);
    queueDestroyed_ = false;
    queueNotEmpty_.notify_one();
}

template <typename _Producer>
void AsyncProducer<_Producer>::destroyAll()
{
    // destroy all
    std::unique_lock<std::mutex> lock(queueMutex_);
    producer_.destroyAll();
    
    // set flag
    queueDestroyed_ = true;
    
    // notify consumers
    queueNotEmpty_.notify_all();
}

GHOST_NAMESPACE_END

#endif
