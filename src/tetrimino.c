#include "tetrimino.h"

static const TetriminoLayout tetrimino_layout[TT_COUNT] = {
    {{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0}}, // TT_EMPTY
    {{1,1,1,1},{0,0,0,0},{0,0,0,0},{0,0,0,0}}, // TT_STRAIGHT
    {{1,1,0,0},{1,1,0,0},{0,0,0,0},{0,0,0,0}}, // TT_SQUARE
    {{1,1,1,0},{0,1,0,0},{0,0,0,0},{0,0,0,0}}, // TT_T
    {{1,0,0,0},{1,0,0,0},{1,1,0,0},{0,0,0,0}}, // TT_L1
    {{0,1,0,0},{0,1,0,0},{1,1,0,0},{0,0,0,0}}, // TT_L2
    {{0,1,1,0},{1,1,0,0},{0,0,0,0},{0,0,0,0}}, // TT_SKEW1
    {{1,1,0,0},{0,1,1,0},{0,0,0,0},{0,0,0,0}}  // TT_SKEW2
};

// used for J,L,S,T,Z tetriminoes
static const KickOffset kick_offsets1[TO_COUNT][5] = {
    {{+0,+0},{+0,+0},{+0,+0},{+0,+0},{+0,+0}}, // TO_ZERO
    {{+0,+0},{+1,+0},{+1,-1},{+0,+2},{+1,+2}}, // TO_L
    {{+0,+0},{+0,+0},{+0,+0},{+0,+0},{+0,+0}}, // TO_180
    {{+0,+0},{-1,+0},{-1,-1},{+0,+2},{-1,+2}}  // TO_R
};

// used for I tetrimino
static const KickOffset kick_offsets2[TO_COUNT][5] = {
    {{+0,+0},{-1,+0},{+2,+0},{-1,+0},{+2,+0}}, // TO_ZERO
    {{-1,+0},{+0,+0},{+0,+0},{+0,+1},{+0,-2}}, // TO_L
    {{-1,+1},{+1,+1},{-2,+1},{+1,+0},{-2,+0}}, // TO_180
    {{+0,+1},{+0,+1},{+0,+1},{+0,-1},{+0,+2}}  // TO_R
};

// used for O tetrimino
static const KickOffset kick_offsets3[TO_COUNT][5] = {
    {{+0,+0},{+0,+0},{+0,+0},{+0,+0},{+0,+0}},
    {{+0,-1},{+0,-1},{+0,-1},{+0,-1},{+0,-1}},
    {{-1,-1},{-1,-1},{-1,-1},{-1,-1},{-1,-1}},
    {{-1,+0},{-1,+0},{-1,+0},{-1,+0},{-1,+0}}
};

const KickOffset *get_kick_offsets(Tetrimino *tetrimino) {
    switch (tetrimino->type) {
    case TT_SQUARE:
        return &kick_offsets3[tetrimino->orientation][0];
    case TT_STRAIGHT:
        return &kick_offsets2[tetrimino->orientation][0];
    case TT_T:
    case TT_L1: 
    case TT_L2: 
    case TT_SKEW1:
    case TT_SKEW2: 
        return &kick_offsets1[tetrimino->orientation][0];
    default: {
        fprintf(stderr, "get_kick_offsets(): Invalid tetrimino type!\n");
        exit(1);
    }
    }
#ifdef __GNUC__
    __builtin_unreachable();
#endif
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

void tetrimino_layout_rotate(TetriminoLayout *layout, TetriminoRotationDirection const dir) {
    if (dir == TRD_COUNT) {
        fprintf(stderr, "tetrimino_layout_rotate(): invalid direction passed as argument.\n");
        exit(1);
    }

    // transpose
    for (int r = 0; r < 4; ++r)
        for (int c = r; c < 4; ++c) {
            TetriminoType tmp = (*layout)[r][c];
            (*layout)[r][c] = (*layout)[c][r];
            (*layout)[c][r] = tmp;
        }

    // reverse rows for CW, cols for CCW
    if (dir == TRD_CW) {
        for (int r = 0; r < 4; ++r) {
            for (int c = 0; c < 2; ++c) {
                TetriminoType tmp = (*layout)[r][c];
                (*layout)[r][c] = (*layout)[r][3-c];
                (*layout)[r][3-c] = tmp;
            }
        }
    } else {
        for (int c = 0; c < 4; ++c) {
            for (int r = 0; r < 2; ++r) {
                TetriminoType tmp = (*layout)[r][c];
                (*layout)[r][c] = (*layout)[3-r][c];
                (*layout)[3-r][c] = tmp;
            }
        }
    }
}

void tetrimino_new(Tetrimino *tetrimino, int type) {
    tetrimino->type = (TetriminoType)type;
    tetrimino->orientation = TO_ZERO;
    memcpy(tetrimino->layout, tetrimino_layout[type], sizeof(TetriminoLayout));
}

void tetrimino_random_new(Tetrimino *tetrimino) {
    tetrimino_new(tetrimino, (rand() % (TT_COUNT - 1) + 1));
}

