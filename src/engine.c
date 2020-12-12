#include <libdragon.h>
#include "engine.h"
#include "gym.h"
#include "music.h"
#include "saves.h"

extern volatile uint32_t animcounter; //updated by a continuous timer. see main.c for details
extern volatile int powerup_spawn_counter;
extern volatile int weight_spawn_counter;
extern volatile int mob_spawn_counter;
extern volatile bool unplugged;
extern volatile bool paused;
extern volatile bool game_over;
extern timer_link_t *juice_timer;
extern timer_link_t *weight_timer;
extern timer_link_t *mob_timer;

int juice_type_counter = 0;

void update_controller(GAME* game)
{
    if(get_controllers_present() & CONTROLLER_1_INSERTED)
    {
        unplugged = false;
        controller_scan();
        game->prev_pad_dir = game->pad_dir;
        game->pad_dir = get_dpad_direction(0);
        game->keys = get_keys_down();
    }
    else
    {
        paused = true;
        unplugged = true;
    }
    
    
}

void update_mc_pos(GAME* game);
void update_mob_pos(GAME* game);
void spawn_new_powerups(GAME* game);
void spawn_new_weights(GAME* game);
void spawn_new_mob(GAME* game);
void check_collisions(GAME* game);

void update_logic(GAME* game)
{
    if(game->keys.c[0].start)
    {
        paused = !paused;
    }

    if(paused == false)
    {
        if(game->ending_seq <= 0)
        {
            spawn_new_powerups(game);

            spawn_new_weights(game);

            spawn_new_mob(game);
            if(game->start_seq <= 0)
            {
                update_mc_pos(game);
            
                update_mob_pos(game);

                check_collisions(game);
            }
        }
    }

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

    if(paused)
    {
        draw_bottom_wall(game->disp, game->gym_tiles[BORDER]);
        if(unplugged)
            graphics_draw_text( game->disp, 96, 112, "Insert Controller" );
        else
            graphics_draw_text( game->disp, 136, 112, "Paused" );    
        display_show(game->disp);
        return;
    }
    else if(game->start_seq > 0)
    {
        for(int it = 0; it < game->active_powerups; it++)
        {
            graphics_draw_sprite_trans_stride( game->disp, game->powerups[it].x + 24, game->powerups[it].y + 48, game->powerup_sprites, game->powerups[it].type );
        }
        for(int it = 0; it < game->active_weights; it++)
        {
            graphics_draw_sprite_trans_stride( game->disp, game->weights[it].x+8, game->weights[it].y + 48 + 3, game->weight_sprites, game->weights[it].type - 2 ); //hard 2 because first weights type is 2 in the enum
        }
        for(int it = 0; it < game->active_mobs; it++)
        {
            graphics_draw_sprite_trans_stride( game->disp, game->mobs[it].x, game->mobs[it].y, game->mob_sprites[game->mobs[it].dir], ((animcounter / 6) & 0x3)  );
        }
        graphics_draw_sprite_trans_stride( game->disp, game->mc.x, game->mc.y, game->mc_current_sprite, ((animcounter / 6) & 0x3)  );

        draw_bottom_wall(game->disp, game->gym_tiles[BORDER]);

        if(game->start_seq < 30)
            graphics_draw_sprite_trans( game->disp, 106, 100, game->lift); //lift
        else
            graphics_draw_sprite_trans( game->disp, 106, 100, game->ready); //ready

        display_show(game->disp);

        if((animcounter - game->frame_count) > 0)
            game->start_seq -= (animcounter - game->frame_count);
        // if(start_seq <= 0)
        // {
        //     insert_score(game->scores, game->gains);
        //     write_scores(game->scores);
        //     game_over = true;
        // }
        return;
    }
    else if(game->ending_seq > 0)
    {
        graphics_draw_sprite_trans_stride( game->disp, game->mobs[0].x, game->mobs[0].y, game->mob_sprites[game->mobs[0].dir], 0 );

        graphics_draw_sprite_trans_stride( game->disp, game->mc.x, game->mc.y, game->mc_current_sprite, ((animcounter / 6) & 0x3)  );

        draw_bottom_wall(game->disp, game->gym_tiles[BORDER]);

        if(game->ending_seq < 60)
            graphics_draw_sprite_trans( game->disp, 88, 80, game->no_fighting);

        display_show(game->disp);

        if((animcounter - game->frame_count) > 0)
            game->ending_seq -= (animcounter - game->frame_count);
        if(game->ending_seq <= 0)
        {
            game->highscore_pos = insert_score(game->scores, game->gains);
            write_scores(game->scores);
            game_over = true;
        }
        return;
    }

    char temp[18];
    sprintf(temp, "Gains:%010lu", game->gains);
    graphics_draw_box_trans(game->disp, 78, 16, 172, 28, graphics_make_color( 0x20, 0x20, 0x20, 0xFF));
    graphics_draw_text( game->disp, 80, 18, temp );
    
    for(int it = 9; it > game->juice; it -= 2)
    {
        graphics_draw_sprite_trans_stride( game->disp, 80 + (9*(it / 2)), 26, game->hud_juice, 0 );
    }
    for(int it = 2; it <= game->juice; it+=2)
    {
        graphics_draw_sprite_trans_stride( game->disp, 71 + (9*(it / 2)), 26, game->hud_juice, 2 );
    }
    if(game->juice % 2)
        graphics_draw_sprite_trans_stride( game->disp, 80 + 9 * (game->juice / 2), 26, game->hud_juice, 1 );
    
    int avatar_index;
    switch(game->rage)
    {
        case(0):
            avatar_index = 0;
            break;
        case(1):
        case(2):
        case(3):
            avatar_index = 1;
            break;
        case(4):
        case(5):
        case(6):
            avatar_index = 2;
            break;
        case(7):
        case(8):
            avatar_index = 3;
            break;
        case(9):
        case(10):
            avatar_index = 4;
            break;
        default:
            avatar_index = 4;
            break;
    }
    graphics_draw_sprite_trans_stride( game->disp, 224, 18, game->emote_sprites, avatar_index );

    for(int it = 0; it < game->active_powerups; it++)
    {
        graphics_draw_sprite_trans_stride( game->disp, game->powerups[it].x + 24, game->powerups[it].y + 48, game->powerup_sprites, game->powerups[it].type );
    }
    for(int it = 0; it < game->active_weights; it++)
    {
        graphics_draw_sprite_trans_stride( game->disp, game->weights[it].x+8, game->weights[it].y + 48 + 3, game->weight_sprites, game->weights[it].type - 2 ); //hard 2 because first weights type is 2 in the enum
    }
    for(int it = 0; it < game->active_mobs; it++)
    {
        graphics_draw_sprite_trans_stride( game->disp, game->mobs[it].x, game->mobs[it].y, game->mob_sprites[game->mobs[it].dir], ((animcounter / 6) & 0x3)  );
    }
    graphics_draw_sprite_trans_stride( game->disp, game->mc.x, game->mc.y, game->mc_current_sprite, ((animcounter / 6) & 0x3)  );
    for(int it = 0; it < MAX_WEIGHTS; it++)
    {
        if(game->score_pops[it].ttl > 0)
        {
            graphics_draw_sprite_trans_stride( game->disp, game->score_pops[it].x+8, game->score_pops[it].y + 48 + 3, game->score_pop_sprites, game->score_pops[it].score - 1 ); //hard 2 because first weights type is 2 in the enum
            game->score_pops[it].ttl -= (animcounter - game->frame_count);
        }
    }
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

        game->bgm = play_song(game, game->current_song);
    }

    if( game->keys.c[0].C_up ) //song volume up
    {
        int newvol = (game->bgm->volume + 20 < 127 ? game->bgm->volume + 20 : 127);
        Player_SetVolume(newvol);
    }
    else if( game->keys.c[0].C_down ) //song volume down
    {
        int newvol = (game->bgm->volume >= 20 ? game->bgm->volume - 20 : 0);
        Player_SetVolume(newvol);
    }
    
    if( game->keys.c[0].C_right ) //skip to next song
    {
        int newvol = game->bgm->volume;
        stop_song(game->bgm);
        game->current_song++;
        if(game->current_song >= MAX_SONGS)
            game->current_song = 0;

        game->bgm = play_song(game, game->current_song);
        Player_SetVolume(newvol);
    }
    else if( game->keys.c[0].C_left ) //skip to previous song
    {
        int newvol = game->bgm->volume;
        stop_song(game->bgm);
        game->current_song--;
        if(game->current_song < 0)
            game->current_song = MAX_SONGS - 1;

        game->bgm = play_song(game, game->current_song);
        Player_SetVolume(newvol);
    }

    MikMod_Update();
}

