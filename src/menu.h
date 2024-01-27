#ifndef MENU_H
#define MENU_H

#include "util.h"

typedef enum MenuOptionType {
    MOT_PLAY,
    MOT_SETTINGS,
    MOT_QUIT,
    MOT_COUNT
} MenuOptionType;

typedef struct Button {
    Rectangle      bounds;
    Color          color;
} Button;

typedef struct Menu {
    struct {
        Button play;
        Button settings;
        Button quit;
    } buttons;
} Menu;

void menu_init(Menu *menu);
void menu_handle_input(Menu *menu);

#endif
