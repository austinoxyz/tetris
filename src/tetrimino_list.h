#ifndef TETRIS_TETRIMINO_LIST_H
#define TETRIS_TETRIMINO_LIST_H

#include "tetrimino.h"

#define NPL_BAGSIZE (2*(TT_COUNT-1))
#define NPL_INC_BAGPOS(npl) ((npl->bagpos)=((npl->bagpos+1)%NPL_BAGSIZE))
#define NPL_BAG_NO(bagpos) (((bagpos)/(TT_COUNT-1)))
#define NPL_CURR_BAG_NO(npl) (NPL_BAG_NO((npl)->bagpos))
#define NPL_OTHER_BAG(npl) (((NPL_CURR_BAG_NO((npl)))+1)%2)
typedef struct NextPieceList {
    int bagpos;
    TetriminoType bag[NPL_BAGSIZE];
} NextPieceList;

void      nextpiecelist_new(NextPieceList*);
void      nextpiecelist_free(NextPieceList*);
void      nextpiecelist_generate_bag(NextPieceList*, int); // arg2: [0,1] - denotes bag number
Tetrimino nextpiecelist_pop_next_piece(NextPieceList*);
Tetrimino nextpiecelist_peek_next_piece(NextPieceList*);

#endif
