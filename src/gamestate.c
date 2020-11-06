#include "gamestate.h"
#include "music.h"

extern volatile uint32_t animcounter;

//base initialization function to setup a new gamestate
GAME* setup_main_game(void)
{
    GAME* newgame = (GAME*)malloc(sizeof(GAME));

    newgame->pad_dir = 6;
    newgame->prev_pad_dir = 6;

    int fp = 0;
    fp = dfs_open("/mc_idle_down.sprite");
    newgame->mc_sprites[MC_IDLE_DOWN] = malloc( dfs_size( fp ) );
    dfs_read( newgame->mc_sprites[MC_IDLE_DOWN], 1, dfs_size( fp ), fp );
    dfs_close( fp );

	fp = dfs_open("/mc_idle_left.sprite");
    newgame->mc_sprites[MC_IDLE_LEFT] = malloc( dfs_size( fp ) );
    dfs_read( newgame->mc_sprites[MC_IDLE_LEFT], 1, dfs_size( fp ), fp );
    dfs_close( fp );

	fp = dfs_open("/mc_idle_right.sprite");
    newgame->mc_sprites[MC_IDLE_RIGHT] = malloc( dfs_size( fp ) );
    dfs_read( newgame->mc_sprites[MC_IDLE_RIGHT], 1, dfs_size( fp ), fp );
    dfs_close( fp );

	fp = dfs_open("/mc_idle_up.sprite");
    newgame->mc_sprites[MC_IDLE_UP] = malloc( dfs_size( fp ) );
    dfs_read( newgame->mc_sprites[MC_IDLE_UP], 1, dfs_size( fp ), fp );
    dfs_close( fp );

	fp = dfs_open("/mc_walk_down.sprite");
    newgame->mc_sprites[MC_WALK_DOWN] = malloc( dfs_size( fp ) );
    dfs_read( newgame->mc_sprites[MC_WALK_DOWN], 1, dfs_size( fp ), fp );
    dfs_close( fp );

	fp = dfs_open("/mc_walk_left.sprite");
    newgame->mc_sprites[MC_WALK_LEFT] = malloc( dfs_size( fp ) );
    dfs_read( newgame->mc_sprites[MC_WALK_LEFT], 1, dfs_size( fp ), fp );
    dfs_close( fp );

	fp = dfs_open("/mc_walk_right.sprite");
    newgame->mc_sprites[MC_WALK_RIGHT] = malloc( dfs_size( fp ) );
    dfs_read( newgame->mc_sprites[MC_WALK_RIGHT], 1, dfs_size( fp ), fp );
    dfs_close( fp );

	fp = dfs_open("/mc_walk_up.sprite");
    newgame->mc_sprites[MC_WALK_UP] = malloc( dfs_size( fp ) );
    dfs_read( newgame->mc_sprites[MC_WALK_UP], 1, dfs_size( fp ), fp );
    dfs_close( fp );

    fp = dfs_open("/border.sprite");
    newgame->gym_tiles[BORDER] = malloc( dfs_size( fp ) );
    dfs_read( newgame->gym_tiles[BORDER], 1, dfs_size( fp ), fp );
    dfs_close( fp );

    fp = dfs_open("/wall1.sprite");
    newgame->gym_tiles[WALL1] = malloc( dfs_size( fp ) );
    dfs_read( newgame->gym_tiles[WALL1], 1, dfs_size( fp ), fp );
    dfs_close( fp );

    fp = dfs_open("/wall2.sprite");
    newgame->gym_tiles[WALL2] = malloc( dfs_size( fp ) );
    dfs_read( newgame->gym_tiles[WALL2], 1, dfs_size( fp ), fp );
    dfs_close( fp );

    fp = dfs_open("/wall3.sprite");
    newgame->gym_tiles[WALL3] = malloc( dfs_size( fp ) );
    dfs_read( newgame->gym_tiles[WALL3], 1, dfs_size( fp ), fp );
    dfs_close( fp );

    fp = dfs_open("/floors.sprite");
    newgame->gym_tiles[FLOORS] = malloc( dfs_size( fp ) );
    dfs_read( newgame->gym_tiles[FLOORS], 1, dfs_size( fp ), fp );
    dfs_close( fp );

    newgame->mc_current_sprite = newgame->mc_sprites[MC_IDLE_DOWN];

    newgame->disp = 0;

    newgame->mc.x = 100;
    newgame->mc.y = 60;

    newgame->current_song = 0;
    newgame->bgm = play_song(newgame->current_song);
    newgame->sfx_index = 0;

    for(int it = 0; it < MAX_SFX; it++)
    {
        newgame->samples[it] = NULL;
        newgame->voices[it] = -1;
    }

    newgame->frame_count = animcounter;

    return newgame;
}