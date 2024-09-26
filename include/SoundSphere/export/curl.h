#ifndef SOUND_SPHERE_EXPORT_CURL_H
#define SOUND_SPHERE_EXPORT_CURL_H
#ifdef __cplusplus
extern "C" {
#endif

typedef struct SS_CURL SS_CURL;

typedef struct SS_curl
{
    void (*free)(void* ptr);
    SS_CURL* (*easy_init)(void);
    int (*easy_setopt)(SS_CURL* curl, int option, ...);
    int (*easy_perform)(SS_CURL* curl);
    void (*easy_cleanup)(SS_CURL* curl);
    char* (*easy_escape)(SS_CURL* curl, const char* str, int length);
    char* (*easy_unescape)(SS_CURL* curl, const char* str, int length, int* outlength);
} SS_curl;

#ifdef __cplusplus
}
#endif
#endif
