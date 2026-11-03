#include "../include/param_selector.h"
#include <stdbool.h>


bool mouse_verificator(Button button, float mouse_x, float mouse_y) {
	if ((mouse_x >= button.position_x && mouse_x <= (button.position_x + button.widht)) &&
		(mouse_y >= button.position_y && mouse_y <= (button.position_y + button.height)))
		return true;
	else
		return false;
}

void param_operation(FunctionParams* param, Button button){

		if (param->state && button.operation == ADDITION) {
			if (param->value < param->max_value) {
				param->value += 1.0f;
			}
		}

		if (param->state && button.operation == SUBTRACTION) {
			if (param->value >= param->min_value) {
				param->value -= 1.0f;
			}
		}
}