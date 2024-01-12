#include "draw.h"
#include "game.h"

#include "common.h"
#include "util.h"

#define TETRIS_FONT_SIZE 32
#define TETRIS_FONT_SPACING 5
#define TETRIS_RAYCLEAR (CLITERAL(Color) {(RAYWHITE).r, (RAYWHITE).g, (RAYWHITE).b, 120})

#define BLOCK_POS(row,col,sidelen) (CLITERAL(Vector2){(col)*(sidelen),(row)*(sidelen)})

struct DrawInfo info;

Color color_of_tetrimino_type(TetriminoType const type) {
    switch (type) {
    case TT_I: return hex_to_color(0x54f7efff); // cyan
    case TT_O: return hex_to_color(0xe3ea1cff); // yellow
    case TT_T: return hex_to_color(0xe02fedff); // magenta
    case TT_J: return hex_to_color(0x2d46e5ff); // blue
    case TT_L: return hex_to_color(0xe27a2bff); // orange
    case TT_S: return hex_to_color(0x1dbc1aff); // green
    case TT_Z: return hex_to_color(0xd81806ff); // red
    default: {
        fprintf(stderr, "Invalid tetrimino passed to color_of_tetrimino().");
        exit(1);
    }
    }
}

Vector2 offset_of_tetrimino_type(TetriminoType const type, const float sidelen) {
    switch (type) {
    case TT_I: return CLITERAL(Vector2) { 0, -sidelen/2.f };
    case TT_O: return CLITERAL(Vector2) { sidelen, 0 };
    default: return CLITERAL(Vector2) { sidelen/2.f, 0 };
    }
}

void draw_init(TetrisGame *game) {
    info.canvaswidth = window_width();
    info.canvasheight = window_height();

    info.scale = 0.75f;
    info.displaypiece_scale = 1.2f;

    info.boardaspect  = ((float)game->cols+2)/((float)game->rows+2);
    info.boardheight  = info.scale * info.canvasheight;
    info.boardwidth   = info.boardaspect * info.canvaswidth;
    info.blocksidelen = info.boardwidth / ((float)game->cols+2);
    info.displayblocksidelen = info.displaypiece_scale * info.blocksidelen;

    info.board_offset = CLITERAL(Vector2) {
        .x=(0.05f * info.canvaswidth), .y=(0.05f * info.canvasheight) };
    info.nextpieces_offset = CLITERAL(Vector2) {
        .x=((2*info.board_offset.x + info.boardwidth + info.canvaswidth)/2.f - 1.5f*info.displaypiece_scale*info.blocksidelen - info.board_offset.x),
        .y=(2*info.board_offset.y) };
    info.score_offset = CLITERAL(Vector2){
        .x=(info.board_offset.x),
        .y=(2*info.board_offset.y + info.boardheight) };
    info.holdpiece_offset = CLITERAL(Vector2) {
        .x=info.nextpieces_offset.x,
        .y=info.score_offset.y,
    };
}


void draw_block(Vector2 pos, float sidelen, Color color) {
    DrawRectangleV(pos, CLITERAL(Vector2) {sidelen,sidelen}, color);
    DrawLine(pos.x+0.2f*sidelen,pos.y+0.2f*sidelen,
             pos.x+0.8f*sidelen,pos.y+0.8f*sidelen, BLACK);
}

void draw_tetrimino(Tetrimino *tetrimino, Vector2 pos, float sidelen, Color color) {
    for (int r = 0; r < 4; ++r) {
        for (int c = 0; c < 4; ++c) {
            if (tetrimino->layout[r][c]) {
                draw_block(Vector2Add(pos, BLOCK_POS(r, c, sidelen)), sidelen, color);
            }
        }
    }
}

