#include "font.h"
#include "window.h"

#define TETRIS_FONT_SIZE_DEFAULT 32
#define TETRIS_FONT_LINE_SPACING_DEFAULT 32
#define TETRIS_FONT_SPACING_DEFAULT 8

Font g_font;
int  g_fontspacing;
int  g_fontsize;
int  g_fontlinespacing;
int  g_fontheight;

void font_init(void) {
    int scale = (((float) window_height()) / WINDOW_DEFAULT_HEIGHT);
    g_font = GetFontDefault();
    g_fontspacing     = (int) (scale * TETRIS_FONT_SPACING_DEFAULT);
    g_fontsize        = (int) (scale * TETRIS_FONT_SIZE_DEFAULT);
    g_fontlinespacing = (int) (scale * TETRIS_FONT_LINE_SPACING_DEFAULT);
    g_fontheight = MeasureTextEx(GetFontDefault(), "PLACEHOLDER_TEXT", g_fontsize, g_fontspacing).y;
    SetTextLineSpacing(g_fontlinespacing);
}
