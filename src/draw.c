#include "draw.h"
#include "common.h"

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
    DrawRectangleLinesEx(block, 1.2, BLACK);
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

void draw_tetris_game(TetrisGame *game) {
    BeginTextureMode(game->canvas);
    ClearBackground(RAYWHITE);

    int const sidelen = game->canvas.texture.width / game->cols;

    // draw gridlines
    for (int r = 0; r <= game->rows; ++r)
        DrawLine(0, r * sidelen, game->canvas.texture.width, r * sidelen, BLACK);
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

    EndTextureMode();
}
