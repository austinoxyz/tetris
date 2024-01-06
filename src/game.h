#ifndef TETRIS_GAME_H
#define TETRIS_GAME_H

#include <raylib.h>

#include "tetrimino.h"

typedef struct TetriminoListNode {
    Tetrimino item;
    struct TetriminoListNode *next;
} TetriminoListNode;

TetriminoListNode *tetrimino_list_node_new(void);
void               tetrimino_list_node_free(TetriminoListNode *node);

typedef struct TetriminoList {
    TetriminoListNode *head;
    TetriminoListNode *tail;
    int size;
} TetriminoList;

void      tetrimino_list_new(TetriminoList *list, int len);
void      tetrimino_list_free(TetriminoList *list);
void      tetrimino_list_append_random(TetriminoList *list);
Tetrimino tetrimino_list_pop_head(TetriminoList *list);

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
typedef struct TetrisGame {
    int             level;
    int             rowscompleted;
    int             combo;
    int             score;

    int             softdropping;
    int             rows;
    int             cols;
    TetriminoType **board;

    TetriminoList   nextpiece_list;
    Tetrimino       activepiece;
    Position        activepiece_pos;

    RenderTexture2D canvas;

    TetrisGameState state;

    uint64_t        ticks_per_sec;
} TetrisGame;

void tetris_game_new(TetrisGame *game, int winwidth, int winheight, int rows, int cols);
void tetris_game_free(TetrisGame *game);

void tetris_game_handle_user_input(TetrisGame *game);
void tetris_game_update(TetrisGame *game);

#endif
