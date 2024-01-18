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
    timestamp_t now, then = current_time_us();
    timestamp_t _dt, dt = 0;
    while (!WindowShouldClose()) {
        draw_tetris_game(&game);

        now = current_time_us(), _dt = (now - then);
        tetris_game_handle_user_input(&game);
        if ((dt+=_dt)>game.us_per_update) {
            tetris_game_update(&game, dt);

            // NOTE:
            // taking modulus of us_per_update instead of substracting it off the counter
            // prevents from the game doing many updates quickly to catch back up 
            // after the game comes in and out focus
            dt %= game.us_per_update;
        }
        then = now;
    }
}

int main(void) {
    init();
    main_loop();
    cleanup();
}

