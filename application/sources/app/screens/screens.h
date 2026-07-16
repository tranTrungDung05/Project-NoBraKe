#ifndef __SCREENS_H__
#define __SCREENS_H__

#include "scr_game_score.h"
#include "scr_game_finish.h"
#include "scr_game_over.h"
#include "scr_game_settings.h"
#include "scr_idle.h"
#include "scr_menu_game.h"
#include "scr_startup.h"
#include "scr_welcome.h"

// scr_startup
extern view_dynamic_t dyn_view_startup;
extern view_screen_t scr_startup;
extern void scr_startup_handle(ak_msg_t* msg);

// scr_welcome
extern view_dynamic_t dyn_view_welcome;
extern view_screen_t scr_welcome;
extern void scr_welcome_handle(ak_msg_t* msg);

// scr_menu_game
extern view_dynamic_t dyn_view_menu;
extern view_screen_t scr_menu_game;
extern void scr_menu_game_handle(ak_msg_t* msg);

//scr_nobrake_game
extern view_dynamic_t dyn_view_item_nobrake_game;
extern view_screen_t scr_nobrake_game;
extern void scr_nobrake_game_handle(ak_msg_t* msg);

// scr_game_score
extern view_dynamic_t dyn_view_item_game_score;
extern view_screen_t scr_game_score;
extern void scr_game_score_handle(ak_msg_t* msg);

// scr_game_finish
extern view_dynamic_t dyn_view_item_game_finish;
extern view_screen_t scr_game_finish;
extern void scr_game_finish_handle(ak_msg_t* msg);

// scr_game_over
extern view_dynamic_t dyn_view_item_game_over;
extern view_screen_t scr_game_over;
extern void scr_game_over_handle(ak_msg_t* msg);

// scr_game_settings
extern view_dynamic_t dyn_view_item_game_settings;
extern view_screen_t scr_game_settings;
extern void scr_game_settings_handle(ak_msg_t* msg);

// scr_idle
extern view_dynamic_t dyn_view_idle;
extern view_screen_t scr_idle;
extern void scr_idle_handle(ak_msg_t* msg);

#endif //__SCREENS_H__
