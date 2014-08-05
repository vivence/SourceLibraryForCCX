//
//  GTHttpClient.cpp
//  SourceLibraryForCCX
//
//  Created by 李 侠懿 on 7/16/14.
//
//

#include "GTHttpClient.h"
#include "GTCurlSession.h"
#include "GTHttpRequest.h"
#include "GTHttpResponse.h"
#include <pthread.h>
#include <mutex>
#include <fstream>
#include <sstream>

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
    
    template<typename _OStream>
    class StreamWriter{
    public:
        typedef _OStream OStreamType;
        
    private:
        OStreamType ostream_;
        
    public:
        template<typename... _Args>
        StreamWriter(_Args... args)
        : ostream_(args...)
        {
            
        }
        ~StreamWriter()
        {
            ostream_.close();
        }
        
        StreamWriter(const StreamWriter&) = delete;
        StreamWriter& operator =(const StreamWriter&) = delete;
        
    public:
        OStreamType& getStream()
        {
            return ostream_;
        }
        
        bool valid() const
        {
            return !ostream_.fail();
        }
        
        bool writeData(const char* pData, size_t size)
        {
            ostream_.write(pData, size);
            return valid();
        }
        
    public:
        static size_t writeCallback(char* ptr, size_t size, size_t nmemb, void* userdata)
        {
            auto sizes = size*nmemb;
            StreamWriter* pThis = (StreamWriter*)userdata;
            if (pThis)
            {
                pThis->writeData(ptr, sizes);
            }
            return sizes;
        }
    };
    
    template<>
    StreamWriter<std::stringstream>::~StreamWriter()
    {
        
    }
    
    typedef StreamWriter<std::ofstream> FileStreamWriter;
    typedef StreamWriter<std::stringstream> StringStreamWriter;
 
    class ProgressCallback{
        const ghost::HttpRequest& request_;
        const ghost::HttpRequest::ProgressCallbackFunction& callback_;
        
    public:
        ProgressCallback(const ghost::HttpRequest& request, const ghost::HttpRequest::ProgressCallbackFunction& callback)
        : request_(request)
        , callback_(callback)
        {
            
        }
        
        int callback(double dltotal, double dlnow, double ultotal, double ulnow)
        {
            callback_(request_, dlnow, dltotal, ulnow, ultotal);
            return CURLE_OK;
        }
        
    public:
        static int progressCallback(void* clientp, double dltotal, double dlnow, double ultotal, double ulnow)
        {
            ProgressCallback* pThis = (ProgressCallback*)clientp;
            if (pThis)
            {
                return pThis->callback(dltotal, dlnow, ultotal, ulnow);
            }
            return CURLE_OK;
        }
    };
    
    template<typename _DataWriter, typename _HeaderWriter>
    void initCurlSession(ghost::CurlSession& session, const ghost::HttpRequest& request, char* errorBuffer, _DataWriter& dataWriter, _HeaderWriter& headerWriter, ProgressCallback* pProgressCallback)
    {
        session.easySetOpt<CURLOPT_ERRORBUFFER>(errorBuffer);
        session.easySetOpt<CURLOPT_TIMEOUT>(request.transferTimeoutSeconds);
        session.easySetOpt<CURLOPT_CONNECTTIMEOUT>(request.connectTimeoutSeconds);
        session.easySetOpt<CURLOPT_SSL_VERIFYPEER>(request.sslVerifyPeer);
        session.easySetOpt<CURLOPT_SSL_VERIFYHOST>(request.sslVerifyHost);
        session.easySetOpt<CURLOPT_NOSIGNAL>(request.nosignal);
        
        session.easySetOpt<CURLOPT_URL>(request.url);
        session.easySetOpt<CURLOPT_USERAGENT>(request.userAgent);
        if (!request.headers.empty())
        {
            ghost::CurlStringList headerList;
            for (auto& header : request.headers)
            {
                headerList.append(header);
            }
            session.easySetOpt<CURLOPT_HTTPHEADER>(headerList.get());
        }
        session.easySetOpt<CURLOPT_COOKIE>(request.cookie);
        session.easySetOpt<CURLOPT_COOKIEFILE>(request.cookieFileReadFrom);
        session.easySetOpt<CURLOPT_COOKIEJAR>(request.cookieFileStoreTo);
        
        session.easySetOpt<CURLOPT_WRITEFUNCTION>(&FileStreamWriter::writeCallback);
        session.easySetOpt<CURLOPT_WRITEDATA>(&dataWriter);
        
        session.easySetOpt<CURLOPT_HEADERFUNCTION>(&StringStreamWriter::writeCallback);
        session.easySetOpt<CURLOPT_HEADERDATA>(&headerWriter);
        
        if (request.progressCallback && pProgressCallback)
        {
            session.easySetOpt<CURLOPT_NOPROGRESS>(false);
            session.easySetOpt<CURLOPT_PROGRESSFUNCTION>(&ProgressCallback::progressCallback);
            session.easySetOpt<CURLOPT_PROGRESSDATA>(pProgressCallback);
        }
    }
    
}

