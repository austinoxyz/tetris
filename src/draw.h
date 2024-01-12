#ifndef TETRIS_DRAW_H
#define TETRIS_DRAW_H

#include "game.h"
#include "window.h"

struct DrawInfo {
    int canvaswidth, canvasheight;

    float boardaspect;
    int boardwidth, boardheight;
    int blocksidelen;
    int displayblocksidelen;

    float scale;
    float displaypiece_scale;

    Vector2 board_offset;
    Vector2 nextpieces_offset;
    Vector2 holdpiece_offset;
    Vector2 score_offset;
};

extern struct DrawInfo drawinfo;

#endif
