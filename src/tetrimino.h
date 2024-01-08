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

#define ROTATE_ORIENTATION(dir, ori) \
    ((TetriminoOrientation)(((dir)+((ori)==TRD_CW?-1:1))%TO_COUNT))

TetriminoRotationDirection opposite_rotation_direction(TetriminoRotationDirection const);

typedef bool TetriminoLayout[4][4];

typedef struct Tetrimino {
    TetriminoType        type;
    TetriminoOrientation orientation;
    TetriminoLayout      layout;
} Tetrimino;

Tetrimino tetrimino_new(TetriminoType type);
void      tetrimino_rotate(Tetrimino *tetrimino, TetriminoRotationDirection const dir);

typedef Position KickOffset;

void get_kick_offsets(KickOffset *offsets, Tetrimino *tetrimino, TetriminoRotationDirection const dir);

#endif
