#include "gamestate.h"
#include "music.h"

extern volatile uint32_t animcounter;
volatile int powerup_spawn_counter = 0;
volatile int weight_spawn_counter = 0;
volatile int mob_spawn_counter = 0;

timer_link_t *juice_timer, *weight_timer, *mob_timer;

volatile bool unplugged = false;
volatile bool paused = false;

void spawn_powerup(int ovfl)
{
    if(paused == false)
        powerup_spawn_counter++;
}

void spawn_weight(int ovfl)
{
    if(paused == false)
        weight_spawn_counter++;
}

void spawn_mob(int ovfl)
{
    if(paused == false)
        mob_spawn_counter++;
}

void setup_sprites(GAME* game);
void setup_songs(GAME* game);
void setup_sfx(GAME* game);

//base initialization function to setup a new gamestate
GAME* setup_main_game(void)
{
    uint32_t count;
    count = timer_ticks();
    srand(count);

    GAME* newgame = (GAME*)malloc(sizeof(GAME));

    newgame->gains = 0;
    newgame->juice = 0;
    newgame->rage = 0;

    newgame->pad_dir = 6;
    newgame->prev_pad_dir = 6;

    setup_sprites(newgame);    

    newgame->disp = 0;

    newgame->mc.x = 96;
    newgame->mc.y = 64;
    newgame->mc.coll_width = 8;
    newgame->mc.coll_height = 16;
    newgame->mc.draw_width = 16;
    newgame->mc.draw_height = 24;
    newgame->mc.dir = DOWN;
    newgame->mc.action = IDLE;

    setup_songs(newgame);

    newgame->current_song = 0;
    newgame->bgm = play_song(newgame, newgame->current_song);
    
    setup_sfx(newgame);

    for(int y_it=0; y_it < GYM_ROWS; y_it++)
    {
        for(int x_it = 0; x_it < GYM_COLS; x_it++)
        {
            newgame->occupied[x_it][y_it] = false;
        }
    }

    newgame->active_powerups = 0;
    newgame->active_weights = 0;
    newgame->active_mobs = 0;

    powerup_spawn_counter = 0;
    weight_spawn_counter = 0;
    mob_spawn_counter = 0;

    juice_timer = new_timer(TIMER_TICKS(5000000), TF_CONTINUOUS, spawn_powerup);
    weight_timer = new_timer(TIMER_TICKS(2000000), TF_CONTINUOUS, spawn_weight);
    mob_timer = new_timer(TIMER_TICKS(10000000), TF_CONTINUOUS, spawn_mob);
    
    newgame->frame_count = animcounter;

    return newgame;
}

void setup_sprites(GAME* newgame)
{
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

    fp = dfs_open("/mc_punch_down.sprite");
    newgame->mc_sprites[MC_PUNCH_DOWN] = malloc( dfs_size( fp ) );
    dfs_read( newgame->mc_sprites[MC_PUNCH_DOWN], 1, dfs_size( fp ), fp );
    dfs_close( fp );

    fp = dfs_open("/mc_punch_left.sprite");
    newgame->mc_sprites[MC_PUNCH_LEFT] = malloc( dfs_size( fp ) );
    dfs_read( newgame->mc_sprites[MC_PUNCH_LEFT], 1, dfs_size( fp ), fp );
    dfs_close( fp );

    fp = dfs_open("/mc_punch_right.sprite");
    newgame->mc_sprites[MC_PUNCH_RIGHT] = malloc( dfs_size( fp ) );
    dfs_read( newgame->mc_sprites[MC_PUNCH_RIGHT], 1, dfs_size( fp ), fp );
    dfs_close( fp );

    fp = dfs_open("/mc_punch_up.sprite");
    newgame->mc_sprites[MC_PUNCH_UP] = malloc( dfs_size( fp ) );
    dfs_read( newgame->mc_sprites[MC_PUNCH_UP], 1, dfs_size( fp ), fp );
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

    fp = dfs_open("/mob_walk_right.sprite");
    newgame->mob_sprites[0] = malloc( dfs_size( fp ) );
    dfs_read( newgame->mob_sprites[0], 1, dfs_size( fp ), fp );
    dfs_close( fp );

    fp = dfs_open("/mob_walk_up.sprite");
    newgame->mob_sprites[1] = malloc( dfs_size( fp ) );
    dfs_read( newgame->mob_sprites[1], 1, dfs_size( fp ), fp );
    dfs_close( fp );

    fp = dfs_open("/mob_walk_left.sprite");
    newgame->mob_sprites[2] = malloc( dfs_size( fp ) );
    dfs_read( newgame->mob_sprites[2], 1, dfs_size( fp ), fp );
    dfs_close( fp );

    fp = dfs_open("/mob_walk_down.sprite");
    newgame->mob_sprites[3] = malloc( dfs_size( fp ) );
    dfs_read( newgame->mob_sprites[3], 1, dfs_size( fp ), fp );
    dfs_close( fp );

    fp = dfs_open("/emotes.sprite");
    newgame->emote_sprites = malloc( dfs_size( fp ) );
    dfs_read( newgame->emote_sprites, 1, dfs_size( fp ), fp );
    dfs_close( fp );

    fp = dfs_open("/hud_juice.sprite");
    newgame->hud_juice = malloc( dfs_size( fp ) );
    dfs_read( newgame->hud_juice, 1, dfs_size( fp ), fp );
    dfs_close( fp );

    fp = dfs_open("/no_fighting.sprite");
    newgame->no_fighting = malloc( dfs_size( fp ) );
    dfs_read( newgame->no_fighting, 1, dfs_size( fp ), fp );
    dfs_close( fp );

    newgame->mc_current_sprite = newgame->mc_sprites[MC_IDLE_DOWN];
}
void setup_songs(GAME* newgame)
{
    newgame->songs[0] = Player_Load("rom://music/enter_sand.mod", 127, 0);
    newgame->songs[1] = Player_Load("rom://music/teen_spirit.mod", 127, 0);
    newgame->songs[2] = Player_Load("rom://music/basket_case.mod", 127, 0);
    newgame->songs[3] = Player_Load("rom://music/sweet_child.mod", 127, 0);
    newgame->songs[4] = Player_Load("rom://music/toxicity.mod", 127, 0);
}

void setup_sfx(GAME* newgame)
{

    newgame->effects[POWERUP1] = Sample_Load("rom://fx/powerup1.wav");
    newgame->effects[POWERUP2] = Sample_Load("rom://fx/powerup2.wav");
    newgame->effects[HURT1] = Sample_Load("rom://fx/hurt1.wav");
    newgame->effects[HURT2] = Sample_Load("rom://fx/hurt2.wav");

    newgame->sfx_index = 0;

    for(int it = 0; it < MAX_SFX; it++)
    {
        newgame->samples[it] = NULL;
        newgame->voices[it] = -1;
    }
}