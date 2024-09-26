#include <curl/curl.h>
#include "__init__.hpp"

const SS_curl soundsphere::export_com_curl = {
    curl_free,
    (SS_CURL*(*)())curl_easy_init,
    (int(*)(SS_CURL*, int, ...))curl_easy_setopt,
    (int(*)(SS_CURL*))curl_easy_perform,
    (void(*)(SS_CURL*))curl_easy_cleanup,
    (char*(*)(SS_CURL*, const char*, int))curl_easy_escape,
    (char*(*)(SS_CURL*, const char*, int, int*))curl_easy_unescape,
};
