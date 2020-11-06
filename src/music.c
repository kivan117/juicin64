#include "music.h"

MODULE* play_song(int SongID)
{
    //background music
    MODULE *bgm = NULL;
    switch(SongID)
    {
        case(0):
            bgm = Player_Load("rom://music/enter_sand.mod", 16, 0);
            break;
        case(1):
            bgm = Player_Load("rom://music/basket_case.mod", 16, 0);
            break;
        case(2):
            bgm = Player_Load("rom://music/teen_spirit.mod", 16, 0);
            break;
        case(3):
            bgm = Player_Load("rom://music/sweet_child.mod", 16, 0);
            break;
        case(4):
            bgm = Player_Load("rom://music/toxicity.mod", 16, 0);
            break;
        default:
            bgm = Player_Load("rom://music/teen_spirit.mod", 16, 0);
            break;
    }
    Player_Start(bgm);

    return bgm;
}

void stop_song(MODULE* song)
{
    audio_write_silence();
    audio_write_silence();
    Player_Stop();
    Player_Free(song);
}