GHOST_NAMESPACE_BEGIN

HttpClient::HttpClient()
: errorBuffer_(CURL_ERROR_SIZE)
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

HttpClient::ErrorCode HttpClient::get(const HttpRequest& request, HttpResponse& response)
{
    StringStreamWriter dataWriter;
    StringStreamWriter headerWriter;
    std::shared_ptr<ProgressCallback> pProgressCallback;
    if (request.progressCallback)
    {
        pProgressCallback.reset(new ProgressCallback(request, request.progressCallback));
    }
    
    CurlSession session;
    initCurlSession(session, request, &errorBuffer_[0], dataWriter, headerWriter, pProgressCallback.get());
    session.easySetOpt<CURLOPT_FOLLOWLOCATION>(true);
    
    CURLcode errorCode = CURLE_OK;
    std::tie(errorCode, response.code) = session.perform();
    if (!CurlSession::codeOK(errorCode))
    {
        return E_PERFORM;
    }
    
    response.header = std::move(headerWriter.getStream().str());
    response.data = std::move(dataWriter.getStream().str());
    
    return E_OK;
}


HttpClient::ErrorCode HttpClient::post(const HttpRequest& request, HttpResponse& response, const char* pData, size_t dataSize)
{
    StringStreamWriter dataWriter;
    StringStreamWriter headerWriter;
    std::shared_ptr<ProgressCallback> pProgressCallback;
    if (request.progressCallback)
    {
        pProgressCallback.reset(new ProgressCallback(request, request.progressCallback));
    }
    
    CurlSession session;
    initCurlSession(session, request, &errorBuffer_[0], dataWriter, headerWriter, pProgressCallback.get());
    session.easySetOpt<CURLOPT_POST>(true);
    if (pData && 0 < dataSize)
    {
        session.easySetOpt<CURLOPT_POSTFIELDS>(pData);
        session.easySetOpt<CURLOPT_POSTFIELDSIZE>((long)dataSize);
    }
    
    CURLcode errorCode = CURLE_OK;
    std::tie(errorCode, response.code) = session.perform();
    if (!CurlSession::codeOK(errorCode))
    {
        return E_PERFORM;
    }
    
    response.header = std::move(headerWriter.getStream().str());
    response.data = std::move(dataWriter.getStream().str());
    return E_OK;
}

HttpClient::ErrorCode HttpClient::download(const HttpRequest& request, HttpResponse& response, const char* fileName)
{
    assert(fileName);

    FileStreamWriter dataWriter(fileName, std::ios::out|std::ios::binary|std::ios::trunc);
    if (!dataWriter.valid())
    {
        return E_OPEN_FILE;
    }
    StringStreamWriter headerWriter;
    std::shared_ptr<ProgressCallback> pProgressCallback;
    if (request.progressCallback)
    {
        pProgressCallback.reset(new ProgressCallback(request, request.progressCallback));
    }
    
    CurlSession session;
    initCurlSession(session, request, &errorBuffer_[0], dataWriter, headerWriter, pProgressCallback.get());
    session.easySetOpt<CURLOPT_FOLLOWLOCATION>(true);
    
    CURLcode errorCode = CURLE_OK;
    std::tie(errorCode, response.code) = session.perform();
    if (!CurlSession::codeOK(errorCode))
    {
        return E_PERFORM;
    }
    
    response.header = std::move(headerWriter.getStream().str());
    response.data = fileName;
    
    if (!dataWriter.valid())
    {
        return E_WRITE_FILE;
    }
    
    return E_OK;
}

GHOST_NAMESPACE_END