#ifndef __GAMESTATE_H__
#define __GAMESTATE_H__

#include <libdragon.h>
#include <mikmod.h>
#include "player.h"
#include "pickups.h"

#define MAX_SFX 5
#define MC_SPRITE_TOTAL 8
#define GYM_TILE_TOTAL 5
#define GYM_ROWS 11
#define GYM_COLS 18
#define MAX_POWERUPS 5
#define MAX_WEIGHTS 4

enum GAME_STATE {INTROS, MAIN_MENU, PREP_GAME, MAIN_GAME, RESET};
enum MC_SPRITES {MC_IDLE_DOWN, MC_IDLE_LEFT, MC_IDLE_RIGHT, MC_IDLE_UP,
                MC_WALK_DOWN, MC_WALK_LEFT, MC_WALK_RIGHT, MC_WALK_UP};
enum GYM_TILES {BORDER, WALL1, WALL2, WALL3, FLOORS};

typedef struct {

    struct controller_data keys;
    int pad_dir;
    int prev_pad_dir;

    PLAYER mc;
    uint32_t frame_count;

    sprite_t* mc_sprites[MC_SPRITE_TOTAL];
    sprite_t* mc_current_sprite;
    sprite_t* gym_tiles[GYM_TILE_TOTAL];
    sprite_t* powerup_sprites;
    sprite_t* weight_sprites;

    display_context_t disp;

    int current_song;
    MODULE *bgm;
    int sfx_index;

    SAMPLE* samples[MAX_SFX];
    int voices[MAX_SFX];

    int active_powerups; //iterator for our powerups buffer
    int active_weights; //iterator for the weights buffer
    PICKUP powerups[MAX_POWERUPS]; //ring buffer for powerups
    PICKUP weights[MAX_WEIGHTS]; //ring buffer for weights
    bool occupied[GYM_COLS][GYM_ROWS];

} GAME;

GAME* setup_main_game(void);

#endif