#ifndef WINDOW_H
#define WINDOW_H

#define WINDOW_ASPECT_RATIO (3.f/4.f)
#define WINDOW_DEFAULT_HEIGHT 1000
#define WINDOW_DEFAULT_WIDTH ((WINDOW_DEFAULT_HEIGHT*(float)WINDOW_ASPECT_RATIO)
typedef struct WindowInfo {
    int         width;
    int         height;
} WindowInfo;

void window_init(float aspect);
void window_close(void);

int window_width(void);
int window_height(void);

#endif
