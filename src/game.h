#ifndef TETRIS_GAME_H
#define TETRIS_GAME_H

#include <raylib.h>

#include "chrono.h"
#include "tetriminolist.h"

typedef enum TetrisGameState {
    TGS_MAIN_MENU = 0,
    TGS_HIGHSCORES,
    TGS_NEW_HIGHSCORE,
    TGS_IN_PLAY,
    TGS_PAUSED,
    TGS_GAME_OVER,
    TGS_COUNT
} TetrisGameState;

typedef enum TspinType {
    TST_NONE = 0,
    TST_PROPER,
    TST_MINI,
    TST_COUNT
} TspinType;

#define BOARD_DEFAULT_ROWS 20
#define BOARD_DEFAULT_COLS 10
#define PIECE_LOOKAHEAD_COUNT 5
#define PIECE_START_POS(rows,cols) CLITERAL(Position) { 0, ((cols)/2)-2 }
#define TETRIS_GAME_SCORE_TEXT_MAX_LEN 127

#define TETRIS_GAME_MS_UNTIL_FINALIZATION_AFTER_ROTATION 1000
#define TETRIS_GAME_MS_UNTIL_KEY_CAN_BE_HELD 300

#define TETRIS_GAME_POS_IS_ON_BOARD(game, row, col) \
    (((row) > 0 && (row) < (game)->rows) && ((col) > 0 && (col) < (game)->cols))

#define TETRIS_GAME_SET_TICKS_PER_MIN(game, tpm) do {                                 \
    (game)->ticks_per_min = (tpm);                                                    \
    (game)->ms_per_update = (timestamp_t)((60000.0f)/((float)(game)->ticks_per_min)); \
} while (0);

typedef struct TetrisGame {
    int             level;
    int             score;
    int             rowscompleted;
    int             nblocks_filled;
    int             combo;
    int             difficult_combo;
    int             tetris;
    TspinType       tspin;
    bool            softdropping;

    timestamp_t     justrotated;
    timestamp_t     lastmoved;

    int             rows;
    int             cols;
    TetriminoType **board;

    NextPieceList   nextpiece_list;
    Tetrimino       activepiece;
    Position        activepiece_pos;

    Tetrimino       holdpiece;
    bool            pieceheld;

    uint64_t        ticks_per_min;
    timestamp_t     ms_per_update;

    TetrisGameState state;

    bool debug;
} TetrisGame;

GLOBAL extern TetrisGame g_game;

void tetris_game_new(TetrisGame *game, int rows, int cols);
void tetris_game_free(TetrisGame *game);

void tetris_game_update(TetrisGame *game, timestamp_t dt);

void tetris_game_handle_user_input(TetrisGame *game);
void new_highscore_handle_input(void);

#endif
