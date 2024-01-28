#include "common.h"
#include "game.h"
#include "config.h"

void tetris_game_new(TetrisGame *game, int rows, int cols) {
    game->level = 1;
    game->score = 0;
    game->rowscompleted = 0;
    game->nblocks_filled = 0;
    game->combo = 0;
    game->difficult_combo = 0;
    game->softdropping = false;
    game->tspin = TST_NONE;

    game->justrotated = 0;
    game->lastmoved = 0;

    game->rows = rows;
    game->cols = cols;
    game->board = (TetriminoType**)calloc(rows, sizeof(TetriminoType*));
    for (int i = 0; i < rows; ++i)
        game->board[i] = (TetriminoType*)calloc(cols, sizeof(TetriminoType));

    nextpiecelist_new(&game->nextpiece_list);
    game->activepiece = nextpiecelist_pop_next_piece(&game->nextpiece_list);
    game->activepiece_pos = PIECE_START_POS(game->rows, game->cols);
    
    game->holdpiece = tetrimino_new(TT_EMPTY);
    game->pieceheld = false;

    TETRIS_GAME_SET_UPDATE_SPEED(game, 1);

    game->state = TGS_MAIN_MENU;
}

void tetris_game_free(TetrisGame *game) {
    if (game->board) {
        for (int i = 0; i < game->rows; ++i)
            if (game->board[i])
                free(game->board[i]);
        free(game->board);
    }
}

void tetris_game_restart(TetrisGame *game) {
    int rows = game->rows;
    int cols = game->cols;
    tetris_game_free(game);
    tetris_game_new(game, rows, cols);
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
    if (game->pieceheld) return;

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

    game->activepiece_pos = PIECE_START_POS(game->rows, game->cols);
    game->pieceheld = true;
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
    game->nblocks_filled += 4;
}

void tetris_game_cycle_next_piece(TetrisGame *game) {
    game->activepiece = nextpiecelist_pop_next_piece(&game->nextpiece_list);
    game->activepiece_pos = PIECE_START_POS(game->rows, game->cols);
}

void tetris_game_clear_completed_rows(TetrisGame *game) {
    bool rowcomplete;
    int nrows_complete = 0;
    for (int r = game->rows - 1; r >= 0; /**/) {
        rowcomplete = true;
        for (int c = 0; c < game->cols; ++c) {
            if (!game->board[r][c]) {
                rowcomplete = false;
                break;
            }
        }
        if (rowcomplete) {
            ++nrows_complete;
            for (int r2 = r - 1; r2 >= 0; --r2)
                memmove(game->board[r2+1], game->board[r2], game->cols * sizeof(TetriminoType));
            memset(game->board[0], TT_EMPTY, sizeof(TetriminoType) * game->cols);
        } else {
            --r;
        }
    }

    int scoregained = 0;
    bool difficult = false;
    bool got_tetris = false;

    // no score to add
    if (!nrows_complete && !game->tspin) {
        scoregained = 0;
        goto adjust_state_and_return;
    }

    // Action score
    if (game->tspin == TST_MINI) {
        switch (nrows_complete) {
        case 0: scoregained = 100 * game->level; break;
        case 1: scoregained = 200 * game->level; break;
        case 2: scoregained = 400 * game->level; break;
        default: break;
        }
    } else if (game->tspin == TST_PROPER) {
        scoregained = 400*(1+nrows_complete)*game->level;
    } else {
        switch (nrows_complete) {
        case 1: scoregained = 100 * game->level; break;
        case 2: scoregained = 300 * game->level; break;
        case 3: scoregained = 500 * game->level; break;
        case 4: {
            scoregained = 800 * game->level; 
            difficult = true;
            ++game->difficult_combo; 
            got_tetris = true;
        } break;
        }
    }

    // Award combo bonuses
    if (game->combo)
        scoregained += 50 * game->combo * game->level;
    if (game->difficult_combo > 0)
        scoregained *= 1.5;

    // Perfect clear bonuses
    game->nblocks_filled -= nrows_complete * game->cols;
    if (game->nblocks_filled == 0) {
    switch (nrows_complete) {
    case 1: scoregained +=  800 * game->level; break;
    case 2: scoregained += 1200 * game->level; break;
    case 3: scoregained += 1600 * game->level; break;
    case 4: {
        if (got_tetris) {
            if (game->tetris) {
                scoregained += 3200 * game->level;
            } else {
                scoregained += 2000 * game->level;
            }
        }
    } break;
    }
    }

adjust_state_and_return:
    if (nrows_complete > 0) 
        ++game->combo;
    else
        game->combo = 0;

    if ((game->tspin && nrows_complete > 0) || difficult) 
        ++game->difficult_combo;
    else
        game->difficult_combo = 0;

    if (got_tetris)
        game->tetris = true;
    else
        game->tetris = false;

    if ((game->rowscompleted % 10) + nrows_complete >= 10) {
        ++game->level;
        ++game->ticks_per_sec;
        TETRIS_GAME_SET_UPDATE_SPEED(game, game->ticks_per_sec);
    }

    game->rowscompleted += nrows_complete;
    game->score += scoregained;
}

