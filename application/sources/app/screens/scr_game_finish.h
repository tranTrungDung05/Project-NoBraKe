#ifndef __SCR_GAME_FINISH_H__
#define __SCR_GAME_FINISH_H__

#include "fsm.h"
#include "message.h"
#include "task_display.h"
#include "view_render.h"

extern view_dynamic_t dyn_view_item_game_finish;
extern view_screen_t scr_game_finish;
extern void scr_game_finish_handle(ak_msg_t* msg);

#endif
