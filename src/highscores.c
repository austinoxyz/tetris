#include "highscores.h"

#define HIGHSCORES_FILENAME "highscores.txt"
static FILE *highscores_file;

void highscores_init(void) {
    if (!(highscores_file = fopen(HIGHSCORES_FILENAME, "r"))) {
        fprintf(stderr, "Couldn't open '%s'.\n", HIGHSCORES_FILENAME);
        quit();
    }
}

void highscores_deinit(void) {
    
}

void highscores_handle_input(void) {
    UNIMPLEMENTED();
}
