#include "menu.h"
#include "game.h"
#include "window.h"

extern TetrisGame g_game;

void menu_init(Menu *menu) {
    int buttonwidth = 0.60f * window_width();
    int buttonheight = 0.25f * window_height();
    int buttonvertgap = 0.8f * window_height();

    int xoffset = (window_width() - buttonwidth) / 2;

    menu->buttons.settings.bounds = CLITERAL(Rectangle) {
        xoffset, (window_height()-buttonheight)/2, buttonwidth, buttonheight
    };

    menu->buttons.play.bounds = CLITERAL(Rectangle) {
        xoffset, menu->buttons.settings.bounds.y-(buttonheight+buttonvertgap), buttonwidth, buttonheight
    };

    menu->buttons.quit.bounds = CLITERAL(Rectangle) {
        xoffset, menu->buttons.settings.bounds.y+(buttonheight+buttonvertgap), buttonwidth, buttonheight
    };
}

MenuOptionType menu_wait_for_click(void);

void menu_handle_input(Menu *menu) {
    UNUSED(menu);
}

