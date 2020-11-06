#ifndef __PICKUPS_H__
#define __PICKUPS_H__

enum PICKUP_TYPES {JUICE, WATER, DUMBBELL, BARBELL, KETTLEBELL};

typedef struct {
    int x;
    int y;
    int draw_height;
    int draw_width;
    int coll_height;
    int coll_width;
    int type;
} PICKUP;

#endif