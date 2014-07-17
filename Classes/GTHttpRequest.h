//
//  GTHttpRequest.h
//  SourceLibraryForCCX
//
//  Created by 李 侠懿 on 7/16/14.
//
//

#ifndef __SourceLibraryForCCX__GTHttpRequest__
#define __SourceLibraryForCCX__GTHttpRequest__

#include "GTMacros.h"
#include <string>
#include <list>
#include <functional>
#include <memory>

GHOST_NAMESPACE_BEGIN

struct HttpRequest{
    typedef std::list<std::string> StringList;
    typedef std::function<void(const HttpRequest& request, double now, double total)> ProgressCallbackFunction;
    
    
    long transferTimeoutSeconds = 0;
    long connectTimeoutSeconds = 0;
    bool sslVerifyPeer = false;
    bool sslVerifyHost = false;
    bool nosignal= true;
    
    std::string url;
    std::string userAgent;
    StringList headers;
    std::string cookie;
    std::string cookieFileReadFrom;
    std::string cookieFileStoreTo;
    
    ProgressCallbackFunction progressCallback;
};

GHOST_NAMESPACE_END

#endif /* defined(__SourceLibraryForCCX__GTHttpRequest__) */
