#ifndef GAME_CONFIG_H
#define GAME_CONFIG_H

#define SCREEN_WIDTH  800
#define SCREEN_HEIGHT 600
#define FPS           60

#define WORLD_SCALE   30.0f
#define WORLD_BASE_X  50.0f
#define WORLD_BASE_Y  (SCREEN_HEIGHT - 60.0f)

typedef enum {
    STATE_MENU,
    STATE_PLAYING,
    STATE_PAUSE,
    STATE_GAME_OVER,
    STATE_EXIT
} GameState;

// Conversão de unidades matemática para pixels da tela
float scale_to_screen_x(float math_x);
float scale_to_screen_y(float math_y);

#endif 