//
//  GTHttpClient.cpp
//  SourceLibraryForCCX
//
//  Created by 李 侠懿 on 7/16/14.
//
//

#include "GTHttpClient.h"
#include "GTCurlSession.h"
#include <mutex>

namespace {
    
    static std::mutex s_mutexCurlGlobal;
    static size_t s_clientCount;
    static ghost::CurlSession::Global* s_pCurlGlobal = 0;
    
    class HttpClientThreadLocalKeyManager{
        pthread_key_t key_;
        
    public:
        HttpClientThreadLocalKeyManager()
        {
            pthread_key_create(&key_, HttpClientDeleter);
        }
        
        ~HttpClientThreadLocalKeyManager()
        {
            pthread_key_delete(key_);
        }
        
        static void HttpClientDeleter(void* p)
        {
            if (p)
            {
                delete (ghost::HttpClient*)p;
            }
        }
        
    public:
        pthread_key_t& getKey()
        {
            return key_;
        }
    };
    static HttpClientThreadLocalKeyManager s_httpClientThreadLocalKeyManager;
    
}

GHOST_NAMESPACE_BEGIN

HttpClient::HttpClient()
{
    std::lock_guard<std::mutex> lock(s_mutexCurlGlobal);
    ++s_clientCount;
    if (!s_pCurlGlobal)
    {
        s_pCurlGlobal = new ghost::CurlSession::Global();
    }
}

HttpClient::~HttpClient()
{
    try
    {
        std::lock_guard<std::mutex> lock(s_mutexCurlGlobal);
        if (0 == --s_clientCount)
        {
            if (s_pCurlGlobal)
            {
                delete s_pCurlGlobal;
            }
        }
    }
    catch (...)
    {
        
    }
}

//HttpClient& HttpClient::getThreadLocalInstance()
//{
//    thread_local/*not support*/ HttpClient instance;
//    return instance;
//}

HttpClient& HttpClient::getThreadLocalInstance()
{
    auto p = pthread_getspecific(s_httpClientThreadLocalKeyManager.getKey());
    if (!p)
    {
        p = new HttpClient();
        pthread_setspecific(s_httpClientThreadLocalKeyManager.getKey(), p);
    }
    return *(HttpClient*)p;
}

GHOST_NAMESPACE_END