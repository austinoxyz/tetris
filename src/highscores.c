#include "highscores.h"
#include "game.h"

#define HIGHSCORES_FILENAME "highscores.txt"
#define MAX_HIGHSCORE_LINE_LEN 127

GLOBAL Highscores g_highscores;

HighscoreEntry new_highscore_entry(char *name, int score) {
    HighscoreEntry ret; 
    ret.name = strdup(name);
    ret.score = score;
    timestamp_to_timespec(current_time_ms(), &ret.time);
    return ret;
}

void read_in_highscores_file(void);
void write_out_highscores_file(void);

void highscores_init(void) {
    g_highscores.count = 0;
    read_in_highscores_file();
}

void highscores_deinit(void) {
    write_out_highscores_file();
    for (int i = 0; i < g_highscores.count; ++i) {
        free(g_highscores.entries[i].name);
    }
}

void read_in_highscores_file(void) {
    FILE *highscores_file;
    if (!(highscores_file = fopen(HIGHSCORES_FILENAME, "r"))) {
        // try creating the file
        if (!(highscores_file = fopen(HIGHSCORES_FILENAME, "w"))) {
            fprintf(stderr, "[ERROR] Unable to find '%s' and an attempt to create it failed.\n", HIGHSCORES_FILENAME);
            return;
        }
        if (!(highscores_file = fopen(HIGHSCORES_FILENAME, "r"))) {
            fprintf(stderr, "[ERROR] Failed to open '%s' after creating it.\n", HIGHSCORES_FILENAME);
            return;
        }
    }

    HighscoreEntry curr_entry;
    timestamp_t tmp_timestamp;
    int linenum = 0;
    char linebuff[MAX_HIGHSCORE_LINE_LEN];
    while (fgets(linebuff, MAX_HIGHSCORE_LINE_LEN, highscores_file)) {
        char *token = strtok(linebuff, ",");
        if (!token) goto missing_fields;
        curr_entry.name = (char *)malloc((MAX_HIGHSCORE_NAME_LEN+1)*sizeof(char));
        snprintf(curr_entry.name, MAX_HIGHSCORE_NAME_LEN+1, "%s", token);
        if (!curr_entry.name) {
            fprintf(stderr, "Couldn't duplicate string in highscores_init()\n");
            quit(1);
        }

        token = strtok(NULL, ",");
        if (!token) goto missing_fields;
        curr_entry.score = atoi(token);

        token = strtok(NULL, "\n");
        if (!token) goto missing_fields;
        tmp_timestamp = strtoull(token, NULL, 0);
        timestamp_to_timespec(tmp_timestamp, &curr_entry.time);

        g_highscores.entries[g_highscores.count++] = curr_entry;
        
    }
    if (fclose(highscores_file)) {
        fprintf(stderr, "Encountered an error while closing '%s'.\n", HIGHSCORES_FILENAME);
        quit(1);
    }
    return;

missing_fields:
    fprintf(stderr, "Missing fields on line %d of '%s'.\n", linenum, HIGHSCORES_FILENAME);
    fclose(highscores_file);
    exit(1);
}

void write_out_highscores_file(void) {
    FILE *highscores_file;
    if (!(highscores_file = fopen(HIGHSCORES_FILENAME, "w"))) {
        fprintf(stderr, "Couldn't open '%s'.\n", HIGHSCORES_FILENAME);
        quit(1);
    }

    for (int place = 0; place < g_highscores.count; ++place) {
        fprintf(highscores_file, "%s,%d,%lu\n", g_highscores.entries[place].name, 
                 g_highscores.entries[place].score, timespec_to_timestamp(&g_highscores.entries[place].time));
    }
    
    if (fclose(highscores_file)) {
        fprintf(stderr, "Encountered error while closing '%s'.\n", HIGHSCORES_FILENAME);
    }
}

void highscores_handle_input(void) {
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) || IsKeyPressed(KEY_SPACE)) {
        g_game.state = TGS_MAIN_MENU;
    }
}

void highscores_insert_if_highscore(HighscoreEntry entry) {
    int place;
    for (place = 0; place < g_highscores.count; ++place)
        if (entry.score > g_highscores.entries[place].score) break;

    if (place >= MAX_HIGHSCORE_ENTRIES) return;

    if (place < g_highscores.count - 1) {
        memmove(&g_highscores.entries[place+1], &g_highscores.entries[place], 
                sizeof(HighscoreEntry) * (g_highscores.count - place));
    }

    g_highscores.entries[place] = entry;
    if (g_highscores.count < MAX_HIGHSCORE_ENTRIES) ++g_highscores.count;
}

bool score_is_new_highscore(int score) {
    int place;
    for (place = 0; place < g_highscores.count; ++place)
        if (score > g_highscores.entries[place].score) return true;
    return (place <= MAX_HIGHSCORE_ENTRIES - 1);
}

