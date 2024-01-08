#include <raylib.h>
#include <stdio.h>

#include "util.h"
#include "log.h"
#include "draw.h"

//static Font font_iosevka;

static TetrisGame game;

void init(void) {
    srand(time(NULL));

    tetris_open_logfile("debug/tetris.log");

    InitWindow(WIN_WIDTH, WIN_HEIGHT, "tetris");
    SetTargetFPS(60);
    tetris_game_new(&game, BOARD_CANVAS_WIDTH, BOARD_CANVAS_HEIGHT, BOARD_DEFAULT_ROWS, BOARD_DEFAULT_COLS);

    tetris_load_fonts();
}

void cleanup(void) {
    tetris_close_logfile();
    tetris_game_free(&game);
    tetris_unload_fonts();
    CloseWindow();
}

void main_loop(void) {
    uint64_t us_counter = 0;
    uint64_t then = current_time_us();
    uint64_t now;
    while (!WindowShouldClose()) {
        draw_tetris_game(&game);
        BeginDrawing();
            DrawTexturePro(game.canvas.texture, SRC_DRAW_REC, DEST_DRAW_REC, ORIGIN, 0, RAYWHITE);
        EndDrawing();

        tetris_game_handle_user_input(&game);

        now = current_time_us();
        if ((us_counter+=(now-then))>1000000/game.ticks_per_sec) {
            tetris_game_update(&game);
            us_counter -= 1000000/game.ticks_per_sec;
        }
        then = now;
    }
}

int main(void) {
    init();
    main_loop();
    cleanup();
}

