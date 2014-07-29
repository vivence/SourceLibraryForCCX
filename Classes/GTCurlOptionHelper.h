//
//  GTCurlOptionHelper.h
//  SourceLibraryForCCX
//
//  Created by 李 侠懿 on 7/16/14.
//
//

#ifndef SourceLibraryForCCX_GTCurlOptionHelper_h
#define SourceLibraryForCCX_GTCurlOptionHelper_h

#include "GTMacros.h"
#include "curl/curl.h"
#include <string>
#include <cassert>
#include <stdexcept>
#include <string>

GHOST_NAMESPACE_BEGIN

typedef int (*ProgressCallbackFunc)(void* /*clientp*/, double/* dltotal*/, double/* dlnow*/, double/* ultotal*/, double/* ulnow*/);
typedef size_t (*WriteCallbackFunc)(char*/* ptr*/, size_t/* size*/, size_t/* nmemb*/, void*/* userdata*/);
typedef size_t (*HeaderCallbackFunc)(char*/* buffer*/, size_t/* size*/, size_t/* nitems*/, void*/* userdata*/);

class CurlStringList{
    struct curl_slist* pList_ = nullptr;
    
public:
    CurlStringList() = default;
    ~CurlStringList()
    {
        if (pList_)
        {
            curl_slist_free_all(pList_);
        }
    }
    
    CurlStringList(const CurlStringList&) = delete;
    CurlStringList& operator =(const CurlStringList&) = delete;
    
public:
    struct curl_slist* get()
    {
        return pList_;
    }
    
    void append(const char* str)
    {
        pList_ = curl_slist_append(pList_, str);
    }
    
    void append(const std::string& str)
    {
        append(str.c_str());
    }
};

template<CURLoption OPTION>
struct CurlOptionHelper{
    template<typename T>
    static CURLcode easySetOpt(CURL* curl, T data)
    {
        return curl_easy_setopt(curl, OPTION, data);
    }
};

template<>
struct CurlOptionHelper<CURLOPT_VERBOSE>{
    template<typename T>
    static CURLcode easySetOpt(CURL* curl, T data)
    {
        assert(false);
    }
    
    static CURLcode easySetOpt(CURL* curl, long data)
    {
        return curl_easy_setopt(curl, CURLOPT_VERBOSE, data);
    }
    
    static CURLcode easySetOpt(CURL* curl, bool data)
    {
        return easySetOpt(curl, (data ? 1L : 0L));
    }
};

template<>
struct CurlOptionHelper<CURLOPT_ERRORBUFFER>{
    template<typename T>
    static CURLcode easySetOpt(CURL* curl, T data)
    {
        assert(false);
    }
    
    static CURLcode easySetOpt(CURL* curl, char* data)
    {
        return curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, data);
    }
};

template<>
struct CurlOptionHelper<CURLOPT_TIMEOUT>{
    template<typename T>
    static CURLcode easySetOpt(CURL* curl, T data)
    {
        assert(false);
    }
    
    static CURLcode easySetOpt(CURL* curl, long data)
    {
        return curl_easy_setopt(curl, CURLOPT_TIMEOUT, data);
    }
    
    static CURLcode easySetOpt(CURL* curl, unsigned long data)
    {
        return easySetOpt(curl, (long)data);
    }
};

template<>
struct CurlOptionHelper<CURLOPT_CONNECTTIMEOUT>{
    template<typename T>
    static CURLcode easySetOpt(CURL* curl, T data)
    {
        assert(false);
    }
    
    static CURLcode easySetOpt(CURL* curl, long data)
    {
        return curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, data);
    }
    
    static CURLcode easySetOpt(CURL* curl, unsigned long data)
    {
        return easySetOpt(curl, (long)data);
    }
};

template<>
struct CurlOptionHelper<CURLOPT_SSL_VERIFYPEER>{
    template<typename T>
    static CURLcode easySetOpt(CURL* curl, T data)
    {
        assert(false);
    }
    
    static CURLcode easySetOpt(CURL* curl, long data)
    {
        return curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, data);
    }
    
    static CURLcode easySetOpt(CURL* curl, bool data)
    {
        return easySetOpt(curl, (data ? 1L : 0L));
    }
};

template<>
struct CurlOptionHelper<CURLOPT_SSL_VERIFYHOST>{
    template<typename T>
    static CURLcode easySetOpt(CURL* curl, T data)
    {
        assert(false);
    }
    
    static CURLcode easySetOpt(CURL* curl, long data)
    {
        return curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, data);
    }
    
    static CURLcode easySetOpt(CURL* curl, bool data)
    {
        return easySetOpt(curl, (data ? 1L : 0L));
    }
};

template<>
struct CurlOptionHelper<CURLOPT_NOSIGNAL>{
    template<typename T>
    static CURLcode easySetOpt(CURL* curl, T data)
    {
        assert(false);
    }
    
