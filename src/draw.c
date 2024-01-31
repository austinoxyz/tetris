#include "draw.h"

#include "window.h"
#include "font.h"
#include "game.h"
#include "menu.h"
#include "highscores.h"

// for ghost piece
extern Position tetris_game_find_hard_drop_position(TetrisGame *);

#define BLOCK_POS(row, col, sidelen) (CLITERAL(Vector2){(col)*(sidelen),(row)*(sidelen)})

static Color bkgcolor, backdropcolor;

static int canvaswidth, canvasheight;

static float scale;

static float boardaspect;
static int boardwidth, boardheight;
static Vector2 board_offset;
static int blocksidelen;

static int displayblocksidelen;
static float displaypiece_scale;

static Rectangle upcoming_bounds;
static Rectangle holdpiece_bounds;

static float sidebar_borderwidth;
static Vector2 sidebar_offset;

static Vector2 score_offset;

static Vector2 gameover_offset;

static float logoscale;
static Texture2D logo;
static Vector2 logo_offset;

static Vector2 highscores_start_offset;

static Color gridlines_color;

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

void draw_init(void) {
    canvaswidth = window_width();
    canvasheight = window_height();

    scale = (((float) canvasheight) / WINDOW_DEFAULT_HEIGHT);
    displaypiece_scale = 1.2f;

    boardaspect  = ((float)g_game.cols+2)/((float)g_game.rows+2);
    boardheight  = 0.75f * canvasheight;
    boardwidth   = boardaspect * canvaswidth;
    blocksidelen = boardwidth / ((float)g_game.cols+2);
    displayblocksidelen = displaypiece_scale * blocksidelen;

    board_offset = CLITERAL(Vector2) {
        .x=(0.05f * canvaswidth), 
        .y=(0.05f * canvasheight) };
    sidebar_offset = CLITERAL(Vector2) {
        .x=((2*board_offset.x + boardwidth + canvaswidth)/2.f 
            - 1.5f*displaypiece_scale*blocksidelen - board_offset.x),
        .y=(boardheight - 14*blocksidelen)/2.f };
    score_offset = CLITERAL(Vector2) {
        .x=(board_offset.x),
        .y=(1.5f*board_offset.y + boardheight) 
    };
    gameover_offset = CLITERAL(Vector2) {
        .x=(score_offset.x),
        .y=(score_offset.y+2*g_fontheight)
    };

    sidebar_borderwidth = 2;

    logo = LoadTexture("tetris-logo.png");
    logoscale = 0.3f; // TODO: Scale this properly
    logo_offset = CLITERAL(Vector2) {
        .x=((window_width() - logoscale*logo.width)/2),
        .y=((g_mainmenu.buttons.play.bounds.x - logoscale*logo.height)/2 + logoscale*logo.height/2 - 0.02*window_height()),
    };

    bkgcolor = DARKBLUE;
    backdropcolor = RAYWHITE;

    upcoming_bounds = CLITERAL(Rectangle) {
        .x=((1/2.f)*-displayblocksidelen),
        .y=(-displayblocksidelen),
        .width=(5*displayblocksidelen),
        .height=(16*displayblocksidelen) };

    holdpiece_bounds = CLITERAL(Rectangle) {
        .x=(upcoming_bounds.x), 
        .y=(16.5*displayblocksidelen),
        .width=(upcoming_bounds.width),
        .height=(4*displayblocksidelen),
    };

    highscores_start_offset = CLITERAL(Vector2) {
        .x=(0.08f*window_width()),
        .y=(0.05f*window_height()),
    };

    gridlines_color = hex_to_color(0x2B2B2B80);
}

