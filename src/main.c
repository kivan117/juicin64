
#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <stdint.h>
#include <libdragon.h>

#include "version.h"
#include "flashcart.h"

static volatile uint32_t animcounter = 0;

void update_counter( int ovfl )
{
    animcounter++;
}

void draw_gym(display_context_t disp_list, void* wall_sm, void* floor_sm);
void draw_bottom_wall(display_context_t disp_list, void* wall_sm);
void draw_intros(void);

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

    while(1) 
    {
        static display_context_t disp = 0;

        /* Grab a render buffer */
        while( !(disp = display_lock()) );
       
        graphics_fill_screen( disp, 0x00000000 );

        graphics_set_color( 0xFFFFFFFF, 0x00000000 );

        rdp_sync( SYNC_PIPE );

        rdp_set_default_clipping();

        rdp_enable_texture_copy();

        draw_gym(disp, walls, floors);

		graphics_draw_text( disp, 80, 18, "N64Brew Jam Test" );

        graphics_draw_sprite_trans_stride( disp, player_x, player_y, mc_current_sprite, ((animcounter / 8) & 0x3)  ); 

        draw_bottom_wall(disp, walls); 

        /* Force backbuffer flip */
        display_show(disp);

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
        // if( keys.c[0].A )
        // {
        //     /* Lazy switching */
        //     mode = 1 - mode;
        // }
		
    }
}

void draw_intros()
{
    int fp = 0;
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
        if(animcounter < 240)
        {
            graphics_draw_sprite( disp, 170, 102, n64logo);
        }
        else if (animcounter < 360)
        {
            graphics_draw_sprite( disp, 243, 120, brewlogo);
            graphics_set_color( 0xFFFFFFFF, 0x00000000 );
            graphics_draw_text( disp, 253, 380, "(C) 2020 KIVAN117" );
        }
        else
        {
            graphics_draw_sprite( disp, 16, 158, jamlogo);
            graphics_set_color( 0xFFFFFFFF, 0x00000000 );
            graphics_draw_text( disp, 253, 380, "(C) 2020 KIVAN117" );
        }
        /* Force backbuffer flip */
        display_show(disp);
    } while (animcounter < 480);

    free(n64logo);
    free(jamlogo);
    free(brewlogo);

    display_close();
}

void draw_gym(display_context_t disp_list, void* wall_sm, void* floor_sm)
{
    /* Attach RDP to display */
        rdp_attach_display( disp_list );
           
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