TspinType tetris_game_check_for_tspin(TetrisGame *game) {
    if (!game->justrotated) return false;
    if (game->activepiece.type != TT_T) return false;

    Position frontoff1, frontoff2;
    Position const _pos = game->activepiece_pos;
    switch (game->activepiece.orientation) {
    case TO_ZERO: {
        frontoff1 = CLITERAL(Position){ _pos.col,   _pos.row   };
        frontoff2 = CLITERAL(Position){ _pos.col+2, _pos.row   };} break;
    case TO_L: {
        frontoff1 = CLITERAL(Position){ _pos.col,   _pos.row   };
        frontoff2 = CLITERAL(Position){ _pos.col  , _pos.row+2 };} break;
    case TO_180: {
        frontoff1 = CLITERAL(Position){ _pos.col,   _pos.row+2 };
        frontoff2 = CLITERAL(Position){ _pos.col+2, _pos.row+2 };} break;
    case TO_R: {
        frontoff1 = CLITERAL(Position){ _pos.col+2, _pos.row   };
        frontoff2 = CLITERAL(Position){ _pos.col+2, _pos.row+2 };} break;
    case TO_COUNT: {
        fprintf(stderr, "tetris_game_check_for_tspin(): game->activepiece.orientation is TO_COUNT\n");
        exit(1);
    }
    }

    int nminosbehind = 0, row, col;
    Tetrimino const _piece = game->activepiece;
    if (_piece.orientation == TO_L || _piece.orientation == TO_R) {
        col = (_piece.orientation == TO_L) ? _pos.col + 2 : _pos.col;
        if (col < 0) {
            for (row = _pos.col; row < _pos.row+3; ++row) {
                if (TETRIS_GAME_POS_IS_ON_BOARD(game, row, col) && game->board[row][col]) 
                    ++nminosbehind;
            }
        }
    } else {
        row = (_piece.orientation == TO_180) ? _pos.row + 2 : _pos.col;
        if (row != 0) {
            for (col = _pos.col; col < _pos.col+3; ++col) {
                if (TETRIS_GAME_POS_IS_ON_BOARD(game, row, col) && game->board[row][col]) 
                    ++nminosbehind;
            }
        }
    }

    int nminosfront = 0;
    if (TETRIS_GAME_POS_IS_ON_BOARD(game, frontoff1.row, frontoff1.col)
            && game->board[frontoff1.row][frontoff1.col])
        ++nminosfront;
    if (TETRIS_GAME_POS_IS_ON_BOARD(game, frontoff2.row, frontoff2.col)
            && game->board[frontoff2.row][frontoff2.col])
        ++nminosfront;

    if (nminosfront == 2 && nminosbehind == 1) {
        return TST_PROPER;
    } else if (nminosfront == 1 && nminosbehind == 2) {
        return TST_MINI;
    } else {
        return TST_NONE;
    }
}

void tetris_game_finalize_piece(TetrisGame *game, Tetrimino *tetrimino, Position const pos) {
    tetris_game_emplace_piece(game, tetrimino, pos);
    tetris_game_clear_completed_rows(game);
    tetris_game_cycle_next_piece(game);
    if (!tetris_game_try_fit_piece(game, &game->activepiece, game->activepiece_pos)) {
        printf("Game over!\n");
        game->state = TGS_GAME_OVER;
        return;
    }
    game->pieceheld = false;
}

void tetris_game_try_rotate_piece(TetrisGame *game, TetriminoRotationDirection const dir) {
    tetrimino_rotate(&game->activepiece, dir);
    if (!tetris_game_try_fit_piece(game, &game->activepiece, game->activepiece_pos)) {
        KickOffset kickoffsets[5];
        get_kick_offsets(kickoffsets, &game->activepiece, dir);
        for (int i = 0; i < 5; ++i) {
            Position const _pos = CLITERAL(Position) {
                .row=(game->activepiece_pos.row + kickoffsets[i].row),
                .col=(game->activepiece_pos.col + kickoffsets[i].col),
            };
            if (tetris_game_try_fit_piece(game, &game->activepiece, _pos)) {
                game->tspin = tetris_game_check_for_tspin(game); // NONE, PROPER, or MINI
                game->activepiece_pos = _pos;
                return;
            }
        }
        tetrimino_rotate(&game->activepiece, opposite_rotation_direction(dir));
    }
    game->justrotated = TETRIS_GAME_US_UNTIL_FINALIZATION_AFTER_ROTATION;
}

