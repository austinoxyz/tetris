#ifndef TETRIS_COMMON_H
#define TETRIS_COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <limits.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <assert.h>

#include <unistd.h>
#include <sys/time.h>

#include <raylib.h>
#include <raymath.h>
#include <rlgl.h>

#include "util.h"

#define STR(x) #x
#define UNUSED(x) {(void)x;}
#define UNIMPLEMENTED() {assert(false && "Called unimplemented function at " __FILE__ ":" STR(__LINE__));}

#define GLOBAL

#endif
