//
//  GTFunctionProducer.h
//  SourceLibraryForCCX
//
//  Created by 李 侠懿 on 7/14/14.
//
//

#ifndef SourceLibraryForCCX_GTFunctionProducer_h
#define SourceLibraryForCCX_GTFunctionProducer_h

#include "GTMacros.h"
#include <functional>
#include <queue>

GHOST_NAMESPACE_BEGIN

template<typename _Product, typename _Queue>
struct FunctionProducerQueueTraits{
    typedef _Product ProductType;
    typedef _Queue QueueType;
    
    static void pushProductIntoQueue(QueueType& queue, const ProductType& product)
    {
        queue.push(product);
    }
    
    static void destroyAllProducts(QueueType& queue)
    {
        size_t productCount = queue.size();
        for (size_t i = 0; i < productCount; ++productCount)
        {
            queue.pop();
        }
    }
};

template<typename _Product = std::function<void()>, typename _Queue = std::queue<_Product>, typename _QueueTraits = FunctionProducerQueueTraits<_Product, _Queue> >
class FunctionProducer{
public:
    typedef _QueueTraits QueueTraitsType;
    typedef typename QueueTraitsType::ProductType ProductType;
    typedef typename QueueTraitsType::QueueType QueueType;
    
private:
    QueueType queue_;
    
public:
    FunctionProducer() = default;
    
public:
    QueueType& getQueue()
    {
        return queue_;
    }
    
    void produce(const ProductType& product);
    void destroyAll();
    
};

template<typename _Product, typename _Queue, typename _QueueTraits>
void FunctionProducer<_Product, _Queue, _QueueTraits>::produce(const ProductType& product)
{
    QueueTraitsType::pushProductIntoQueue(queue_, product);
}

template<typename _Product, typename _Queue, typename _QueueTraits>
void FunctionProducer<_Product, _Queue, _QueueTraits>::destroyAll()
{
    QueueTraitsType::destroyAllProducts(queue_);
}

GHOST_NAMESPACE_END

#endif
