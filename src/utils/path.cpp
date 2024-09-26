#include <ev.h>
#include "path.hpp"

std::string soundsphere::dirname(const std::string& path)
{
    // Find the last occurrence of the path separators
    size_t last_slash = path.find_last_of("/\\");
    if (last_slash == std::string::npos)
    {
        return ".";
    }

    return path.substr(0, last_slash);
}

std::string soundsphere::basename(const std::string& path, bool with_ext)
{
    // Find the last occurrence of the path separators
    size_t lastSlash = path.find_last_of("/\\");

    // Extract the file name from the path
    std::string fileName = (lastSlash == std::string::npos) ? path : path.substr(lastSlash + 1);

    if (!with_ext)
    {
        // Find the last occurrence of the dot in the file name
        size_t lastDot = fileName.find_last_of('.');

        // Extract the file name without the extension
        if (lastDot != std::string::npos)
        {
            fileName = fileName.substr(0, lastDot);
        }
    }

    return fileName;
}

std::string soundsphere::extname(const std::string& path)
{
    std::string name = soundsphere::basename(path, true);
    size_t first_dot = name.find_first_of('.');
    if (first_dot == std::string::npos)
    {
        return "";
    }
    return name.substr(first_dot);
}

std::string soundsphere::exepath(void)
{
    size_t buff_sz = 8192;
    char* buff = (char*)ev_malloc(buff_sz);

    ssize_t path_sz = ev_exepath(buff, buff_sz);
    if (path_sz >= (ssize_t)buff_sz)
    {
        ev_free(buff);
        return "";
    }

    std::string ret(buff);
    ev_free(buff);
    return ret;
}
