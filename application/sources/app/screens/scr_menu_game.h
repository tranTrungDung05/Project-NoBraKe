#ifndef __SCR_MENU_GAME_H__
#define __SCR_MENU_GAME_H__

#include "fsm.h"
#include "message.h"
#include "task_display.h"
#include "view_render.h"

extern view_dynamic_t dyn_view_menu;
extern view_screen_t scr_menu_game;
extern void scr_menu_game_handle(ak_msg_t* msg);

#endif
