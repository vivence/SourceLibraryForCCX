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
#include <pthread.h>

GHOST_NAMESPACE_BEGIN

class HttpClient{
    
private:
    HttpClient();
    
public:
    ~HttpClient();
    
    HttpClient(const HttpClient&) = delete;
    HttpClient& operator =(const HttpClient&) = delete;
    
//    static HttpClient& getThreadLocalInstance();
    static HttpClient& getThreadLocalInstance();
    
public:
    
};

GHOST_NAMESPACE_END

#endif /* defined(__SourceLibraryForCCX__GTHttpClient__) */