    static CURLcode easySetOpt(CURL* curl, long data)
    {
        return curl_easy_setopt(curl, CURLOPT_NOSIGNAL, data);
    }
    
    static CURLcode easySetOpt(CURL* curl, bool data)
    {
        return easySetOpt(curl, (data ? 1L : 0L));
    }
};

template<>
struct CurlOptionHelper<CURLOPT_NOPROGRESS>{
    template<typename T>
    static CURLcode easySetOpt(CURL* curl, T data)
    {
        assert(false);
    }
    
    static CURLcode easySetOpt(CURL* curl, long data)
    {
        return curl_easy_setopt(curl, CURLOPT_NOPROGRESS, data);
    }
    
    static CURLcode easySetOpt(CURL* curl, bool data)
    {
        return easySetOpt(curl, (data ? 1L : 0L));
    }
};

template<>
struct CurlOptionHelper<CURLOPT_PROGRESSFUNCTION>{
    template<typename T>
    static CURLcode easySetOpt(CURL* curl, T data)
    {
        assert(false);
    }
    
    static CURLcode easySetOpt(CURL* curl, ProgressCallbackFunc data)
    {
        return curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, data);
    }
};

template<>
struct CurlOptionHelper<CURLOPT_PROGRESSDATA>{
    template<typename T>
    static CURLcode easySetOpt(CURL* curl, T data)
    {
        assert(false);
    }
    
    static CURLcode easySetOpt(CURL* curl, void* data)
    {
        return curl_easy_setopt(curl, CURLOPT_PROGRESSDATA, data);
    }
    
    template<typename T>
    static CURLcode easySetOpt(CURL* curl, T* data)
    {
        return easySetOpt(curl, (void*)data);
    }
    
};

template<>
struct CurlOptionHelper<CURLOPT_HTTPHEADER>{
    template<typename T>
    static CURLcode easySetOpt(CURL* curl, T data)
    {
        assert(false);
    }
    
    static CURLcode easySetOpt(CURL* curl, struct curl_slist* data)
    {
        return curl_easy_setopt(curl, CURLOPT_HTTPHEADER, data);
    }
};

template<>
struct CurlOptionHelper<CURLOPT_COOKIE>{
    template<typename T>
    static CURLcode easySetOpt(CURL* curl, T data)
    {
        assert(false);
    }
    
    static CURLcode easySetOpt(CURL* curl, char* data)
    {
        return curl_easy_setopt(curl, CURLOPT_COOKIE, data);
    }
    
    static CURLcode easySetOpt(CURL* curl, const char* data)
    {
        return easySetOpt(curl, (char*)data);
    }
    
    static CURLcode easySetOpt(CURL* curl, const std::string& data)
    {
        return easySetOpt(curl, data.c_str());
    }
};

template<>
struct CurlOptionHelper<CURLOPT_COOKIEFILE>{
    template<typename T>
    static CURLcode easySetOpt(CURL* curl, T data)
    {
        assert(false);
    }
    
    static CURLcode easySetOpt(CURL* curl, char* data)
    {
        return curl_easy_setopt(curl, CURLOPT_COOKIEFILE, data);
    }
    
    static CURLcode easySetOpt(CURL* curl, const char* data)
    {
        return easySetOpt(curl, (char*)data);
    }
    
    static CURLcode easySetOpt(CURL* curl, const std::string& data)
    {
        return easySetOpt(curl, data.c_str());
    }
};

template<>
struct CurlOptionHelper<CURLOPT_COOKIEJAR>{
    template<typename T>
    static CURLcode easySetOpt(CURL* curl, T data)
    {
        assert(false);
    }
    
    static CURLcode easySetOpt(CURL* curl, char* data)
    {
        return curl_easy_setopt(curl, CURLOPT_COOKIEJAR, data);
    }
    
    static CURLcode easySetOpt(CURL* curl, const char* data)
    {
        return easySetOpt(curl, (char*)data);
    }
    
    static CURLcode easySetOpt(CURL* curl, const std::string& data)
    {
        return easySetOpt(curl, data.c_str());
    }
};

template<>
struct CurlOptionHelper<CURLOPT_URL>{
    template<typename T>
    static CURLcode easySetOpt(CURL* curl, T data)
    {
        assert(false);
    }
    
    static CURLcode easySetOpt(CURL* curl, char* data)
    {
        return curl_easy_setopt(curl, CURLOPT_URL, data);
    }
    
    static CURLcode easySetOpt(CURL* curl, const char* data)
    {
        return easySetOpt(curl, (char*)data);
    }
    
    static CURLcode easySetOpt(CURL* curl, const std::string& data)
    {
        return easySetOpt(curl, data.c_str());
    }
};

template<>
struct CurlOptionHelper<CURLOPT_USERAGENT>{
    template<typename T>
    static CURLcode easySetOpt(CURL* curl, T data)
    {
        assert(false);
    }
    
