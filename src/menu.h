#ifndef MENU_H
#define MENU_H

#include "util.h"

#if 0
typedef enum MenuOptionType {
    MOT_PLAY,
    MOT_SETTINGS,
    MOT_QUIT,
    MOT_COUNT
} MenuOptionType;
#endif

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

void mainmenu_init(void);
void mainmenu_handle_input(void);

#endif
