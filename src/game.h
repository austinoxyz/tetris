#ifndef TETRIS_GAME_H
#define TETRIS_GAME_H

#include <raylib.h>

#include "tetrimino_list.h"

typedef enum TetrisGameState {
    TGS_IN_PLAY = 0,
    TGS_PAUSED,
    TGS_GAME_OVER,
    TGS_COUNT
} TetrisGameState;

#define BOARD_DEFAULT_ROWS 20
#define BOARD_DEFAULT_COLS 10
#define PIECE_LOOKAHEAD_COUNT 5
#define PIECE_START_POS(rows,cols) CLITERAL(Position) { 0, ((cols)/2)-2 }
#define TETRIS_GAME_MAX_SCORE_TEXT_LEN 127
typedef struct TetrisGame {
    int             level;
    int             score;
    int             rowscompleted;
    int             combo;
    int             difficult_combo;
    int             tetris;
    bool            softdropping;
    bool            justtouchedpiece;
    int             nblocks_filled;

    int             rows;
    int             cols;
    TetriminoType **board;

    NextPieceList   nextpiece_list;
    Tetrimino       activepiece;
    Position        activepiece_pos;
    Tetrimino       holdpiece;

    RenderTexture2D canvas;

    TetrisGameState state;

    uint64_t        ticks_per_sec;
} TetrisGame;

void tetris_game_new(TetrisGame *game, int winwidth, int winheight, int rows, int cols);
void tetris_game_free(TetrisGame *game);

void tetris_game_handle_user_input(TetrisGame *game);
void tetris_game_update(TetrisGame *game);

#endif
