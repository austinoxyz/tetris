#include "common.h"
#include "game.h"

void tetris_game_new(TetrisGame *game, int winwidth, int winheight, int rows, int cols) {
    game->level = 1;
    game->score = 0;
    game->rowscompleted = 0;
    game->combo = -1;
    game->difficult_combo = -1;
    game->softdropping = false;

    game->rows = rows;
    game->cols = cols;
    game->board = (TetriminoType**)calloc(rows, sizeof(TetriminoType*));
    for (int i = 0; i < rows; ++i)
        game->board[i] = (TetriminoType*)calloc(cols, sizeof(TetriminoType));

    nextpiecelist_new(&game->nextpiece_list);
    game->activepiece = nextpiecelist_pop_next_piece(&game->nextpiece_list);
    game->activepiece_pos = PIECE_START_POS(game->rows, game->cols);
    game->holdpiece = tetrimino_new(TT_EMPTY);

    game->canvas = LoadRenderTexture(winwidth, winheight);

    game->ticks_per_sec = 4;
}

void tetris_game_free(TetrisGame *game) {
    if (game->board) {
        for (int i = 0; i < game->rows; ++i)
            if (game->board[i])
                free(game->board[i]);
        free(game->board);
    }
}

bool tetris_game_try_fit_piece(TetrisGame *game, Tetrimino *tetrimino, Position const pos) {
    for (int r = 0; r < 4; ++r) {
        for (int c = 0; c < 4; ++c) {
            if (tetrimino->layout[r][c]) {
                Position const _pos = CLITERAL(Position) { pos.row + r, pos.col + c };
                if ((_pos.row < 0 || _pos.row >= game->rows)
                        || (_pos.col < 0 || _pos.col >= game->cols)
                        || game->board[_pos.row][_pos.col]) {
                    return false;
                }
            }
        }
    }
    return true;
}

void tetris_game_hold_piece(TetrisGame *game) {
    Tetrimino piece_to_swap_in = tetrimino_new(TT_EMPTY);
    bool holdempty = false;
    if (game->holdpiece.type != TT_EMPTY) {
        piece_to_swap_in = game->holdpiece;
    } else {
        holdempty = true;
        piece_to_swap_in = nextpiecelist_peek_next_piece(&game->nextpiece_list);
    }

    if (!tetris_game_try_fit_piece(game, &piece_to_swap_in, game->activepiece_pos))
        return;

    game->holdpiece = game->activepiece;
    if (holdempty) {
        game->activepiece = nextpiecelist_pop_next_piece(&game->nextpiece_list);
    } else {
        game->activepiece = piece_to_swap_in;
    }
}

void tetris_game_emplace_piece(TetrisGame *game, Tetrimino *tetrimino, Position const pos) {
    for (int r = 0; r < 4; ++r) {
        for (int c = 0; c < 4; ++c) {
            if (tetrimino->layout[r][c]) {
                Position const _pos = CLITERAL(Position) { pos.row + r, pos.col + c };
                if (_pos.col >= game->cols || _pos.row >= game->rows) continue;
                game->board[_pos.row][_pos.col] = tetrimino->type;
            }
        }
    }
}

void tetris_game_cycle_next_piece(TetrisGame *game) {
    game->activepiece = nextpiecelist_pop_next_piece(&game->nextpiece_list);
    game->activepiece_pos = PIECE_START_POS(game->rows, game->cols);
}

int tetris_game_clear_completed_rows(TetrisGame *game) {
    bool rowcomplete;
    int nrows = 0;
    for (int r = game->rows - 1; r >= 0; /**/) {
        rowcomplete = true;
        for (int c = 0; c < game->cols; ++c) {
            if (!game->board[r][c]) {
                rowcomplete = false;
                break;
            }
        }
        if (rowcomplete) {
            ++nrows;
            for (int r2 = r - 1; r2 >= 0; --r2)
                memmove(game->board[r2+1], game->board[r2], game->cols * sizeof(TetriminoType));
            memset(game->board[0], TT_EMPTY, sizeof(TetriminoType) * game->cols);
        } else {
            --r;
        }
    }
    return nrows;
}

void tetris_game_finalize_piece(TetrisGame *game, Tetrimino *tetrimino, Position const pos) {
    tetris_game_emplace_piece(game, tetrimino, pos);
    int ncompleted = tetris_game_clear_completed_rows(game);
    if (ncompleted > 0) {
        // TODO scoring
    }
    tetris_game_cycle_next_piece(game);
    if (!tetris_game_try_fit_piece(game, &game->activepiece, game->activepiece_pos)) {
        printf("Game over!\n");
        game->state = TGS_GAME_OVER;
        return;
    }
}

