#include "window.h"
#include "common.h"

static bool window_is_open = false;
static WindowInfo window_info;

void window_init(float scale) {
    if (window_is_open) window_close();

    window_info.height = scale * WINDOW_DEFAULT_HEIGHT;
    window_info.width = WINDOW_ASPECT_RATIO * window_info.height;

    InitWindow(window_info.width, window_info.height, "tetris");
    window_is_open = true;
}

void window_close(void) {
    if (window_is_open) CloseWindow();
}

int window_width(void) {
    if (!window_is_open) {
        fprintf(stderr, "window_width(): Window hasn't been initialized yet.\n");
        exit(1);
    }
    return window_info.width;
}

int window_height(void) {
    if (!window_is_open) {
        fprintf(stderr, "window_width(): Window hasn't been initialized yet.\n");
        exit(1);
    }
    return window_info.height;
}
