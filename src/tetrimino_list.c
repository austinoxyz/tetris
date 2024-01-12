#include "tetrimino_list.h"

void nextpiecelist_new(NextPieceList *npl) {
    npl->bagpos = 0;
    memset(npl->bag, TT_EMPTY, NPL_BAGSIZE * sizeof(TetriminoType));
    nextpiecelist_generate_bag(npl, 0);
}

void nextpiecelist_generate_bag(NextPieceList *npl, int bagn) {
    memset(&npl->bag[bagn*(TT_COUNT-1)], TT_EMPTY, (TT_COUNT-1)*sizeof(TetriminoType));
    for (int i = 0; i < (TT_COUNT-1); /**/) {
        TetriminoType type = (TetriminoType) ((rand() % (TT_COUNT-1)) + 1);
        bool already_in = false;
        for (int j = 0; j < 7; ++j) {
            if (npl->bag[bagn*(TT_COUNT-1)+j] == type) {
                already_in = true;
                break;
            }
        }
        if (already_in) {
            continue;
        } else {
            npl->bag[bagn*(TT_COUNT-1)+i++] = type;
        }
    }
}

Tetrimino nextpiecelist_peek_next_piece(NextPieceList* npl) {
    return tetrimino_new(npl->bag[npl->bagpos]);
}

Tetrimino nextpiecelist_pop_next_piece(NextPieceList *npl) {
    if (npl->bagpos%(TT_COUNT-1) == 0)
        nextpiecelist_generate_bag(npl, NPL_OTHER_BAG(npl));
    Tetrimino result = tetrimino_new(npl->bag[npl->bagpos]);
    NPL_INC_BAGPOS(npl);
    return result;
}

TetriminoType nextpiecelist_piecetype_n_ahead(NextPieceList* npl, int n) {
    if (n < 0 || n >= NPL_BAGSIZE) {
        fprintf(stderr, "nextpiecelist_piece_n_ahead(): Argument n too large.\n");
        assert(false);
        exit(1);
    }

    return npl->bag[(n+npl->bagpos)%NPL_BAGSIZE];
}

