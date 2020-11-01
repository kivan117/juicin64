
#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <stdint.h>
#include <libdragon.h>
#include <mikmod.h>

#include "version.h"
#include "flashcart.h"

//tbh I'm not sure what's going on here, I just lifted this from the libdragon dfs example
MIKMODAPI extern UWORD md_mode __attribute__((section (".data")));
MIKMODAPI extern UWORD md_mixfreq __attribute__((section (".data")));

static volatile uint32_t animcounter = 0;

void update_counter( int ovfl )
{
    animcounter++;
}

void draw_gym(display_context_t disp_list, void* wall_sm, void* floor_sm);
void draw_bottom_wall(display_context_t disp_list, void* wall_sm);
void draw_intros(void);
MODULE* play_song(int SongID);
void stop_song(MODULE *song);

int main(void)
{
    //int mode = 0;
	int player_x = 100;
	int player_y = 60;
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
    //md_mode |= DMODE_STEREO;
                                            
    md_mixfreq = audio_get_frequency();

    MikMod_Init("");

    /* Kick off animation update timer to fire thirty times a second */
    new_timer(TIMER_TICKS(1000000 / 30), TF_CONTINUOUS, update_counter);
	int pad_dir=6;
	int prev_pad_dir = pad_dir;
    
    draw_intros();

    display_init( RESOLUTION_320x240, DEPTH_16_BPP, 2, GAMMA_NONE, ANTIALIAS_RESAMPLE );

    int fp = 0;
    fp = dfs_open("/mc_idle_down.sprite");
    sprite_t *mc_idle_down = malloc( dfs_size( fp ) );
    dfs_read( mc_idle_down, 1, dfs_size( fp ), fp );
    dfs_close( fp );

	fp = dfs_open("/mc_idle_left.sprite");
    sprite_t *mc_idle_left = malloc( dfs_size( fp ) );
    dfs_read( mc_idle_left, 1, dfs_size( fp ), fp );
    dfs_close( fp );

	fp = dfs_open("/mc_idle_right.sprite");
    sprite_t *mc_idle_right = malloc( dfs_size( fp ) );
    dfs_read( mc_idle_right, 1, dfs_size( fp ), fp );
    dfs_close( fp );

	fp = dfs_open("/mc_idle_up.sprite");
    sprite_t *mc_idle_up = malloc( dfs_size( fp ) );
    dfs_read( mc_idle_up, 1, dfs_size( fp ), fp );
    dfs_close( fp );

	fp = dfs_open("/mc_walk_down.sprite");
    sprite_t *mc_walk_down = malloc( dfs_size( fp ) );
    dfs_read( mc_walk_down, 1, dfs_size( fp ), fp );
    dfs_close( fp );

	fp = dfs_open("/mc_walk_left.sprite");
    sprite_t *mc_walk_left = malloc( dfs_size( fp ) );
    dfs_read( mc_walk_left, 1, dfs_size( fp ), fp );
    dfs_close( fp );

	fp = dfs_open("/mc_walk_right.sprite");
    sprite_t *mc_walk_right = malloc( dfs_size( fp ) );
    dfs_read( mc_walk_right, 1, dfs_size( fp ), fp );
    dfs_close( fp );

	fp = dfs_open("/mc_walk_up.sprite");
    sprite_t *mc_walk_up = malloc( dfs_size( fp ) );
    dfs_read( mc_walk_up, 1, dfs_size( fp ), fp );
    dfs_close( fp );

    fp = dfs_open("/walls.sprite");
    sprite_t *walls = malloc( dfs_size( fp ) );
    dfs_read( walls, 1, dfs_size( fp ), fp );
    dfs_close( fp );

    fp = dfs_open("/floors.sprite");
    sprite_t *floors = malloc( dfs_size( fp ) );
    dfs_read( floors, 1, dfs_size( fp ), fp );
    dfs_close( fp );

    sprite_t * mc_current_sprite = mc_idle_down;

    static display_context_t disp = 0;

    //play background music
    int current_song = 0;
    MODULE *bgm = play_song(current_song);
    
    int sfx_index = 0;
    const int MAX_SFX = 5;

    SAMPLE* samples[MAX_SFX];
    int voices[MAX_SFX];

    for(int i = 0; i < MAX_SFX; i++)
    {
        samples[i] = NULL;
        voices[i] = -1;
    }

    MikMod_SetNumVoices(-1, MAX_SFX);



    while(1) 
    {
        disp = 0;

        /* Grab a render buffer */
        while( !(disp = display_lock()) );
       
        graphics_fill_screen( disp, 0x00000000 );

        graphics_set_color( 0xFFFFFFFF, 0x00000000 );

        rdp_sync( SYNC_PIPE );

        rdp_set_default_clipping();

        rdp_enable_texture_copy();

        draw_gym(disp, walls, floors);

		graphics_draw_text( disp, 80, 18, "Juicin' 64 Test Build" );

        graphics_draw_sprite_trans_stride( disp, player_x, player_y, mc_current_sprite, ((animcounter / 8) & 0x3)  ); 

        draw_bottom_wall(disp, walls); 

        /* Force backbuffer flip */
        display_show(disp);

        if(!Player_Active()) //song is done playing
        {
            stop_song(bgm);
            current_song++;
            if(current_song > 4)
                current_song = 0;

            bgm = play_song(current_song);
        }

        MikMod_Update();
        

        /* Do we need to switch video displays? */
        controller_scan();
		pad_dir = get_dpad_direction(0);
        //struct controller_data keys = get_keys_pressed();
		switch(pad_dir)
		{
			case(-1):  //pad isn't held, switch to idle
				switch(prev_pad_dir)
				{
					case(0): //right
						mc_current_sprite = mc_idle_right;
						break;
					case(2): //up
						mc_current_sprite = mc_idle_up;
						break;
					case(4): //left
						mc_current_sprite = mc_idle_left;
						break;
					case(6): //down
						mc_current_sprite = mc_idle_down;
						break;
					default:
						break;
				}
				break;
			case(0): //dpad right
				player_x = player_x + 2;
				if(player_x > 288)
					player_x = 288;
				mc_current_sprite = mc_walk_right;
				break;
			case(2): //dpad up
				player_y = player_y - 2;
				if(player_y < 32)
					player_y = 32;
				mc_current_sprite = mc_walk_up;
				break;
			case(4): //dpad left
				player_x = player_x - 2;
				if(player_x < 16)
					player_x = 16;
				mc_current_sprite = mc_walk_left;
				break;
			case(6): //dpad down
				player_y = player_y + 2;
				if(player_y > 208)
					player_y = 208;
				mc_current_sprite = mc_walk_down;
				break;
			default:
				break;
		}
		prev_pad_dir = pad_dir;
        struct controller_data keys = get_keys_down();
        if( keys.c[0].C_up )
        {
            int newvol = (bgm->volume + 20 < 127 ? bgm->volume + 20 : 127);
            Player_SetVolume(newvol);
        }
        if( keys.c[0].C_down )
        {
            int newvol = (bgm->volume >= 20 ? bgm->volume - 20 : 0);
            Player_SetVolume(newvol);
        }
        if( keys.c[0].A )
        {
            int newvol = bgm->volume;
            stop_song(bgm);
            current_song++;
            if(current_song > 4)
                current_song = 0;

            bgm = play_song(current_song);
            Player_SetVolume(newvol);
        }
        if( keys.c[0].B )
        {
            if(samples[sfx_index])
            {
                Sample_Free(samples[sfx_index]);
            }
            switch(sfx_index)
            {
                case(0):
                    samples[sfx_index] = Sample_Load("rom://fx/blip1.wav");
                    break;
                case(1):
                    samples[sfx_index] = Sample_Load("rom://fx/hurt1.wav");
                    break;
                case(2):
                    samples[sfx_index] = Sample_Load("rom://fx/hurt2.wav");
                    break;
                case(3):
                    samples[sfx_index] = Sample_Load("rom://fx/powerup1.wav");
                    break;
                case(4):
                    samples[sfx_index] = Sample_Load("rom://fx/powerup2.wav");
                    break;
                default:
                    samples[sfx_index] = Sample_Load("rom://fx/hurt1.wav");
                    break;
            }

            voices[sfx_index] = Sample_Play(samples[sfx_index], 0, 0);
            Voice_SetPanning(voices[sfx_index], PAN_CENTER);
            Voice_SetVolume(voices[sfx_index], 256);

            sfx_index++;
            if(sfx_index >= MAX_SFX)
            {
                sfx_index = 0;
            }        
        }
		
    }
    Player_Stop();
    Player_Free(bgm);
    for(int i = 0; i < MAX_SFX; i++)
    {
        Voice_Stop(voices[i]);
        Sample_Free(samples[i]);
    }
}

