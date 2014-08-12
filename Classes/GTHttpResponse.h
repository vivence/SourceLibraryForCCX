//
//  GTHttpResponse.h
//  SourceLibraryForCCX
//
//  Created by 李 侠懿 on 7/17/14.
//
//

#ifndef __SourceLibraryForCCX__GTHttpResponse__
#define __SourceLibraryForCCX__GTHttpResponse__

#include "GTMacros.h"
#include <string>

GHOST_NAMESPACE_BEGIN

struct HttpResponse{
    
    long code = -1;
    std::string errorString;
    
    std::string header;
    std::string data; // file name(download) or content
    
};

GHOST_NAMESPACE_END

#endif /* defined(__SourceLibraryForCCX__GTHttpResponse__) */
