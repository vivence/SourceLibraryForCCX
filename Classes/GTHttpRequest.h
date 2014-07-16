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

GHOST_NAMESPACE_BEGIN

struct HttpRequest{
    typedef std::list<std::string> StringList;
    
    
    
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
    
    bool followLocation = false;
};

GHOST_NAMESPACE_END

#endif /* defined(__SourceLibraryForCCX__GTHttpRequest__) */