bool tetris_game_try_move_piece(TetrisGame *game, TetriminoMoveDirection const dir) {
    if (dir != TMD_EAST && dir != TMD_WEST) {
        fprintf(stderr, "tetris_game_try_move_piece(): Invalid move direction passed as argument.\n");
        exit(1);
    }

    int offset;
    for (int r = 0; r < 4; ++r) {
        int col_extant = -1;
        if (dir == TMD_EAST) {
            for (int c = 3; c >= 0; --c) {
                if (game->activepiece.layout[r][c]) {
                    col_extant = c;
                    break;
                }
            }
            offset = +1;
        } else {
            for (int c = 0; c < 4; ++c) {
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

Position tetris_game_find_hard_drop_position(TetrisGame *game) {
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

    return CLITERAL(Position) {
        game->activepiece_pos.row + mindist_to_stack,
        game->activepiece_pos.col 
    };
}

void tetris_game_hard_drop(TetrisGame *game) {
    Position const placepos = tetris_game_find_hard_drop_position(game);
    game->score += 2 * (placepos.row - game->activepiece_pos.row);
    tetris_game_finalize_piece(game, &game->activepiece, placepos);
}

void tetris_game_handle_user_input(TetrisGame *game) {
    if (game->state == TGS_IN_PLAY) {
        if (game->debug) {
            if (IsKeyPressed(TETRIS_KEYBIND_RESTART)) {
                tetris_game_restart(game);
                return;
            }
        }

        if (IsKeyPressed(TETRIS_KEYBIND_TOGGLE_DEBUG)) {
            game->debug = !game->debug;
        }

        if (IsKeyPressed(TETRIS_KEYBIND_ROTATE_CW)) {
            tetris_game_try_rotate_piece(game, TRD_CW);
        } 
        if (IsKeyPressed(TETRIS_KEYBIND_ROTATE_CCW)) {
            tetris_game_try_rotate_piece(game, TRD_CCW);
        } 
        if (IsKeyPressed(TETRIS_KEYBIND_MOVE_LEFT)) {
            tetris_game_try_move_piece(game, TMD_WEST);
        } 
        if (IsKeyPressed(TETRIS_KEYBIND_MOVE_RIGHT)) {
            tetris_game_try_move_piece(game, TMD_EAST);
        } 

        if (IsKeyPressed(TETRIS_KEYBIND_HARD_DROP)) {
            tetris_game_hard_drop(game);
        } 
        if (IsKeyPressed(TETRIS_KEYBIND_HOLD_PIECE)) {
            tetris_game_hold_piece(game);
        }

        if (IsKeyPressed(TETRIS_KEYBIND_SOFT_DROP)) {
            game->softdropping = true;
            TETRIS_GAME_SET_UPDATE_SPEED(game, game->ticks_per_sec*3);
        } else if (IsKeyReleased(TETRIS_KEYBIND_SOFT_DROP)) {
            game->softdropping = false;
            TETRIS_GAME_SET_UPDATE_SPEED(game, game->ticks_per_sec/3);
        }
    } else if (game->state == TGS_GAME_OVER) {
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) || IsKeyPressed(KEY_SPACE))
            tetris_game_restart(game);
    }
}

void tetris_game_update(TetrisGame *game, timestamp_t dt) {
    UNUSED(dt);

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

    bool piece_at_bottom = false;
    for (int c = 0; c < 4; ++c) {
        if (lowestrow[c] < 0) continue; // column is empty
        int const _row = game->activepiece_pos.row + lowestrow[c] + 1;
        int const _col = game->activepiece_pos.col + c;
        piece_at_bottom = ((_col >= 0 && _col < game->cols) && (_row >= game->rows || game->board[_row][_col]));
        bool const time_to_finalize_piece = (!game->justrotated || game->tspin);
        if (piece_at_bottom) {
            if (time_to_finalize_piece) {
                tetris_game_finalize_piece(game, &game->activepiece, game->activepiece_pos);
                if (game->tspin) {
                    printf("Tspin!\n");
                }
                return;
            } else {
                break;
            }
        }
    }

    if (game->softdropping) ++game->score;

    if (!piece_at_bottom) ++game->activepiece_pos.row;

    game->justrotated = 0;
}

