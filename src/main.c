
#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <stdint.h>
#include <libdragon.h>
#include <mikmod.h>

#include "version.h"
#include "flashcart.h"

#include "gamestate.h"
#include "engine.h"
#include "saves.h"


//tbh I'm not sure what's going on here, I just lifted this part from the libdragon dfs example
MIKMODAPI extern UWORD md_mode __attribute__((section (".data")));
MIKMODAPI extern UWORD md_mixfreq __attribute__((section (".data")));

volatile uint32_t animcounter = 0;
volatile bool game_over = false;

void update_counter( int ovfl )
{
    animcounter++;
}

void init_all_systems(void);
void draw_intros(void);
void draw_main_menu(void);
void draw_high_scores(uint32_t* scores);

int main(void)
{
    init_all_systems();
    int state = INTROS;
    GAME* game;

    while(1)
    {
        switch(state)
        {
            case(INTROS):
                draw_intros();
                state = MAIN_MENU;
                break;
            case(MAIN_MENU):
                draw_main_menu();
                state = PREP_GAME;
                break;
            case(PREP_GAME):
                game = setup_main_game();
                state = MAIN_GAME;
                break;
            case(MAIN_GAME):
                
                update_controller(game);
                update_logic(game);
                update_graphics(game);
                update_audio(game);

                if(game_over)
                {
                    state = HIGH_SCORES;
                }
                break;
            case(HIGH_SCORES):
                draw_high_scores(game->scores);
                state = RESET;
                break;
            case(RESET):
                game_over = false;
                cleanup_main_game(game);
                state = MAIN_MENU;
                break;
            default: //should not happen
                state = INTROS;
                break;
        }

    }

    cleanup_songs();   

}

void init_all_systems()
{
    /* enable interrupts (on the CPU) */
    init_interrupts();

    /* Initialize peripherals */
    dfs_init( DFS_DEFAULT_LOCATION );
    rdp_init();
    controller_init();
    timer_init();
    audio_init(44100, 2);
    MikMod_RegisterAllDrivers();
    MikMod_RegisterAllLoaders();

    md_mode |= DMODE_16BITS;
    md_mode |= DMODE_SOFT_MUSIC;
    md_mode |= DMODE_SOFT_SNDFX;
    md_mode |= DMODE_STEREO;
                                            
    md_mixfreq = audio_get_frequency();

    MikMod_Init("");
    MikMod_SetNumVoices(-1, MAX_SFX);
    MikMod_EnableOutput();
    display_init( RESOLUTION_320x240, DEPTH_16_BPP, 2, GAMMA_NONE, ANTIALIAS_RESAMPLE );

    //start a timer that will trigger 30 times per second
    //the "update_counter" function is called each time the timer is triggered
    new_timer(TIMER_TICKS(1000000 / 30), TF_CONTINUOUS, update_counter);
    if(!eeprom_present())
    {
        display_context_t debug_disp = 0;
        
        while(1){
            /* Grab a render buffer */
            while( !(debug_disp = display_lock()) );
        
            /*Fill the screen */
            graphics_fill_screen( debug_disp, 0x000000FF );

            graphics_set_color( 0xFFFFFFFF, 0x000000FF );
            graphics_draw_text( debug_disp, 116, 80, "EEPROM ERROR" );

            /* Force backbuffer flip */
            display_show(debug_disp);
        }
        
    }
    else //eeprom does exist. run checksum, reset it if it fails.
    {
        if(!check_eeprom())
        {
            reset_eeprom();
        }        
    }    
}

