#ifndef PARAM_SELECTOR_H
#define PARAM_SELECTOR_H

#include <stdbool.h>

typedef enum {
	ADDITION = 0,
	SUBTRACTION = 1
}MathOperation;

typedef struct {
	float position_x;
	float position_y;
	float widht;
	float height;

	MathOperation operation;
}Button;

typedef struct {
	float value;
	bool state;

	float max_value;
	float min_value;
}FunctionParams;

bool mouse_verificator(Button button, float mouse_x, float mouse_y);
void param_operation(FunctionParams* param, Button button);

#endif // !PARAM_SELECTOR_H