void draw_tetris_game_board(TetrisGame *game) {
    // Translate to game board position
    rlPushMatrix();
    rlTranslatef(info.board_offset.x, info.board_offset.y, 0);

    // draw gridlines
    for (int r = 0; r <= game->rows+1; ++r)
        DrawLine(0, r * info.blocksidelen, info.boardwidth, r * info.blocksidelen, BLACK);
    for (int c = 0; c <= game->cols+1; ++c)
        DrawLine(c * info.blocksidelen, 0, c * info.blocksidelen, info.boardheight, BLACK);

    // draw boundary
    for (int c = 0; c < game->cols+2; ++c) {
        draw_block(BLOCK_POS(0, c, info.blocksidelen), info.blocksidelen, GRAY);
        draw_block(BLOCK_POS(game->rows+1, c, info.blocksidelen), info.blocksidelen, GRAY);
    }
    for (int r = 1; r <= game->rows+1; ++r) {
        draw_block(BLOCK_POS(r, 0, info.blocksidelen), info.blocksidelen, GRAY);
        draw_block(BLOCK_POS(r, game->cols+1, info.blocksidelen), info.blocksidelen, GRAY);
    }

    // draw placed tetriminos
    for (int r = 0; r < game->rows; ++r) {
        for (int c = 0; c < game->cols; ++c) {
            if (game->board[r][c] != TT_EMPTY) {
                Color const color = color_of_tetrimino_type(game->board[r][c]);
                Position const pos = { r+1, c+1 };
                Vector2 drawpos = BLOCK_POS(pos.row, pos.col, info.blocksidelen);
                draw_block(drawpos, info.blocksidelen, color);
            }
        }
    }

    // draw active and ghost pieces
    Position drawpos = game->activepiece_pos;
    Position droppos = tetris_game_find_hard_drop_position(game);

    ++drawpos.row, ++drawpos.col;
    ++droppos.row, ++droppos.col;

    Vector2 piecevec = BLOCK_POS(drawpos.row, drawpos.col, info.blocksidelen);
    Vector2 ghostvec = BLOCK_POS(droppos.row, droppos.col, info.blocksidelen);

    Color color = color_of_tetrimino_type(game->activepiece.type);
    Color ghostcolor = color;
    ghostcolor.a /= 2;

    draw_tetrimino(&game->activepiece, piecevec, info.blocksidelen, color);
    draw_tetrimino(&game->activepiece, ghostvec, info.blocksidelen, ghostcolor);

    rlPopMatrix();
}

void draw_tetris_game_score(TetrisGame *game) {
    rlPushMatrix();
    rlTranslatef(info.score_offset.x, info.score_offset.y, 0);

    // draw score
    char scoretext[TETRIS_GAME_MAX_SCORE_TEXT_LEN];
    sprintf(scoretext, "Score: %d\nLevel: %d\nRows: %d", 
            game->score, game->level, game->rowscompleted);
    DrawTextEx(GetFontDefault(), scoretext, CLITERAL(Vector2){0,0},
            TETRIS_FONT_SIZE, TETRIS_FONT_SPACING, BLACK);

    rlPopMatrix();
}

void draw_tetris_game_over(TetrisGame *game) {
    UNUSED(game);
    const char *gameover_text = "Game Over!\nPress Spacebar to play again.";
    const int popup_border_size = 10;
    const Color popup_color = GRAY;
    Vector2 textdim = MeasureTextEx(GetFontDefault(), gameover_text, 
            TETRIS_FONT_SIZE, TETRIS_FONT_SPACING);

    Vector2 textdrawpos = CLITERAL(Vector2) {
        .x=((info.canvaswidth/2)-(textdim.x/2)),
        .y=((info.canvasheight/2)-(textdim.y/2)),
    };

    // background rectangle
    Rectangle popup_bkg = CLITERAL(Rectangle) {
        .x=(textdrawpos.x-popup_border_size),
        .y=(textdrawpos.y-popup_border_size),
        .width=(textdim.x+2*popup_border_size),
        .height=(textdim.y+2*popup_border_size)
    };
    DrawRectangleRec(popup_bkg, popup_color);

    DrawRectangleLinesEx(popup_bkg, 5, RED);

    DrawTextEx(GetFontDefault(), gameover_text, textdrawpos, 
            TETRIS_FONT_SIZE, TETRIS_FONT_SPACING, RED);
}

void draw_tetris_game_piece_sidebar(TetrisGame *game) {
    rlPushMatrix();
    rlTranslatef(info.nextpieces_offset.x, info.nextpieces_offset.y, 0);

    Vector2 drawvec = CLITERAL(Vector2) { 0, 0 };
    for (int i = 0; i < 5; ++i) {
        TetriminoType type = nextpiecelist_piecetype_n_ahead(&game->nextpiece_list, i);
        Tetrimino piece = tetrimino_new(type);
        Vector2 offset = offset_of_tetrimino_type(type, info.displayblocksidelen);
        draw_tetrimino(&piece, Vector2Add(drawvec, offset), info.displayblocksidelen, color_of_tetrimino_type(type));
        drawvec.y += 3 * info.displayblocksidelen;
    }

    drawvec.y += 3 * info.displayblocksidelen;

    if (game->holdpiece.type == TT_EMPTY) {
        rlPopMatrix();
        return;
    }

    Tetrimino dummy = tetrimino_new(game->holdpiece.type);
    Vector2 offset = offset_of_tetrimino_type(game->holdpiece.type, info.displayblocksidelen);
    offset = Vector2Add(info.holdpiece_offset, offset);
    draw_tetrimino(&dummy, offset, info.displayblocksidelen, color_of_tetrimino_type(dummy.type));

    rlPopMatrix();
}

void draw_tetris_game(TetrisGame *game) {
    BeginDrawing();
    ClearBackground(TETRIS_RAYCLEAR);

    draw_tetris_game_board(game);
    draw_tetris_game_piece_sidebar(game);
    draw_tetris_game_score(game);

    if (game->state == TGS_GAME_OVER)
        draw_tetris_game_over(game);

    EndDrawing();
}
