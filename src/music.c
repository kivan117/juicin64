#include "music.h"
#include "gamestate.h"

MODULE* play_song(GAME* game, int SongID)
{
    //background music
    MODULE *bgm = NULL;
    if(SongID < MAX_SONGS)
        bgm = game->songs[SongID];
    else
        bgm = game->songs[0];
    
    Player_Start(bgm);

    return bgm;
}

void stop_song(MODULE* song)
{
    audio_write_silence();
    audio_write_silence();
    Player_SetPosition(0);
    Player_Stop();
}