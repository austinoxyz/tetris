#include "util.h"
#include "common.h"

timestamp_t current_time_us(void) {
    struct timeval time;
    gettimeofday(&time, NULL);
    return time.tv_sec*1000000+time.tv_usec;
}

Color hex_to_color(uint32_t n) {
    return CLITERAL(Color) {
        .r=((n & 0xff000000) >> 24),
        .g=((n & 0x00ff0000) >> 16),
        .b=((n & 0x0000ff00) >> 8),
        .a=(n & 0x000000ff)
    };
}
