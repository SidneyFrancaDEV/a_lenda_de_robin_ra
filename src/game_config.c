#include "../include/game_config.h"
#include <allegro5/allegro5.h>


float scale_to_screen_x(float math_x) {
    float width = SCREEN_WIDTH - WORLD_BASE_X - GRAPH_GAP;
    return WORLD_BASE_X + math_x * (width / WORLD_MAX);
}

float scale_to_screen_y(float math_y) {
    float height = WORLD_BASE_Y - TOP_PANEL_HEIGHT - GRAPH_GAP;
    return WORLD_BASE_Y - math_y * (height / WORLD_MAX);
}