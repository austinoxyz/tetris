#include "log.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

static FILE *g_logfile = NULL;

void tetris_open_logfile(const char *filename) {
    g_logfile = fopen(filename, "w");
    if (!g_logfile) {
        fprintf(stderr, "Failed to open logfile at location '%s'\n", filename);
        exit(1);
    }
}

void tetris_close_logfile(void) {
    if (g_logfile)
        fclose(g_logfile);
}

void tetris_log(const char *fmt, ...) {
    if (!g_logfile) {
        fprintf(g_logfile, "Must open g_logfile before making a call to log()!");
        exit(1);
    }
    va_list args;
    va_start(args, fmt);
    vfprintf(g_logfile, fmt, args);
    vprintf(fmt, args);
    va_end(args);
}
