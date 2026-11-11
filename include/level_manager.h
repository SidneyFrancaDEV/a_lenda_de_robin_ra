#ifndef LEVEL_MANAGER_H
#define LEVEL_MANAGER_H

#include <stdbool.h>
#include "math_engine.h"

typedef struct {
    float x_min;
    float y_min;
    float x_max;
    float y_max;
} CollisionArea;

typedef struct {
    const char* title;
    const char* objective;
    FunctionType function_type;
    float initial_a;
    float initial_b;
    float initial_c;
    CollisionArea target;
    const CollisionArea* obstacles;
    int obstacle_count;
} LevelDefinition;

typedef enum {
    SHOT_IN_FLIGHT,
    SHOT_HIT_TARGET,
    SHOT_HIT_OBSTACLE,
    SHOT_MISSED
} ShotResult;

extern const LevelDefinition levels[];
extern const int level_count;
ShotResult level_check_shot(const LevelDefinition* level, float x, float y);

#endif
