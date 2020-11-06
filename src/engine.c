#include <libdragon.h>
#include "engine.h"
#include "gym.h"
#include "music.h"

extern volatile uint32_t animcounter; //updated by a continuous timer. see main.c for details
extern volatile int powerup_spawn_counter;
extern volatile int weight_spawn_counter;

void update_controller(GAME* game)
{
    controller_scan();
    game->prev_pad_dir = game->pad_dir;
    game->pad_dir = get_dpad_direction(0);
    game->keys = get_keys_down();
}

void update_mc_pos(GAME* game);
void spawn_new_powerups(GAME* game);
void spawn_new_weights(GAME* game);
void check_collisions(GAME* game);

void update_logic(GAME* game)
{
    update_mc_pos(game);

    spawn_new_powerups(game);

    spawn_new_weights(game);

    check_collisions(game);

    game->frame_count = animcounter;
}

void update_graphics(GAME* game)
{
    while( !(game->disp = display_lock()) );
    
    graphics_fill_screen( game->disp, 0x00000000 );

    graphics_set_color( 0xFFFFFFFF, 0x00000000 );

    rdp_sync( SYNC_PIPE );

    rdp_set_default_clipping();

    rdp_enable_texture_copy();

    draw_gym(game->disp, game->gym_tiles);

    graphics_draw_text( game->disp, 80, 18, "Juicin' 64 Test Build" );

    for(int it = 0; it < game->active_powerups; it++)
        graphics_draw_sprite_trans_stride( game->disp, game->powerups[it].x + 24, game->powerups[it].y + 48, game->powerup_sprites, game->powerups[it].type );

    for(int it = 0; it < game->active_weights; it++)
        graphics_draw_sprite_trans_stride( game->disp, game->weights[it].x + 24, game->weights[it].y + 48, game->weight_sprites, game->weights[it].type - 2 ); //hard 2 because first weights type is 2 in the enum

    graphics_draw_sprite_trans_stride( game->disp, game->mc.x, game->mc.y, game->mc_current_sprite, ((animcounter / 6) & 0x3)  );

    

    draw_bottom_wall(game->disp, game->gym_tiles[BORDER]); 

    display_show(game->disp);
}

void update_audio(GAME* game)
{
    if(!Player_Active()) //song is done playing
    {
        stop_song(game->bgm);
        game->current_song++;
        if(game->current_song > 4)
            game->current_song = 0;

        game->bgm = play_song(game->current_song);
    }

    if( game->keys.c[0].C_up )
        {
            int newvol = (game->bgm->volume + 20 < 127 ? game->bgm->volume + 20 : 127);
            Player_SetVolume(newvol);
        }
        if( game->keys.c[0].C_down )
        {
            int newvol = (game->bgm->volume >= 20 ? game->bgm->volume - 20 : 0);
            Player_SetVolume(newvol);
        }
        if( game->keys.c[0].A )
        {
            int newvol = game->bgm->volume;
            stop_song(game->bgm);
            game->current_song++;
            if(game->current_song >= MAX_SONGS)
                game->current_song = 0;

            game->bgm = play_song(game->current_song);
            Player_SetVolume(newvol);
        }
        if( game->keys.c[0].B )
        {
            if(game->samples[game->sfx_index])
            {
                Sample_Free(game->samples[game->sfx_index]);
            }
            switch(game->sfx_index)
            {
                case(0):
                    game->samples[game->sfx_index] = Sample_Load("rom://fx/blip1.wav");
                    break;
                case(1):
                    game->samples[game->sfx_index] = Sample_Load("rom://fx/hurt1.wav");
                    break;
                case(2):
                    game->samples[game->sfx_index] = Sample_Load("rom://fx/hurt2.wav");
                    break;
                case(3):
                    game->samples[game->sfx_index] = Sample_Load("rom://fx/powerup1.wav");
                    break;
                case(4):
                    game->samples[game->sfx_index] = Sample_Load("rom://fx/powerup2.wav");
                    break;
                default:
                    game->samples[game->sfx_index] = Sample_Load("rom://fx/hurt1.wav");
                    break;
            }

            game->voices[game->sfx_index] = Sample_Play(game->samples[game->sfx_index], 0, 0);
            Voice_SetPanning(game->voices[game->sfx_index], PAN_CENTER);
            Voice_SetVolume(game->voices[game->sfx_index], 256);

            game->sfx_index++;
            if(game->sfx_index >= MAX_SFX)
            {
                game->sfx_index = 0;
            }        
        }

    MikMod_Update();
}

void cleanup_main_game(GAME* game)
{
    //blank the screen
    while( !(game->disp = display_lock()) );
    graphics_fill_screen( game->disp, 0x000000FF );
    display_show(game->disp);

    //stop and free background music
    Player_Stop();
    Player_Free(game->bgm);
    //stop and free all sound effect samples
    for(int it = 0; it < MAX_SFX; it++)
    {
        Voice_Stop(game->voices[it]);
        Sample_Free(game->samples[it]);
    }
    //free all the main character sprites
    for(int it = 0; it < MC_SPRITE_TOTAL; it++)
    {
        free(game->mc_sprites[it]);
    }
    //free all the gym tiles
    for(int it = 0; it < GYM_TILE_TOTAL; it++)
    {
        free(game->gym_tiles[it]);
    }
    //finally, destroy the game object itself
    free(game);
}

