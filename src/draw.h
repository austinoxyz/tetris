#ifndef TETRIS_DRAW_H
#define TETRIS_DRAW_H

#include "game.h"
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

    Vector2 board_offset;
    Vector2 sidebar_offset;
    Vector2 score_offset;
    Vector2 gameover_offset;

    float sidebar_borderwidth;
};

void draw_init(void);
void draw(void);

//extern struct DrawInfo drawinfo;

#endif