void draw_gym(display_context_t disp_list, void* wall_sm, void* floor_sm)
{
    /* Attach RDP to display */
        rdp_attach_display( disp_list );

        //////////////////////////////////////////////////////////////////////////////////
        //known working, but silly method
        //sync, load each new tile into til 0 with a tmem offset of 0, then draw
        //////////////////////////////////////////////////////////////////////////////////   
        rdp_sync( SYNC_PIPE );
        rdp_load_texture_stride( 0, 0, MIRROR_DISABLED, wall_sm, 0); //top left corner
        rdp_draw_textured_rectangle(0, 0, 0, 15, 15, MIRROR_DISABLED);
        rdp_sync( SYNC_PIPE );
        rdp_load_texture_stride( 0, 0, MIRROR_DISABLED, wall_sm, 2); //top right corner
        rdp_draw_textured_rectangle(0, 304, 0, 319, 15, MIRROR_DISABLED);        
        rdp_sync( SYNC_PIPE );
        rdp_load_texture_stride( 0, 0, MIRROR_DISABLED, wall_sm, 5); //bottom left corner
        rdp_draw_textured_rectangle(0, 0, 224, 15, 239, MIRROR_DISABLED);
        rdp_sync( SYNC_PIPE );
        rdp_load_texture_stride( 0, 0, MIRROR_DISABLED, wall_sm, 7); //bottom right corner
        rdp_draw_textured_rectangle(0, 304, 224, 319, 239, MIRROR_DISABLED);
        rdp_sync( SYNC_PIPE );
        rdp_load_texture_stride( 0, 0, MIRROR_DISABLED, wall_sm, 1); //top edge
        rdp_draw_textured_rectangle(0, 16, 0, 303, 15, MIRROR_DISABLED);
        rdp_sync( SYNC_PIPE );
        rdp_load_texture_stride( 0, 0, MIRROR_DISABLED, wall_sm, 3); //left edge
        rdp_draw_textured_rectangle(0, 0, 16, 15, 223, MIRROR_DISABLED); 
        rdp_sync( SYNC_PIPE );
        rdp_load_texture_stride( 0, 0, MIRROR_DISABLED, wall_sm, 4); //right edge
        rdp_draw_textured_rectangle(0, 304, 16, 319, 223, MIRROR_DISABLED); 

        rdp_sync( SYNC_PIPE );
        rdp_load_texture_stride( 0, 0, MIRROR_DISABLED, wall_sm, 8); //upper half blank wall
        rdp_draw_textured_rectangle(0, 16, 16, 47, 31, MIRROR_DISABLED);
        rdp_draw_textured_rectangle(0, 80, 16, 255, 31, MIRROR_DISABLED);
        rdp_draw_textured_rectangle(0, 288, 16, 303, 31, MIRROR_DISABLED);
        rdp_sync( SYNC_PIPE );
        rdp_load_texture_stride( 0, 0, MIRROR_DISABLED, wall_sm, 9); //lower half blank wall
        rdp_draw_textured_rectangle(0, 16, 32, 47, 47, MIRROR_DISABLED);
        rdp_draw_textured_rectangle(0, 80, 32, 255, 47, MIRROR_DISABLED);
        rdp_draw_textured_rectangle(0, 288, 32, 303, 47, MIRROR_DISABLED);
        rdp_sync( SYNC_PIPE );
        rdp_load_texture_stride( 0, 0, MIRROR_DISABLED, wall_sm, 10); //poster top left
        rdp_draw_textured_rectangle(0, 48, 16, 63, 31, MIRROR_DISABLED);
        rdp_sync( SYNC_PIPE );
        rdp_load_texture_stride( 0, 0, MIRROR_DISABLED, wall_sm, 11); //poster top right
        rdp_draw_textured_rectangle(0, 64, 16, 79, 31, MIRROR_DISABLED);
        rdp_sync( SYNC_PIPE );
        rdp_load_texture_stride( 0, 0, MIRROR_DISABLED, wall_sm, 12); //poster bottom left
        rdp_draw_textured_rectangle(0, 48, 32, 63, 47, MIRROR_DISABLED);
        rdp_sync( SYNC_PIPE );
        rdp_load_texture_stride( 0, 0, MIRROR_DISABLED, wall_sm, 13); //poster bottom right
        rdp_draw_textured_rectangle(0, 64, 32, 79, 47, MIRROR_DISABLED);
        rdp_sync( SYNC_PIPE );
        rdp_load_texture_stride( 0, 0, MIRROR_DISABLED, wall_sm, 14); //mirror top left
        rdp_draw_textured_rectangle(0, 256, 16, 271, 31, MIRROR_DISABLED);
        rdp_sync( SYNC_PIPE );
        rdp_load_texture_stride( 0, 0, MIRROR_DISABLED, wall_sm, 15); //mirror top right
        rdp_draw_textured_rectangle(0, 272, 16, 287, 31, MIRROR_DISABLED);
        rdp_sync( SYNC_PIPE );
        rdp_load_texture_stride( 0, 0, MIRROR_DISABLED, wall_sm, 16); //mirror bottom left
        rdp_draw_textured_rectangle(0, 256, 32, 271, 47, MIRROR_DISABLED);
        rdp_sync( SYNC_PIPE );
        rdp_load_texture_stride( 0, 0, MIRROR_DISABLED, wall_sm, 17); //mirror bottom right
        rdp_draw_textured_rectangle(0, 272, 32, 287, 47, MIRROR_DISABLED);
        rdp_sync( SYNC_PIPE );
        rdp_load_texture_stride( 0, 0, MIRROR_DISABLED, floor_sm, 0); //floor upper left corner
        rdp_draw_textured_rectangle(0, 16, 48, 31, 63, MIRROR_DISABLED);
        rdp_sync( SYNC_PIPE );
        rdp_load_texture_stride( 0, 0, MIRROR_DISABLED, floor_sm, 1); //floor upper edge
        rdp_draw_textured_rectangle(0, 32, 48, 303, 63, MIRROR_DISABLED);
        rdp_sync( SYNC_PIPE );
        rdp_load_texture_stride( 0, 0, MIRROR_DISABLED, floor_sm, 2); //floor left edge
        rdp_draw_textured_rectangle(0, 16, 64, 31, 223, MIRROR_DISABLED); 
        rdp_sync( SYNC_PIPE );
        rdp_load_texture_stride( 0, 0, MIRROR_DISABLED, floor_sm, 3); //regular floor
        rdp_draw_textured_rectangle(0, 32, 64, 303, 223, MIRROR_DISABLED);             

        /* Inform the RDP we are finished drawing and that any pending operations should be flushed */
        rdp_detach_display();
}

