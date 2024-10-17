#include <ev.h>
#include <cstdio>
#include <cassert>
#include "time.hpp"

void soundsphere::time_seconds_to_string(char *buff, size_t size, double seconds)
{
    assert(seconds >= 0);
    unsigned hours = static_cast<unsigned>(seconds) / 3600;
    unsigned minutes = (static_cast<unsigned>(seconds) % 3600) / 60;
    unsigned secs = static_cast<unsigned>(seconds) % 60;
    snprintf(buff, size, "%02u:%02u:%02u", hours, minutes, secs);
}

std::string soundsphere::time_seconds_to_string(double seconds)
{
    char buf[32];
    time_seconds_to_string(buf, sizeof(buf), seconds);
    return buf;
}

uint64_t soundsphere::clock_time_ms(void)
{
    uint64_t t = ev_hrtime();
    return t / 1000000;
}