void draw_block(Vector2 pos, float sidelen, Color color) {
    ++pos.x;
    // FIXME: this needs to be incremented on one machine and doesn't on another. 
    //        it is likely a result of improper normalizing of measurements
    //        with regards to the window dimensions
    //++pos.y;
    DrawRectangleV(pos, CLITERAL(Vector2) {sidelen-1,sidelen-1}, color);
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

void draw_board_gridlines(void) {
    DrawLine(0, 0, (g_game.cols+2)*blocksidelen, 0, BLACK);
    DrawLine(blocksidelen, blocksidelen, (g_game.cols+1)*blocksidelen, blocksidelen, BLACK);

    for (int r = 1; r <= g_game.rows+1; ++r)
        DrawLine(0, r * blocksidelen, (g_game.cols+2)*blocksidelen, r * blocksidelen, gridlines_color);

    DrawLine(blocksidelen, (g_game.rows+1)*blocksidelen, (g_game.cols+1)*blocksidelen, (g_game.rows+1)*blocksidelen, BLACK);
    DrawLine(           0, (g_game.rows+2)*blocksidelen, (g_game.cols+2)*blocksidelen, (g_game.rows+2)*blocksidelen, BLACK);

    DrawLine(0, 0, 0, (g_game.rows+2)*blocksidelen, BLACK);
    DrawLine(blocksidelen, blocksidelen, blocksidelen, (g_game.rows+1)*blocksidelen, BLACK);

    for (int c = 1; c <= g_game.cols+1; ++c)
        DrawLine(c * blocksidelen, 0, c * blocksidelen, (g_game.rows+2)*blocksidelen, gridlines_color);

    DrawLine((g_game.cols+1)*blocksidelen, blocksidelen, (g_game.cols+1)*blocksidelen, (g_game.rows+1)*blocksidelen, BLACK);
    DrawLine((g_game.cols+2)*blocksidelen,            0, (g_game.cols+2)*blocksidelen, (g_game.rows+2)*blocksidelen, BLACK);
}

void draw_board(void) {
    // Translate to game board position
    rlPushMatrix();
    rlTranslatef(board_offset.x, board_offset.y, 0);

    // draw backdrop
    DrawRectangleRec((CLITERAL(Rectangle) {
        .x=0, .y=0,
        .width=(blocksidelen*(g_game.cols+2)),
        .height=(blocksidelen*(g_game.rows+2)),
    }), backdropcolor);

    draw_board_gridlines();

    // draw boundary
    for (int c = 0; c < g_game.cols+2; ++c) {
        draw_block(BLOCK_POS(0, c, blocksidelen), blocksidelen, GRAY);
        draw_block(BLOCK_POS(g_game.rows+1, c, blocksidelen), blocksidelen, GRAY);
    }
    for (int r = 1; r <= g_game.rows+1; ++r) {
        draw_block(BLOCK_POS(r, 0, blocksidelen), blocksidelen, GRAY);
        draw_block(BLOCK_POS(r, g_game.cols+1, blocksidelen), blocksidelen, GRAY);
    }

    // draw placed tetriminos
    for (int r = 0; r < g_game.rows; ++r) {
        for (int c = 0; c < g_game.cols; ++c) {
            if (g_game.board[r][c] != TT_EMPTY) {
                Color const color = color_of_tetrimino_type(g_game.board[r][c]);
                Position const pos = { r+1, c+1 };
                Vector2 drawpos = BLOCK_POS(pos.row, pos.col, blocksidelen);
                draw_block(drawpos, blocksidelen, color);
            }
        }
    }

    // draw active and ghost pieces
    Position drawpos = g_game.activepiece_pos;
    Position droppos = tetris_game_find_hard_drop_position(&g_game);

    ++drawpos.row, ++drawpos.col;
    ++droppos.row, ++droppos.col;

    Vector2 piecevec = BLOCK_POS(drawpos.row, drawpos.col, blocksidelen);
    Vector2 ghostvec = BLOCK_POS(droppos.row, droppos.col, blocksidelen);

    Color color = color_of_tetrimino_type(g_game.activepiece.type);
    Color ghostcolor = color;
    ghostcolor.a /= 2;

    draw_tetrimino(&g_game.activepiece, piecevec, blocksidelen, color);
    draw_tetrimino(&g_game.activepiece, ghostvec, blocksidelen, ghostcolor);

    rlPopMatrix();
}

void draw_score(void) {
    rlPushMatrix();
    rlTranslatef(score_offset.x, score_offset.y, 0);

    // draw score
    char scoretext[TETRIS_GAME_SCORE_TEXT_MAX_LEN];
    sprintf(scoretext, "Level: %d\nScore: %d", g_game.level, g_game.score);
    DrawTextEx(g_font, scoretext, CLITERAL(Vector2){0,0},
            g_fontsize, g_fontspacing, BLACK);

    rlPopMatrix();
}

void draw_game_over(void) {
    DrawTextEx(GetFontDefault(), "Game Over!", gameover_offset, g_fontsize * 1.5, g_fontspacing, RED);
}

void draw_piece_sidebar(void) {
    rlPushMatrix();
    rlTranslatef(sidebar_offset.x, sidebar_offset.y, 0);

    // draw upcoming pieces
    DrawRectangleRec(upcoming_bounds, backdropcolor);
    Vector2 drawvec = CLITERAL(Vector2) { 0, 0 };
    for (int i = 0; i < 5; ++i) {
        TetriminoType type = nextpiecelist_piecetype_n_ahead(&g_game.nextpiece_list, i);
        Tetrimino piece = tetrimino_new(type);
        Vector2 offset = offset_of_tetrimino_type(type, displayblocksidelen);
        draw_tetrimino(&piece, Vector2Add(drawvec, offset), displayblocksidelen, color_of_tetrimino_type(type));
        drawvec.y += 3 * displayblocksidelen;
    }
    drawvec.y += 2.5*displayblocksidelen;
    DrawRectangleLinesEx(upcoming_bounds, sidebar_borderwidth, BLACK);

    // draw hold piece
    DrawRectangleRec(holdpiece_bounds, backdropcolor);
    DrawRectangleLinesEx(holdpiece_bounds, sidebar_borderwidth, BLACK);
    if (g_game.holdpiece.type == TT_EMPTY) {
        rlPopMatrix();
        return;
    }
    Tetrimino holdpiece = tetrimino_new(g_game.holdpiece.type);
    Vector2 offset = offset_of_tetrimino_type(g_game.holdpiece.type, displayblocksidelen);
    Color holdcolor = g_game.pieceheld ? GRAY : color_of_tetrimino_type(holdpiece.type);
    draw_tetrimino(&holdpiece, Vector2Add(drawvec, offset), displayblocksidelen, holdcolor);

    rlPopMatrix();
}

void draw_button(Button *button) {
    DrawRectangleRec(button->bounds, button->color);
    DrawRectangleLinesEx(button->bounds, 2, BLACK);
//    DrawTextEx(GetFontDefault(), button->text, button->textpos, button->fontsize, g_fontspacing, BLACK);
    DrawTextEx(g_font, button->text, button->textpos, button->fontsize, g_fontspacing, BLACK);
}

void draw_mainmenu(void) {
    if (g_game.state != TGS_MAIN_MENU) return;

    BeginDrawing();
    ClearBackground(bkgcolor);
        DrawTextureEx(logo, logo_offset, 0, logoscale, RAYWHITE);
        draw_button(&g_mainmenu.buttons.play);
        draw_button(&g_mainmenu.buttons.highscores);
        draw_button(&g_mainmenu.buttons.quit);
    EndDrawing();
}

void draw_highscores(void) {
    if (g_game.state != TGS_HIGHSCORES) return;

    int const highscores_fontsize    = 2*g_fontsize;
    int const highscores_fontspacing = g_fontspacing;
    Vector2 offset = CLITERAL(Vector2) { 0, 0 };
    Vector2 textdim = MeasureTextEx(g_font, "PLACEHOLDER_TEXT", highscores_fontsize, highscores_fontspacing);
    char tmpbuf[20];

    BeginDrawing();
    ClearBackground(bkgcolor);
        for (int i = 0; i < g_highscores.count; ++i) {
            DrawTextEx(g_font, g_highscores.entries[i].name, Vector2Add(highscores_start_offset, offset), 
                       highscores_fontsize, highscores_fontspacing, BLACK);
            offset.x += 0.25f*window_width();
            sprintf(tmpbuf, "%d", g_highscores.entries[i].score);
            DrawTextEx(g_font, tmpbuf, Vector2Add(highscores_start_offset, offset), 
                       highscores_fontsize, highscores_fontspacing, BLACK);
            offset.x += 0.25f*window_width();
            strftime(tmpbuf, sizeof tmpbuf, "%D", gmtime(&g_highscores.entries[i].time.tv_sec));
            DrawTextEx(g_font, tmpbuf, Vector2Add(highscores_start_offset, offset), 
                       highscores_fontsize, highscores_fontspacing, BLACK);
            offset.x = 0;
            offset.y += textdim.y;
        }
    EndDrawing();
}

void draw_new_highscore_dialog() {
}

void draw_game(void) {
    if (g_game.state == TGS_MAIN_MENU) return;

    BeginDrawing();
    ClearBackground(bkgcolor);
        draw_board();
        draw_piece_sidebar();
        draw_score();
        if (g_game.state == TGS_NEW_HIGHSCORE) 
            draw_new_highscore_dialog();
        else if (g_game.state == TGS_GAME_OVER)
            draw_game_over();
    EndDrawing();
}
