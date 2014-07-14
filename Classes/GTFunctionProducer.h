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
    void produce(const ProductType& product);
    
};

template<typename _Product, typename _Queue, typename _QueueTraits>
void FunctionProducer<_Product, _Queue, _QueueTraits>::produce(const ProductType& product)
{
    QueueTraitsType::pushProductIntoQueue(queue_, product);
}

GHOST_NAMESPACE_END

#endif
