#include "../include/level_manager.h"
#include "../include/game_config.h"

#define ARRAY_COUNT(array) ((int)(sizeof(array) / sizeof((array)[0])))

static const CollisionArea level_1_obstacles[] = {
    { 2.4f, 0.0f, 3.6f, 0.6f }
};
 
static const CollisionArea level_2_obstacles[] = {
    { 0.8f, 0.0f, 1.1f, 4.5f }
};

// A ordem desse cara define a ordem dos niveis.
const LevelDefinition levels[] = {
    {
        "RASANTE", "Passe sobre o bloco baixo e acerte o alvo distante.",
        FUNC_QUADRATIC, -0.1f, 0.4f, 0.5f,
        { 5.55f, 0.08f, 5.85f, 0.35f },
        level_1_obstacles, ARRAY_COUNT(level_1_obstacles)
    },
    {
        "ARCO ALTO", "Passe sobre o bloco alto e acerte o alvo perto.",
        FUNC_QUADRATIC, -1.5f, 6.0f, 0.5f,
        { 1.9f, 7.6f, 2.2f, 8.2f },
        level_2_obstacles, ARRAY_COUNT(level_2_obstacles)
    }
};

const int level_count = ARRAY_COUNT(levels);

static bool arrow_is_inside_area(float x, float y, CollisionArea area) {
    return x >= area.x_min && x <= area.x_max &&
        y >= area.y_min && y <= area.y_max;
}

ShotResult level_check_shot(const LevelDefinition* level, float x, float y) {
    CollisionArea field = { 0.0f, 0.0f, WORLD_MAX, WORLD_MAX };
    if (!arrow_is_inside_area(x, y, field))
        return SHOT_MISSED;

    for (int i = 0; i < level->obstacle_count; i++) {
        if (arrow_is_inside_area(x, y, level->obstacles[i]))
            return SHOT_HIT_OBSTACLE;
    }

    if (arrow_is_inside_area(x, y, level->target))
        return SHOT_HIT_TARGET;

    return SHOT_IN_FLIGHT;
}
