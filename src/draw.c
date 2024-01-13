#include "draw.h"
#include "game.h"

#include "common.h"
#include "util.h"

#define TETRIS_FONT_SIZE_DEFAULT 32
#define TETRIS_FONT_LINE_SPACING_DEFAULT 32
#define TETRIS_FONT_SPACING_DEFAULT 8
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

    info.scale = (((float) info.canvasheight) / WINDOW_DEFAULT_HEIGHT);
    info.displaypiece_scale = 1.2f;

    info.boardaspect  = ((float)game->cols+2)/((float)game->rows+2);
    info.boardheight  = 0.75f * info.canvasheight;
    info.boardwidth   = info.boardaspect * info.canvaswidth;
    info.blocksidelen = info.boardwidth / ((float)game->cols+2);
    info.displayblocksidelen = info.displaypiece_scale * info.blocksidelen;

    info.board_offset = CLITERAL(Vector2) {
        .x=(0.05f * info.canvaswidth), 
        .y=(0.05f * info.canvasheight) };
    info.sidebar_offset = CLITERAL(Vector2) {
        .x=((2*info.board_offset.x + info.boardwidth + info.canvaswidth)/2.f - 1.5f*info.displaypiece_scale*info.blocksidelen - info.board_offset.x),
        .y=(info.boardheight - 14*info.blocksidelen)/2.f };
    info.score_offset = CLITERAL(Vector2) {
        .x=(info.board_offset.x),
        .y=(1.5f*info.board_offset.y + info.boardheight) };
#if 0
    info.gameover_offset = CLITERAL(Vector2) {
        .x=(),
        .y=(info.score_offset.y)
    };
#endif

    info.sidebar_borderwidth = 2;

    info.fontsize = (int) (info.scale * TETRIS_FONT_SIZE_DEFAULT);
    info.fontspacing = (int) (info.scale * TETRIS_FONT_SPACING_DEFAULT);
    info.fontlinespacing = (int) (info.scale * TETRIS_FONT_LINE_SPACING_DEFAULT);
    SetTextLineSpacing(info.fontlinespacing);
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
        DrawLine(0, r * info.blocksidelen, (game->cols+2)*info.blocksidelen, r * info.blocksidelen, BLACK);
    for (int c = 0; c <= game->cols+1; ++c)
        DrawLine(c * info.blocksidelen, 0, c * info.blocksidelen, (game->rows+2)*info.blocksidelen, BLACK);

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
    sprintf(scoretext, "Level: %d\nScore: %d", game->level, game->score);
    DrawTextEx(GetFontDefault(), scoretext, CLITERAL(Vector2){0,0},
            info.fontsize, info.fontspacing, BLACK);

    rlPopMatrix();
}

#if 0
void draw_tetris_game_over(TetrisGame *game) {
    UNUSED(game);

    const char *gameover_text = "Game Over!";
    const char *playagain_text = "Press Spacebar to play again.";
    const int popup_border_size = 10;
    const Color popup_color = GRAY;

    float fontheight = GetFontDefault().baseSize;

    Vector2 textdim1 = MeasureTextEx(GetFontDefault(), gameover_text, 
            info.fontsize, info.fontspacing);
    Vector2 textdim2 = MeasureTextEx(GetFontDefault(), playagain_text, 
            info.fontsize, info.fontspacing);

    float popupwidth = 1.2f*textdim2.x;
    float popupheight = 3*fontheight + 2*popup_border_size;

    Vector2 popup_drawpos = CLITERAL(Vector2) {
        .x=((info.canvaswidth - popupwidth)/2.f),
        .y=((info.canvasheight - popupheight)/2.f)
    };

    rlPushMatrix();
    rlTranslatef(popup_drawpos.x, popup_drawpos.y, 0);

    Vector2 textdrawpos1 = CLITERAL(Vector2) {
        .x=((popupwidth - textdim1.x)/2.f),
        .y=((popupheight - textdim1.y)/2.f - fontheight)
    };

    Vector2 textdrawpos2 = CLITERAL(Vector2) {
        .x=((popupwidth - textdim2.x)/2.f),
        .y=((popupheight - textdim2.y)/2.f - fontheight)
    };

    // background rectangle
    Rectangle popup_bkg = CLITERAL(Rectangle) {
        .x=-0.1f*popupwidth,
        .y=-0.1f*popupheight,
        .width=popupwidth,
        .height=popupheight
    };
    DrawRectangleRec(popup_bkg, popup_color);
    DrawRectangleLinesEx(popup_bkg, 5, RED);

    DrawTextEx(GetFontDefault(), gameover_text, textdrawpos1, 
            info.fontsize, info.fontspacing, RED);
    DrawTextEx(GetFontDefault(), playagain_text, textdrawpos2, 
            info.fontsize, info.fontspacing, RED);

    rlPopMatrix();
}
#endif

