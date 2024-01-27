#ifndef TETRIS_UTIL_H
#define TETRIS_UTIL_H

#include <stdint.h>
#include <raylib.h>

// unix time in us
typedef uint64_t timestamp_t;
#define US_IN_A_SECOND 1000000

extern void quit(void);

timestamp_t current_time_us(void);

Color hex_to_color(uint32_t);

#endif
