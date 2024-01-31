#include "common.h"
#include "game.h"
#include "highscores.h"
#include "window.h"

void cleanup(void) {
    highscores_deinit();
    tetris_game_free(&g_game);
    window_close();
}

void quit(int exitcode) {
    cleanup();
    exit(exitcode);
}

char *strdup(const char *s) {
    size_t len = strlen(s);
    char *result = malloc(len + 1);
    if (!result) return NULL;
    memcpy(result, s, len + 1);
    return result;
}

Color hex_to_color(uint32_t n) {
    return CLITERAL(Color) {
        .r=((n & 0xFF000000) >> 24),
        .g=((n & 0x00FF0000) >> 16),
        .b=((n & 0x0000FF00) >> 8),
        .a=(n & 0x000000FF)
    };
}

int is_keyboard_key_pressed(void) {
    for (int key = KEY_A; key <= KEY_Z; ++key)
        if (IsKeyPressed(key)) return key;
    return 0;
}
