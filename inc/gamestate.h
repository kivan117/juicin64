#ifndef __GAMESTATE_H__
#define __GAMESTATE_H__

#include <libdragon.h>
#include <mikmod.h>
#include "player.h"
#include "mob.h"
#include "pickups.h"
#include "score_popups.h"

#define MAX_SFX 5
#define MC_SPRITE_TOTAL 12
#define GYM_TILE_TOTAL 5
#define GYM_ROWS 11
#define GYM_COLS 18
#define MAX_POWERUPS 5
#define MAX_WEIGHTS 4
#define MAX_MOBS 50
#define MOB_SPRITE_TOTAL 4
#define MAX_SONGS 5

enum GAME_STATE {INTROS, MAIN_MENU, PREP_GAME, MAIN_GAME, HIGH_SCORES, RESET};
enum MC_SPRITES {MC_IDLE_DOWN,  MC_IDLE_LEFT,  MC_IDLE_RIGHT,  MC_IDLE_UP,
                 MC_WALK_DOWN,  MC_WALK_LEFT,  MC_WALK_RIGHT,  MC_WALK_UP,
                 MC_PUNCH_DOWN, MC_PUNCH_LEFT, MC_PUNCH_RIGHT, MC_PUNCH_UP};
enum GYM_TILES {BORDER, WALL1, WALL2, WALL3, FLOORS};
enum EFFECTS {POWERUP1, POWERUP2, HURT1, HURT2, TOTAL_SFX=4};

typedef struct {
    int x;
    int y;
    int sort_y;
    sprite_t* graphic;
    uint8_t graphic_index;
} GAMESPRITE;

typedef struct {

    uint8_t highscore_pos;
    uint32_t scores[10];
    uint32_t gains;
    uint8_t juice, rage;
    int ending_seq;
    int start_seq;

    uint8_t sprites_to_draw;

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
    sprite_t* mob_sprites[MOB_SPRITE_TOTAL];
    sprite_t* emote_sprites;
    sprite_t* hud_juice;
    sprite_t* score_pop_sprites;

    GAMESPRITE sprite_draw_list[MAX_MOBS + MAX_WEIGHTS + MAX_POWERUPS + 1];

    display_context_t disp;

    int current_song;
    MODULE *bgm;
    int sfx_index;

    SAMPLE* effects[TOTAL_SFX];
    SAMPLE* samples[MAX_SFX]; 
    int voices[MAX_SFX];

    int active_mobs;
    MOB mobs[MAX_MOBS];
    //uint8_t mob_draw_order[MAX_MOBS];

    int active_powerups; //iterator for our powerups buffer
    int active_weights; //iterator for the weights buffer
    PICKUP powerups[MAX_POWERUPS]; //ring buffer for powerups
    PICKUP weights[MAX_WEIGHTS]; //ring buffer for weights
    bool occupied[GYM_COLS][GYM_ROWS];

    sprite_t* ready, *lift, *no_fighting;

    MODULE* songs[MAX_SONGS];

    POPUP score_pops[MAX_WEIGHTS];
    uint8_t current_score_pop;

} GAME;

GAME* setup_main_game(void);

#endif