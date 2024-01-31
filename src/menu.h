#ifndef MENU_H
#define MENU_H

#include "common.h"

typedef struct Button {
    Rectangle      bounds;
    int            fontsize;
    const char*    text;
    Vector2        textpos;
    Color          color;
} Button;

Button button_new(Rectangle, const char *, int, Color);

typedef struct MainMenu {
    struct {
        Button play;
        Button highscores;
        Button quit;
    } buttons;
    Color buttoncolor, highlight_buttoncolor;
} MainMenu;

GLOBAL extern MainMenu g_mainmenu;

void mainmenu_init(void);
void mainmenu_handle_input(void);

#endif
