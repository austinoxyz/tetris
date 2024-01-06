#include <raylib.h>
#include <stdio.h>

#include "util.h"
#include "log.h"
#include "draw.h"

#define WIN_WIDTH 400
#define WIN_HEIGHT 800

#define BOARD_CANVAS_SCALE_FACTOR 1
//#define BOARD_CANVAS_WIDTH  (BOARD_CANVAS_SCALE_FACTOR*(WIN_WIDTH/2))
#define BOARD_CANVAS_WIDTH (BOARD_CANVAS_SCALE_FACTOR*WIN_WIDTH)
#define BOARD_CANVAS_HEIGHT (BOARD_CANVAS_SCALE_FACTOR*WIN_HEIGHT)
#define BOARD_CANVAS_PADDING ((WIN_HEIGHT-BOARD_CANVAS_HEIGHT)/2)

#define SRC_DRAW_REC  CLITERAL(Rectangle){0,0,BOARD_CANVAS_WIDTH,-BOARD_CANVAS_HEIGHT}
#define DEST_DRAW_REC CLITERAL(Rectangle){BOARD_CANVAS_PADDING,BOARD_CANVAS_PADDING,BOARD_CANVAS_WIDTH,BOARD_CANVAS_HEIGHT-BOARD_CANVAS_PADDING}
#define ORIGIN CLITERAL(Vector2){0,0}

static TetrisGame game;

void init(void) {
    srand(time(NULL));

    tetris_open_logfile("debug/tetris.log");

    InitWindow(WIN_WIDTH, WIN_HEIGHT, "tetris");
    SetTargetFPS(60);
    tetris_game_new(&game, BOARD_CANVAS_WIDTH, BOARD_CANVAS_HEIGHT, BOARD_DEFAULT_ROWS, BOARD_DEFAULT_COLS);
}

void cleanup(void) {
    tetris_close_logfile();
    tetris_game_free(&game);
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

