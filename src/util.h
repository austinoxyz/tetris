#ifndef TETRIS_UTIL_H
#define TETRIS_UTIL_H

#include <raylib.h>
#include <stdint.h>
#include <time.h>

void quit(int);
void cleanup(void);

char *strdup(const char *s);

Color hex_to_color(uint32_t);

int is_keyboard_key_pressed(void);

#endif
