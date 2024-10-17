#include "config/__init__.hpp"
#include "curl.hpp"

struct soundsphere::EasyCurlInternal
{
    EasyCurlInternal();
    ~EasyCurlInternal();

    CURL *curl;
    std::string body;
};

soundsphere::EasyCurlInternal::EasyCurlInternal()
{
    curl = ::curl_easy_init();
}

soundsphere::EasyCurlInternal::~EasyCurlInternal()
{
    ::curl_easy_cleanup(curl);
}

static size_t WriteCallback(char *ptr, size_t size, size_t nmemb, void *userdata)
{
    soundsphere::EasyCurl *curl = static_cast<soundsphere::EasyCurl *>(userdata);
    size_t total_sz = size * nmemb;
    curl->m_iner->body.append(ptr, total_sz);
    return total_sz;
}

soundsphere::EasyCurl::EasyCurl()
{
    m_iner = new EasyCurlInternal;
    if (!soundsphere::_config.proxy.empty())
    {
        curl_easy_setopt(m_iner->curl, CURLOPT_PROXY, soundsphere::_config.proxy.c_str());
    }
    curl_easy_setopt(m_iner->curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(m_iner->curl, CURLOPT_WRITEDATA, this);
}

soundsphere::EasyCurl::~EasyCurl()
{
    delete m_iner;
}

CURL *soundsphere::EasyCurl::get()
{
    return m_iner->curl;
}

std::string soundsphere::EasyCurl::escape(const std::string &data)
{
    char *escape_data = curl_easy_escape(m_iner->curl, data.data(), (int)data.size());
    std::string ret(escape_data);
    curl_free(escape_data);
    return ret;
}

CURLcode soundsphere::EasyCurl::perform(std::string &body)
{
    m_iner->body.clear();

    CURLcode ret = curl_easy_perform(m_iner->curl);
    if (ret != CURLE_OK)
    {
        goto finish;
    }

    body = m_iner->body;

finish:
    return ret;
}
