#ifndef __SCORE_POPUPS_H__
#define __SCORE_POPUPS_H__

typedef struct {
    int x; //x and y coords to draw the popup
    int y;
    int score; //value 1 to 10, indicating the score ranges 100 to 1000
    int ttl; //how long to keep drawing this thing, in fame counts
             // 0 or negative ttl means do not draw
} POPUP;

#endif