#ifndef GAME_CONFIG_H
#define GAME_CONFIG_H



#include "../hud.h"

#define SCREEN_WIDTH  1024
#define SCREEN_HEIGHT 768
#define FPS           60

#define WORLD_SCALE   30.0f
#define WORLD_BASE_X  50.0f
#define WORLD_BASE_Y  (SCREEN_HEIGHT - BOTTOM_PANEL_HEIGHT)

// Conversão de unidades matemática para pixels da tela
float scale_to_screen_x(float math_x);
float scale_to_screen_y(float math_y);

#endif 