void draw_bottom_wall(display_context_t disp_list, void* wall_sm)
{
        rdp_attach_display( disp_list );
        rdp_sync( SYNC_PIPE );
        rdp_load_texture_stride( 0, 0, MIRROR_DISABLED, wall_sm, 6); //bottom edge
        rdp_draw_textured_rectangle(0, 16, 224, 303, 239, MIRROR_DISABLED); 
        rdp_detach_display();
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

    display_init( RESOLUTION_640x480, DEPTH_16_BPP, 2, GAMMA_NONE, ANTIALIAS_RESAMPLE );
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
                case(1):
                    graphics_draw_sprite( disp, 170, 102, n64logo);
                    break;
                case(2):
                    graphics_draw_sprite( disp, 243, 120, brewlogo);
                    graphics_set_color( 0xFFFFFFFF, 0x00000000 );
                    graphics_draw_text( disp, 273, 380, "(C) KIVAN117" );
                    break;
                case(3):
                    graphics_draw_sprite( disp, 16, 158, jamlogo);
                    graphics_set_color( 0xFFFFFFFF, 0x00000000 );
                    graphics_draw_text( disp, 273, 380, "(C) KIVAN117" );
                    break;
                default:
                    break;
            }
            

        /* Force backbuffer flip */
        display_show(disp);

        controller_scan();
        struct controller_data keys = get_keys_pressed();
        if( keys.c[0].start || keys.c[0].A )
        {
            logo++;
            /* Lazy switching */
            //mode = 1 - mode;
        }
    } while (logo < 4);

    free(n64logo);
    free(jamlogo);
    free(brewlogo);

    display_close();
}

MODULE* play_song(int SongID)
{
    //background music
    MODULE *bgm = NULL;
    switch(SongID)
    {
        case(0):
            bgm = Player_Load("rom://music/teen_spirit.mod", 16, 0);
            break;
        case(1):
            bgm = Player_Load("rom://music/basket_case.mod", 16, 0);
            break;
        case(2):
            bgm = Player_Load("rom://music/enter_sand.mod", 16, 0);
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