void cleanup_main_game(GAME* game)
{
    //blank the screen
    while( !(game->disp = display_lock()) );
    graphics_fill_screen( game->disp, 0x000000FF );
    display_show(game->disp);

    //stop and free all sound effect samples
    for(int it = 0; it < MAX_SFX; it++)
    {
        Voice_Stop(game->voices[it]);
        game->samples[it] = NULL;
        //Sample_Free(game->samples[it]);
    }
    for(int it = 0; it < TOTAL_SFX; it++)
    {
        Sample_Free(game->effects[it]);
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
    //free all the enemy sprites
    for(int it = 0; it < MOB_SPRITE_TOTAL; it++)
    {
        free(game->mob_sprites[it]);
    }

    free(game->emote_sprites);
    free(game->hud_juice);
    free(game->no_fighting);

    stop_song(game->bgm);

    //finally, destroy the game object itself
    free(game);

    delete_timer(juice_timer);
    delete_timer(weight_timer);
    delete_timer(mob_timer);
}

void update_mc_pos(GAME* game)
{
    if(game->pad_dir >= 0)
    {
        game->mc.action = WALK;
        game->mc.dir = game->pad_dir;
    }
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
        switch(game->mc.dir)
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

void update_mob_pos(GAME* game)
{
    for(int it = 0; it < game->active_mobs; it++)
    {
        switch(game->mobs[it].dir)
        {
            case(0):
                game->mobs[it].x += 2 * (animcounter - game->frame_count);
                if(game->mobs[it].x > 288)
			    {
                    game->mobs[it].x = 288;
                    game->mobs[it].dir = 2;
                }
                break;
            case(1):
                game->mobs[it].y -= 2 * (animcounter - game->frame_count);
                if(game->mobs[it].y < 32)
                {
			        game->mobs[it].y = 32;
                    game->mobs[it].dir = 3;
                }
                break;
            case(2):
                game->mobs[it].x -= 2 * (animcounter - game->frame_count);
                if(game->mobs[it].x < 16)
			    {
                    game->mobs[it].x = 16;
                    game->mobs[it].dir = 0;
                }
                break;
            case(3):
                game->mobs[it].y += 2 * (animcounter - game->frame_count);
                if(game->mobs[it].y > 208)
			    {
                    game->mobs[it].y = 208;
                    game->mobs[it].dir = 1;
                }                
                break;
            default:
                break;
        }
    }
    
}

void spawn_new_powerups(GAME* game)
{
    if(powerup_spawn_counter > 0)
    {
        
        int pos_x = 1 +  (rand() % (GYM_COLS-2));
        int pos_y = 1 + (rand() % (GYM_ROWS-2));
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
                game->powerups[game->active_powerups].coll_width = 8;
                
                juice_type_counter++;
                if(juice_type_counter == 9)
                {
                    game->powerups[game->active_powerups].type = WATER;
                    juice_type_counter = 0;
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
        
        int pos_x = 1 +  (rand() % (GYM_COLS-2));
        int pos_y = 1 + (rand() % (GYM_ROWS-2));
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
                game->weights[game->active_weights].coll_height = 11;
                game->weights[game->active_weights].coll_width = 16;
                game->weights[game->active_weights].type = DUMBBELL;
                game->active_weights++;
            }
        }
    }
}

void spawn_new_mob(GAME* game)
{
    if(mob_spawn_counter > 0)
    {
        int pos_x, pos_y, cardinal_dir;
        int coin_toss = rand() % 2;

        if(coin_toss) //let's spawn at top or bottom and go north/south
        {
            coin_toss = rand() % 2;
            if(coin_toss) //spawn at top
            {
                pos_x = (rand() % GYM_COLS);
                pos_y = 0;
                cardinal_dir = 3;                
            }
            else //spawn at bottom
            {
                pos_x = (rand() % GYM_COLS);
                pos_y = GYM_ROWS - 1;
                cardinal_dir = 1;
            }           
        }
        else //let's spawn at one of the sides and go east/west
        {
            coin_toss = rand() % 2;
            if(coin_toss) //spawn at left
            {
                pos_x = 0;
                pos_y = (rand() % GYM_ROWS);
                cardinal_dir = 0;
            }
            else //spawn at right
            {
                pos_x = GYM_COLS - 1;
                pos_y = (rand() % GYM_ROWS);
                cardinal_dir = 2;
            }
        }
        

        //pos_x = (rand() % GYM_COLS);
        //pos_y = (rand() % GYM_ROWS);
        
        mob_spawn_counter--;
        if(game->active_mobs < MAX_MOBS)
        {
            game->mobs[game->active_mobs].x = 16 * pos_x + 16;
            game->mobs[game->active_mobs].y = 16 * pos_y + 32;
            game->mobs[game->active_mobs].draw_height = 24;
            game->mobs[game->active_mobs].draw_width = 16;
            game->mobs[game->active_mobs].coll_height = 16;
            game->mobs[game->active_mobs].coll_width = 8;
            game->mobs[game->active_mobs].dir = cardinal_dir;//rand() & 0x03; //only pick cardinal directions, but do so at random
            game->active_mobs++;
        }
        
    }
}

void check_collisions(GAME* game)
{
    for(int it = 0; it < game->active_powerups; it++)
    {
        if((game->mc.x + game->mc.coll_width + 4) < game->powerups[it].x + 24) //hard 16 is the offset due to the left wall
            continue;
        if(game->mc.x + 4 > (game->powerups[it].x + game->powerups[it].coll_width + 24))
            continue;
        if((game->mc.y + game->mc.draw_height) < game->powerups[it].y + 48) //hard 48 is the offset due to the upper wall
            continue;
        if((game->mc.y + game->mc.draw_height - game->mc.coll_height) > (game->powerups[it].y + game->powerups[it].coll_height + 48))
            continue;

        game->occupied[game->powerups[it].x / 16][game->powerups[it].y / 16] = false;                  
        
        // if(game->samples[game->sfx_index])
        // {
        //     Sample_Free(game->samples[game->sfx_index]);
        // }
        // if(game->powerups[it].type == JUICE)
        //     game->samples[game->sfx_index] = Sample_Load("rom://fx/powerup1.wav");
        // else if(game->powerups[it].type == WATER)
        //     game->samples[game->sfx_index] = Sample_Load("rom://fx/powerup2.wav");

        if(game->powerups[it].type == JUICE)
            game->samples[game->sfx_index] = game->effects[POWERUP1];
        else if(game->powerups[it].type == WATER)
            game->samples[game->sfx_index] = game->effects[POWERUP2];

        game->voices[game->sfx_index] = Sample_Play(game->samples[game->sfx_index], 0, 0);
        Voice_SetPanning(game->voices[game->sfx_index], PAN_CENTER);
        Voice_SetVolume(game->voices[game->sfx_index], 256);

        game->sfx_index++;
        if(game->sfx_index >= MAX_SFX)
        {
            game->sfx_index = 0;
        }

        if(game->powerups[it].type == JUICE && game->juice < 10)
        {
            game->juice++;
        }
        else if(game->powerups[it].type == WATER && game->rage > 0)
        {
            game->rage = game->rage >= 4 ? game->rage - 4 : 0;
        }

        if((it != game->active_powerups - 1) && (game->active_powerups >= 1))
        {
            game->powerups[it].coll_height = game->powerups[game->active_powerups - 1].coll_height;
            game->powerups[it].coll_width = game->powerups[game->active_powerups - 1].coll_width;
            game->powerups[it].draw_height = game->powerups[game->active_powerups - 1].draw_height;
            game->powerups[it].draw_width = game->powerups[game->active_powerups - 1].draw_width;
            game->powerups[it].type = game->powerups[game->active_powerups - 1].type;
            game->powerups[it].x = game->powerups[game->active_powerups - 1].x;
            game->powerups[it].y = game->powerups[game->active_powerups - 1].y;
        }  

        game->active_powerups--;
        if(game->active_powerups < 0)
            game->active_powerups = 0;
        if(game->active_powerups == 0)
            powerup_spawn_counter++;

    }

    for(int it = 0; it < game->active_weights; it++)
    {
        if((game->mc.x + game->mc.coll_width + 4) < game->weights[it].x+16) //8 is visual offset of the weight inside the tile image
            continue;
        if(game->mc.x + 4 > (game->weights[it].x + game->weights[it].coll_width+16))
            continue;
        if((game->mc.y + game->mc.draw_height) < game->weights[it].y + 48 + 3) //hard 48 is the offset due to the upper wall
            continue;
        if((game->mc.y + game->mc.draw_height - game->mc.coll_height) > (game->weights[it].y + game->weights[it].coll_height + 48 + 3))
            continue;

        game->occupied[game->weights[it].x / 16][game->weights[it].y / 16] = false; 

        if(game->juice == 0)
        {
            game->score_pops[game->current_score_pop].x = game->weights[it].x;
            game->score_pops[game->current_score_pop].y = game->weights[it].y;
            game->score_pops[game->current_score_pop].ttl = 15;
            game->score_pops[game->current_score_pop].score = 1;

            game->gains += 100;
        }
        else
        {
            game->score_pops[game->current_score_pop].x = game->weights[it].x;
            game->score_pops[game->current_score_pop].y = game->weights[it].y;
            game->score_pops[game->current_score_pop].ttl = 15;
            game->score_pops[game->current_score_pop].score = game->juice;

            game->gains += 100 * game->juice;
            game->juice--;
        }

        if((it != game->active_weights - 1) && (game->active_weights >= 1))
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
        //game->samples[game->sfx_index] = Sample_Load("rom://fx/hurt2.wav");
        game->samples[game->sfx_index] = game->effects[HURT2];
        game->voices[game->sfx_index] = Sample_Play(game->samples[game->sfx_index], 0, 0);
        Voice_SetPanning(game->voices[game->sfx_index], PAN_CENTER);
        Voice_SetVolume(game->voices[game->sfx_index], 256);

        game->sfx_index++;
        if(game->sfx_index >= MAX_SFX)
        {
            game->sfx_index = 0;
        }  

        game->active_weights--;
        if(game->active_weights < 0)
            game->active_weights = 0;
        if(game->active_weights == 0)
            weight_spawn_counter++;

    }

    for(int it = 0; it < game->active_mobs; it++)
    {
        if((game->mc.x + game->mc.coll_width + 4) < game->mobs[it].x + 4) //hard 16 is the offset due to the left wall
            continue;
        if(game->mc.x + 4 > (game->mobs[it].x + game->mobs[it].coll_width + 4))
            continue;
        if((game->mc.y + game->mc.draw_height) < game->mobs[it].y + game->mobs[it].draw_height - game->mobs[it].coll_height) //hard 48 is the offset due to the upper wall
            continue;
        if((game->mc.y + game->mc.draw_height - game->mc.coll_height) > (game->mobs[it].y + game->mobs[it].draw_height))
            continue;

        // if(game->samples[game->sfx_index])
        // {
        //     Sample_Free(game->samples[game->sfx_index]);
        // }
        // game->samples[game->sfx_index] = Sample_Load("rom://fx/hurt1.wav");
        game->samples[game->sfx_index] = game->effects[HURT1];
        game->voices[game->sfx_index] = Sample_Play(game->samples[game->sfx_index], 0, 0);
        Voice_SetPanning(game->voices[game->sfx_index], PAN_CENTER);
        Voice_SetVolume(game->voices[game->sfx_index], 256);

        game->sfx_index++;
        if(game->sfx_index >= MAX_SFX)
        {
            game->sfx_index = 0;
        }

        if(game->juice)
        {
            game->rage += game->juice;
            game->juice--;
        }
        else
            game->rage++;        

        if(game->rage >= 10)
        {
            //game over!
            game->rage = 10;
            game->ending_seq = 90;

            switch(game->mc.dir)
            {
                case(UP): //up
                    game->mc_current_sprite = game->mc_sprites[MC_PUNCH_UP];
                    break;
                case(DOWN): //down
                    game->mc_current_sprite = game->mc_sprites[MC_PUNCH_DOWN];
                    break;
                case(R_UP):
                case(R_DOWN):
                case(RIGHT): //right
                    game->mc_current_sprite = game->mc_sprites[MC_PUNCH_RIGHT];
                    break;
                case(L_UP):
                case(L_DOWN):
                case(LEFT): //left
                    game->mc_current_sprite = game->mc_sprites[MC_PUNCH_LEFT];
                    break;
                default: //probably already idling
                    break;
            }
            
            if(it != 0)
            {
                game->mobs[0].coll_height = game->mobs[it].coll_height;
                game->mobs[0].coll_width =  game->mobs[it].coll_width;
                game->mobs[0].draw_height = game->mobs[it].draw_height;
                game->mobs[0].draw_width =  game->mobs[it].draw_width;
                game->mobs[0].dir =         game->mobs[it].dir;
                game->mobs[0].x =           game->mobs[it].x;
                game->mobs[0].y =           game->mobs[it].y;
            }
            game->active_mobs = 0;

        }
        else if((it != game->active_mobs - 1) && (game->active_mobs >= 1))
        {
            game->mobs[it].coll_height = game->mobs[game->active_mobs - 1].coll_height;
            game->mobs[it].coll_width =  game->mobs[game->active_mobs - 1].coll_width;
            game->mobs[it].draw_height = game->mobs[game->active_mobs - 1].draw_height;
            game->mobs[it].draw_width =  game->mobs[game->active_mobs - 1].draw_width;
            game->mobs[it].dir =         game->mobs[game->active_mobs - 1].dir;
            game->mobs[it].x =           game->mobs[game->active_mobs - 1].x;
            game->mobs[it].y =           game->mobs[game->active_mobs - 1].y;
        }
        game->active_mobs--;
        if(game->active_mobs < 0)
            game->active_mobs = 0;
        mob_spawn_counter++;
    }
}