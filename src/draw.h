#ifndef TETRIS_DRAW_H
#define TETRIS_DRAW_H

#include "game.h"

Color color_of_tetrimino_type(TetriminoType const type);

void draw_block(Position const pos, int const sidelen, Color const color);
void draw_tetrimino(Tetrimino *tetrimino, Position const pos, int const sidelen);
void draw_tetris_game(TetrisGame *game);

#endif
