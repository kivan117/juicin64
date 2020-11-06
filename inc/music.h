#ifndef __MUSIC_H__
#define __MUSIC_H__

#include <libdragon.h>
#include <mikmod.h>

#define MAX_SONGS 5

MODULE* play_song(int SongID);
void stop_song(MODULE *song);

#endif