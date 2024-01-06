#include "util.h"

uint64_t current_time_us(void) {
    struct timeval time;
    gettimeofday(&time, NULL);
    return time.tv_sec*1000000+time.tv_usec;
}