void tetris_game_try_rotate_piece(TetrisGame *game, TetriminoRotationDirection const dir) {
    tetrimino_layout_rotate(&game->activepiece.layout, dir);
    if (!tetris_game_try_fit_piece(game, &game->activepiece, game->activepiece_pos)) {
        KickOffset kickoffsets[5];
        get_kick_offsets(kickoffsets, &game->activepiece, dir);
        for (int i = 0; i < 5; ++i) {
            Position const _pos = CLITERAL(Position) {
                .row=(game->activepiece_pos.row + kickoffsets[i].row),
                .col=(game->activepiece_pos.col + kickoffsets[i].col),
            };
            if (tetris_game_try_fit_piece(game, &game->activepiece, _pos)) {
                game->activepiece_pos = _pos;
                return;
            }
        }
        tetrimino_layout_rotate(&game->activepiece.layout, opposite_rotation_direction(dir));
    }
}

bool tetris_game_try_move_piece(TetrisGame *game, TetriminoMoveDirection const dir) {
    if (dir != TMD_EAST && dir != TMD_WEST) {
        fprintf(stderr, "tetris_game_try_move_piece(): Invalid move direction passed as argument.\n");
        exit(1);
    }

    int col_extant;
    int offset;
    for (int r = 0; r < 4; ++r) {
        int c;
        col_extant = -1;
        if (dir == TMD_EAST) {
            for (c = 3; c >= 0; --c) {
                if (game->activepiece.layout[r][c]) {
                    col_extant = c;
                    break;
                }
            }
            offset = +1;
        } else {
            for (c = 0; c < 4; ++c) {
                if (game->activepiece.layout[r][c]) {
                    col_extant = c;
                    break;
                }
            }
            offset = -1;
        }

        if (col_extant < 0) continue;

        Position const checkpos = CLITERAL(Position) {
            game->activepiece_pos.row + r,
            game->activepiece_pos.col + col_extant + offset
        };
        if ((checkpos.col < 0 || checkpos.col >= game->cols)
                || game->board[checkpos.row][checkpos.col]) {
            return false;
        }
    }

    game->activepiece_pos.col += offset;
    return true;
}

void tetris_game_hard_drop(TetrisGame *game) {
    int mindist_to_stack = INT_MAX;
    int dist;
    int lowestrow;
    for (int c = 0; c < 4; ++c) {
        lowestrow = -1;
        for (int r = 3; r >= 0; --r) {
            if (game->activepiece.layout[r][c]) {
                lowestrow = r;
                break;
            }
        }
        if (lowestrow < 0) continue;
        dist = 0;
        int _row = game->activepiece_pos.row + lowestrow + 1;
        int _col = game->activepiece_pos.col + c;
        while (_row < game->rows && !game->board[_row][_col]) {
            ++dist;
            ++_row;
        }
        if (dist < mindist_to_stack) {
            mindist_to_stack = dist;
        }
    }
    Position const placepos = {
        game->activepiece_pos.row + mindist_to_stack,
        game->activepiece_pos.col 
    };
    game->score += 2*mindist_to_stack;
    tetris_game_finalize_piece(game, &game->activepiece, placepos);
}

void tetris_game_handle_user_input(TetrisGame *game) {
    if (game->state == TGS_IN_PLAY) {
        if (IsKeyPressed(KEY_UP)) {
            tetris_game_try_rotate_piece(game, TRD_CW);
        } else if (IsKeyPressed(KEY_DOWN)) {
            tetris_game_try_rotate_piece(game, TRD_CCW);
        } else if (IsKeyPressed(KEY_LEFT)) {
            tetris_game_try_move_piece(game, TMD_WEST);
        } else if (IsKeyPressed(KEY_RIGHT)) {
            tetris_game_try_move_piece(game, TMD_EAST);
        } else if (IsKeyPressed(KEY_SPACE)) {
            tetris_game_hard_drop(game);
        } else if (IsKeyPressed(KEY_C)) {
            tetris_game_hold_piece(game);
        }

        if (IsKeyPressed(KEY_LEFT_SHIFT)) {
            game->softdropping = true;
            game->ticks_per_sec *= 3;
        } else if (IsKeyReleased(KEY_LEFT_SHIFT)) {
            game->softdropping = false;
            game->ticks_per_sec /= 3;
        }
    }
}

void tetris_game_update(TetrisGame *game) {
    if (game->state != TGS_IN_PLAY) return;

    // check for landing
    int lowestrow[4] = {-1,-1,-1,-1};
    for (int c = 0; c < 4; ++c) {
        for (int r = 3; r >= 0; --r) {
            if (game->activepiece.layout[r][c]) {
                lowestrow[c] = r;
                break;
            }
        }
    }
    for (int c = 0; c < 4; ++c) {
        if (lowestrow[c] < 0) continue; // column is empty
        int const _row = game->activepiece_pos.row + lowestrow[c] + 1;
        int const _col = game->activepiece_pos.col + c;
        if ((_col >= 0 && _col < game->cols) 
                && (_row >= game->rows || game->board[_row][_col])) {
            tetris_game_finalize_piece(game, &game->activepiece, game->activepiece_pos);
            return;
        }
    }

    ++game->activepiece_pos.row;
    if (game->softdropping) ++game->score;
}

