#ifndef __SCR_GAME_SETTINGS_H__
#define __SCR_GAME_SETTINGS_H__

#include "task_list.h"
#include "task_display.h"
#include "view_render.h"

extern view_dynamic_t dyn_view_item_game_settings;
extern view_screen_t scr_game_settings;
extern void scr_game_settings_handle(ak_msg_t* msg);

#endif