void draw_tetris_game_over(TetrisGame *game) {
    UNUSED(game);

    rlPushMatrix();
    rlTranslatef(info.gameover_offset.x, info.gameover_offset.y, 0);

    const char *gameover_text = "Game Over!";
    DrawTextEx(GetFontDefault(), gameover_text, CLITERAL(Vector2){0,0},
            info.fontsize, info.fontspacing, RED);

    rlPopMatrix();
}

void draw_tetris_game_piece_sidebar(TetrisGame *game) {
    rlPushMatrix();
    rlTranslatef(info.sidebar_offset.x, info.sidebar_offset.y, 0);

    // draw upcoming pieces
    Vector2 drawvec = CLITERAL(Vector2) { 0, 0 };
    for (int i = 0; i < 5; ++i) {
        TetriminoType type = nextpiecelist_piecetype_n_ahead(&game->nextpiece_list, i);
        Tetrimino piece = tetrimino_new(type);
        Vector2 offset = offset_of_tetrimino_type(type, info.displayblocksidelen);
        draw_tetrimino(&piece, Vector2Add(drawvec, offset), info.displayblocksidelen, color_of_tetrimino_type(type));
        drawvec.y += 3 * info.displayblocksidelen;
    }
    drawvec.y += 2.5*info.displayblocksidelen;

    Rectangle upcomingpieces_border = CLITERAL(Rectangle) { 
        .x=(1/2.f)*-info.displayblocksidelen, 
        .y=-info.displayblocksidelen, 
        .width=5*info.displayblocksidelen, 
        .height=16*info.displayblocksidelen };
    DrawRectangleLinesEx(upcomingpieces_border, info.sidebar_borderwidth, BLACK);

    // draw hold piece
    Rectangle holdpiece_border = CLITERAL(Rectangle) {
        .x=(upcomingpieces_border.x), 
        .y=(drawvec.y - info.displayblocksidelen),
        .width=(upcomingpieces_border.width),
        .height=(4*info.displayblocksidelen),
    };
    DrawRectangleLinesEx(holdpiece_border, info.sidebar_borderwidth, BLACK);

    if (game->holdpiece.type == TT_EMPTY) {
        rlPopMatrix();
        return;
    }
    Tetrimino holdpiece = tetrimino_new(game->holdpiece.type);
    Vector2 offset = offset_of_tetrimino_type(game->holdpiece.type, info.displayblocksidelen);
    Color holdcolor = game->pieceheld ? GRAY : color_of_tetrimino_type(holdpiece.type);
    draw_tetrimino(&holdpiece, Vector2Add(drawvec, offset), info.displayblocksidelen, holdcolor);

    rlPopMatrix();
}

void draw_tetris_game(TetrisGame *game) {
    BeginDrawing();
    ClearBackground(TETRIS_RAYCLEAR);

    draw_tetris_game_board(game);
    draw_tetris_game_piece_sidebar(game);
    draw_tetris_game_score(game);

    if (game->state == TGS_GAME_OVER) draw_tetris_game_over(game);

    EndDrawing();
}
