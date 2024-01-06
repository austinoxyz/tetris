#ifndef TETRIS_TETRIMINO_H
#define TETRIS_TETRIMINO_H

#include "common.h"
#include "pos.h"

typedef enum TetriminoType {
    TT_EMPTY = 0,
    TT_STRAIGHT, 
    TT_SQUARE, 
    TT_T, 
    TT_L1, 
    TT_L2, 
    TT_SKEW1, 
    TT_SKEW2, 
    TT_COUNT
} TetriminoType;

typedef enum TetriminoOrientation {
    TO_ZERO = 0, 
    TO_L, 
    TO_180,
    TO_R,
    TO_COUNT
} TetriminoOrientation;

typedef enum TetriminoMoveDirection {
    TMD_EAST = 0,
    TMD_WEST,
    TMD_COUNT
} TetriminoMoveDirection;

typedef enum TetriminoRotationDirection {
    TRD_CW = 0,
    TRD_CCW,
    TRD_COUNT
} TetriminoRotationDirection;

TetriminoRotationDirection opposite_rotation_direction(TetriminoRotationDirection const);

typedef bool TetriminoLayout[4][4];

void tetrimino_layout_rotate(TetriminoLayout *layout, TetriminoRotationDirection const dir);

typedef Position KickOffset;

typedef struct Tetrimino {
    TetriminoType        type;
    TetriminoOrientation orientation;
    TetriminoLayout      layout;
} Tetrimino;

void tetrimino_new(Tetrimino*, int);
void tetrimino_random_new(Tetrimino*);

#endif
