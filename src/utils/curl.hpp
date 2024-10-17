#ifndef SOUND_SPHERE_UTILS_CURL_HPP
#define SOUND_SPHERE_UTILS_CURL_HPP

#include <curl/curl.h>
#include <memory>
#include "utils/string.hpp"

namespace soundsphere
{

struct EasyCurlInternal;

class EasyCurl
{
public:
    typedef std::shared_ptr<EasyCurl> Ptr;

public:
    EasyCurl();
    virtual ~EasyCurl();

public:
    CURL *get();
    std::string escape(const std::string &data);
    CURLcode perform(std::string &body);

public:
    EasyCurl(const EasyCurl &orig) = delete;

public:
    struct EasyCurlInternal *m_iner;
};

} // namespace soundsphere

#endif
