#ifndef TETRIS_UTIL_H
#define TETRIS_UTIL_H

#include <raylib.h>
#include <stdint.h>
#include <time.h>

#define is_ascii_alphanum(c) \
    (((c) >= 'A' && (c) <= 'Z') || ((c) >= '0' && (c) <= '9'))

void quit(int);
void cleanup(void);

char *strdup(const char *s);

Color hex_to_color(uint32_t);

int is_keyboard_key_pressed(void);

#endif
