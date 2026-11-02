#include "../include/sprites_maneger.h"
#include <stdio.h>

void draw_player(PlayerSprite player){
	if (player.robin_ra == NULL)
		return;

	int width_sprite = al_get_bitmap_width(player.robin_ra) / COLUMNS_ROBIN_RA_SPRITE;
	int height_sprite = al_get_bitmap_height(player.robin_ra) / LINES_ROBIN_RA_SPRITE;

	al_draw_scaled_bitmap(
		player.robin_ra,
		player.current_sprite * (width_sprite), player.state * (height_sprite),
		width_sprite, height_sprite,
		player.position_X, player.position_Y,
		player.width, player.height,
		0
	);
}

void destroy_player(PlayerSprite* player) {
	if (player->robin_ra != NULL) {
		al_destroy_bitmap(player->robin_ra);
		player->robin_ra = NULL;
	}
}