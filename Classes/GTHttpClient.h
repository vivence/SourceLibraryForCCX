//
//  GTHttpClient.h
//  SourceLibraryForCCX
//
//  Created by 李 侠懿 on 7/16/14.
//
//

#ifndef __SourceLibraryForCCX__GTHttpClient__
#define __SourceLibraryForCCX__GTHttpClient__

#include "GTMacros.h"
#include <vector>

GHOST_NAMESPACE_BEGIN

class HttpRequest;
class HttpResponse;

class HttpClient{
public:
    enum ErrorCode{
        E_OK,
        E_OPEN_FILE,
        E_PERFORM
    };
    
private:
    std::vector<char> errorBuffer_;
    
private:
    HttpClient();
    
public:
    ~HttpClient();
    
    HttpClient(const HttpClient&) = delete;
    HttpClient& operator =(const HttpClient&) = delete;
    
//    static HttpClient& getThreadLocalInstance();
    static HttpClient& getThreadLocalInstance();
    
public:
    ErrorCode download(const HttpRequest& request, HttpResponse& response, const char* fileName);
    
};

GHOST_NAMESPACE_END

#endif /* defined(__SourceLibraryForCCX__GTHttpClient__) */
