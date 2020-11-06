#include "gamestate.h"
#include "music.h"

extern volatile uint32_t animcounter;
volatile int powerup_spawn_counter = 0;
volatile int weight_spawn_counter = 0;

void spawn_powerup(int ovfl)
{
    powerup_spawn_counter++;
}

void spawn_weight(int ovfl)
{
    weight_spawn_counter++;
}

//base initialization function to setup a new gamestate
GAME* setup_main_game(void)
{
    uint32_t count;
    count = timer_ticks();
    srand(count);

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

    fp = dfs_open("/juice.sprite");
    newgame->powerup_sprites = malloc( dfs_size( fp ) );
    dfs_read( newgame->powerup_sprites, 1, dfs_size( fp ), fp );
    dfs_close( fp );
    
    fp = dfs_open("/weights.sprite");
    newgame->weight_sprites = malloc( dfs_size( fp ) );
    dfs_read( newgame->weight_sprites, 1, dfs_size( fp ), fp );
    dfs_close( fp );

    newgame->mc_current_sprite = newgame->mc_sprites[MC_IDLE_DOWN];

    newgame->disp = 0;

    newgame->mc.x = 96;
    newgame->mc.y = 64;
    newgame->mc.coll_width = 16;
    newgame->mc.coll_height = 16;
    newgame->mc.draw_width = 16;
    newgame->mc.draw_height = 24;
    newgame->mc.dir = DOWN;
    newgame->mc.action = IDLE;

    newgame->current_song = 0;
    newgame->bgm = play_song(newgame->current_song);
    newgame->sfx_index = 0;

    for(int it = 0; it < MAX_SFX; it++)
    {
        newgame->samples[it] = NULL;
        newgame->voices[it] = -1;
    }

    for(int y_it=0; y_it < GYM_ROWS; y_it++)
    {
        for(int x_it = 0; x_it < GYM_COLS; x_it++)
        {
            newgame->occupied[x_it][y_it] = false;
        }
    }

    newgame->active_powerups = 0;

    newgame->active_weights = 0;


    new_timer(TIMER_TICKS(5000000), TF_CONTINUOUS, spawn_powerup);
    new_timer(TIMER_TICKS(2000000), TF_CONTINUOUS, spawn_weight);
    newgame->frame_count = animcounter;

    return newgame;
}