#ifndef TETRIS_LOG_H
#define TETRIS_LOG_H

void tetris_open_logfile(const char *filename);
void tetris_close_logfile(void);
void tetris_log(const char *fmt, ...);

#endif
