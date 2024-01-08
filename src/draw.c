#include "draw.h"
#include "common.h"

#define TETRIS_RAYCLEAR (CLITERAL(Color) {(RAYWHITE).r, (RAYWHITE).g, (RAYWHITE).b, 120})

#define TETRIS_FONT_IOSEVKA_FS_LOCATION "./iosevka.ttc"
static Font s_font_iosevka;

#define TETRIS_FONT_SIZE 32
#define TETRIS_FONT_SPACING 5
Font tetris_get_font(void) {
    return GetFontDefault();
}

void tetris_load_fonts(void) {
    s_font_iosevka = LoadFontEx(TETRIS_FONT_IOSEVKA_FS_LOCATION, TETRIS_FONT_SIZE, NULL, 0);

    SetTextLineSpacing(TETRIS_FONT_SIZE);
}

void tetris_unload_fonts(void) {
    UnloadFont(s_font_iosevka);
}

Color color_of_tetrimino_type(TetriminoType const type) {
    switch (type) {
    case TT_STRAIGHT: return PURPLE;
    case TT_SQUARE:   return GREEN;
    case TT_T:        return YELLOW;
    case TT_L1:       return ORANGE;
    case TT_L2:       return RED;
    case TT_SKEW1:    return BLUE;
    case TT_SKEW2:    return VIOLET;
    default: {
        fprintf(stderr, "Invalid tetrimino passed to color_of_tetrimino().");
        exit(1);
    }
    }
#ifdef __GNUC__
    __builtin_unreachable();
#endif
}

void draw_block(Position const pos, int const sidelen, Color const color) {
    Rectangle block = CLITERAL(Rectangle) {
        .x=pos.col*sidelen, 
        .y=pos.row*sidelen,
        .width=sidelen, 
        .height=sidelen
    };
    DrawRectangleRec(block, color);
    DrawRectangleLinesEx(block, 1, BLACK);
}

void draw_tetrimino(Tetrimino *tetrimino, Position const pos, int const sidelen) {
    for (int r = 0; r < 4; ++r) {
        for (int c = 0; c < 4; ++c) {
            if (tetrimino->layout[r][c]) {
                Position const blockpos = { pos.row+r, pos.col+c };
                Color const color = color_of_tetrimino_type(tetrimino->type);
                draw_block(blockpos, sidelen, color);
            }
        }
    }
}

void draw_tetris_game_board(TetrisGame *game) {
    int const sidelen = game->canvas.texture.width / game->cols / 2;

    // draw gridlines
    for (int r = 0; r <= game->rows; ++r)
        DrawLine(0, r * sidelen, game->canvas.texture.width/2, r * sidelen, BLACK);
    for (int c = 0; c <= game->cols; ++c)
        DrawLine(c * sidelen, 0, c * sidelen, game->canvas.texture.height, BLACK);

    // draw placed tetriminos
    for (int r = 0; r < game->rows; ++r) {
        for (int c = 0; c < game->cols; ++c) {
            if (game->board[r][c] != TT_EMPTY) {
                Position const pos = { r, c };
                Color const color = color_of_tetrimino_type(game->board[r][c]);
                draw_block(pos, sidelen, color);
            }
        }
    }

    // draw active tetrimino
    draw_tetrimino(&game->activepiece, game->activepiece_pos, sidelen);
}

void draw_tetris_game_score(TetrisGame *game) {
    // draw score
    char scoretext[TETRIS_GAME_MAX_SCORE_TEXT_LEN];
    const char *dummytext = "Score: 9999999999\nLevel: 9999999999\nRows: 9999999999\n";
    sprintf(scoretext, "Score: %d\nLevel: %d\nRows: %d", 
            game->score, game->level, game->rowscompleted);
    Vector2 textdim = MeasureTextEx(tetris_get_font(), dummytext, 
            TETRIS_FONT_SIZE, TETRIS_FONT_SPACING);
    Vector2 const drawpos = CLITERAL(Vector2) { 
        .x=(BOARD_CANVAS_WIDTH - textdim.x),
        .y=textdim.y 
    };
    DrawTextEx(tetris_get_font(), scoretext, drawpos,
            TETRIS_FONT_SIZE, TETRIS_FONT_SPACING, BLACK);
}

void draw_tetris_game_over(TetrisGame *game) {
    UNUSED(game);
    const char *gameover_text = "Game Over!\nPress Spacebar to play again.";
    const int popup_border_size = 10;
    const Color popup_color = GRAY;
    Vector2 textdim = MeasureTextEx(tetris_get_font(), gameover_text, 
            TETRIS_FONT_SIZE, TETRIS_FONT_SPACING);

    Vector2 textdrawpos = CLITERAL(Vector2) {
        .x=((BOARD_CANVAS_WIDTH/2)-textdim.x/2),
        .y=((BOARD_CANVAS_HEIGHT/2)-textdim.y/2),
    };

    // background rectangle
    Rectangle popup_bkg = CLITERAL(Rectangle) {
        .x=(textdrawpos.x-popup_border_size),
        .y=(textdrawpos.y-popup_border_size),
        .width=(textdim.x+2*popup_border_size),
        .height=(textdim.y+2*popup_border_size)
    };
    DrawRectangleRec(popup_bkg, popup_color);

    // border rectangle
    Rectangle popup_border = CLITERAL(Rectangle) {
        .x=(popup_bkg.x+popup_border_size/2),
        .y=(popup_bkg.y+popup_border_size/2),
        .width=(popup_bkg.width-popup_border_size),
        .height=(popup_bkg.height-popup_border_size)
    };
    UNUSED(popup_border);
    DrawRectangleLinesEx(popup_bkg, 5, RED);

    DrawTextEx(tetris_get_font(), gameover_text, textdrawpos, 
            TETRIS_FONT_SIZE, TETRIS_FONT_SPACING, RED);
}

void draw_tetris_game_piece_bag(TetrisGame *game) {
    UNUSED(game);
    UNIMPLEMENTED();
}

void draw_tetris_game(TetrisGame *game) {
    BeginTextureMode(game->canvas);
    ClearBackground(TETRIS_RAYCLEAR);

    draw_tetris_game_board(game);
    draw_tetris_game_score(game);

    if (game->state == TGS_GAME_OVER)
        draw_tetris_game_over(game);

    EndTextureMode();
}
