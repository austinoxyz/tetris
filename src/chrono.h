#ifndef CHRONO_H
#define CHRONO_H

#include <stdint.h>
#include <time.h>

// unix time in ms
typedef uint64_t timestamp_t;

timestamp_t current_time_ms(void);

void        timestamp_to_timespec(timestamp_t timestamp, struct timespec *ts);
timestamp_t timespec_to_timestamp(struct timespec *ts);

#endif
