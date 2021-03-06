//
//  GTConsumer.h
//  SourceLibraryForCCX
//
//  Created by 李 侠懿 on 7/14/14.
//
//

#ifndef __SourceLibraryForCCX__GTConsumer__
#define __SourceLibraryForCCX__GTConsumer__

#include "GTMacros.h"
#include <functional>
#include <queue>

GHOST_NAMESPACE_BEGIN

template<typename _Product, typename _Queue>
struct FunctionConsumerQueueTraits{
    typedef _Product ProductType;
    typedef _Queue QueueType;
    
    static bool queueEmpty(const QueueType& queue)
    {
        return queue.empty();
    }
    
    static ProductType& getProductFromQueue(QueueType& queue)
    {
        return queue.front();
    }
    
    static void popProductFromQueue(QueueType& queue)
    {
        queue.pop();
    }
    
    static void consumeProduct(ProductType& product)
    {
        product();
    }
};

template<typename _Product = std::function<void()>, typename _Queue = std::queue<_Product>, typename _QueueTraits = FunctionConsumerQueueTraits<_Product, _Queue> >
class Consumer{
public:
    typedef _QueueTraits QueueTraitsType;
    typedef typename QueueTraitsType::ProductType ProductType;
    typedef typename QueueTraitsType::QueueType QueueType;
    
private:
    QueueType& queue_;
    
public:
    explicit Consumer(QueueType& queue)
    : queue_(queue)
    {
        
    }
    
public:
    // return consumed count
    int consume(int count = 1); // as many as possible if 0 >= count
    
};

template<typename _QueueTraits = FunctionConsumerQueueTraits<std::function<void()>, std::queue<std::function<void()> > > >
using ConsumerByTraits = Consumer<typename _QueueTraits::ProductType, typename _QueueTraits::QueueType, _QueueTraits>;

template<typename _Product, typename _Queue, typename _QueueTraits>
int Consumer<_Product, _Queue, _QueueTraits>::consume(int count/* = 1*/)
{
    int consumedCount = 0;
    if (0 < count)
    {
        for (int i = 0; i < count; ++i)
        {
            if (QueueTraitsType::queueEmpty(queue_))
            {
                break;
            }
            QueueTraitsType::consumeProduct(QueueTraitsType::getProductFromQueue(queue_));
            QueueTraitsType::popProductFromQueue(queue_);
            ++consumedCount;
        }
    }
    else
    {
        while (!QueueTraitsType::queueEmpty(queue_))
        {
            QueueTraitsType::consumeProduct(QueueTraitsType::getProductFromQueue(queue_));
            QueueTraitsType::popProductFromQueue(queue_);
            ++consumedCount;
        }
    }
    return consumedCount;
}

GHOST_NAMESPACE_END

#endif /* defined(__SourceLibraryForCCX__GTConsumer__) */
