#include "gym.h"
#include "gamestate.h"

void draw_gym(display_context_t disp_list, sprite_t** tiles)
{
    /* Attach RDP to display */
        rdp_attach_display( disp_list );

        //////////////////////////////////////////////////////////////////////////////////
        //known working, but silly method
        //sync, load each new tile into tile 0 with a tmem offset of 0, then draw
        //////////////////////////////////////////////////////////////////////////////////

        //
        //outside border
        //
        rdp_sync( SYNC_PIPE );
        rdp_load_texture_stride( 0, 0, MIRROR_XY, tiles[BORDER], 0); //load border corner texture
        rdp_draw_textured_rectangle(0, 0, 0, 15, 15, MIRROR_DISABLED); //top left corner
        rdp_draw_textured_rectangle(0, 304, 0, 319, 15, MIRROR_X); //top right corner
        rdp_draw_textured_rectangle(0, 0, 224, 15, 239, MIRROR_Y); //bottom left corner
        rdp_draw_textured_rectangle(0, 304, 224, 319, 239, MIRROR_XY); //bottom right corner
        rdp_sync( SYNC_PIPE ); 
        rdp_load_texture_stride( 0, 0, MIRROR_Y, tiles[BORDER], 1); //load border top edge texture
        rdp_draw_textured_rectangle(0, 16, 0, 303, 15, MIRROR_DISABLED); //draw top edge
        rdp_sync( SYNC_PIPE );
        rdp_load_texture_stride( 0, 0, MIRROR_X, tiles[BORDER], 2); //load border left edge
        rdp_draw_textured_rectangle(0, 0, 16, 15, 223, MIRROR_DISABLED); //draw left edge 
        rdp_draw_textured_rectangle(0, 304, 16, 319, 223, MIRROR_X); //draw right edge

        //
        //green wall along the top
        //
        rdp_sync( SYNC_PIPE );
        rdp_load_texture( 0, 0, MIRROR_DISABLED, tiles[WALL1]); //blank wall
        rdp_draw_textured_rectangle(0, 16, 16, 47, 47, MIRROR_DISABLED);
        rdp_draw_textured_rectangle(0, 80, 16, 255, 47, MIRROR_DISABLED);
        rdp_draw_textured_rectangle(0, 272, 16, 303, 47, MIRROR_DISABLED);
        rdp_sync( SYNC_PIPE );
        rdp_load_texture( 0, 0, MIRROR_DISABLED, tiles[WALL2]); //poster
        rdp_draw_textured_rectangle(0, 48, 16, 79, 47, MIRROR_DISABLED);
        rdp_sync( SYNC_PIPE );
        rdp_load_texture( 0, 0, MIRROR_DISABLED, tiles[WALL3]); //mirror
        rdp_draw_textured_rectangle(0, 256, 16, 287, 47, MIRROR_DISABLED);


        //
        //wooden floor
        //
        rdp_sync( SYNC_PIPE );
        rdp_load_texture_stride( 0, 0, MIRROR_DISABLED, tiles[FLOORS], 0); //floor upper left corner
        rdp_draw_textured_rectangle(0, 16, 48, 31, 63, MIRROR_DISABLED);
        rdp_sync( SYNC_PIPE );
        rdp_load_texture_stride( 0, 0, MIRROR_DISABLED, tiles[FLOORS], 1); //floor upper edge shadow
        rdp_draw_textured_rectangle(0, 32, 48, 303, 63, MIRROR_DISABLED);
        rdp_sync( SYNC_PIPE );
        rdp_load_texture_stride( 0, 0, MIRROR_DISABLED, tiles[FLOORS], 2); //floor left edge shadow
        rdp_draw_textured_rectangle(0, 16, 64, 31, 223, MIRROR_DISABLED); 
        rdp_sync( SYNC_PIPE );
        rdp_load_texture_stride( 0, 0, MIRROR_DISABLED, tiles[FLOORS], 3); //regular floor
        rdp_draw_textured_rectangle(0, 32, 64, 303, 223, MIRROR_DISABLED);             

        rdp_detach_display();
}

void draw_bottom_wall(display_context_t disp_list, void* wall_sm)
{
        rdp_attach_display( disp_list );
        rdp_sync( SYNC_PIPE );
        rdp_load_texture_stride( 0, 0, MIRROR_Y, wall_sm, 1); //bottom edge
        rdp_draw_textured_rectangle(0, 16, 224, 303, 239, MIRROR_Y); 
        rdp_detach_display();
}