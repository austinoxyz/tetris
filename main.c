#include <raylib.h>
#include <stdio.h>

#include "window.h"
#include "util.h"
#include "draw.h"

TetrisGame g_game;
MainMenu   g_mainmenu;

void init(void) {
    srand(time(NULL));
    window_init(1.1);
    mainmenu_init();
    tetris_game_new(&g_game, BOARD_DEFAULT_ROWS, BOARD_DEFAULT_COLS);
    draw_init();
    SetTargetFPS(60);
}

void cleanup(void) {
    tetris_game_free(&g_game);
    window_close();
}

void quit(void) {
    cleanup();
    exit(0);
}

void menu_loop(void) {
    while (g_game.state == TGS_MAIN_MENU) {
        draw_mainmenu();
        mainmenu_handle_input();

        if (WindowShouldClose()) quit();
    }
}

void game_loop(void) {
    timestamp_t now;
    timestamp_t then = current_time_us();
    timestamp_t time_since_last_update = 0;
    timestamp_t dt;
    while (g_game.state == TGS_IN_PLAY || g_game.state == TGS_GAME_OVER) {
        draw_game();

        now = current_time_us(), dt = (now - then);
        tetris_game_handle_user_input(&g_game);
        if ((time_since_last_update += dt) > g_game.us_per_update) {
            tetris_game_update(&g_game, time_since_last_update);

            time_since_last_update %= g_game.us_per_update;
        }
        then = now;

        if (WindowShouldClose()) quit();
    }
}

void main_loop(void) {
    for (;;) {
        switch (g_game.state) {
        case TGS_MAIN_MENU: 
            menu_loop(); break;
        case TGS_IN_PLAY:   
        case TGS_GAME_OVER:   
            game_loop(); break;
        default: {
            fprintf(stderr, "Invalid TetrisGameState in main_loop()\n");
        }
        }
    }
}

int main(void) {
    init();
    main_loop();
    quit();
}

