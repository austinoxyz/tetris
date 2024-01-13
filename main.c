#include <raylib.h>
#include <stdio.h>

#include "window.h"
#include "util.h"
#include "draw.h"

static TetrisGame game;

void init(void) {
    srand(time(NULL));
    window_init(1.1);
    tetris_game_new(&game, BOARD_DEFAULT_ROWS, BOARD_DEFAULT_COLS);
    SetTargetFPS(60);
}

void cleanup(void) {
    tetris_game_free(&game);
    window_close();
}

void main_loop(void) {
    timestamp_t us_counter = 0;
    timestamp_t then = current_time_us();
    timestamp_t now, dt;
    while (!WindowShouldClose()) {
        draw_tetris_game(&game);

        now = current_time_us(), dt = (now - then);
        tetris_game_handle_user_input(&game, dt);
        if ((us_counter+=dt)>game.us_per_update) {
            tetris_game_update(&game, dt);
            us_counter %= game.us_per_update;
        }
        then = now;
    }
}

int main(void) {
    init();
    main_loop();
    cleanup();
}

