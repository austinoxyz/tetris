#include <raylib.h>
#include <stdio.h>

#include "window.h"
#include "util.h"
#include "draw.h"

TetrisGame g_game;

void init(void) {
    srand(time(NULL));
    window_init(1.1);
    tetris_game_new(&g_game, BOARD_DEFAULT_ROWS, BOARD_DEFAULT_COLS);
    draw_init();
    SetTargetFPS(60);
}

void cleanup(void) {
    tetris_game_free(&g_game);
    window_close();
}

void main_loop(void) {
    timestamp_t now, then = current_time_us();
    timestamp_t _dt, dt = 0;
    while (!WindowShouldClose()) {
        draw();

        now = current_time_us(), _dt = (now - then);
        tetris_game_handle_user_input(&g_game);
        if ((dt += _dt) > g_game.us_per_update) {
            tetris_game_update(&g_game, dt);

            dt %= g_game.us_per_update;
        }
        then = now;
    }
}

int main(void) {
    init();
    main_loop();
    cleanup();
}

