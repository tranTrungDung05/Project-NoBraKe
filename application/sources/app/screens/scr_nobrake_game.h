#ifndef __SCR_NOBRAKE_GAME_H__
#define __SCR_NOBRAKE_GAME_H__

#include "fsm.h"
#include "app.h"
#include "task_display.h"
#include "view_render.h"
#include "screens.h"

// Game States
#define GAME_OFF        (0)
#define GAME_PLAY       (1)
#define GAME_OVER       (2)
#define GAME_WIN        (3)
#define GAME_FINISHING  (4)

extern uint8_t game_state;

extern view_dynamic_t dyn_view_item_nobrake_game;
extern view_screen_t scr_nobrake_game;
extern void scr_nobrake_game_handle(ak_msg_t* msg);

#endif
