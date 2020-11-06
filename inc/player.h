#ifndef __PLAYER_H__
#define __PLAYER_H__

enum PLAYER_DIR {RIGHT=0, R_UP=1, UP=2, L_UP=3, LEFT=4, L_DOWN=5, DOWN=6, R_DOWN=7};
enum PLAYER_ACTION {IDLE=0, WALK=1, PUMP = 2, ATTACK=3};

typedef struct {

    int x;
    int y;
    int draw_height;
    int draw_width;
    int coll_height;
    int coll_width;
    int dir;
    int action;

} PLAYER;


#endif