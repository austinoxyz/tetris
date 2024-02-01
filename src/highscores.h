#ifndef HIGHSCORES_H
#define HIGHSCORES_H

#include "common.h"

#define MAX_HIGHSCORE_NAME_LEN 3
typedef struct HighscoreEntry {
    char           *name;
    int             score;
    struct timespec time; 
} HighscoreEntry;

HighscoreEntry new_highscore_entry(char *name, int score);

#define MAX_HIGHSCORE_ENTRIES 10
typedef HighscoreEntry HighscoreArray[MAX_HIGHSCORE_ENTRIES];

typedef struct Highscores {
    int            count;
    HighscoreArray entries;
} Highscores;

GLOBAL extern Highscores g_highscores;

void highscores_init(void);
void highscores_deinit(void);

void highscores_handle_input(void);

void highscores_insert_if_highscore(HighscoreEntry);
bool score_is_new_highscore(int);

#endif
