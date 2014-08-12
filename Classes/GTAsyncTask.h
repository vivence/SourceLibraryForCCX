//
//  GTAsyncTask.h
//  SourceLibraryForCCX
//
//  Created by 李 侠懿 on 7/15/14.
//
//

#ifndef __SourceLibraryForCCX__GTAsyncTask__
#define __SourceLibraryForCCX__GTAsyncTask__

#include "GTMacros.h"
#include <functional>
#include <queue>
#include <memory>
#include <atomic>

GHOST_NAMESPACE_BEGIN

class AsyncTask{
public:
    template<typename _Product = std::shared_ptr<AsyncTask>, typename _Queue = std::queue<_Product> >
    struct ProducerQueueTraits{
        typedef _Product ProductType;
        typedef _Queue QueueType;
        
        static void pushProductIntoQueue(QueueType& queue, const ProductType& product)
        {
            queue.push(product);
        }
        
        static void destroyAllProducts(QueueType& queue)
        {
            size_t productCount = queue.size();
            for (size_t i = 0; i < productCount; ++i)
            {
                queue.front()->cancelInCocosThread();
                queue.pop();
            }
        }
    };
    
    template<typename _Product = std::shared_ptr<AsyncTask>, typename _Queue = std::queue<_Product> >
    struct ConsumerQueueTraits{
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
            product->consumeInBackgroundThread();
        }
    };
    
private:
    std::function<void()> doConsume_;
    std::function<void()> doCancel_;
    std::atomic_flag invalidate_ = ATOMIC_FLAG_INIT;
    
public:
    AsyncTask(const std::function<void()>& doConsumeInBackgroundThread, const std::function<void()>& doCancelInCocosThread)
    : doConsume_(doConsumeInBackgroundThread)
    , doCancel_(doCancelInCocosThread)
    {
        
    }
    
public:
    void consumeInBackgroundThread()
    {
        if (!invalidate_.test_and_set())
        {
            doConsume_();
        }
    }
    void cancelInCocosThread()
    {
        if (!invalidate_.test_and_set())
        {
            doCancel_();
        }
    }
    void reset()
    {
        invalidate_.clear();
    }
};

GHOST_NAMESPACE_END

#endif /* defined(__SourceLibraryForCCX__GTAsyncTask__) */
