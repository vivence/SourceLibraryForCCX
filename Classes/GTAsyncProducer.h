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
#include <mutex>
#include <condition_variable>
#include <functional>

GHOST_NAMESPACE_BEGIN

template <typename _Producer>
class AsyncProducer{
public:
    typedef _Producer ProducerType;
    typedef typename ProducerType::ProductType ProductType;
    typedef typename ProducerType::QueueType QueueType;
    
private:
    ProducerType producer_;
    
    std::mutex queueMutex_;
    std::condition_variable queueNotEmpty_;
    
public:
    AsyncProducer() = default;
    
    AsyncProducer(const AsyncProducer&) = delete;
    AsyncProducer& operator =(const AsyncProducer&) = delete;
    
public:
    void produce(const ProductType& product);
    
};

template <typename _Producer>
void AsyncConsumer<_Producer>::produce(const ProductType& product)
{
    std::unique_lock<std::mutex> lock(queueMutex_);
    producer_.produce(product);
    queueNotEmpty_.notify_one();
}

GHOST_NAMESPACE_END

#endif
