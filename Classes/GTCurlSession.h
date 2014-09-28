//
//  GTCurlSession.h
//  SourceLibraryForCCX
//
//  Created by 李 侠懿 on 7/16/14.
//
//

#ifndef __SourceLibraryForCCX__GTCurlSession__
#define __SourceLibraryForCCX__GTCurlSession__

#include "GTMacros.h"
#include "curl/curl.h"
#include "GTCurlOptionHelper.h"
#include <exception>
#include <tuple>
#include <memory>

GHOST_NAMESPACE_BEGIN

class CurlSession{
    CURL* curl_;
    std::shared_ptr<CurlStringList> pHeaderList_;
    
public:
    // instantiate Global before creating thread and CurlSession
    class Global{
    public:
        Global(long flags = CURL_GLOBAL_ALL) throw(std::exception);
        ~Global();
    };
    
public:
    CurlSession() throw(std::exception);
    ~CurlSession();
    
    CurlSession(const CurlSession&) = delete;
    CurlSession& operator =(const CurlSession&) = delete;
    
public:
    CurlStringList& getHeaderList()
    {
        if (!pHeaderList_)
        {
            pHeaderList_.reset(new CurlStringList());
        }
        return *pHeaderList_;
    }
    void resetHeaderList()
    {
        pHeaderList_.reset(new CurlStringList());
    }
    
public:
    static bool codeOK(CURLcode code)
    {
        return CURLE_OK == code;
    }
    
    template<CURLoption OPTION, typename T>
    CURLcode easySetOpt(T data)
    {
        return CurlOptionHelper<OPTION>::easySetOpt(curl_, data);
    }
    
    /*
     * 0: error code
     * 1: response code
     */
    typedef std::tuple<CURLcode, long> PerformResult;
    PerformResult perform();
    
};

GHOST_NAMESPACE_END

#endif /* defined(__SourceLibraryForCCX__GTCurlSession__) */
