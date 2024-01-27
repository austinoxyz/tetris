#include "menu.h"
#include "game.h"
#include "window.h"

extern TetrisGame g_game;
extern MainMenu g_mainmenu;

void mainmenu_init(void) {
    int buttonwidth = 0.60f * window_width();
    int buttonheight = 0.15f * window_height();
    int buttonvertgap = 0.05f * window_height();

    int xoffset = (window_width() - buttonwidth) / 2;

    g_mainmenu.buttons.settings = CLITERAL(Button) {
        .bounds=(CLITERAL(Rectangle) {
            .x=(xoffset), 
            .y=((window_height()-buttonheight)/2 + 0.08*window_height()), 
            .width=(buttonwidth), 
            .height=(buttonheight)
        }),
        .color=GRAY,
        .text="SETTINGS"
    };

    g_mainmenu.buttons.play = CLITERAL(Button) {
        .bounds=(CLITERAL(Rectangle) {
            .x=(xoffset), 
            .y=(g_mainmenu.buttons.settings.bounds.y-(buttonheight+buttonvertgap)), 
            .width=(buttonwidth), 
            .height=(buttonheight)
        }),
        .color=GRAY,
        .text="PLAY"
    };

    g_mainmenu.buttons.quit = CLITERAL(Button) {
        .bounds=(CLITERAL(Rectangle) {
            .x=(xoffset), 
            .y=(g_mainmenu.buttons.settings.bounds.y+(buttonheight+buttonvertgap)), 
            .width=(buttonwidth), 
            .height=(buttonheight)
        }),
        .color=GRAY,
        .text="QUIT"
    };

    g_mainmenu.buttoncolor = GRAY;
    g_mainmenu.highlight_buttoncolor = ORANGE;
}

bool button_is_clicked(Vector2 clickpos, Button button) {
    return (clickpos.x >= button.bounds.x && clickpos.x <= button.bounds.x + button.bounds.width)
        && (clickpos.y >= button.bounds.y && clickpos.y <= button.bounds.y + button.bounds.height);
}

void mainmenu_handle_input(void) {
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        Vector2 clickpos = GetMousePosition();
        if (button_is_clicked(clickpos, g_mainmenu.buttons.play)) {
            printf("Clicked play button!\n");
            g_game.state = TGS_IN_PLAY;
        } else if (button_is_clicked(clickpos, g_mainmenu.buttons.settings)) {
            printf("Clicked settings button!\n");
            // TODO
        } else if (button_is_clicked(clickpos, g_mainmenu.buttons.quit)) {
            printf("Clicked quit button!\n");
            quit();
        }
    }
}

