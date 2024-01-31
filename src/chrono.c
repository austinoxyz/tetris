#include "chrono.h"

timestamp_t current_time_ms(void) {
    struct timespec ts;
    timespec_get(&ts, TIME_UTC);
    return timespec_to_timestamp(&ts);
}

timestamp_t timespec_to_timestamp(struct timespec *ts) {
    return ((timestamp_t)(1e3*ts->tv_sec))+((timestamp_t)(ts->tv_nsec/1e6));
}

void timestamp_to_timespec(timestamp_t timestamp, struct timespec *ts) {
    ts->tv_sec = timestamp / 1000;
    ts->tv_nsec = (timestamp % 1000) * 1e6;
}

