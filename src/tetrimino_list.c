#include "tetrimino_list.h"

void nextpiecelist_new(NextPieceList *npl) {
    npl->bagpos = 0;
    memset(npl->bag, TT_EMPTY, NPL_BAGSIZE * sizeof(TetriminoType));
    nextpiecelist_generate_bag(npl, 0);
}

void nextpiecelist_generate_bag(NextPieceList *npl, int bagn) {
    for (int i = 0; i < (TT_COUNT-1); /**/) {
        TetriminoType type = (TetriminoType) ((rand() % NPL_BAGSIZE) + 1);
        bool already_in = false;
        for (int i = 0; i < 7; ++i) {
            if (npl->bag[bagn*(TT_COUNT-1)+i] == type) {
                already_in = true;
                break;
            }
        }
        if (already_in) {
            continue;
        } else {
            npl->bag[bagn*(TT_COUNT-1)+i] = type;
            ++i;
        }
    }
}

Tetrimino nextpiecelist_get_next_piece(NextPieceList *npl) {
    if (npl->bagpos%(TT_COUNT-1) == TT_COUNT-2) {
        nextpiecelist_generate_bag(npl, ((npl->bagpos)/(TT_COUNT-1)+1)%2);
    }
    Tetrimino result = tetrimino_new(npl->bag[NPL_CURR_BAG_NO(npl)]);
    NPL_INC_BAGPOS(npl);
    return result;
}

