#ifndef TETRIS_DRAW_H
#define TETRIS_DRAW_H

#include "game.h"
#include "menu.h"
#include "window.h"

struct DrawInfo {
    int fontsize;
    int fontspacing;
    int fontlinespacing;
    int fontheight;

    int canvaswidth, canvasheight;

    float boardaspect;
    int boardwidth, boardheight;
    int blocksidelen;
    int displayblocksidelen;

    float scale;
    float displaypiece_scale;

    float sidebar_borderwidth;

    Vector2 board_offset;
    Vector2 sidebar_offset;
    Vector2 score_offset;
    Vector2 gameover_offset;

    Font font;

    float logoscale;
    Texture2D logo;
    Vector2 logo_offset;

    Color bkgcolor, backdropcolor;
};

void draw_init(void);

void draw_game(void);
void draw_mainmenu(void);

#endif
