#ifndef __MUSIC_H__
#define __MUSIC_H__

#include <libdragon.h>
#include <mikmod.h>
#include "gamestate.h"

MODULE* play_song(GAME* game, int SongID);
void stop_song(MODULE *song);
void setup_songs(void);
void cleanup_songs(void);

#endif