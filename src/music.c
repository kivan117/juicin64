#include "music.h"
#include "gamestate.h"

static global_songs[MAX_SONGS];

MODULE* play_song(GAME* game, int SongID)
{
    //background music
    MODULE *bgm = NULL;
    if(SongID < MAX_SONGS)
        bgm = global_songs[SongID];
    else
        bgm = global_songs[0];
    
    Player_Start(bgm);

    return bgm;
}

void setup_songs(void)
{
    global_songs[0] = Player_Load("rom://music/enter_sand.mod", 127, 0);
    global_songs[1] = Player_Load("rom://music/teen_spirit.mod", 127, 0);
    global_songs[2] = Player_Load("rom://music/basket_case.mod", 127, 0);
    global_songs[3] = Player_Load("rom://music/sweet_child.mod", 127, 0);
    global_songs[4] = Player_Load("rom://music/toxicity.mod", 127, 0);
}

void cleanup_songs(void)
{
    Player_Stop();
    for(int it = 0; it < MAX_SONGS; it++)
    {
        Player_Free(global_songs[it]);
    } 
}

void stop_song(MODULE* song)
{
    audio_write_silence();
    audio_write_silence();
    Player_SetPosition(0);
    Player_Stop();
}