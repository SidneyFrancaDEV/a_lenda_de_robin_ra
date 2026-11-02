#ifndef SPRITES_MANEGER_H 
#define SPRITES_MANEGER_H

#define COLUMNS_ROBIN_RA_SPRITE 8
#define LINES_ROBIN_RA_SPRITE 4

#include <allegro5/allegro5.h>

typedef enum {
    DEFAULT = 0,
    ATTACK =  1,
    VICTORY = 2,
    DEFAET =  3,
}CharacterState;

typedef struct {
	ALLEGRO_BITMAP* robin_ra;
    float position_X;
    float position_Y;
	float width;
    float height;
    int current_sprite;
    CharacterState state;
}PlayerSprite;

void draw_player(PlayerSprite player);
void destroy_player(PlayerSprite* player);

#endif