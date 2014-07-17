//
//  GTCurlSession.cpp
//  SourceLibraryForCCX
//
//  Created by 李 侠懿 on 7/16/14.
//
//

#include "GTCurlSession.h"
#include <cassert>
#include <stdexcept>
#include <sstream>

GHOST_NAMESPACE_BEGIN

CurlSession::Global::Global(long flags/* = CURL_GLOBAL_ALL*/) throw(std::exception)
{
    auto code = curl_global_init(flags);
    if (!CurlSession::codeOK(code))
    {
        std::stringstream ss;
        ss<<"CURL global init failed: curl_global_init() returned "<<code<<", "<<curl_easy_strerror(code)<<"\n";
        throw std::runtime_error(ss.str().c_str());
    }
}

CurlSession::Global::~Global()
{
    curl_global_cleanup();
}

CurlSession::CurlSession() throw(std::exception)
: curl_(curl_easy_init())
{
    if (!curl_)
    {
        throw std::runtime_error("CURL init failed: curl_easy_init() returned NULL");
    }
}

CurlSession::~CurlSession()
{
    curl_easy_cleanup(curl_);
}

CurlSession::PerformResult CurlSession::perform()
{
    long responseCode = 0;
    auto code = curl_easy_perform(curl_);
    if (codeOK(code))
    {
        code = curl_easy_getinfo(curl_, CURLINFO_RESPONSE_CODE, &responseCode);
    }
    return std::make_tuple(code, responseCode);
}

GHOST_NAMESPACE_END