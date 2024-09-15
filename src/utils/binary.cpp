#include <ev.h>
#include "binary.hpp"

void soundsphere::dump(const char* path, const void* data, size_t size)
{
    ev_file_t file;
    ev_file_open(NULL, &file, NULL, path, EV_FS_O_CREAT | EV_FS_O_WRONLY, EV_FS_S_IRWXU, NULL);

    ev_file_write(&file, NULL, data, size, NULL);

    ev_file_close(&file, NULL);
}