void update_mc_pos(GAME* game)
{
    game->mc.dir = game->pad_dir;
    if(game->mc.dir >= 0)
        game->mc.action = WALK;
    else
    {
        game->mc.action = IDLE;
    }

    if(game->mc.action == WALK) //move player and update sprite if we're walking
    {
        switch(game->mc.dir)
        {
            case(UP): //up
            	game->mc.y -= 2 * (animcounter - game->frame_count);
                game->mc_current_sprite = game->mc_sprites[MC_WALK_UP];
                break;
            case(DOWN): //down
                game->mc.y += 2 * (animcounter - game->frame_count);
                game->mc_current_sprite = game->mc_sprites[MC_WALK_DOWN];
                break;
            case(R_UP):
                game->mc.y -= 1 * (animcounter - game->frame_count);
                game->mc.x += 2 * (animcounter - game->frame_count);
                game->mc_current_sprite = game->mc_sprites[MC_WALK_RIGHT];// mc_idle_right;
                break;
            case(R_DOWN):
                game->mc.y += 1 * (animcounter - game->frame_count);
                game->mc.x += 2 * (animcounter - game->frame_count);
                game->mc_current_sprite = game->mc_sprites[MC_WALK_RIGHT];// mc_idle_right;
                break;
            case(RIGHT): //right
                game->mc.x += 2 * (animcounter - game->frame_count);
                game->mc_current_sprite = game->mc_sprites[MC_WALK_RIGHT];// mc_idle_right;
                break;
            case(L_UP):
                game->mc.y -= 1 * (animcounter - game->frame_count);
            	game->mc.x -= 2 * (animcounter - game->frame_count);
                game->mc_current_sprite = game->mc_sprites[MC_WALK_LEFT];
                break;
            case(L_DOWN):
                game->mc.y += 1 * (animcounter - game->frame_count);
                game->mc.x -= 2 * (animcounter - game->frame_count);
                game->mc_current_sprite = game->mc_sprites[MC_WALK_LEFT];
                break;
            case(LEFT): //left
            	game->mc.x -= 2 * (animcounter - game->frame_count);
                game->mc_current_sprite = game->mc_sprites[MC_WALK_LEFT];
                break;
            default: //probably already idling
                break;
        }
        if(game->mc.y > 208)
			game->mc.y = 208;
        else if(game->mc.y < 32)
			game->mc.y = 32;
        if(game->mc.x > 288)
			game->mc.x = 288;
        else if(game->mc.x < 16)
			game->mc.x = 16;
    }
    else if(game->mc.action == IDLE) //update sprite if idle
    {
        switch(game->prev_pad_dir)
		{
            case(UP): //up
                game->mc_current_sprite = game->mc_sprites[MC_IDLE_UP];
                break;
            case(DOWN): //down
                game->mc_current_sprite = game->mc_sprites[MC_IDLE_DOWN];
                break;
            case(R_UP):
            case(R_DOWN):
            case(RIGHT): //right
                game->mc_current_sprite = game->mc_sprites[MC_IDLE_RIGHT];// mc_idle_right;
                break;
            case(L_UP):
            case(L_DOWN):
            case(LEFT): //left
                game->mc_current_sprite = game->mc_sprites[MC_IDLE_LEFT];
                break;
            default: //probably already idling
                break;
		}
    }
}

void spawn_new_powerups(GAME* game)
{
    if(powerup_spawn_counter > 0)
    {
        
        int pos_x = (rand() % GYM_COLS);
        int pos_y = (rand() % GYM_ROWS);
        if(game->occupied[pos_x][pos_y] == false)
        {
            powerup_spawn_counter--;
            if(game->active_powerups < MAX_POWERUPS)
            {
                game->occupied[pos_x][pos_y] = true;
                game->powerups[game->active_powerups].x = 16 * pos_x;
                game->powerups[game->active_powerups].y = 16 * pos_y;
                game->powerups[game->active_powerups].draw_height = 16;
                game->powerups[game->active_powerups].draw_width = 8;
                game->powerups[game->active_powerups].coll_height = 16;
                game->powerups[game->active_powerups].coll_width = 16;
                if((rand() & 0x07) == 7)
                {
                    game->powerups[game->active_powerups].type = WATER;
                }
                else
                {
                    game->powerups[game->active_powerups].type = JUICE;
                }
                
                game->active_powerups++;
            }
        }
    }
}

