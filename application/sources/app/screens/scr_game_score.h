#ifndef __SCR_GAME_SCORE_H__
#define __SCR_GAME_SCORE_H__

#include "task_list.h"
#include "task_display.h"
#include "view_render.h"

#define NB_GAME_SCORE_COUNT (3)

extern view_dynamic_t dyn_view_item_game_score;
extern view_screen_t scr_game_score;
extern void scr_game_score_handle(ak_msg_t* msg);
void nb_game_score_record_finish(uint16_t elapsed_s);

#endif
