#include "../include/game_config.h"

float scale_to_screen_x(float math_x) {
    return WORLD_BASE_X + (math_x * WORLD_SCALE);
}

float scale_to_screen_y(float math_y) {
    return WORLD_BASE_Y - (math_y * WORLD_SCALE);
}