#ifndef __GYM_H__
#define __GYM_H__

#include <libdragon.h>

void draw_gym(display_context_t disp_list, sprite_t** tiles);
void draw_bottom_wall(display_context_t disp_list, void* wall_sm);

#endif