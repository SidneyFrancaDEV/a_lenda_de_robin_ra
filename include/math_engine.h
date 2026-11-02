#ifndef MATH_ENGINE_H
#define MATH_ENGINE_H


typedef enum {
    FUNC_NONE = 0,
    FUNC_LINEAR = 1,
    FUNC_QUADRATIC = 2,
    FUNC_EXPONENTIAL = 3,
    FUNC_LOGARITHMIC = 4,
    FUNC_SINUSOIDAL = 5
} FunctionType;

float calc_linear(float x, float a, float b, float c);
float calc_quadratic(float x, float a, float b, float c);
float calc_exponential(float x, float a, float b, float c);
float calc_logarithmic(float x, float a, float b, float c);
float calc_sinusoidal(float x, float a, float b, float c);

float calculate_trajectory(FunctionType type, float x, float a, float b, float c);



#endif