void spawn_new_weights(GAME* game)
{
    if(weight_spawn_counter > 0)
    {
        
        int pos_x = (rand() % GYM_COLS);
        int pos_y = (rand() % GYM_ROWS);
        if(game->occupied[pos_x][pos_y] == false)
        {
            weight_spawn_counter--;
            if(game->active_weights < MAX_WEIGHTS)
            {
                game->occupied[pos_x][pos_y] = true;
                game->weights[game->active_weights].x = 16 * pos_x;
                game->weights[game->active_weights].y = 16 * pos_y;
                game->weights[game->active_weights].draw_height = 11;
                game->weights[game->active_weights].draw_width = 32;
                game->weights[game->active_weights].coll_height = 16;
                game->weights[game->active_weights].coll_width = 32;
                game->weights[game->active_weights].type = DUMBBELL;
                game->active_weights++;
            }
        }
    }
}

void check_collisions(GAME* game)
{
    for(int it = 0; it < MAX_POWERUPS; it++)
    {
        if(it >= game->active_powerups)
            break;

        if((game->mc.x + game->mc.coll_width) < game->powerups[it].x + 16) //hard 16 is the offset due to the left wall
            continue;
        if(game->mc.x > (game->powerups[it].x + game->powerups[it].coll_width + 16))
            continue;
        if((game->mc.y + game->mc.draw_height) < game->powerups[it].y + 48) //hard 48 is the offset due to the upper wall
            continue;
        if((game->mc.y + game->mc.draw_height - game->mc.coll_height) > (game->powerups[it].y + game->powerups[it].coll_height + 48))
            continue;

        game->occupied[game->powerups[it].x / 16][game->powerups[it].y / 16] = false;                  
        
        if(it != game->active_powerups - 1)
        {
            game->powerups[it].coll_height = game->powerups[game->active_powerups - 1].coll_height;
            game->powerups[it].coll_width = game->powerups[game->active_powerups - 1].coll_width;
            game->powerups[it].draw_height = game->powerups[game->active_powerups - 1].draw_height;
            game->powerups[it].draw_width = game->powerups[game->active_powerups - 1].draw_width;
            game->powerups[it].type = game->powerups[game->active_powerups - 1].type;
            game->powerups[it].x = game->powerups[game->active_powerups - 1].x;
            game->powerups[it].y = game->powerups[game->active_powerups - 1].y;
        }
        if(game->samples[game->sfx_index])
        {
            Sample_Free(game->samples[game->sfx_index]);
        }
        if(game->powerups[it].type == JUICE)
            game->samples[game->sfx_index] = Sample_Load("rom://fx/powerup1.wav");
        else if(game->powerups[it].type == WATER)
            game->samples[game->sfx_index] = Sample_Load("rom://fx/powerup2.wav");

        game->voices[game->sfx_index] = Sample_Play(game->samples[game->sfx_index], 0, 0);
        Voice_SetPanning(game->voices[game->sfx_index], PAN_CENTER);
        Voice_SetVolume(game->voices[game->sfx_index], 256);

        game->sfx_index++;
        if(game->sfx_index >= MAX_SFX)
        {
            game->sfx_index = 0;
        }  

        game->active_powerups--;

    }

    for(int it = 0; it < MAX_WEIGHTS; it++)
    {
        if(it >= game->active_weights)
            break;

        if((game->mc.x + game->mc.coll_width) < game->weights[it].x + 16) //hard 16 is the offset due to the left wall
            continue;
        if(game->mc.x > (game->weights[it].x + game->weights[it].coll_width + 16))
            continue;
        if((game->mc.y + game->mc.draw_height) < game->weights[it].y + 48) //hard 48 is the offset due to the upper wall
            continue;
        if((game->mc.y + game->mc.draw_height - game->mc.coll_height) > (game->weights[it].y + game->weights[it].coll_height + 48))
            continue;

        game->occupied[game->weights[it].x / 16][game->weights[it].y / 16] = false;                  
        
        if(it != game->active_weights - 1)
        {
            game->weights[it].coll_height = game->weights[game->active_weights - 1].coll_height;
            game->weights[it].coll_width = game->weights[game->active_weights - 1].coll_width;
            game->weights[it].draw_height = game->weights[game->active_weights - 1].draw_height;
            game->weights[it].draw_width = game->weights[game->active_weights - 1].draw_width;
            game->weights[it].type = game->weights[game->active_weights - 1].type;
            game->weights[it].x = game->weights[game->active_weights - 1].x;
            game->weights[it].y = game->weights[game->active_weights - 1].y;
        }
        // if(game->samples[game->sfx_index])
        // {
        //     Sample_Free(game->samples[game->sfx_index]);
        // }
        // if(game->powerups[it].type == JUICE)
        //     game->samples[game->sfx_index] = Sample_Load("rom://fx/powerup1.wav");
        // else if(game->powerups[it].type == WATER)
        //     game->samples[game->sfx_index] = Sample_Load("rom://fx/powerup2.wav");

        // game->voices[game->sfx_index] = Sample_Play(game->samples[game->sfx_index], 0, 0);
        // Voice_SetPanning(game->voices[game->sfx_index], PAN_CENTER);
        // Voice_SetVolume(game->voices[game->sfx_index], 256);

        // game->sfx_index++;
        // if(game->sfx_index >= MAX_SFX)
        // {
        //     game->sfx_index = 0;
        // }  

        game->active_weights--;

    }
}