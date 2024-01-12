#include "tetrimino.h"

static const TetriminoLayout tetrimino_layout[TT_COUNT] = {
    {{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0}}, // TT_EMPTY
    {{0,0,0,0},{1,1,1,1},{0,0,0,0},{0,0,0,0}}, // TT_I
    {{1,1,0,0},{1,1,0,0},{0,0,0,0},{0,0,0,0}}, // TT_O
    {{0,1,0,0},{1,1,1,0},{0,0,0,0},{0,0,0,0}}, // TT_T
    {{1,0,0,0},{1,1,1,0},{0,0,0,0},{0,0,0,0}}, // TT_J
    {{0,0,1,0},{1,1,1,0},{0,0,0,0},{0,0,0,0}}, // TT_L
    {{0,1,1,0},{1,1,0,0},{0,0,0,0},{0,0,0,0}}, // TT_S
    {{1,1,0,0},{0,1,1,0},{0,0,0,0},{0,0,0,0}}  // TT_Z
};

static const KickOffset kick_offsets[3][TO_COUNT][5] = {
    // used for J,L,S,T,Z tetriminoes
    {{{+0,+0},{+0,+0},{+0,+0},{+0,+0},{+0,+0}},  // TO_ZERO
     {{+0,+0},{+1,+0},{+1,-1},{+0,+2},{+1,+2}},  // TO_L
     {{+0,+0},{+0,+0},{+0,+0},{+0,+0},{+0,+0}},  // TO_180
     {{+0,+0},{-1,+0},{-1,-1},{+0,+2},{-1,+2}}}, // TO_R
    // used for I tetrimino
    {{{+0,+0},{-1,+0},{+2,+0},{-1,+0},{+2,+0}},  // TO_ZERO
     {{-1,+0},{+0,+0},{+0,+0},{+0,+1},{+0,-2}},  // TO_L
     {{-1,+1},{+1,+1},{-2,+1},{+1,+0},{-2,+0}},  // TO_180
     {{+0,+1},{+0,+1},{+0,+1},{+0,-1},{+0,+2}}}, // TO_R
    // used for O tetrimino
    {{{+0,+0},{+0,+0},{+0,+0},{+0,+0},{+0,+0}},  // TO_ZERO
     {{+0,-1},{+0,-1},{+0,-1},{+0,-1},{+0,-1}},  // TO_L
     {{-1,-1},{-1,-1},{-1,-1},{-1,-1},{-1,-1}},  // TO_180
     {{-1,+0},{-1,+0},{-1,+0},{-1,+0},{-1,+0}}}  // TO_R
};

void get_tetrimino_layout(TetriminoLayout layout, TetriminoType type) {
    if (type == TT_COUNT) return;
    memcpy(layout, tetrimino_layout[type], sizeof(TetriminoLayout));
}

void get_kick_offsets(KickOffset *offsets, Tetrimino *tetrimino, TetriminoRotationDirection const dir) {
    if (!offsets) {
        fprintf(stderr, "get_kick_offsets(): Passed NULL value for KickOffsets *offsets\n");
        exit(1);
    }

    int id = 0;
    switch (tetrimino->type) {
    case TT_O:
        id = 2;
        break;
    case TT_I:
        id = 1;
        break;
    case TT_T: case TT_J: case TT_L: case TT_S: case TT_Z: 
        id = 0;
        break;
    default: {
        fprintf(stderr, "get_kick_offsets(): Invalid tetrimino type!\n");
        exit(1);
    }
    }

    TetriminoOrientation nextori = ROTATE_ORIENTATION(tetrimino->orientation, dir);
    for (int i = 0; i < 5; ++i) {
        offsets[i].row = kick_offsets[id][tetrimino->orientation][i].row - kick_offsets[id][nextori][i].row;
        offsets[i].col = kick_offsets[id][tetrimino->orientation][i].col - kick_offsets[id][nextori][i].col;
    }
}

TetriminoRotationDirection opposite_rotation_direction(TetriminoRotationDirection const dir) {
    switch (dir) {
    case TRD_CW: return TRD_CCW;
    case TRD_CCW: return TRD_CW;
    default: {
        fprintf(stderr, "opposite_rotation_direction(): invalid direction passed as argument.\n");
        exit(1);
    }
    }
#ifdef __GNUC__
    __builtin_unreachable();
#endif
}

void tetrimino_rotate(Tetrimino *tetrimino, TetriminoRotationDirection const dir) {
    if (dir == TRD_COUNT) {
        fprintf(stderr, "tetrimino_layout_rotate(): invalid direction passed as argument.\n");
        exit(1);
    }

    // O-tetrimino can't rotate
    if (tetrimino->type == TT_O) return;

    int bb_width = (tetrimino->type == TT_I) ? 4 : 3;

    // transpose
    for (int r = 0; r < bb_width; ++r)
        for (int c = r; c < bb_width; ++c) {
            TetriminoType tmp = tetrimino->layout[r][c];
            tetrimino->layout[r][c] = tetrimino->layout[c][r];
            tetrimino->layout[c][r] = tmp;
        }

    // reverse rows for CW, cols for CCW (after transposing)
    if (dir == TRD_CW) {
        for (int r = 0; r < bb_width; ++r) {
            for (int c = 0; c < bb_width/2; ++c) {
                TetriminoType tmp = tetrimino->layout[r][c];
                tetrimino->layout[r][c] = tetrimino->layout[r][bb_width-1-c];
                tetrimino->layout[r][bb_width-1-c] = tmp;
            }
        }
    } else {
        for (int c = 0; c < bb_width; ++c) {
            for (int r = 0; r < bb_width/2; ++r) {
                TetriminoType tmp = tetrimino->layout[r][c];
                tetrimino->layout[r][c] = tetrimino->layout[bb_width-1-r][c];
                tetrimino->layout[bb_width-1-r][c] = tmp;
            }
        }
    }
}

Tetrimino tetrimino_new(TetriminoType type) {
    Tetrimino result;
    result.type = (TetriminoType)type;
    result.orientation = TO_ZERO;
    get_tetrimino_layout(result.layout, type);
    return result;
}

