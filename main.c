#include "window.h"
#include "font.h"
#include "menu.h"
#include "highscores.h"
#include "game.h"
#include "draw.h"

void init(void) {
    srand(time(NULL));

    window_init(1.1);
    highscores_init();
    font_init();
    mainmenu_init();
    tetris_game_new(&g_game, BOARD_DEFAULT_ROWS, BOARD_DEFAULT_COLS);
    draw_init();

    SetTargetFPS(60);
    SetExitKey(KEY_Q);
}

void mainmenu_loop(void) {
    while (g_game.state == TGS_MAIN_MENU) {
        draw_mainmenu();
        mainmenu_handle_input();

        if (WindowShouldClose()) quit(0);
    }
}

void highscores_loop(void) {
    while (g_game.state == TGS_HIGHSCORES) {
        draw_highscores();
        highscores_handle_input();

        if (WindowShouldClose()) quit(0);
    }
}

void new_highscore_loop(void) {
    while (g_game.state == TGS_NEW_HIGHSCORE) {
        draw_game();
        draw_new_highscore_dialog();

        new_highscore_handle_input();
        if (WindowShouldClose()) quit(0);
    }
}

void game_loop(void) {
    timestamp_t now;
    timestamp_t then = current_time_ms();
    timestamp_t time_since_last_update = 0;
    timestamp_t dt;
    while (g_game.state == TGS_IN_PLAY || g_game.state == TGS_GAME_OVER) {
        draw_game();

        now = current_time_ms(), dt = (now - then);
        tetris_game_handle_user_input(&g_game);
        if ((time_since_last_update += dt) > g_game.us_per_update) {
            tetris_game_update(&g_game, time_since_last_update);

            time_since_last_update %= g_game.us_per_update;
        }
        then = now;

        if (WindowShouldClose()) quit(0);
    }
}

void main_loop(void) {
    for (;;) {
        switch (g_game.state) {
        case TGS_MAIN_MENU: 
            mainmenu_loop(); break;
        case TGS_HIGHSCORES:
            highscores_loop(); break;
        case TGS_IN_PLAY:   
        case TGS_GAME_OVER:   
            game_loop(); break;
        case TGS_NEW_HIGHSCORE:
            new_highscore_loop(); break;
        default: {
            fprintf(stderr, "Invalid TetrisGameState in main_loop()\n");
        }
        }
    }
}

int main(void) {
    init();
    main_loop();
    quit(0);
}