void draw_intros()
{
    int fp = 0;
    int logo = 0;
    uint32_t logocounter = animcounter;
    fp = dfs_open("/jamlogo.sprite");
    sprite_t *jamlogo = malloc( dfs_size( fp ) );
    dfs_read( jamlogo, 1, dfs_size( fp ), fp );
    dfs_close( fp );

    fp = dfs_open("/n64logo.sprite");
    sprite_t *n64logo = malloc( dfs_size( fp ) );
    dfs_read( n64logo, 1, dfs_size( fp ), fp );
    dfs_close( fp );

    fp = dfs_open("/brewlogo.sprite");
    sprite_t *brewlogo = malloc( dfs_size( fp ) );
    dfs_read( brewlogo, 1, dfs_size( fp ), fp );
    dfs_close( fp );

    /* intro movies */
    do
    {
        static display_context_t disp = 0;

        /* Grab a render buffer */
        while( !(disp = display_lock()) );
       
        /*Fill the screen */
        graphics_fill_screen( disp, 0x00000000 );

        /* Set the text output color */
        graphics_set_color( 0x00000000, 0x00000000 );

		//graphics_draw_text( disp, 16, 16, "N64Brew Jam Test" );

		/* Draw jam logo */
        if(animcounter - logocounter > 120)
        {
            logo++;
            logocounter = animcounter;
        }

            switch(logo)
            {
                case(0):
                    graphics_draw_sprite( disp, 103, 51, n64logo);
                    display_show(disp);
                    setup_songs();
                    logo++;
                    break;
                case(1):
                case(2):
                    graphics_draw_sprite( disp, 103, 51, n64logo);
                    break;
                case(3):
                    graphics_draw_sprite( disp, 120, 45, brewlogo);
                    //putting game copyright here was confusing visually
                    //I don't have a copyright on the logos
                    //graphics_set_color( 0xFFFFFFFF, 0x00000000 );
                    //graphics_draw_text( disp, 116, 190, "(C)KIVAN117" );
                    break;
                case(4):
                    graphics_draw_sprite( disp, 8, 79, jamlogo);
                    //putting game copyright here was confusing visually
                    //I don't have a copyright on the logos
                    //graphics_set_color( 0xFFFFFFFF, 0x00000000 );
                    //graphics_draw_text( disp, 116, 190, "(C)KIVAN117" );
                    break;
                default:
                    break;
            }
            

        /* Force backbuffer flip */
        display_show(disp);
        
        controller_scan();
        struct controller_data keys = get_keys_down();
        if( keys.c[0].start || keys.c[0].A )
        {
            logo++;
            /* Lazy switching */
            //mode = 1 - mode;
        }
    } while (logo < 5);

    free(n64logo);
    free(jamlogo);
    free(brewlogo);
}

void draw_main_menu(void)
{
    display_context_t disp = 0;
    bool keep_waiting = true;
    while(keep_waiting)
    {
        /* Grab a render buffer */
        while( !(disp = display_lock()) );
        
        /*Fill the screen */
        graphics_fill_screen( disp, 0x00000000 );

        /* Set the text output color */
        graphics_set_color( 0xFFFFFFFF, 0x00000000 );

        //draw Menu
        graphics_draw_text( disp, 124, 40, "Main Menu" );
        
        graphics_draw_text( disp, 132, 120, "Press A" );
        /* Force backbuffer flip */
        display_show(disp);


        controller_scan();
        struct controller_data keys = get_keys_down();
        if( keys.c[0].start || keys.c[0].A || keys.c[0].B)
        {
            keep_waiting = false;
            /* Grab a render buffer */
            while( !(disp = display_lock()) );
            
            /*Fill the screen */
            graphics_fill_screen( disp, 0x00000000 );

            /* Set the text output color */
            graphics_set_color( 0xFFFFFFFF, 0x00000000 );

            //draw Menu
            graphics_draw_text( disp, 132, 80, "Loading" );
            /* Force backbuffer flip */
            display_show(disp);
        }
    }
}

void draw_high_scores(uint32_t* scores)
{
    display_context_t disp = 0;
    bool keep_waiting = true;
    while(keep_waiting)
    {
        /* Grab a render buffer */
        while( !(disp = display_lock()) );
        
        /*Fill the screen */
        graphics_fill_screen( disp, 0x00000000 );

        /* Set the text output color */
        graphics_set_color( 0xFFFFFFFF, 0x00000000 );

        //draw high score table
        graphics_draw_text( disp, 116, 40, "HUGE GAINS:" );
        char temp_str[16];
        sprintf(temp_str, " 1:  %10lu", scores[0]);
        graphics_draw_text( disp, 100, 64, temp_str );      
        sprintf(temp_str, " 2:  %10lu", scores[1]);
        graphics_draw_text( disp, 100, 76, temp_str );
        sprintf(temp_str, " 3:  %10lu", scores[2]);
        graphics_draw_text( disp, 100, 88, temp_str );
        sprintf(temp_str, " 4:  %10lu", scores[3]);
        graphics_draw_text( disp, 100, 100, temp_str );
        sprintf(temp_str, " 5:  %10lu", scores[4]);
        graphics_draw_text( disp, 100, 112, temp_str );
        sprintf(temp_str, " 6:  %10lu", scores[5]);
        graphics_draw_text( disp, 100, 124, temp_str );
        sprintf(temp_str, " 7:  %10lu", scores[6]);
        graphics_draw_text( disp, 100, 136, temp_str );
        sprintf(temp_str, " 8:  %10lu", scores[7]);
        graphics_draw_text( disp, 100, 148, temp_str );
        sprintf(temp_str, " 9:  %10lu", scores[8]);
        graphics_draw_text( disp, 100, 160, temp_str );
        sprintf(temp_str, "10:  %10lu", scores[9]);
        graphics_draw_text( disp, 100, 172, temp_str );
        /* Force backbuffer flip */
        display_show(disp);


        controller_scan();
        struct controller_data keys = get_keys_down();
        if( keys.c[0].start || keys.c[0].A || keys.c[0].B)
        {
            keep_waiting = false;
        }
    }
}