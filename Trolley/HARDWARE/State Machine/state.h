#ifndef _STATE_H
#define _STATE_H

#include "allFiles.h"

#define MOVE 0
#define TAKEOUT_state 1
#define TAKEIN_state 2
#define QR_state 3
#define findstate_Pre 4
#define findstate_wait 7
#define garage_out 5
#define PositionCheck_state 6
#define move_Rough 8
//”ÎopenmvÕ®–≈
#define QR_Start 0
void state_Init();
//void state_calculation();
void state_ctrl();

extern int state;
extern int out_ku;
#endif
