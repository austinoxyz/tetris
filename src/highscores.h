#ifndef HIGHSCORES_H
#define HIGHSCORES_H

#include "common.h"

typedef struct HighscoreEntry {
    char          *name;
    int            score;
    struct timeval time; 
} HighscoreEntry;

typedef HighscoreEntry Highscores[10];

void highscores_init(void);
void highscores_deinit(void);

void highscores_handle_input(void);

#endif
