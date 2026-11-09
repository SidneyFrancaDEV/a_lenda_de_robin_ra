#ifndef HUD_H
#define HUD_H

#include "math_engine.h"
#include "../param_selector.h"
#include <stdbool.h>



#define TOP_PANEL_HEIGHT 50.0f
#define BOTTOM_PANEL_HEIGHT 150.0f


typedef struct {
	FunctionType function_type;
	FunctionParams a, b, c;

	int select_param; // 0 = a, 1 = b, 2 = c

	bool shoot_trigger;
} PlayerInput;

void render_hud();

#endif