    static CURLcode easySetOpt(CURL* curl, char* data)
    {
        return curl_easy_setopt(curl, CURLOPT_USERAGENT, data);
    }
    
    static CURLcode easySetOpt(CURL* curl, const char* data)
    {
        return easySetOpt(curl, (char*)data);
    }
    
    static CURLcode easySetOpt(CURL* curl, const std::string& data)
    {
        return easySetOpt(curl, data.c_str());
    }
};

template<>
struct CurlOptionHelper<CURLOPT_WRITEFUNCTION>{
    template<typename T>
    static CURLcode easySetOpt(CURL* curl, T data)
    {
        assert(false);
    }
    
    static CURLcode easySetOpt(CURL* curl, WriteCallbackFunc data)
    {
        return curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, data);
    }
};

template<>
struct CurlOptionHelper<CURLOPT_WRITEDATA>{
    template<typename T>
    static CURLcode easySetOpt(CURL* curl, T data)
    {
        assert(false);
    }
    
    static CURLcode easySetOpt(CURL* curl, void* data)
    {
        return curl_easy_setopt(curl, CURLOPT_WRITEDATA, data);
    }
    
    template<typename T>
    static CURLcode easySetOpt(CURL* curl, T* data)
    {
        return easySetOpt(curl, (void*)data);
    }
    
};

template<>
struct CurlOptionHelper<CURLOPT_HEADERFUNCTION>{
    template<typename T>
    static CURLcode easySetOpt(CURL* curl, T data)
    {
        assert(false);
    }
    
    static CURLcode easySetOpt(CURL* curl, HeaderCallbackFunc data)
    {
        return curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, data);
    }
};

template<>
struct CurlOptionHelper<CURLOPT_HEADERDATA>{
    template<typename T>
    static CURLcode easySetOpt(CURL* curl, T data)
    {
        assert(false);
    }
    
    static CURLcode easySetOpt(CURL* curl, void* data)
    {
        return curl_easy_setopt(curl, CURLOPT_HEADERDATA, data);
    }
    
    template<typename T>
    static CURLcode easySetOpt(CURL* curl, T* data)
    {
        return easySetOpt(curl, (void*)data);
    }
    
};

template<>
struct CurlOptionHelper<CURLOPT_FOLLOWLOCATION>{
    template<typename T>
    static CURLcode easySetOpt(CURL* curl, T data)
    {
        assert(false);
    }
    
    static CURLcode easySetOpt(CURL* curl, long data)
    {
        return curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, data);
    }
    
    static CURLcode easySetOpt(CURL* curl, bool data)
    {
        return easySetOpt(curl, (data ? 1L : 0L));
    }
};

template<>
struct CurlOptionHelper<CURLOPT_POST>{
    template<typename T>
    static CURLcode easySetOpt(CURL* curl, T data)
    {
        assert(false);
    }
    
    static CURLcode easySetOpt(CURL* curl, long data)
    {
        return curl_easy_setopt(curl, CURLOPT_POST, data);
    }
    
    static CURLcode easySetOpt(CURL* curl, bool data)
    {
        return easySetOpt(curl, (data ? 1L : 0L));
    }
};

template<>
struct CurlOptionHelper<CURLOPT_POSTFIELDS>{
    template<typename T>
    static CURLcode easySetOpt(CURL* curl, T data)
    {
        assert(false);
    }
    
    static CURLcode easySetOpt(CURL* curl, char* data)
    {
        return curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data);
    }
    
    static CURLcode easySetOpt(CURL* curl, const char* data)
    {
        return easySetOpt(curl, (char*)data);
    }
    
    static CURLcode easySetOpt(CURL* curl, const std::string& data)
    {
        return easySetOpt(curl, data.c_str());
    }
};

template<>
struct CurlOptionHelper<CURLOPT_POSTFIELDSIZE>{
    template<typename T>
    static CURLcode easySetOpt(CURL* curl, T data)
    {
        assert(false);
    }
    
    static CURLcode easySetOpt(CURL* curl, long data)
    {
        return curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, data);
    }
    
    static CURLcode easySetOpt(CURL* curl, unsigned long data)
    {
        return easySetOpt(curl, (long)data);
    }
};

template<>
struct CurlOptionHelper<CURLOPT_CUSTOMREQUEST>{
    template<typename T>
    static CURLcode easySetOpt(CURL* curl, T data)
    {
        assert(false);
    }
    
    static CURLcode easySetOpt(CURL* curl, char* data)
    {
        return curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, data);
    }
    
    static CURLcode easySetOpt(CURL* curl, const char* data)
    {
        return easySetOpt(curl, (char*)data);
    }
    
    static CURLcode easySetOpt(CURL* curl, const std::string& data)
    {
        return easySetOpt(curl, data.c_str());
    }
};

GHOST_NAMESPACE_END

#endif
