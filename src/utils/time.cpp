#include <ev.h>
#include <cstdio>
#include "time.hpp"

void soundsphere::time_seconds_to_string(char* buff, size_t size, double seconds)
{
    int hours = static_cast<int>(seconds) / 3600;
    int minutes = (static_cast<int>(seconds) % 3600) / 60;
    int secs = static_cast<int>(seconds) % 60;
    snprintf(buff, size, "%02d:%02d:%02d", hours, minutes, secs);
}

std::string soundsphere::time_seconds_to_string(double seconds)
{
    char buf[12];
    time_seconds_to_string(buf, sizeof(buf), seconds);
    return buf;
}

uint64_t soundsphere::clock_time_ms(void)
{
    uint64_t t = ev_hrtime();
    return t / 1000000;
}
