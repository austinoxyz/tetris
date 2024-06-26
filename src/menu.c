#include "menu.h"
#include "font.h"
#include "game.h"
#include "window.h"

GLOBAL MainMenu g_mainmenu;

Button button_new(Rectangle _bounds, const char *_text, int _fontsize, Color _color) {
    Button result;
    result.bounds   = _bounds;
    result.text     = _text;
    result.color    = _color;
    result.fontsize = _fontsize;

    Vector2 textdim = MeasureTextEx(g_font, _text, _fontsize, g_fontspacing);
    result.textpos = (CLITERAL(Vector2) {
        .x=(_bounds.x + _bounds.width/2  - textdim.x/2),
        .y=(_bounds.y + _bounds.height/2 - textdim.y/2)
    });

    return result;
}

bool button_is_clicked(Vector2 clickpos, Button button) {
    return (clickpos.x >= button.bounds.x && clickpos.x <= button.bounds.x + button.bounds.width)
        && (clickpos.y >= button.bounds.y && clickpos.y <= button.bounds.y + button.bounds.height);
}

void mainmenu_init(void) {
    int buttonwidth   = 0.60f * window_width();
    int buttonheight  = 0.15f * window_height();
    int buttonvertgap = 0.05f * window_height();

    int xoffset = (window_width() - buttonwidth) / 2;

    g_mainmenu.buttons.highscores = button_new(
        (CLITERAL(Rectangle) {
            .x=(xoffset), 
            .y=((window_height()-buttonheight)/2 + 0.15*window_height()), 
            .width=(buttonwidth), 
            .height=(buttonheight)
        }), "HIGHSCORES", 50, GRAY);

    g_mainmenu.buttons.play = button_new(
        (CLITERAL(Rectangle) {
            .x=(xoffset), 
            .y=(g_mainmenu.buttons.highscores.bounds.y-(buttonheight+buttonvertgap)), 
            .width=(buttonwidth), 
            .height=(buttonheight)
        }), "PLAY", 60, GRAY);

    g_mainmenu.buttons.quit = button_new(
        (CLITERAL(Rectangle) {
            .x=(xoffset), 
            .y=(g_mainmenu.buttons.highscores.bounds.y+(buttonheight+buttonvertgap)), 
            .width=(buttonwidth), 
            .height=(buttonheight)
        }), "QUIT", 60, GRAY);

    g_mainmenu.buttoncolor = GRAY;
    g_mainmenu.highlight_buttoncolor = ORANGE;
}

void mainmenu_handle_input(void) {
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        Vector2 clickpos = GetMousePosition();
        if (button_is_clicked(clickpos, g_mainmenu.buttons.play)) {
            g_game.state = TGS_IN_PLAY;
        } else if (button_is_clicked(clickpos, g_mainmenu.buttons.highscores)) {
            g_game.state = TGS_HIGHSCORES;
        } else if (button_is_clicked(clickpos, g_mainmenu.buttons.quit)) {
            quit(0);
        }
    }
}

