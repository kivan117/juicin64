#ifndef __ENGINE_H__
#define __ENGINE_H__

#include "gamestate.h"

void update_controller(GAME* game);
void update_logic(GAME* game);
void update_graphics(GAME* game);
void update_audio(GAME* game);
void cleanup_main_game(GAME* game);

#endif